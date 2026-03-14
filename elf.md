# ELF

## Commandes
- man elf : explique le contenu d'un fichier ELF
- readelf : obtenir des informations sur un fichier ELF 

## gcc
- voir les options activées par défaut : ```gcc -v``` dans la partie "Configured with". Par défaut, mon gcc a notamment ```--enable-default-pie``` => e_type = ET_DYN (Position-Independent Executable file). Si l'on souhaite obtenir un exécutable classique, il faut désactiver PIE avec l'option ```-no-pie```. Finalement, on obtient e_type = ET_EXEC (exécutable)
- ajouter des attributs dans notre code (ex avec weak) : ```__attribute__((weak))```


## Contenu
Un fichier ELF est composé des éléments suivants : 
- ELF header
- Program header table (optionnelle)
- Section header table (optionnelle)
- des sections 

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
- Type (*e_type*) : indique le type du fichier (relocatable file, exécutable, librairie partagée .so, fichier core). 
- Machine (*e_machine*) : architecture de la machine (ex : AMD x86-64)
- Version (*e_version*) : doit valoir 1
- Entry point address (*e_entry*) : adresse virtuelle du code qui doit être exécuté en premier (main() en C) ou 0
- Start of program headers (*e_phoff*) : indique l'offset depuis le début du fichier ELF pour atteindre la program header table, ou 0
- Start of section headers (*e_shoff*) : indique l'offset depuis le début du fichier ELF pour atteindre la section header table, ou 0
- Flags (*e_flags*) : vaut 0 habituellement
- Size of this header (*e_ehsize*) : taille du ELF Header
- Size of program headers (*e_phentsize*) : taille d'une entrée dans la program header table
- Number of program headers (*e_phnum*) : contient le nombre d'entrées dans la program header table
- Size of section headers (*e_shentsize*) : taille d'une entrée dans la section header table
- Number of section headers (*e_shnum*) : contient le nombre d'entrées dans la section header table
- Section header string table index (*e_shstrndx*) : contient l'indice dans la section header table de l'entrée qui pointe sur la section contenant les noms des sections (.shstrtab)

### Program header table
Elle est optionnelle.
Elle est composée des champs suivants : 
- 


