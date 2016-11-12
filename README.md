# Projet RTOS 1

J'ai déjà créer quelques classes qui seront utiles.
- class **Task** qui représente une constrained deadline asynchronous task.
- class **SystemTask** qui représente un système de tâche. Quelques fonctions ont été implémentées pour vous faciliter le boulot.
Vous pouvez y retrouver par exemple :
  - **U()** qui retourne le facteur d'utilisation du CPU pour ce système.
  - **P()** qui retourne l'hyper-period. (Le fameux P que vous voyez dans les bornes)
  - **Omax()** qui retourne le plus grand offset
  - **feasibleInterval()** qui retourne l'intervalle de faisabilité du système
  - etc.
  
Ensuite, vous trouverez le fichier main.cpp dans lequel a déjà été implémenté le module taskGenerator.
Il reste encore à faire simDM et studyDM. 
Vous devrez les implémenter dans les fonction int studyM(int, char *[]) et int simDM(int, char *[]) (il ne faut pas toucher au main, je vou expliquerai pourquoi plus bas)


Pour compiler votre travail, il suffit juste de taper "make" dans le dosier courant en ligne de commande. C'est dans ce make que je définis la génération des tous les modules. Et j'active certains flags qui font en sorte que le main() redirige vers le bon module.
