# ELF

L'Executable and Linking Format (ELF) est le format des fichiers objets sur les systèmes Unix.
Il y a 3 types de fichiers objets : 
- relocatable file (les relocatable files sont liés entre eux pour créer un exécutable ou une librairie partagée)
- executable file 
- shared object file (le linker ```ld``` lie la librairie partagée à d'autres relocatable files, ou le dynamic linker lie la librairie partagée à l'exécutable au runtime)

## Commandes
- ```man elf``` : explique le contenu d'un fichier ELF
- ```readelf``` : obtenir des informations sur un fichier ELF 
- ```nm``` : afficher la table des symboles du fichier ELF

## gcc
- voir les options activées par défaut : ```gcc -v``` dans la partie "Configured with". Par défaut, mon gcc a notamment ```--enable-default-pie``` => e_type = ET_DYN (Position-Independent Executable file). Si l'on souhaite obtenir un exécutable classique, il faut désactiver PIE avec l'option ```-no-pie```. Finalement, on obtient e_type = ET_EXEC (exécutable)
- ajouter des attributs dans notre code (ex avec weak) : ```__attribute__((weak))```
- pour obtenir un fichier compilé mais pas link (file.o), on utilise la commande : ```gcc -c file.c```
- pour obtenir des informations de déboguage (sections .debug*), on utilise l'option ```-g``` avec gcc
- gcc dispose des éléments suivants : 
	- préprocesseur : ```cpp```. L'option associée dans gcc est ```-E``` et l'extension de fichier associée est *.i*
	- compilateur : ```cc1``` ou ```cc1plus``` (est interne à gcc : sur ma machine, il est dans */usr/libexec/gcc/x86_64-linux-gnu/13/*). L'option associée dans gcc est ```-S``` et l'extension de fichier associée est *.s*
	- assembleur : ```as```. L'option asssociée dans gcc est ```-c``` et l'extension de fichier associée est *.o* ou *.so*
	- linker (statique) : ```ld```. On arrive jusqu'à cette étape si on utilise gcc sans l'une des options ci-dessus. Le fichier généré par ```ld``` est un fichier exécutable ELF.


## Contenu
Un fichier ELF est composé des éléments suivants : 
- ELF header
- Program header table (optionnelle)
- sections
- Section header table (optionnelle)

### ELF Header
On peut l'afficher avec la commande : ```readelf -h prog```
Il est composé des champs suivants : 
- Magic (*e_ident[NIDENT=16]*) : ce champ est composé des sous-champs suivants : 
	- *EI_MAG0*->*EI_MAG3* : 0x7f + "ELF" en hexadécimal
	- *EI_CLASS* : architecture 32 bits ou 64 bits
	- *EI_DATA* : little-endian ou big-endian
	- *EI_VERSION* : doit valoir 1
	- *EI_OSABI* : indique l'ABI utilisée par notre OS (ex : UNIX System V ABI)
	- *EI_ABIVERSION* : doit valoir 0
	- *EI_PAD* : padding de zéros
- Type (*e_type*) : indique le type du fichier (relocatable file .o, exécutable, librairie partagée .so, fichier core).
Il existe également les commandes : ```nm -a prog``` et ```nm -D prog``` pour respectivement afficher .symtab et .dynsym. 
- Machine (*e_machine*) : architecture du processeur => ISA (ex : AMD x86-64)
- Version (*e_version*) : doit valoir 1
- Entry point address (*e_entry*) : adresse virtuelle du premier octet de l'instruction qui doit être exécutée en premier (_start() en C) ou 0
- Start of program headers (*e_phoff*) : indique l'offset depuis le début du fichier ELF pour atteindre la program header table, ou 0
- Start of section headers (*e_shoff*) : indique l'offset depuis le début du fichier ELF pour atteindre la section header table, ou 0
- Flags (*e_flags*) : vaut 0 habituellement
- Size of this header (*e_ehsize*) : taille du ELF Header
- Size of program headers (*e_phentsize*) : taille d'une entrée dans la program header table, ou 0
- Number of program headers (*e_phnum*) : contient le nombre d'entrées dans la program header table, ou 0
- Size of section headers (*e_shentsize*) : taille d'une entrée dans la section header table
- Number of section headers (*e_shnum*) : contient le nombre d'entrées dans la section header table
- Section header string table index (*e_shstrndx*) : contient l'indice dans la section header table de l'entrée qui pointe sur la section contenant les noms des sections (.shstrtab)


### Program header table
Elle est optionnelle. On peut l'afficher avec la commande : ```readelf -l -W prog```.
Uniquement les fichiers exécutables et les librairies partagées peuvent disposer d'une program header table.
Chaque entrée de la program header table décrit un segment via un program header.
Les segments sont utilisés par l'OS (le loader) pour charger un programme en mémoire (type = *PT_LOAD*) => ce sont les segments qui sont placés/chargés en mémoire.
Un segment contient une ou plusieurs sections. La commande ci-dessus affiche également les sections contenues par chaque segment.

Une section appartient à un segment si son intervalle [*sh_addr*, *sh_addr* + *sh_size*] appartient à l'intervalle [*p_vaddr*, *p_vaddr* + *p_memsz*] du segment en question.  
Exemple : 
- Soit le program header suivant : 	
	```Type           Offset   VirtAddr           PhysAddr           FileSiz  MemSiz   Flg Align```
    ```LOAD           0x001000 0x0000000000001000 0x0000000000001000 0x000189 0x000189 R E 0x1000```
	- Le segment associé à ce program header contient les sections suivantes : .init .plt .plt.got .text .fini
	- Soient les section headers de .init et .fini (respectivement la première et dernière section du segment ci-dessus) :
	```[Nr] Name              Type            Address          Off    Size   ES Flg Lk Inf Al``` 
	```[11] .init             PROGBITS        0000000000001000 001000 00001b 00  AX  0   0  4```
	```[15] .fini             PROGBITS        000000000000117c 00117c 00000d 00  AX  0   0  4```
- On a : 
	- adresse début du segment = 0x1000 (*p_vaddr*), adresse de fin du segment = 0x1000 + 0x0189 = 0x1189 (*p_vaddr* + *p_memsz*)
	- la section .init a son permier octet à l'adresse 0x1000 (*sh_addr*), ce qui correspond bien à l'adresse de début du segment
	- la section .fini a son premier octet à l'adresse 0x117c (*sh_addr*) et son dernier octet à l'adresse 0x117c + 0x000d = 0x1189 (*sh_addr* + *sh_size*), ce qui correspond bien à l'adresse de fin du segment

Il n'y a pas de segments dans un fichier objet (.o).

Elle est composée des champs suivants : 
- Type (*p_type*) : type du segment. Voici les principaux types : 
	- *PT_NULL* : indique que l'entrée est inutilisée
	- *PT_LOAD* : segment qui peut être chargé en mémoire
	- *PT_DYNAMIC* : segment qui contient des informations pour du dynamic linking
	- *PT_NOTE* : segment qui contient des informations spécifiques définies par le vendeur ou le système
	- *PT_PHDR* : l'entrée donne la position et la taille de la program header table
	- *PT_GNU_STACK* : extension GNU utilisée par le noyau Linux pour assigner des flags à la pile du programme
- Offset (*p_offset*) : indique l'offset depuis le début du fichier ELF pour atteindre le premier octet du segment
- VirtAddr (*p_vaddr*) : indique l'adresse virtuelle à laquelle le premier octet du segment doit être placé en mémoire
- PhysAddr (*p_paddr*) : ignoré pour les systèmes qui respectent l'ABI System V
- FileSiz (*p_filesz*) : taille du segment dans le fichier ELF, ou 0
- MemSiz (*p_memsz*) : taille du segment en mémoire, ou 0. A noter, *p_memsz* n'a pas forcément la même valeur que *p_filesz* (ex : la taille du .bss est ajoutée en mémoire)
- Flg (*p_flags*) : flags associés au segment. Il y a : 
	- E (*PF_X*) : on peut exécuter le segment
	- W (*PF_W*) : on peut écrire dans le segment
	- R (*PF_R*) : on peut lire le segment
- Align (*p_align*) : contient soit une puissance de deux pour indiquer une contrainte d'alignement pour *p_vaddr* (=> *p_vaddr* / *p_align* a le même reste que *p_offset* / *p_align*, idem pour la taille des pages), ou 0 ou 1


### Section header table
Elle est optionnelle. On peut l'afficher avec la commande : ```readelf -S -W prog```.
Chaque entrée de la section header table décrit une section via un section header. 
Les sections sont utilisées par le linker.
On peut afficher le contenu d'une section avec la commande : ```readelf -x [nom_section] prog```. Pour les sections qui contiennent des strings (ex : .shstrtab), il faut utiliser la commande : ```readelf -p .shstrtab prog```
Elle est composée des champs suivants : 
- Nr : indice de l'entrée
- Name (*sh_name*) : nom de la section. En réalité, c'est un indice dans la section header string table (.shstrtab)
- Type (*sh_type*) : type de la section. Voici les principaux types : 
	- *SHT_NULL* : le section header est considéré comme inactif et n'est donc associé à aucune section
	- *SHT_PROGBITS* : section qui contient des informations sur le programme comme .text ou .data
	- *SHT_NOTE* : section qui contient des informations spécifiques définies par le vendeur ou le système
	- *SHT_STRTAB* : section qui contient une string table
	- *SHT_RELA* : relocation section qui contient un champ *r_addend*
	- *SHT_REL* : relocation section qui ne contient pas de champ *r_addend*
	- *SHT_DYNAMIC* : section qui contient des informations pour du dynamic linking
	- *SHT_SYMTAB* : section qui contient une table des symboles (.symtab, utilisée lors de l'édition des liens)
	- *SHT_DYNSYM* : section qui contient une table des symboles (.dynsym, utilisée lors d'un dynamic linking)
	- *SHT_NOBITS* : section qui n'occupe aucune place dans le fichier ELF (ex : .bss) stocké sur le disque
- Address (*sh_addr*) : si la section associée doit apparaître dans la mémoire du processus, ce champ contient l'adresse virtuelle à laquelle le premier octet de la section doit être placée, ou 0 (ex : un fichier objet .o a toutes ses sections avec *sh_addr* à 0)
- Off (*sh_offset*) : indique l'offset depuis le début du fichier ELF pour atteindre le premier octet de la section 
- Size (*sh_size*) : taille de la section. A noter : la valeur de ce champ peut être ignorée pour une section de type *SHT_NOBITS* car elle n'occupe aucune place dans le fichier ELF
- EntSize (ES, *sh_entsize*) : pour les sections qui contiennent une table, donne la taille d'une entrée de cette table, ou 0
- Flg (*sh_flags*) : flags associés à la section. Il y a : 
	- W (*SHF_WRITE*) : la section contient des données dans lesquelles le processus peut écrire pendant son exécution
	- A (*SHF_ALLOC*) : la section occupe de la mémoire lors de l'exécution du processus
	- X (*SHF_EXECINSTR*) : la section contient des instructions exécutables
	- p (*SHF_MASKPROC*) : la signification dépend du processeur
- Lk (*sh_link*) : l'interprétation de ce champ dépend du type de la section. Il contient l'indice d'un section header. Dans le cas d'une table des symboles (*SHT_SYMTAB* ou *SHT_DYNSYM*), ce champ contient l'indice du section header de la string table associée
- Inf (*sh_info*) : l'interprétation de ce champ dépend du type de la section
- Al (*sh_addralign*) : contient soit une puissance de deux pour indiquer une contrainte d'alignement pour *sh*addr* (=> *sh_addr* % *sh_addralign* = 0), ou 0 ou 1

Les principales sections à connaître sont : 
- .bss : BSS signifie Block Started by Symbol, contient dans le cas classique (option ```-fno-common``` de gcc) les variables globales (normales ou statiques) non initialisées et les variables statiques locales non initialisées (seront initalisées à 0 au run time) et celles initialisées à 0.
- .data : contient les variables globales et statiques locales initialisées
- .rodata : contient des variables en lecture seule (ex : chaînes de caractères)
- .strtab et .shstrtab : tableaux de chaînes de caractères séparées par des \0
- .symtab : table des symboles (à ne pas confondre avec la table des symboles du compilateur)
- .text : code compilé du programme
- .init : contient une fonction _init()


#### Table des symboles
C'est une ou plusieurs sections (.symtab et .dynsym). On peut les afficher avec la commande : ```readelf -s prog```
Il existe également les commandes : ```nm -a prog``` et ```nm -D prog``` pour respectivement afficher .symtab et .dynsym
La commande ```strip prog``` supprime la section .symtab de l'exécutable.
La table des symboles ne contient pas les variables locales non-statiques.

Une table des symboles est composée des champs suivants : 
- Num : indice de l'entrée
- Value (*st_value*) : dans le cas d'un fichier exécutable et d'une librairie partagée (voir *e_type*), ce champ contient l'adresse virtuelle du symbole dans l'espace d'adressage du processus
- Size (*st_size*) : taille de l'objet associé au symbole
- Type (sous-partie de *st_info*) : type du symbole dont voici les principaux : 
	- *STT_NOTYPE* : type non spécifié
	- *STT_OBJECT* : symbole associé à un objet (ex : variable)
	- *STT_FUNC* : symbole associé à une fonction
	- *STT_FILE* : le nom (*st_name*) du symbole est un fichier 
- Bind (sous-partie de *st_info*) : portée et comportement du symbole. Il y a : 
	- *STB_LOCAL* : symbole local à son fichier objet (= inaccessible depuis un autre fichier objet). Exemple : variable locale ou globale statique, fonction statique
	- *STB_GLOBAL* : symbole global accessible à tous les fichiers objets compilés et liés ensembles
	- *STB_WEAK* : symbole global qui peut être redéfini. 
	En effet, soient deux fichiers hello1.c et hello2.c qui contiennent tous les deux une définition pour une même fonction add. Si on compile deux fichiers hello1.c et hello2.c (commande : ```gcc -c helloX.c``` => génère un .o) avec la commande ```gcc hello1.o hello2.o -o hello```, on aura une erreur "multiple definition of `add'". Pour éviter cela, on peut déclarer l'une des fonctions add avec l'attribut gcc "weak". Avec cela, le symbole add aura le bind *STB_WEAK* pour ce fichier objet et l'exécutable final (hello ici) contiendra la définition non-weak.
- Vis (*st_other*) : visibilité du symbole (DEFAULT, HIDDEN, PROTECTED ou INTERNAL)
- Ndx (*st_shndx*) : indice dans la section header table du section header associé à ce symbole. Il existe des valeurs spéciales comme : 
	- *SHN_UNDEF* : symbole non défini, la définition est autre-part, ex : printf est dans une librairie partagée 
	- *SHF_ABS* : indice dont la valeur ne sera pas modifiée par une relocation 
	- *SHF_COMMON* : symboles dont l'objet associé est non initialisé et non alloué en mémoire. Cette valeur existe uniquement si l'option ```-fcommon``` de gcc est précisée. Dans ce cas-là, *SHF_COMMON* est à ne pas confondre avec la section .bss :
		- *SHF_COMMON* est réservé aux variables globales non statiques non initialisées 
		- la section .bss est réservée aux variables statiques non initialisées et les variables globales et statiques locales initalisées à 0.
- Name (*st_name*) : nom du symbole. En réalité, c'est un indice dans la section header string table (.strtab). A noter : le section header associé à une table des symboles contient dans son champ *sh_link* l'indice du section header de .strtab

#### String table
C'est une ou plusieurs sections (.strtab et .shstrtab). 
Chaque section contient des strings séparées par des "\0". Les champs qui contiennent un index dans une string table (*e_shstrndx*, *sh_name* et *st_name*) pointent sur le premier caractère de la string voulue.
La section .strtab contient le nom de chaque symbole.
La section .shstrtab contient le nom de chaque section.


#### Relocation
On peut les voir avec la commande ```objdump -r prog```.
