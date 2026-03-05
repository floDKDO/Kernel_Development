
# x86 assembly

## Introduction

Une opérande peut être de 3 types : 
- *immédiate* : valeur constante
- *registre* : numéro de registre
- *mémoire* : adresse mémoire qui suit la syntaxe ```%segment:displacement(base, index, scale)```
	
Adresse en mémoire : ```(base + scale * index) + displacement```
	
Chaque registre est identifié par un numéro unique. Exemple : EAX = numéro 0, EBX = numéro 3, ECX = numéro 1 etc.


## Anatomie d'une instruction assembleur convertie en binaire

![image](./Images/x86_instruction_anatomy.png)

Une instruction x86 en binaire est composée de 6 champs : 
- *Instruction prefixes* : champ optionnel mais certaines instructions nécessitent obligatoirement un préfixe. On peut placer un ou plusieurs préfixes au début d'une instruction pour indiquer des actions supplémentaires à réaliser. 
Il y a 4 groupes de préfixes : 
	- *groupe 1* : ```LOCK``` et ```REP``` 
	- *groupe 2* : utiliser un segment spécifique 
	- *groupe 3* : spécifier une taille 16 bits ou 32 bits pour les opérandes 
	- *groupe 4* : spécifier une taille 16 bits ou 32 bits pour les adresses 
- *Opcode* : identifiant unique d'une instruction assembleur. La longueur de ce champ peut être de 1, 2 ou 3 octets, et 3 bits supplémentaires peuvent être utilisés dans le champ ModR/M pour représenter l'opcode 
- *ModR/M* : ce champ, sur 1 octet, contient 3 sous-champs : 
	- *mod* : sur 2 bits, sa valeur indique ce que contient *r/m*. On a : 
		- soit un registre (valeur = 11)
		- soit un mode d'adressage (valeur = 00, 01 ou 10)
	- *reg/opcode* : sur 3 bits, sa valeur peut indiquer : 
		- soit un numéro de registre (le registre est l'opérande source ou destination => dépend de l'instruction) 
		- soit pour ajouter 3 bits supplémentaires à l'opcode (permet de distinguer des instructions qui auraient le même opcode)
	- *r/m* : sur 3 bits, la signification de sa valeur dépend de la valeur de *mod* : 
		- soit un registre (le registre est l'opérande source ou destination => dépend de l'instruction)
		- soit un mode d'adressage
- *SIB* (Scale-Index-Base) : utilisé si les adresses sont de taille 32 bits et que le champ ModR/M possède une certaine valeur. 
Ce champ, sur 1 octet, contient 3 sous-champs : 
	- *scale* : sur 2 bits, indique le facteur utilisé pour obtenir un facteur (facteur = $2^{scale}$)
	- *index* : sur 3 bits, indique le numéro de registre utilisé comme index register
	- *base* : sur 3 bits, indique le numéro de registre utilisé comme base register
- *Displacement* : sur 1, 2 ou 4 octets, sa présence dépend de la valeur de ModR/M et de SIB, contient un offset qui est ajouté à l'adresse en mémoire calculée avec *base*, *index* et *scale*
- *Immediate* : sur 1, 2 ou 4 octets, sa présence dépend de l'instruction, contient l'éventuelle opérande immédiate utilisée par l'instruction


### Quelques exemples 

Utilisation de *nasm* pour assembler les instructions et écrire le code machine dans un fichier binaire (commande : ```nasm -f bin -o test test.asm```). Attention, *nasm* écrit par défaut des instructions sur 16 bits quand on écrit dans un fichier binaire !
 
