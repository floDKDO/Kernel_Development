# CPU caches

## Introduction
Il y a habituellement trois caches dans un processeur : L1, L2 et L3. Ils sont constitués de SRAM.
Le cache L1 est le plus rapide mais aussi le plus petit, le cache L2 est au milieu et le cache L3 est le moins rapide mais aussi le plus grand.

TODO : cache L1 instruction et data, plusieurs caches L1 et L2 (un par coeur) mais un seul cache L3


## Commandes Linux
La commande ```lscpu``` donne des informations sur le processeur de notre machine, notamment la taille des caches L1, L2 et L3 et leur nombre (ajouter ```| grep cache``` pour uniquement obtenir des informations sur les caches).
Les commandes suivantes donnent la taille de chaque ligne des caches du processeur : 
- ```getconf LEVEL1_DCACHE_LINESIZE``` et ```getconf LEVEL1_ICACHE_LINESIZE```
- ```getconf LEVEL2_CACHE_LINESIZE```
- ```getconf LEVEL3_CACHE_LINESIZE```
=> j'obtiens 64 octets pour toutes les cache lines.


## Explications
Un cache est composé de cache sets qui contiennent une ou plusieurs cache lines.
Chaque cache line est composée des éléments suivants : 
- bit Valid = si ce bit est positionné, la cache line contient des informations (= un cache block) 
- bit Dirty = si ce bit est positionné, le cache block de la cache line a été modifié depuis qu'il a été lu en mémoire => ces modifications n'ont donc pas encore été sauvegardées en mémoire !
- tag = identifiant unique du cache block dans la cache line
- cache block = contenu de la mémoire

Il y a 3 sortes de cache misses (= défauts de cache) : 
- compulsory miss : accès à une donnée pour la première fois => celle-ci n'est donc pas encore stockée dans le cache. Cela arrive toujours quand un cache est vide (= cold cache).
- conflict miss : si plusieurs blocs mémoires correspondent au même emplacement dans le cache, des accès alternés à ces blocs va créer des misses répétés. 
Exemple : soient deux blocs mémoires qui correspondent à l'emplacement X dans le cache. Si un programme accéde, de manière répétée, à ces deux blocs, le cache va, à chaque fois, remplacer l'emplacement X pour stocker le bloc accédé.
- capacity miss : le cache est plus petit que le working set du programme => tous les blocs du working set ne pourront donc pas être tous stockés dans le cache. Il y aura donc des misses.
TODO : dire à quelle sorte de cache chaque miss se rapporte
TODO : write-through, write-back


### Direct-mapped cache
Equivalent à 1-way set associative cache, c'est un cache où il y a exactement une cache line par cache set.

Format des adresses : 
- soit une adresse en mémoire à laquelle un programme veut accéder
- cette adresse est découpée en 3 parties : 
	- tag : doit correspondre au tag de la cache line stockée dans le cache set d'indice *set index*
	- set index : indice du cache set qui devrait stocker le mot demandé
	- block offset : offset (= position du mot demandé) dans le cache block

Fonctionnement :   
Soit un programme qui veut accéder à une donnée (taille = 1 mot) stockée à une certaine adresse en mémoire.
1) Le cache extrait le *set index* de l'adresse pour trouver le cache set associé à cette adresse
2) Comme on a un direct-mapped cache, il n'y a qu'une seule cache line dans chaque cache set. Le cache doit vérifier que le bit Valid est positionné pour cette cache line et que le tag de l'adresse correspond à celui stocké dans la cache line : 
	- si oui : cache hit
	- si non : cache miss
3) Il faut maintenant trouver le mot demandé dans le cache block de la cache line. Pour cela, le cache récupère le *block offset* et l'utilise comme indice dans le cache block : le mot est donc stocké dans l'intervalle [cache_block+block_offset, cache_block+block_offset+taille_mot]. 


### N-way set associative cache
Un N-way set associative cache est un cache où chaque cache set contient N cache lines.
Plus de cache lines par cache sets implique qu'un N-way set associative cache possède moins de cache sets qu'un direct-mapped cache, donc les adresses ont leur champ *set index* sur moins de bits mais ont leur champ *tag* sur plus de bits.
Si un programme veut accéder à une donnée en mémoire, le cache va d'abord trouver le cache set associé avec le champ *set index*, puis la cache line associée (celle dont son champ *tag* est égal au champ *tag* de l'adresse demandée par le programme).


### Fully associative cache 
Ce type de cache possède un unique cache set qui contient donc toutes les cache lines.
Les adresses ne possèdent donc plus de champ *set index* car il n'y a qu'un seul cache set (= indice 0).

TODO : continuer (page 656).



