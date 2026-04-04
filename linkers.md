
# Linkers

L'assembleur (ex : ```as```) prend un fichier avec du code assembleur et le convertit en un fichier objet.

Le linker combine plusieurs fichiers objets pour créer un fichier exécutable. Pour cela, il combine les sections de tous les fichiers objets (ex : regrouper toutes les sections .data des fichiers objets en une seule section .data dans l'exécutable final).
On différencie le *program linker* (fichiers objets -> fichier exécutable) du *dynamic linker* (lier les librairies partagées à l'exécutable quand ce dernier est exécuté = au runtime).
Le programme est chargé en mémoire et exécuté par le loader. Ce dernier est appelé quand le programme appelle la primitive exec*(). C'est le loader qui appelle le dynamic linker.

Les paramètres de fonctions et les variables locales sans le mot clef "extern" ne sont pas des symboles.
Les symboles sont dans les sections
Les symboles qui valent 0 sont dans aucune section quand le fichier ELF est sur le disque (.bss)

## Relocation
Une relocation est une action réalisée par le linker. Elle consiste à modifier l'adresse de symboles pour qu'elle corresponde à la disposition de l'exécutable final, qui est une combinaison de plusieurs fichiers objets (une adresse dans un fichier objet serait inexacte dans l'exécutable final).
Dans un exécutable, il ne devrait plus avoir de sections de relocation (hors .rela.dyn et .rela.plt).
Toutes les sections ne sont pas dans des segments : par exemple, les sections .symtab et .strtab ne sont pas dans un segment de l'exécutable final.

Signification de sh_link et sh_info pour SHT_REL et SHT_RELA : 
- *sh_link* contient l'indice du section header de .symtab
- *sh_info* contient l'indice du section header associé à la section qui va subir la relocation

Exemple : 
```
Section Headers:
  [Nr] Name              Type            Address          Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            0000000000000000 000000 000000 00      0   0  0
  [ 1] .text             PROGBITS        0000000000000000 000040 000057 00  AX  0   0  1
  [ 2] .rela.text        RELA            0000000000000000 000250 000048 18   I 13   1  8
  ...
  [13] .symtab           SYMTAB          0000000000000000 000170 0000c0 18     14   5  8
  ...
 ```
Ici, pour .rela.text, on a : 
- *sh_link* = 13 = indice de .symtab
- *sh_info* = 1 = indice de .text car .rela.text est associée à .text

La commande ```readelf -r prog``` affiche le contenu des sections de relocation
Chaque section de relocation contient des entrées : 
- *r_offset* : pour un relocatable file, offset depuis le début de la section en question => c'est à partir de cet offset qu'une relocation va avoir lieue (souvent l'opérande "adresse" d'une instruction) 
- *r_info* : contient deux sous-champs : 
	- *symbol* : indice dans la table des symboles (.symtab) qui indique le symbole sur lequel doit pointer l'adresse "section en question + r_offset"
	- *type* : type de relocation
		Exemple : 
			- R_386_32 / R_X86_64_32 : remplacer directement l'adresse en question (dans la section indiquée par sh_info et à l'offset dans cette section indiqué par r_offset) par la valeur du symbole (= son adresse virtuelle dans l'espace d'adressage du processus) telle qu'indiquée dans la table des symboles, plus un éventuel addend
			- R_386_PC32 / R_X86_64_PC32 : adresse relative au program counter (= EIP)
- *r_addend* : membre présent uniquement pour les sections de relocation de type SHT_RELA, nombre constant (ex : 0x32)

### Exemple de relocation
Soit le programme suivant : 
```
0000000000000000 <main>:
   0: 48 83 ec 08 		sub $0x8,%rsp
   4: be 02 00 00 00 		mov $0x2,%esi
   9: bf 00 00 00 00 		mov $0x0,%edi 
                	a: R_X86_64_32 array 
   e: e8 00 00 00 00 		callq 13 <main+0x13> 
                	f: R_X86_64_PC32 sum-0x4 
  13: 48 83 c4 08 		add $0x8,%rsp
  17: c3 			retq
```

Cas 1 : *R_386_32*

Soit l'entrée associée dans .rela.text : 
- r_offset = 0xa
- r_info = variable array, R_X86_64_32
- r_addend = 0

