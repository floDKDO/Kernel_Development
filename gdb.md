# gdb

## Introduction 
gdb pour GNU Debugger est un débuggeur pour systèmes Unix qui fonctionne sur plusieurs langages comme le C, C++ et Go.

## Commandes
Il faudrait compiler notre programme avec l'option ```-g``` de gcc pour rendre la sortie de gdb plus lisible.
Ces commandes peuvent être abrégées (ex : ```run``` => ```r```).

Liste de commandes : 
- ```info target``` : afficher une liste des sections, l'adresse du point d'entrée et le type du fichier exécutable
- ```maintenance info sections``` : comme la commande précédente mais affiche plus d'informations
- ```info functions``` : afficher une liste des fonctions présentes dans l'exécutable
- ```info variables``` : afficher une liste des variables (globales et statiques) présentes dans l'exécutable
- ```info registers``` : afficher le contenu des registres (commande à utiliser quand le programme a été lancé par gdb)
- ```disassemble [function_name]``` : afficher une liste des instructions assembleurs associées à une fonction
	- ```disassemble /s [function_name]``` : idem mais ajoute en plus la correspondance entre chaque instruction C et ses instructions assembleurs associées
	- ```disassemble /r [function_name]``` : idem mais ajoute en plus une représentation hexadécimale de chaque instruction assembleur
- ```x [address]``` : afficher le contenu d'une adresse mémoire
	- ```x /[num][letter] [address]``` : afficher le contenu contenu dans les adresses [address, address + num], la lettre indique le format de la sortie (binaire, hexadécimal, octal etc.)
- ```print [function_name]``` : afficher l'adresse d'une fonction
- ```kill``` : tuer le programme qui est en train d'être débuggué
- ```quit``` : quitter gdb
- ```run``` : lance le programme 
- ```start``` : lance le programme et ajoute un breakpoint à la première instruction dans main()
- ```break [line_number OR function_name OR '*'address]``` : ajouter un breakpoint à la ligne/fonction/addresse indiquée. On peut préciser un nom de fichier, ex : ```break hello.c:3``` => ajouter un breakpoint à la ligne 3 du fichier hello.c
- ```next``` : passer à la prochaine instruction C. Si une instruction est une fonction, ne pas aller au code de la fonction mais passer à la ligne qui suit cet appel
- ```step``` : passer à la prochaine instruction C. Si une instruction est une fonction, aller au code (= à la première instruction) de la fonction 
- ```nexti``` : idem que ```next``` mais passe à la prochaine instruction assembleur
- ```stepi``` : idem que ```step``` mais passe à la prochaine instruction assembleur
- ```until [line_number]``` : continuer l'exécution jusqu'à atteindre le numéro de ligne indiqué (pratique pour passer une boucle sans avoir à créer de breakpoint après cette boucle)
- ```finish``` : dans une fonction, continuer l'exécution jusqu'à que la fonction retourne (= aller sur l'instruction qui suit l'appel à la fonction) et afficher sa valeur de retour

## Passer des arguments à gdb 
Pour cela, il faut lancer gdb de cette façon : ```gdb --args executable_name arg1 arg2 argn```

## Fonctionnement interne des breakpoints
Lorsqu'on ajoute un breakpoint à une certaine ligne de notre programme avec gdb, ce dernier va modifier l'opcode de l'instruction en le remplaçant par 0xCC (= instruction ```int 3``` avec le numéro 3 qui est l'exception Breakpoint).
Dès que ```int 3``` est exécuté, SIGTRAP est généré et le noyau stoppe le processus de notre programme. A ce moment-là, gdb remplace 0xCC par l'opcode originel de l'instruction.
On peut ajouter la ligne ```asm("int 3");``` dans notre programme pour créer des breakpoints directement depuis le code de notre programme.

