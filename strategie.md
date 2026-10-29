# Algorithme du Turc : Stratégie & Implémentation

L'algorithme du Turc est un algorithme glouton (greedy) basé sur le **calcul des coûts optimisés**. À chaque tour, il évalue le mouvement le moins coûteux pour transférer un élément d'une pile à l'autre tout en maintenant l'ordre.

---

## 1. Les 4 Grandes Étapes de l'Algorithme

### Phase 1 : Le déversement stratégique (`a` $\rightarrow$ `b`)
* **Amorçage** : Pousser directement les **deux premiers éléments** de `a` vers `b` via `pb` (sans calcul) pour initialiser la pile `b`.
* **Sélection du moins cher** : Pour chaque élément restant dans `a`, calculer le coût de son déplacement vers sa position idéale dans `b`.
* **Transfert** : Sélectionner l'élément ayant le `total_cost` le plus bas, l'amener au sommet en combinant les rotations (`rr` / `rrr`), puis l'envoyer dans `b` (`pb`).
* **Condition d'arrêt** : Répéter l'opération jusqu'à ce qu'il ne reste que **3 éléments** dans `a`.

### Phase 2 : Le tri des 3 derniers (`a`)
* Appliquer la fonction `sort_three(a)` pour trier proprement les 3 derniers éléments restants dans `a` en un maximum de 2 mouvements.

### Phase 3 : Le retour trié (`b` $\rightarrow$ `a`)
* Reprendre chaque élément de `b` un par un, calculer sa position cible idéale dans la pile `a` (qui est désormais triée), évaluer son coût, et le replacer au pixel près dans `a` via `pa`.

### Phase 4 : L'alignement final (`a`)
* Une fois `b` entièrement vidée, faire pivoter la pile `a` (via `ra` ou `rra`) pour placer le **plus petit élément** exactement à l'index `0`.

---

## 2. Le Cœur du Système : Cibles et Coûts

### A. Trouver la cible idéale (Target)
Pour chaque élément, il faut identifier sa place exacte dans la pile de destination :

* **Sens `a` $\rightarrow$ `b` (`b` est triée par ordre décroissant)** :
  * La cible d'un élément de `a` est **le plus grand nombre inférieur à lui** dans `b`.
  * *Cas limite (Edge Case)* : Si l'élément est plus petit que tous les éléments de `b`, sa cible devient **le maximum absolu** de `b` (pour qu'il se place tout en bas, sous le plus petit, préservant ainsi l'ordre décroissant).

* **Sens `b` $\rightarrow$ `a` (`a` est triée par ordre croissant)** :
  * La cible d'un élément de `b` est **le plus petit nombre supérieur à lui** dans `a`.
  * *Cas limite (Edge Case)* : Si l'élément est plus grand que tous les éléments de `a`, sa cible devient **le minimum absolu** de `a` (pour qu'il se place au bon endroit dans la rotation).

### B. Calculer la distance au sommet (`cost_src` et `cost_target`)
Grâce à la structure circulaire de la pile et son index actuel :
* Si l'index est dans la **première moitié** ($\le$ `size / 2`) : Le coût est **positif** (nécessite des rotations vers le haut : `ra` ou `rb`).
* Si l'index est dans la **seconde moitié** ($>$ `size / 2`) : Le coût est **négatif** (nécessite des rotations vers le bas : `rra` ou `rrb`).

### C. La mutualisation des mouvements (`rr` et `rrr`)
Pour économiser des opérations, on combine les mouvements lorsque c'est possible :
1. **Même signe** (tous deux positifs ou tous deux négatifs) : On mutualise. Le coût total combiné est égal au **maximum des deux valeurs absolues** (les mouvements se font en même temps via `rr` ou `rrr`), puis on termine par des rotations simples si l'une des piles a un chemin plus long.
2. **Signes opposés** (l'un monte, l'autre descend) : Pas de mutualisation possible. Le coût total est simplement la **somme des valeurs absolues** des deux coûts.