# Shared libraries

## Introduction
Une librairie partagée (extension .so) est un fichier ELF.
Il est possible d'en créer une avec la commande ```gcc -shared -fpic -o libtest.so file1.o file2.o``` => créer la librairie partagée (option ```-shared``` pour librairie partagée et ```fpic``` pour position independent code) libtest.so composée des fichiers objets file1.o et file2.o

Pour lier une librairie partagée à d'autres fichiers objets, il y a l'option ```-l``` de gcc.
Exemple : ```gcc -shared -o prog main.o -L ./ -ltest``` => lier la librairie partagée (option ```-shared```) libtest.so (l'option ```-l``` doit être directement suivie du nom de la librarie sans le préfixe "lib" et sans l'extension .so) se trouvant dans le répertoire courant (option ```-L```) au fichier objet main.o pour former l'exécutable prog.

Le linker lie une première partie de la librairie partagée à l'exécutable, la suite sera liée au run-time par le dynamic linker.

Par rapport à une librairie statique, les sections d'une librarie partagée ne sont pas ajoutées dans le fichier de l'exécutable. => taille de l'exécutable plus petit.
Exemple : 
- création d'un exécutable avec la librarie statique du C (libc.a) : ```gcc -static -o hello_static hello.c```
	- taille = 785 Ko
	- dépendance à aucune librarie partagée : 
		```$ ldd hello_static```
		```not a dynamic executable```
- création d'un exécutable avec la librarie dynamique du C (libc.so) : ```gcc -o hello hello.c```
	- taille = 15 Ko
	- dépendance à des librairies partagées : 
		```$ ldd hello2```
		```linux-vdso.so.1 (0x00007ffd823ba000)```
		```libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007fe2a6000000)```
		```/lib64/ld-linux-x86-64.so.2 (0x00007fe2a63f9000)```
Les sections .data et .text de la librairie partagée sont ajoutées à l'espace d'adressage de l'exécutable via mmap() par le dynamic linker (pas de copie : les sections de la librarie partagée sont partagées entre les processus qui l'utilise) => un exécutable qui utilise une librairie partagée est dépendent de celle-ci au run-time. Cela n'est pas le cas s'il utilise une librairie statique car celle-ci fait partie intégrante de l'exécutable.
Attention, les sections .data et .text de la librairie partagée ne sont pas fusionnées aux sections .data et .text de l'exécutable (cas avec les librairies statiques) ! Elles sont placées dans une partie de l'espace d'adressage réservée aux librairies partagées via mmap() ("Memory-mapped region for shared libraries", adresse = 0x40000000).

La section .interp contient le chemin du dynamic linker (*/lib64/ld-linux-x86-64.so.2.* sur ma machine).

On parle de librairie dynamique quand une librairie partagée est chargée par un dynamic linker.

Linux met à disposition des fonctions de bibliothèque suivantes pour intéragir avec des librairies partagées (```#include <dlfcn.h>```) : 
- ```dlopen``` (nécessite l'utilisation de l'option gcc ```-rdynamic```)
- ```dlsym```
- ```dlclose```
- ```dlerror```