- ```jmp [0x1234]``` 
On a en binaire : ```ff 26 34 12```
	- ```ff``` est un des opcodes possibles pour ```jmp``` (voir page 622 du volume 2 du manuel Intel)
	- ```26``` = 00100110b est le champ ModR/M. Voici la valeur de chaque sous-champ (taille d'adresse = 16 bits) : 
		- *mod* = 00b => mode d'adressage 
		- *reg/opcode* = 100b => cette valeur n'a pas de signification car cette instruction n'a qu'une seule opérande
		- *r/m* = 110b, opérande = *disp16* = un champ Displacement sur 16 bits suit le champ ModR/M
	- ```3412``` : champ Displacement sur 16 bits, c'est 0x1234 en little-endian
	
- ```add eax, ecx``` 
On a en binaire : ```66 01 c8```
	- ```66``` : préfixe du groupe 3 (= operand-size override prefix) => utiliser la taille d'opérande qui n'est pas par défaut (utiliser des opérandes sur 32 bits car l'instruction est sur 16 bits)
	- ```01``` : est un des opcodes possibles pour ```add``` (voir page 132 du volume 2 du manuel Intel)
	- ```c8``` = 11001000b est le champ ModR/M. Voici la valeur de chaque sous-champ (taille d'adresse = 16 bits) : 
		- *mod* = 11b => registre
		- *reg/opcode* = 001b, opérande = registre ECX car 1 est le numéro associé à ce registre
		- *r/m* = 000b, opérande = registre EAX car 0 est le numéro associé à ce registre
	
- ```jmp [eax*2 + ebx]``` 
On a en binaire : ```67 ff 24 43```
	- ```67``` : préfixe du groupe 4 (= address-size override prefix) => utiliser une taille d'adresse qui n'est pas par défaut (utiliser des adresses sur 32 bits car l'instruction est sur 16 bits)
	- ```ff``` est un des opcodes possibles pour ```jmp``` (voir page 622 du volume 2 du manuel Intel)
	- ```24``` = 00100100b est le champ ModR/M. Voici la valeur de chaque sous-champ (taille d'adresse = 32 bits du fait du préfixe) : 
		- *mod* = 00b => mode d'adressage 
		- *reg/opcode* = 100b => cette valeur n'a pas de signification car cette instruction n'a qu'une seule opérande
		- *r/m* = 100b => un champ SIB suit
		=> la valeur des 3 champs indiquent qu'il y a un champ SIB qui suit
	- ```43``` = 01000011b = SIB. Voici la valeur de chaque sous-champ : 
		- *scale* = 01b => facteur = 2
		- *index* = 000b => registre EAX car 0 est le numéro associé à ce registre
		- *base* = 011b registre EBX car 3 est le numéro associé à ce registre
		=> on obtient bien : eax * 2 + ebx

- ```jmp [eax * 4 + 0x1234]```
On a en binaire : ```67 ff 24 85 34 12 00 00```
	- ```67``` : préfixe du groupe 4 (= address-size override prefix) => utiliser une taille d'adresse qui n'est pas par défaut (utiliser des adresses sur 32 bits car l'instruction est sur 16 bits)
	- ```ff``` est un des opcodes possibles pour ```jmp``` (voir page 622 du volume 2 du manuel Intel)
	- ```24``` = 00100100b est le champ ModR/M. Voici la valeur de chaque sous-champ (taille d'adresse = 32 bits du fait du préfixe) : 
		- *mod* = 00b => mode d'adressage  
		- *reg/opcode* = 100b => cette valeur n'a pas de signification car cette instruction n'a qu'une seule opérande
		- *r/m* = 100b => un champ SIB suit
		=> la valeur des 3 champs indiquent qu'il y a un champ SIB qui suit
	- ```85``` = 10000101b = SIB. Voici la valeur de chaque sous-champ : 
		- *scale* = 10b => facteur = 4
		- *index* = 000b => registre EAX car 0 est le numéro associé à ce registre
		- *base* = 101b : comme *mod* = 00b => *disp32* = un champ Displacement sur 32 bits suit le champ SIB (il n'y a pas de registre mais une adresse comme base)
	- ```34 12 00 00``` : champ Displacement, c'est 0x1234 en little-endian (sur 32 bits car le préfixe 0x67 indique l'utilisation d'adresses sur 32 bits)
	
	
	

