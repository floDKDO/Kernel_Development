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
Les sections data et text de la librairie partagée sont ajoutées à l'espace d'adressage de l'exécutable via mmap() par le dynamic linker (pas de copie : les sections de la librarie partagée sont partagées entre les processus qui l'utilise) => un exécutable qui utilise une librairie partagée est dépendent de celle-ci au run-time. Cela n'est pas le cas s'il utilise une librairie statique car celle-ci fait partie intégrante de l'exécutable.

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
Programme qui s'exécute correctement (= n'a pas besoin d'être modifié) peu importe où il est placé en mémoire. gcc a par défaut ```--enable-default-pie``` (voir les options ```-fpic``` ou ```-fPIC``` pour compiler un programme en tant que position-independent code).
- comme une librairie partagée sera utilisée par des processus différents, on ne sait pas à l'avance où celle-ci se trouvera en mémoire : elle doit donc être position independent (objectif = ne pas avoir à effectuer de relocations) => peu importe où les segments de cette librairie sont placés dans l'espace d'adressage d'un processus, leur code doit fonctionner.

Procedure Linkage Table (PLT) : dans le code segment (.init .plt .plt.got .plt.sec .text .fini), taille des entrées = 16 octets, sections en question = .plt .plt.got .plt.sec
Global Offset Table (GOT) : dans le data segment (.init_array .fini_array .dynamic .got .data .bss), taille des entrées = 8 octets, section en question = .got

TODO : gcc ```-fno-plt```

