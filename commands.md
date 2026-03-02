# Commands

## objdump
Obtenir des informations sur un fichier objet (format => ELF 64 ou 32 bits, contenu de chaque section)

Options utiles : 
- ```-d``` : désassembler le fichier objet (uniquement les sections qui contiennent des instructions et non des données)
- ```-D``` : désassembler le fichier objet (toutes les sections)
- ```-M i386,att``` : afficher le désassemblage en utilisant le format 32 bits et en utilisant la syntaxe AT&T
- ```-S``` : afficher le code source en plus du code désassemblé

Attention, si on désassemble des sections qui ne contiennent pas d'instructions, les instructions du code désassemblé n'auront aucun sens (les données ne sont pas des instructions !).


## objcopy
Copier et convertir un fichier objet

## hexdump
Afficher le contenu d'un fichier dans le format souhaité (ex : hexadécimal)
Info : ```hd``` est un alias de ```hexdump -C```

## od
Afficher le contenu d'un fichier dans le format souhaité (octal par défaut)

## xdd
Afficher le contenu d'un fichier vers le format hexadécimal et inversement

## ldd
Afficher les dépendances d'un programme à des biliothèques partagées 