Soit l'adresse finale de la variable array = 0x601018  (adresse finale de array = valeur du champ *st_value* dans l'entrée de .symtab qui est associée au symbole array).
Comme il n'y a pas d'addend, on a : S + A = S + 0 = S = 0x601018.

Dans l'exécutable final, l'instruction mov est à l'adresse 4004d9 : 
```4004d9: bf 18 10 60 00 mov $0x601018,%edi```

On voit bien que l'adresse 0x601018 a été écrite en little-endian (18 10 60).


Cas 2 : *R_386_PC32*

Soit l'entrée associée dans .rela.text : 
- r_offset = 0xf
- r_info = fonction sum(), R_X86_64_PC32
- r_addend = -4

Soit l'adresse finale de la section .text = 0x4004d0 et l'adresse finale de la fonction sum() = 0x4004e8 (adresse finale de .text = valeur du champ *sh_addr* du section header de .text, et adresse finale de sum() = valeur du champ *st_value* dans l'entrée de .symtab qui est associée au symbole sum()).

On calcule "P" : P = 0x4004d0 + r_offset = 0x4004d0 + 0xf = 0x4004df (adresse remplie de zéros qui est l'opérande de mov à remplacer)

On a : S + A - P = 0x4004e8 + (-4) - 0x4004df = 0x5

Dans l'exécutable final, l'instruction callq est à l'adresse 0x4004de : 
```
    4004de: e8 05 00 00 00 callq 4004e8 <sum> 
    ...
00000000004004e8 <sum>:
    ...
```

On voit bien qu'à la suite de l'opcode de callq (= e8), il y a l'adresse 0x5 = S + A - P.
Au moment où cette instruction sera exécutée, EIP pointera sur l'instruction suivante à l'adresse 0x4004e3.
En réalisant l'opération : 0x4004e3 + 0x5 = 0x4004e8, on obtient bien "S", c'est-à-dire l'adresse de la fonction sum() mais relative au registre EIP. 

Autre exemple : 
```
Relocation section '.rela.text' at offset 0x250 contains 3 entries:
  Offset          Info           Type           Sym. Value    Sym. Name + Addend
00000000000b  000300000002 R_X86_64_PC32     0000000000000000 .rodata - 4
000000000013  000500000004 R_X86_64_PLT32    0000000000000000 puts - 4
000000000049  000600000004 R_X86_64_PLT32    000000000000001a add - 4

Symbol table '.symtab' contains 8 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS hello1.c
     2: 0000000000000000     0 SECTION LOCAL  DEFAULT    1 .text
     3: 0000000000000000     0 SECTION LOCAL  DEFAULT    5 .rodata
     4: 0000000000000000    26 FUNC    LOCAL  DEFAULT    1 init1
     5: 0000000000000000     0 NOTYPE  GLOBAL DEFAULT  UND puts
     6: 000000000000001a    24 FUNC    GLOBAL DEFAULT    1 add
     7: 0000000000000032    37 FUNC    GLOBAL DEFAULT    1 main
     
Disassembly of section .text:

0000000000000000 <init1>:
   8:	48 8d 05 00 00 00 00 	lea    0x0(%rip),%rax        # f <init1+0xf>
   ...
  12:	e8 00 00 00 00       	call   17 <init1+0x17>
   ...

0000000000000032 <main>:
  ...
  48:	e8 00 00 00 00       	call   4d <main+0x1b>
  ...
```
Prenons par exemple la deuxième ligne (appel à printf qui appelle puts) : 
- *r_offset* = 13 => désigne les zéros (= adresse) pour l'instruction call qui débute à "12:"
- *r_info* = 5 (indice de puts dans .symtab) et 4 (type = R_X86_64_PLT32)

Les instructions d'un fichier .o (type = relocatable file) qui utilisent des adresses (ex : call) utilisent l'adresse 0 => adresse temporaire.
Exemple :  ```e8 00 00 00 00  call   4d <main+0x1b>``` => e8 est l'opcode de call, l'opérande est l'adresse 0 sur 32 bits
Les sections de relocations (ex : .rela.text) sont visibles uniquement dans les fichiers .o

## TODOs

Stabs, DWARF

GLOB_DAT : 

JMP_SLOT :  

