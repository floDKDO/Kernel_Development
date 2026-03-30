# Static libraries

Une librairie statique (extension .a) est une archive de fichiers objets.
Il est possible d'en créer une avec la commande ```ar``` (ex : ```ar rcs libtest.a file1.o file2.o``` => créer la librairie statique libtest.a composée des fichiers objets file1.o et file2.o)

Pour lier une librairie statique à d'autres fichiers objets, il y a l'option ```-l``` de gcc.
Exemple : ```gcc -static -o prog main.o -L ./ -ltest``` => lier la librairie statique (option ```-static```) libtest.a (l'option ```-l``` doit être directement suivie du nom de la librarie sans le préfixe "lib" et sans l'extension .a) se trouvant dans le répertoire courant (option ```-L```) au fichier objet main.o pour former l'exécutable prog.

Le linker copie dans l'exécutable uniquement les fichiers objets de l'archive dont des symboles sont utilisés par l'exécutable.

Lorsqu'on qu'on utilise l'option ```-l``` de gcc, l'ordre des arguments de la commande est important. En effet, il faut placer en premier les fichiers qui utilisent des symboles de librairies statiques et ensuite ces librairies statiques (dans ce sens uniquement, le linker connaîtra les symboles *SHN_UNDEF* du fichier objet et pourra les résoudre avec la librairie. Dans l'autre sens, il ne connaît pas les symboles *SHN_UNDEF* des fichiers suivants quand il va lire la librairie). La règle à respecter est de mettre en dernier le fichier dont les symboles sont utilisés par un fichier précédent.

Voici quelques exemples de librairies statiques du langage C (présentes dans le docssier */usr/lib/x86_64-linux-gnu* sur ma machine) : 
- libc.a : librairie standard du langage C (utiliser l'option ```-static``` pour la lier à notre programme car c'est la version dynamique de cette librairie libc.so qui est liée par défaut)
- libm.a : librarie statique de maths du langage C (utiliser l'option ```-lm``` pour la lier à notre programme) 
- libpthread.a : librairie des threads POSIX (utiliser l'option ```-lpthread``` pour la lier à notre programme) 
