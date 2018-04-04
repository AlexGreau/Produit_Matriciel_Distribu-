# Produit Matriciel Distribué

Le but de ce projet est d'implémenter un produit matriciel de la forme C=A.B où A et B sont deux matrices carrées de taille NxN. Le calcul utilisera P machines grâce à MPI. 

Le projet est à déposer sur J@lon. Les contraintes suivantes sont à respecter :
  - Un fichier nom.c où nom doit être remplacé par votre nom et un fichier nom.txt qui indique quelles fonctionnalités vous avez implémenté.
  -  Un fichier nom.txt qui décrira brièvement l'implémentation (structures, linéarisation des matrices, communications...) ainsi que la liste des fonctionnalités implémentées (voir à la fin). 
  -  Ce programme devra utiliser MPI pour le calcul distribué (circulation sur anneau), et openMP pour la parallélisation des calculs
  - Il devra prendre comme paramètre deux fichiers bien formés au format texte contenant la matrice A pour le premier, et la matrice B pour le second (valeurs séparées par des espaces, une ligne du fichier par ligne de matrice). Il affichera la matrice résultante
  - Chaque valeur dans la matrice de sortie sera séparé du suivant par un unique espace, pas d'alignement de données n'est demandé
  - Le processus seront organisés en anneau et le programme doit pouvoir traiter le cas où la matrice a plus de lignes/colonnes qu'il n'y a de machines disponibles 
   

La note sera fonction des fonctionnalités implémentées dans la liste ci-dessous. Les points sont indiqués à titre indicatif :
  - Calcul du produit matriciel avec N multiple de P (15/20)
  - Gestion des matrices très grandes (+2 points)
  - Gestion du déséquilibre dans le calcul, i.e N non multiple de P (+3 points)
  
A compiler utilisant :
  - gcc -Wall -std=c99 matXmat.c -o mXm -lm
  - mpicc matXmat.c -o mXm par la suite
  - mpirun --oversubscribe -np 5 mXm