### Section header table
Elle est optionnelle. On peut l'afficher avec la commande : ```readelf -S -W prog```.
Chaque entrée de la section header table décrit une section via un section header. 
Les sections sont utilisées par le linker.
On peut afficher le contenu d'une section avec la commande : ```readelf -x [nom_section] prog```. Pour les sections qui contiennent des strings (ex : .shstrtab), il faut utiliser la commande : ```readelf -r .shstrtab prog```
Elle est composée des champs suivants : 
- Nr : indice de l'entrée
- Name (*sh_name*) : nom de la section. En réalité, c'est un indice dans la section header string table (.shstrtab)
- Type (*sh_type*) : type de la section. Voici les principaux types : 
	- *SHT_NULL* = le section header est considéré comme inactif et n'est donc associé à aucune section
	- *SHT_PROGBITS* = section qui contient des informations sur le programme comme .text
	- *SHT_NOTE* = section qui contient des informations spécifiques définies par le vendeur ou le système
	- *SHT_STRTAB* = section qui contient une string table
	- *SHT_RELA* = relocation section
	- *SHT_DYNAMIC* = section qui contient des informations pour du dynamic linking
	- *SHT_SYMTAB* = section qui contient une table des symboles (utilisée lors de l'édition des liens)
	- *SHT_DYNSYM* = section qui contient une table des symboles (utilisée lors d'un dynamic linking)
	- *SHT_NOBITS* = section qui n'occupe aucune place dans le fichier ELF (ex : .bss)
- Address (*sh_addr*) : si la section associée doit apparaître dans la mémoire du processus, ce champ contient l'adresse virtuelle à laquelle le premier octet de la section doit être placée, ou 0
- Off (*sh_offset*) : indique l'offset depuis le début du fichier ELF pour atteindre le premier octet de la section 
- Size (*sh_size*) : taille de la section
- EntSize (ES, *sh_entsize*) : pour les sections qui contiennent une table, donne la taille d'une entrée de cette table, ou 0
- Flg (*sh_flags*) : flags associés à la section. Il y a : 
	- W (*SHF_WRITE*) : la section contient des données dans lesquelles le processus peut écrire pendant son exécution
	- A (*SHF_ALLOC*) : la section occupe de la mémoire lors de l'exécution du processus
	- X (*SHF_EXECINSTR*) : la section contient des instructions exécutables
	- p (*SHF_MASKPROC*) : la signification dépend du processeur
- Lk (*sh_link*) : l'interprétation de ce champ dépend du type de la section. Il contient l'indice d'un section header
- Inf (*sh_info*) : l'interprétation de ce champ dépend du type de la section
- Al (*sh_addralign*) : contient soit une puissance de deux pour indiquer une contrainte d'alignement pour *sh*addr* (=> *sh_addr* % *sh_addralign* = 0), ou 0 ou 1

#### Table des symboles
C'est une ou plusieurs sections (.symtab et .dynsym). On peut les afficher avec la commande : ```readelf -s prog```
Une table des symboles est composée des champs suivants : 
- Num : indice de l'entrée
- Value (*st_value*) : dans le cas d'un fichier exécutable et d'une librairie partagée (voir *e_type*), ce champ contient l'adresse virtuelle où est situé le symbole 
- Size (*st_size*) : taille de l'objet associé au symbole
- Type (sous-partie de *st_info*) : type du symbole dont voici les principaux : 
	- *STT_NOTYPE* : type non spécifié
	- *STT_OBJECT* : symbole associé à un objet (ex : variable)
	- *STT_FUNC* : symbole associé à une fonction
	- *STT_FILE* : le nom (*st_name*) du symbole est un fichier 
- Bind (sous-partie de *st_info*) : portée et comportement du symbole. Il y a : 
	- *STB_LOCAL* : symbole local à son fichier objet (= inaccessible depuis un autre fichier objet). Exemple : variable locale à une fonction ou variable globale statique, fonction statique
	- *STB_GLOBAL* : symbole global accessible à tous les fichiers objets compilés et liés ensembles
	- *STB_WEAK* : symbole global qui peut être redéfini. 
	En effet, soient deux fichiers hello1.c et hello2.c qui contiennent tous les deux une définition pour une même fonction add. Si on compile deux fichiers hello1.c et hello2.c (commande : ```gcc -c helloX.c``` => génère un .o) avec la commande ```gcc hello1.o hello2.o -o hello```, on aura une erreur "multiple definition of `add'". Pour éviter cela, on peut déclarer l'une des fonctions add avec l'attribut gcc "weak". Avec cela, le symbole add aura le bind *STB_WEAK* pour ce fichier objet et l'exécutable final (hello ici) contiendra la définition non-weak.
- Vis (*st_other*) : visibilité du symbole (DEFAULT, HIDDEN, PROTECTED ou INTERNAL)
- Ndx (*st_shndx*) : indice dans la section header table du section header associé à ce symbole. Il existe des valeurs spéciales comme *SHN_UNDEF* (symbole non défini, la définition est autre-part, ex : printf est dans une librairie partagée) et *SHF_ABS* (indice dont la valeur ne sera pas modifiée par une relocation)
- Name (*st_name*) : nom du symbole. En réalité, c'est un indice dans la section header string table (.strtab)

#### String table
C'est une ou plusieurs sections (.strtab et .shstrtab). 
Chaque section contient des strings séparées par des "\0". Les champs qui contiennent un index dans une string table (*e_shstrndx*, *sh_name* et *st_name*) pointent sur le premier caractère de la string voulue.
La section .strtab contient le nom de chaque symbole.
La section .shstrtab contient le nom de chaque section.