Voici quelques exemples de librairies partagées du langage C (présentes dans le dossier */usr/lib/x86_64-linux-gnu* sur ma machine) : 
- libc.so : librairie standard du langage C 
- libm.so : librarie de maths du langage C (utiliser l'option ```-lm``` pour la lier à notre programme) 
- libpthread.so.0 : librairie des threads POSIX (utiliser l'option ```-lpthread``` pour la lier à notre programme) 

## Position-independent code
Programme qui s'exécute correctement (= n'a pas besoin d'être modifié) peu importe où il est placé en mémoire => obligatoire pour compiler des libraries partagées. 
gcc a par défaut ```--enable-default-pie``` (voir les options ```-fpic``` ou ```-fPIC``` pour compiler un programme en tant que position-independent code). Pour cela, il n'utilise pas directement des adresses pour accéder à des symboles mais utilise une GOT et PLT. 
Une librarie partagée ne sait pas à l'avance où elle sera placée en mémoire, c'est-à-dire où seront placés ses segments .text et .data. 
Elle sait cependant que la distance entre ces deux sections sera toujours la même : elle peut donc utiliser un adressage PC-relative pour faire référence aux entrées de la GOT, cela pour des symboles UNDEF (= externes) mais aussi pour des symboles de la librarie partagée elle-même (pas obligé mais c'est souvent le cas).
Un exécutable qui appelle des fonctions provenant d'une librarie partagée aura une GOT et PLT.

### Global Offset Table (GOT)

La GOT est placée dans le data segment (.init_array .fini_array .dynamic **.got** .data .bss) de l'exécutable. C'est une section (.got et .got.plt) qui contient un tableau dont les entrées ont une taille de 8 octets. 

Les entrées 0, 1 et 2 sont réservées : 
- GOT[0] et GOT[1] sont utilisées par le dynamic linker 
- GOT[2] contient l'adresse du dynamic linker (= son point d'entrée)
Les autres entrées contiennent : 
- dans le cas des fonctions, les entrées contiennent initialement l'adresse de la seconde instruction dans l'entrée associée de la PLT, et après un premier appel à la fonction, l'adresse de cette fonction
- dans le cas des variables globales, les entrées contiennent l'adresse de la variable globale souhaitée

La GOT est initialisée au link time par le linker et modifiée au load time par le dynamic linker (il met dans chaque entrée l'adresse finale de chaque symbole référencée par chaque entrée de la GOT selon les sections de relocation associées).
.got.plt est inclus (est un sous-ensemble) de .got

Dans le code, un accès à une variable globale est indirectement réalisé via les entrées de la GOT.
Exemple : 
```
extern int ma_var_globale;

int main(void)
{
	ma_var_globale += 1;
	return 0;
}
```
En assembleur : 
```
Dump of assembler code for function main:
   0x000000000040116b <+0>:	endbr64
   0x000000000040116f <+4>:	push   %rbp
   0x0000000000401170 <+5>:	mov    %rsp,%rbp
=> 0x0000000000401173 <+8>:	mov    0x2ea3(%rip),%eax        # 0x40401c <ma_var_globale>
   0x0000000000401179 <+14>:	add    $0x1,%eax
   0x000000000040117c <+17>:	mov    %eax,0x2e9a(%rip)        # 0x40401c <ma_var_globale>
   0x0000000000401182 <+23>:	mov    $0x0,%eax
   0x0000000000401187 <+28>:	pop    %rbp
   0x0000000000401188 <+29>:	ret
```
Sur la ligne pointée, on voit qu'on accède à une certaine adresse (adresse d'une entrée dans la GOT) en utilisant un adressage PC-relative. L'entrée dans la GOT contient l'adresse de la variable *ma_var_globale* et celle-ci est stockée dans EAX. On peut réaliser nos opérations en pointant sur l'adresse contenue dans EAX. 


### Procedure Linkage Table (PLT)
La PLT est placée dans le code segment (.init **.plt .plt.got .plt.sec** .text .fini) de l'exécutable. 
C'est une section (.plt .plt.got .plt.sec) qui contient un tableau dont les entrées ont une taille de 16 octets.

Les entrées 0 et 1 sont réservées : 
- PLT[0] saute vers le dynamic linker, donc vers l'adresse contenue dans GOT[2]
- PLT[1] appelle __libc_start_main()
Les autres entrées appellent les fonctions souhaitées en passant par les entrées associées de la GOT.

A noter : comme la PLT est placée dans le code segment, celle-ci n'est pas modifiable par le dynamic linker (uniquement droit de lecture et d'exécution). Ce dernier peut uniquement modifier la GOT car le data segment possède est modifiable (droits de lecture et d'écriture).

Info : il existe dans gcc l'option ```-fno-plt```

### Lazy binding
Lors du premier appel à une fonction (saut dans l'entrée associée dans la PLT), l'entrée associée dans la GOT ne contient pas encore l'adresse de la fonction (lazy binding) mais l'adresse de la deuxième instruction de son entrée associée dans la PLT. La PLT saute dans l'entrée associée dans la GOT, ce qui la fait sauter vers sa seconde instruction qui consiste à empiler un identifiant sur la pile et enfin à appeler le dynamic linker. Ce dernier va modifier l'entrée associée dans la GOT avec l'adresse de la fonction et saute dans cette fonction. 

Lors du deuxième appel à une fonction (saut dans l'entrée associée dans la PLT), l'entrée associée dans la GOT contient l'adresse de la fonction (mise par le dynamic linker lors du premier appel). La PLT saute dans l'entrée associée dans la GOT et saute directement vers la fonction.

## Quelques variables d'environnement du dynamic linker
- LD_BIND_NOW : si on la met à 1, le dynamic linker met immédiatement les adresses des symboles dans la GOT (inverse du lazy binding)
- LD_PRELOAD : on peut lui passer une liste de librairies partagées qui doivent être chargées en premier (donc avant les libraries standards du C) dans l'exécutable. Si un programme a des symboles UNDEF et que ces librairies partagées définissent ces symboles, alors les symboles seront résolus. Exemple : soit un programme qui appelle la fonction rand(). On ajoute à LD_PRELOAD une librairie partagée qui définir rand() => ce programme va utiliser notre implémentation de la fonction rand() au lieu de celle de la librairie standard du C. Pour annuler les effets de LD_PRELOAD, on peut compiler notre programme avec ```-static``` pour empêcher l'utilisation de libraries partagées.

