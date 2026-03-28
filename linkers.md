# Linkers

L'assembleur (ex : ```as```) prend un fichier avec du code assembleur et le convertit en un fichier objet.

Le linker combine plusieurs fichiers objets pour créer un fichier exécutable.
On différencie le *program linker* (fichiers objets -> fichier exécutable) du *dynamic linker* (lier les librairies partagées à l'exécutable quand ce dernier est exécuté = au runtime).

Les paramètres de fonctions et les variables locales sans le mot clef "extern" ne sont pas des symboles.
Les symboles sont dans les sections
Les symboles qui valent 0 sont dans aucune section quand le fichier ELF est sur le disque (.bss) TODO : à creuser

Une relocation est une action réalisée par le linker. Elle consiste à modifier l'adresse de symboles pour qu'elle corresponde à la disposition de l'exécutable final, qui est une combinaison de plusieurs fichiers objets (une adresse dans un fichier objet serait inexacte dans l'exécutable final car la position des sections serait modifiée).

Signification de sh_link et sh_info pour SHT_REL et SHT_RELA : 
	- sh_link contient l'indice du section header de .symtab
	- sh_info contient l'indice du section header associé à la section qui va subir la relocation
Exemple : 
Section Headers:
  [Nr] Name              Type            Address          Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            0000000000000000 000000 000000 00      0   0  0
  [ 1] .text             PROGBITS        0000000000000000 000040 000057 00  AX  0   0  1
  [ 2] .rela.text        RELA            0000000000000000 000250 000048 18   I 13   1  8
  ...
  [13] .symtab           SYMTAB          0000000000000000 000170 0000c0 18     14   5  8
  ...
Ici, pour .rela.text, on a : 
	- sh_link = 13 = indice de .symtab
	- sh_info = 1 = indice de .text car .rela.text est associée à .text

La commande ```readelf -r prog``` affiche le contenu des sections de relocation
Chaque section de relocation contient des entrées : 
	- r_offset : pour un relocatable file, offset depuis le début de la section en question => c'est à partir de cet offset qu'une relocation va avoir lieue
	- r_info : contient deux sous-champs : 
		- indice dans la table des symboles (.symtab) qui indique le symbole sur lequel la relocation va être effectuée
		- type de relocation. 
		Exemple : 
			- R_386_32 : remplacer directement l'adresse en question (dans la section indiquée par sh_info et à l'offset dans cette section indiqué par r_offset) par la valeur du symbole (= son adresse virtuelle dans l'espace d'adressage du processus) telle qu'indiquée dans la table des symboles
	- r_addend : membre présent uniquement pour les sections de relocation de type SHT_RELA, nombre constant (ex : 0x32)
	
Ex : 
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

Prenons par exemple la deuxième ligne (appel à printf qui appelle puts) : 
- r_offset = 13 => désigne les zéros (= adresse) pour l'instruction call qui débute à "12:"
- r_info = 5 (indice de puts dans .symtab) et 4 (type = R_X86_64_PLT32)




Les instructions d'un fichier .o (type = relocatable file) qui utilisent des adresses (ex : call) utilisent l'adresse 0.
Exemple :  e8 00 00 00 00  call   4d <main+0x1b> => e8 est l'opcode de call, l'opérande est l'adresse 0 sur 32 bits
Les sections de relocations (ex : .rela.text) sont visibles uniquement dans les fichiers .o

TODO : importance de l'ordre de l'option -l dans gcc (il faut mettre cette option à la fin de la commande = après l'exécutable, car dans ce sens uniquement, le linker connaîtra les symboles undef du fichier objet et pourra les résoudre avec la librairie. Dans l'autre sens, il ne connaît pas les symboles undef quand il va lire la librairie). La règle à respecter est de mettre en dernier le fichier qui est utilisé par un fichier précédent

La fonction _start() appelle __libc_start_main() qui appelle main(). C'est pour ça que si on compile un fichier qui ne contient pas de fonction main(), on obtient une erreur qui dit que dans _start(), il y a une référence indéfinie vers main().
Exemple : 
/usr/bin/ld: /usr/lib/gcc/x86_64-linux-gnu/13/../../../x86_64-linux-gnu/Scrt1.o: in function `_start':
(.text+0x1b): undefined reference to `main'
collect2: error: ld returned 1 exit status

Stabs, DWARF

.interp, .plt, .got, .got.plt

ld.so / ld-linux.so : dynamic linker

Position Independent Code (```-fpic``` dans gcc)
Comme une librairie partagée sera utilisée par des processus différents, on ne sait pas à l'avance où celle-ci se trouvera en mémoire : elle doit donc être position independent (objectif = limiter les relocations) => peu importe où les segments de cette librairie sont placés dans l'espace d'adressage d'un processus, leur code doit fonctionner.

Procedure Linkage Table (PLT) : n'existe pas dans un fichier objet .o, 

Global Offset Table (GOT) : n'existe pas dans un fichier objet .o, 

LD_BIND_NOW :

GLOB_DAT : 

JMP_SLOT :  
