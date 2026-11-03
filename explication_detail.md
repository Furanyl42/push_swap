# Explication ultra détaillée de l’algorithme `--medium`

Ce document est une version beaucoup plus détaillée de `explication.md`.
Il repart de zéro : on commence par expliquer ce qu’est `push_swap`, ce que sont
les piles et les opérations, puis on descend progressivement jusqu’au détail de
chaque étape de l’algorithme `--medium`.

L’objectif est de répondre à trois questions pour chaque partie du code :

1. **Quoi ?** Que fait concrètement cette ligne / cette boucle / cette variable ?
2. **Pourquoi ?** Pourquoi est-elle nécessaire, et pourquoi est-elle écrite ainsi ?
3. **Comment ?** Comment fonctionne-t-elle, avec quels invariants, et comment
   peut-on vérifier qu’elle est correcte ?

Pour l’explication ligne par ligne du code source, voir le document
`code_explique.md`, qui complète celui-ci.

---

## Sommaire

1. Push_swap expliqué depuis zéro
2. Les concepts clés de `medium`
3. Vue d’ensemble de l’algorithme
4. Étape 0 : sélection de la stratégie
5. Étape 1 : normalisation
6. Étape 2 : calcul de la géométrie
7. Étape 3 : distribution de `a` vers `b`
8. Étape 4 : remontée de `b` vers `a`
9. L’heuristique de coût en détail
10. Exemple complet entièrement tracé (`n = 10`)
11. Preuve de correction
12. Complexité
13. Cas limites et pièges
14. Comparaison avec `simple` et `complex`
15. Vérification pratique
16. Résumé final et fiche mémoire

---

## 0. Conventions et carte du code

### 0.1 Conventions

- Une pile est écrite **du sommet vers le fond**.
  Exemple : `a = [3, 1, 2]` signifie que `3` est au sommet, puis `1`, puis `2`
  au fond.
- L’index logique `0` désigne toujours le **sommet**.
- Les intervalles de rangs sont **semi-ouverts** : `[low, high)` contient
  `low`, `low + 1`, …, `high - 1`, mais pas `high`.
- Une « opération » désigne une ligne émise sur la sortie standard :
  `sa`, `pb`, `ra`, etc. Le coût CPU des comparaisons C n’est pas compté comme
  une opération `push_swap`.
- Les rangs vont de `0` à `n - 1`. Le rang `0` est la plus petite valeur, le rang
  `n - 1` la plus grande.

### 0.2 Fichiers impliqués

| Fichier | Rôle dans `medium` |
| --- | --- |
| `main.c` | Point d’entrée, calcule le désordre, appelle le dispatcher. |
| `parsing.c` | Reconnaît `--medium`, `--bench`, etc. |
| `sort_dispatcher.c` | Choisit la stratégie et appelle `sort_medium`. |
| `sort_medium.c` | Cœur de `medium` : normalisation, géométrie, distribution. |
| `complex_presort.c` | Contient `normalize_stack`, utilisée par `medium`. |
| `array_utils.c` | Copie triée par tri par insertion. |
| `sorts_utils.c` | Recherche binaire du rang. |
| `complex_pull.c` | Remontée des bandes, calcul des scores, `complex_pull`. |
| `complex_insert.c` | Rotations combinées puis `pa`. |
| `complex_move.c` | Recherche du successeur, du minimum, alignement final. |
| `utils.c` | `get_cost`, le coût signé d’une rotation. |
| `stack.c` | Accès logique aux tableaux circulaires. |
| `ops_push.c` | `pa`, `pb`. |
| `ops_rotate.c` | `ra`, `rb`, `rr`. |
| `ops_reverse.c` | `rra`, `rrb`, `rrr`. |

`medium` n’appelle **pas** `complex_presort`. Il appelle seulement
`normalize_stack`, qui se trouve dans le même fichier que `complex_presort`.
C’est un point de confusion fréquent.

---

## 1. Push_swap expliqué depuis zéro

### 1.1 Le problème

On reçoit une liste d’entiers. Au départ, tous les entiers sont dans une pile
appelée `a`. La pile `b` est vide.

Le but est de produire une suite d’opérations autorisées qui, appliquée à `a`
et `b`, finit avec :

- `a` triée du plus petit au plus grand, du sommet vers le fond ;
- `b` vide.

La qualité d’une solution se mesure au **nombre d’opérations émises**. Moins il
y a d’opérations, meilleure est la solution.

### 1.2 Les deux piles

Une pile est une structure LIFO (*last in, first out*) : on ne peut agir
directement que sur le sommet.

```text
        sommet
          |
          v
a = [ 3, 1, 4, 2 ]
          ^
          |
         fond
```

Ici, `3` est au sommet. Pour accéder à `2`, il faut d’abord faire quelque chose
des éléments au-dessus : les pousser ailleurs, ou faire tourner la pile.

### 1.3 Les onze opérations

| Opération | Effet |
| --- | --- |
| `sa` | Échange les deux éléments du sommet de `a`. |
| `sb` | Échange les deux éléments du sommet de `b`. |
| `ss` | `sa` et `sb` en même temps. |
| `pa` | Prend le sommet de `b` et le pousse au sommet de `a`. |
| `pb` | Prend le sommet de `a` et le pousse au sommet de `b`. |
| `ra` | Fait tourner `a` vers le haut : le sommet passe au fond. |
| `rb` | Fait tourner `b` vers le haut : le sommet passe au fond. |
| `rr` | `ra` et `rb` en même temps. |
| `rra` | Fait tourner `a` vers le bas : le fond passe au sommet. |
| `rrb` | Fait tourner `b` vers le bas : le fond passe au sommet. |
| `rrr` | `rra` et `rrb` en même temps. |

Exemples :

```text
a = [1, 2, 3, 4]

ra  -> [2, 3, 4, 1]
rra -> [4, 1, 2, 3]

a = [1, 2, 3]   b = [8, 9]

pb  -> a = [2, 3]      b = [1, 8, 9]
pa  -> a = [1, 2, 3]   b = [8, 9]
```

`medium` utilise principalement `pb` en phase 1, puis `pa`, `ra`, `rra`, `rb`,
`rrb`, `rr` et `rrr` en phase 2. Il n’utilise pas `sa`, `sb`, `ss` dans son
propre algorithme.

### 1.4 Représentation interne : tableau circulaire

Le code ne déplace pas réellement tous les éléments à chaque rotation. Il
utilise un **tableau circulaire** avec :

- `tab` : le tableau physique ;
- `size` : nombre d’éléments présents ;
- `capacity` : taille maximale du tableau ;
- `head` : index physique du sommet logique.

L’élément logique d’index `i` est stocké à l’index physique :

```text
(head + i) % capacity
```

C’est exactement ce que fait `get_stack_element` dans `stack.c`.

Exemple avec `capacity = 8`, `head = 3`, `size = 4` :

```text
index logique :   0    1    2    3
index physique :  3    4    5    6
valeurs        : [A]  [B]  [C]  [D]
```

Donc la pile logique est `[A, B, C, D]`.

Si `head` vaut `6` et `size` vaut `4`, les index physiques sont `6, 7, 0, 1` :
le tableau « boucle » de la fin vers le début.

Cette représentation rend les rotations très peu coûteuses :

- `ra` ne déplace qu’un élément et avance `head` ;
- `rra` ne déplace qu’un élément et recule `head` ;
- `pb` et `pa` déplacent un élément et ajustent les deux `head`.

C’est pour cela que les opérations peuvent être implémentées en `O(1)`.

### 1.5 Nombre d’opérations vs temps CPU

Le sujet mesure le **nombre d’opérations push_swap**. Une comparaison
`if (valeur < limite)` ne produit aucune ligne de sortie : elle ne compte pas.

C’est une distinction essentielle pour `medium` :

```text
opérations push_swap : O(n√n)
comparaisons C       : O(n²) pour le parcours de b
                       O(n²√n) si l’on compte aussi les recherches de successeur
```

L’algorithme fait beaucoup de comparaisons internes, mais il émet peu
d’opérations. C’est le nombre d’opérations qui est évalué.

### 1.6 Désordre et stratégie adaptative

Avant de trier, `main.c` calcule le **désordre** de `a` :

```text
désordre = nombre de paires inversées / nombre total de paires
```

Une paire `(i, j)` avec `i < j` est inversée si `a[i] > a[j]`. Le résultat est
entre `0` (déjà trié) et `1` (ordre inverse).

`resolve_strategy` choisit alors :

```text
désordre < 0.2  -> simple
0.2 <= d < 0.5  -> medium
d >= 0.5        -> complex
```

Si `--medium` est fourni explicitement, ce choix adaptatif n’a pas lieu :
`resolve_strategy` renvoie directement `STRAT_MEDIUM`.

Enfin, `main` ne lance le tri que si `disorder > 0.0`. Une pile déjà triée ne
produit donc aucune opération, même avec `--medium`.

---

## 2. Les concepts clés de `medium`

### 2.1 Rang et normalisation

Les valeurs réelles peuvent être quelconques : `-1000000`, `42`, `999999`, etc.
Pour simplifier, `medium` remplace chaque valeur par son **rang**.

Exemple :

```text
valeurs : 42  -7  100  3
triées  : -7  3   42   100
rangs   :  1  0   3    2
```

Autrement dit :

- la plus petite valeur reçoit le rang `0` ;
- la deuxième plus petite reçoit le rang `1` ;
- …
- la plus grande reçoit le rang `n - 1`.

Pourquoi normaliser ?

1. Les frontières des bandes deviennent simples : on compare des rangs à des
   entiers de `0` à `n - 1`.
2. On n’a plus besoin de connaître l’amplitude réelle des valeurs.
3. Les intervalles `[0, s)`, `[s, 2s)`, … sont faciles à manipuler.
4. Les valeurs restent uniques, car le parsing refuse les doublons.

La normalisation ne produit **aucune opération push_swap**. Elle modifie
seulement les valeurs stockées dans `a`.

### 2.2 Intervalle semi-ouvert

Un intervalle `[low, high)` contient les entiers `low, low + 1, …, high - 1`.

Exemple : `[5, 10)` contient `5, 6, 7, 8, 9`, mais pas `10`.

C’est pratique pour partitionner sans chevauchement :

```text
[0, 5)  [5, 10)  [10, 15)  ...
```

Chaque valeur appartient à exactement un intervalle.

### 2.3 Bande

Une **bande** est un intervalle de rangs traité comme un groupe.

Exemple avec `n = 10` et une largeur de bande `s = 5` :

```text
bande 1 : [0, 5)   -> rangs 0, 1, 2, 3, 4
bande 2 : [5, 10)  -> rangs 5, 6, 7, 8, 9
```

Avec `n = 13` et `s = 6` :

```text
bande 1 : [0, 6)   -> rangs 0..5
bande 2 : [6, 12)  -> rangs 6..11
bande 3 : [12, 13) -> rang 12 seulement (bande partielle)
```

La dernière bande peut être plus petite : c’est normal quand `n` n’est pas
divisible par la largeur.

### 2.4 Pile triée au sens circulaire

Une pile est **triée au sens circulaire** si, en partant d’un certain élément et
en descendant vers le fond puis en revenant au sommet, les valeurs sont
croissantes.

Exemple :

```text
a = [5, 6, 7, 8, 9]   -> triée linéairement (le minimum est au sommet)
a = [7, 8, 9, 5, 6]   -> triée circulairement
```

Dans le second cas, si on commence à `5`, on lit `5, 6, 7, 8, 9`. La pile n’est
pas triée « à partir du sommet », mais elle le sera après un alignement final.

`medium` maintient l’invariant suivant pendant toute la phase de remontée :

> `a` est triée au sens circulaire.

À la fin, `complex_align` ramène le minimum au sommet pour obtenir un tri
linéaire.

### 2.5 Coût signé d’une rotation

Pour amener un élément d’index `i` au sommet d’une pile de taille `size`, il y a
deux possibilités :

- tourner vers le haut avec `ra` / `rb` : `i` rotations ;
- tourner vers le bas avec `rra` / `rrb` : `size - i` rotations.

On choisit la direction la plus courte. Le code encode cela avec un **coût
signé** :

```text
si i <= size / 2 : coût = +i      -> ra / rb
sinon            : coût = i - size -> rra / rrb
```

Exemple pour `size = 4` :

| index | coût | opération |
| ---: | ---: | --- |
| 0 | 0 | aucune |
| 1 | +1 | `ra` une fois |
| 2 | +2 | `ra` deux fois |
| 3 | -1 | `rra` une fois |

Pour `size = 5` :

| index | coût | opération |
| ---: | ---: | --- |
| 0 | 0 | aucune |
| 1 | +1 | `ra` |
| 2 | +2 | `ra` |
| 3 | -2 | `rra` |
| 4 | -1 | `rra` |

Quand les deux directions coûtent pareil, le code choisit la direction positive
(`ra` / `rb`), parce que la condition est `i <= size / 2`.

### 2.6 Heuristique gloutonne

`medium` ne calcule pas la solution optimale globale. Il utilise une
**heuristique gloutonne** :

> À chaque insertion, choisir le candidat qui coûte le moins cher maintenant.

Ce choix local n’est pas forcément optimal pour le total, mais il est très
efficace et surtout il borne le coût de chaque insertion grâce au découpage en
bandes.

### 2.7 Analogie avec un tri par seaux

Imagine que tu as des cartes numérotées de `0` à `n - 1`.

1. Tu fais des paquets par tranches : `0..s-1`, `s..2s-1`, etc.
2. Tu empiles les paquets dans `b`, du plus petit au plus grand, donc le plus
   grand paquet se retrouve au sommet.
3. Tu reprends les paquets du plus grand au plus petit.
4. Dans chaque paquet, tu insères les cartes une par une dans `a`, à la bonne
   place, en choisissant à chaque fois la carte la moins coûteuse à atteindre.

C’est exactement le principe de `medium`.

---

## 3. Vue d’ensemble de l’algorithme

### 3.1 Les cinq grandes étapes

Pour `n` valeurs :

1. **Normaliser** : remplacer chaque valeur par son rang `0 … n - 1`.
2. **Calculer la géométrie** :
   - `div = max(2, floor(sqrt(n)) / 4)` ;
   - `chunk_size = n / div` ;
   - `fr = 1`.
3. **Distribuer** `a` vers `b` :
   - pour chaque bande, de la plus petite à la plus grande ;
   - pousser les éléments de la bande dans `b` ;
   - faire tourner les autres dans `a`.
4. **Remonter** `b` vers `a` :
   - pour chaque bande, de la plus grande à la plus petite ;
   - choisir l’élément le moins coûteux à insérer ;
   - l’insérer à sa place dans `a`.
5. **Aligner** : ramener le minimum de `a` au sommet.

### 3.2 Schéma général

```text
Départ :
a = [valeurs mélangées]     b = []

1. Normalisation
a = [rangs 0..n-1 mélangés] b = []

2. Distribution (bandes croissantes)
a = []                      b = [plus grande bande]
                                 ...
                                 [plus petite bande]

3. Remontée (bandes décroissantes)
a = [triée circulairement]  b = []

4. Alignement
a = [0, 1, 2, ..., n-1]     b = []
```

### 3.3 Graphe d’appels

```text
main
└── run
    ├── compute_disorder
    ├── resolve_strategy
    └── sort_stacks
        └── sort_medium
            ├── normalize_stack
            │   ├── build_sorted_copy
            │   │   └── insertion_sort
            │   └── find_rank
            ├── medium_chunks
            │   ├── get_stack_element
            │   ├── pb
            │   └── ra
            └── complex_pull
                ├── pull_range
                │   ├── best_in_range
                │   │   └── candidate_score
                │   │       ├── complex_target_index
                │   │       │   ├── best_greater
                │   │       │   └── complex_min_index
                │   │       └── get_cost
                │   └── complex_insert
                │       ├── complex_target_index
                │       ├── complex_find
                │       ├── rotate_ab
                │       │   ├── rr
                │       │   ├── rrr
                │       │   ├── ra / rra
                │       │   └── rb / rrb
                │       └── pa
                └── complex_align
```

### 3.4 Pseudo-code complet

```text
sort_medium(a, b):
    si taille(a) < 2: retour
    n = taille(a)

    si normalize_stack(a) échoue:
        sort_simple(a, b)
        retour

    div = max(2, floor_sqrt(n) / 4)
    fr = 1
    chunk_size = n / div

    medium_chunks(a, b, div)

    geom = { div, fr, n }
    complex_pull(a, b, geom)
```

---

## 4. Étape 0 : sélection de la stratégie

`parsing.c` reconnaît `--medium` et positionne :

```c
opts->strategy = STRAT_MEDIUM;
```

Si aucune stratégie n’est forcée, `opts->strategy` vaut `STRAT_ADAPTIVE`.
`resolve_strategy` choisit alors `medium` pour un désordre compris entre `0.2`
et `0.5`.

`sort_stacks` appelle ensuite :

```c
sort_medium(a, b);
```

Enfin, `main` ne lance le tri que si `disorder > 0.0`. Donc :

- entrée déjà triée : aucune opération ;
- entrée de taille 0 ou 1 : aucune opération ;
- entrée mélangée : `sort_medium` est appelé.

---

## 5. Étape 1 : normalisation

### 5.1 But

Remplacer les valeurs par leurs rangs, sans changer l’ordre relatif.

Exemple :

```text
valeurs : 42  -7  100  3
rangs   :  2   0    3  1
```

Après normalisation, la pile contient `[2, 0, 3, 1]`. Trier cette pile revient
exactement à trier les valeurs d’origine, puisque les rangs conservent l’ordre.

### 5.2 `build_sorted_copy`

`build_sorted_copy(a)` :

1. alloue un tableau de `a->size` entiers ;
2. copie les éléments dans l’ordre logique avec `get_stack_element(a, i)` ;
3. trie la copie avec `insertion_sort` ;
4. renvoie la copie triée.

Le tri par insertion :

```c
i = 1;
while (i < size)
{
    tmp = array[i];
    j = i - 1;
    while (j >= 0 && array[j] > tmp)
    {
        array[j + 1] = array[j];
        j--;
    }
    array[j + 1] = tmp;
    i++;
}
```

Principe : on considère que `array[0..i-1]` est déjà trié. On prend
`array[i]`, on décale vers la droite tous les éléments plus grands que lui, puis
on l’insère à sa place.

Exemple :

```text
[42, -7, 100, 3]

i=1, tmp=-7  -> [-7, 42, 100, 3]
i=2, tmp=100 -> [-7, 42, 100, 3]
i=3, tmp=3   -> [-7, 3, 42, 100]
```

Coût : `O(n²)` dans le pire cas, `O(1)` mémoire supplémentaire.

### 5.3 `find_rank`

`find_rank(sorted, n, value)` fait une **recherche binaire** dans la copie
triée :

```c
lo = 0;
hi = n - 1;
while (lo <= hi)
{
    mid = (lo + hi) / 2;
    if (sorted[mid] == value)
        return (mid);
    if (sorted[mid] < value)
        lo = mid + 1;
    else
        hi = mid - 1;
}
return (0);
```

Comme la copie est triée et que `value` est présente exactement une fois
(les doublons sont refusés au parsing), la fonction renvoie l’index de `value`
dans la copie triée. Cet index est précisément son rang.

Exemple avec `sorted = [-7, 3, 42, 100]` :

```text
find_rank(sorted, 4, 42)
lo=0, hi=3, mid=1 -> sorted[1]=3 < 42 -> lo=2
lo=2, hi=3, mid=2 -> sorted[2]=42 == 42 -> return 2
```

Coût : `O(log n)` par valeur, donc `O(n log n)` pour toutes les valeurs.

### 5.4 `normalize_stack`

```c
sorted = build_sorted_copy(a);
if (!sorted)
    return (0);

i = 0;
while (i < a->size)
{
    index = (a->head + i) % a->capacity;
    a->tab[index] = find_rank(sorted, a->size, a->tab[index]);
    i++;
}
free(sorted);
return (1);
```

Points importants :

- On parcourt les éléments dans l’ordre **logique**, mais on écrit à l’index
  **physique** `(head + i) % capacity`.
- On remplace chaque valeur par son rang, en place.
- Si l’allocation de la copie échoue, on renvoie `0` et `sort_medium` retombe
  sur `sort_simple`.
- Si l’allocation réussit, la normalisation réussit toujours.

### 5.5 Coût de la normalisation

```text
copie + tri par insertion : O(n²) CPU
recherches binaires       : O(n log n) CPU
opérations push_swap      : 0
mémoire temporaire        : O(n)
```

La normalisation est donc chère en CPU, mais elle n’émet aucune opération.
Elle simplifie énormément la suite.

---

## 6. Étape 2 : calcul de la géométrie

### 6.1 `floor_sqrt`

```c
static int floor_sqrt(int n)
{
    int root;

    root = 0;
    while ((root + 1) <= n / (root + 1))
        root++;
    return (root);
}
```

Cette fonction calcule `floor(sqrt(n))` sans utiliser `sqrt()` ni flottants.

Pourquoi la condition est-elle correcte ?

Pour `a > 0` :

```text
a <= n / a   <=>   a * a <= n
```

La boucle incrémente `root` tant que le carré du candidat suivant est inférieur
ou égal à `n`. Elle s’arrête donc sur le plus grand `root` tel que
`root² <= n`, c’est-à-dire `floor(sqrt(n))`.

Avantage : on utilise une division au lieu d’une multiplication, ce qui évite
tout risque de débordement de `(root + 1) * (root + 1)`.

Exemples :

```text
n = 10  -> root = 3
n = 13  -> root = 3
n = 16  -> root = 4
n = 100 -> root = 10
n = 400 -> root = 20
```

### 6.2 `div`

```c
geom.div = floor_sqrt(geom.n) / 4;
if (geom.div < 2)
    geom.div = 2;
```

`div` est le nombre de « découpes » souhaité. On impose un minimum de `2` pour
éviter une seule bande géante.

Exemples :

| `n` | `floor(sqrt(n))` | `div` |
| ---: | ---: | ---: |
| 2 | 1 | 2 |
| 10 | 3 | 2 |
| 16 | 4 | 2 |
| 100 | 10 | 2 |
| 400 | 20 | 5 |
| 1 000 | 31 | 7 |
| 10 000 | 100 | 25 |

### 6.3 `chunk_size`

```c
chunk_size = n / div;
```

C’est la largeur de bande de base. La division est entière.

Exemples :

| `n` | `div` | `chunk_size` | bandes |
| ---: | ---: | ---: | --- |
| 10 | 2 | 5 | `[0,5)`, `[5,10)` |
| 13 | 2 | 6 | `[0,6)`, `[6,12)`, `[12,13)` |
| 100 | 2 | 50 | `[0,50)`, `[50,100)` |
| 400 | 5 | 80 | `[0,80)`, …, `[320,400)` |
| 1 000 | 7 | 142 | `[0,142)`, …, `[994,1000)` |

### 6.4 Nombre réel de bandes

Soit `s = n / div` (division entière). Le nombre de bandes est :

```text
P = ceil(n / s)
```

Ce nombre vaut `div` ou `div + 1` :

- si `n` est divisible par `div`, `s = n / div` et `P = div` ;
- sinon, la dernière bande est partielle et `P = div + 1`.

Exemples :

```text
n = 10, div = 2, s = 5  -> P = 2
n = 13, div = 2, s = 6  -> P = 3
n = 100, div = 2, s = 50 -> P = 2
n = 101, div = 2, s = 50 -> P = 3
```

C’est pour cette raison que `complex_pull` boucle de `chunk = div` jusqu’à `0` :
il couvre `div + 1` plages, ce qui permet d’inclure la bande partielle du haut
quand elle existe, ou d’avoir une plage vide quand elle n’existe pas.

### 6.5 Pourquoi `sqrt(n) / 4` ?

Le compromis est classique :

- trop peu de bandes → chaque bande est grande → beaucoup de rotations par
  insertion ;
- trop de bandes → beaucoup de passes de distribution → beaucoup d’opérations.

Avec `O(√n)` bandes, chaque bande contient `O(√n)` éléments. Le coût total est
alors `O(n√n)`.

Le facteur `/ 4` est une constante pratique : il réduit le nombre de passes tout
en gardant des bandes assez petites. La classe asymptotique reste `O(n√n)`.

### 6.6 `fr = 1`

`fr` est le **ratio de découpage** utilisé par la stratégie complex. Dans
`complex`, `fr` vaut `3` ou `4` : chaque bande est découpée en une partie basse
et une partie haute, pour raffiner les insertions.

Pour `medium`, on force :

```c
geom.fr = 1;
```

Avec `fr = 1`, le point de découpage `mid` vaut :

```text
mid = base + chunk_size / 1 = base + chunk_size
```

La première plage `[mid, high)` est donc vide, et la seconde `[base, mid)` est
la bande entière. `medium` traite donc **une bande = une plage**.

---

## 7. Étape 3 : distribution de `a` vers `b`

### 7.1 Code

```c
static void medium_chunks(t_stack *a, t_stack *b, int div)
{
    int size;
    int chunk;
    int count;
    int i;

    size = a->size / div;
    if (size < 1)
        size = 1;
    chunk = 1;
    while (a->size > 0)
    {
        count = a->size;
        i = 0;
        while (i < count)
        {
            if (get_stack_element(a, 0) < chunk * size)
                pb(a, b);
            else
                ra(a);
            i++;
        }
        chunk++;
    }
}
```

### 7.2 Rôle des variables

| Variable | Rôle |
| --- | --- |
| `size` | largeur de bande `s = n / div`. |
| `chunk` | numéro de la passe / limite courante. |
| `count` | nombre d’éléments présents au début de la passe. |
| `i` | compteur d’itérations de la passe. |

### 7.3 La limite

À chaque passe, la limite est :

```text
limite = chunk * size
```

Un élément est poussé dans `b` si :

```text
rang < limite
```

Sinon, il reste dans `a` et on le fait tourner avec `ra`.

### 7.4 Pourquoi seulement la borne supérieure ?

Au début de la passe `chunk`, tous les éléments de rangs inférieurs à
`(chunk - 1) * size` ont déjà été retirés lors des passes précédentes.

Donc tout élément encore présent dans `a` a un rang supérieur ou égal à
`(chunk - 1) * size`.

Ainsi :

```text
rang < chunk * size
```

signifie automatiquement :

```text
(chunk - 1) * size <= rang < chunk * size
```

C’est exactement la bande courante. Il est donc inutile de tester la borne
inférieure.

### 7.5 Pourquoi `chunk` commence à `1` ?

Si `chunk` valait `0`, la limite serait `0`, aucun élément ne serait poussé, et
la boucle ne progresserait jamais : `a->size` resterait inchangé, donc la boucle
externe serait infinie.

On commence donc à `1` pour que la première limite soit `size` et que la
première bande `[0, size)` soit traitée.

### 7.6 Le rôle crucial de `count`

`count` est capturé **avant** la passe :

```c
count = a->size;
```

La boucle interne fait exactement `count` itérations, même si des `pb` réduisent
la taille de `a` ou si des `ra` réorganisent la pile.

Pourquoi est-ce correct ?

On peut voir les éléments présents au début de la passe comme une file
circulaire d’éléments « à examiner ». À chaque itération :

- on regarde le sommet ;
- si l’élément est dans la bande, `pb` le retire : il est examiné ;
- sinon, `ra` le déplace au fond : il est examiné et remis dans la file.

Après `count` itérations, chaque élément initial a été examiné exactement une
fois. Sans `count`, on risquerait d’examiner plusieurs fois les mêmes éléments
ou d’en oublier.

### 7.7 Effet de `pb` sur l’ordre de `b`

`pb` pousse l’élément au **sommet** de `b`.

Conséquence :

- la première bande poussée, la plus petite, finit au **fond** de `b` ;
- la bande suivante est poussée au-dessus ;
- la dernière bande, la plus grande, finit au **sommet**.

Après la phase 1 :

```text
sommet de b
  bande la plus grande
  ...
  bande intermédiaire
  bande la plus petite
fond de b
```

Chaque bande est contiguë dans `b`, mais **l’ordre interne d’une bande n’est
pas trié** : il dépend de l’ordre de scan et des rotations.

### 7.8 Exemple avec `n = 13`

Entrée (du sommet vers le fond) :

```text
a = [12, 0, 7, 3, 10, 1, 8, 5, 11, 2, 9, 4, 6]
```

Géométrie :

```text
n = 13
floor(sqrt(13)) = 3
div = max(2, 3/4) = 2
size = 13/2 = 6
P = ceil(13/6) = 3
```

Les bandes sont :

```text
passe 1 : rang < 6   -> [0, 6)
passe 2 : rang < 12  -> [6, 12)
passe 3 : rang < 18  -> [12, 13)
```

Trace de la distribution (le nombre après `ra` ou `pb` indique l’élément
concerné, pas un argument de l’opération) :

| Passe | Opération | `a` après | `b` après |
| ---: | --- | --- | --- |
| 1 | `ra` 12 | `[0,7,3,10,1,8,5,11,2,9,4,6,12]` | `[]` |
| 1 | `pb` 0 | `[7,3,10,1,8,5,11,2,9,4,6,12]` | `[0]` |
| 1 | `ra` 7 | `[3,10,1,8,5,11,2,9,4,6,12,7]` | `[0]` |
| 1 | `pb` 3 | `[10,1,8,5,11,2,9,4,6,12,7]` | `[3,0]` |
| 1 | `ra` 10 | `[1,8,5,11,2,9,4,6,12,7,10]` | `[3,0]` |
| 1 | `pb` 1 | `[8,5,11,2,9,4,6,12,7,10]` | `[1,3,0]` |
| 1 | `ra` 8 | `[5,11,2,9,4,6,12,7,10,8]` | `[1,3,0]` |
| 1 | `pb` 5 | `[11,2,9,4,6,12,7,10,8]` | `[5,1,3,0]` |
| 1 | `ra` 11 | `[2,9,4,6,12,7,10,8,11]` | `[5,1,3,0]` |
| 1 | `pb` 2 | `[9,4,6,12,7,10,8,11]` | `[2,5,1,3,0]` |
| 1 | `ra` 9 | `[4,6,12,7,10,8,11,9]` | `[2,5,1,3,0]` |
| 1 | `pb` 4 | `[6,12,7,10,8,11,9]` | `[4,2,5,1,3,0]` |
| 1 | `ra` 6 | `[12,7,10,8,11,9,6]` | `[4,2,5,1,3,0]` |
| 2 | `ra` 12 | `[7,10,8,11,9,6,12]` | `[4,2,5,1,3,0]` |
| 2 | `pb` 7 | `[10,8,11,9,6,12]` | `[7,4,2,5,1,3,0]` |
| 2 | `pb` 10 | `[8,11,9,6,12]` | `[10,7,4,2,5,1,3,0]` |
| 2 | `pb` 8 | `[11,9,6,12]` | `[8,10,7,4,2,5,1,3,0]` |
| 2 | `pb` 11 | `[9,6,12]` | `[11,8,10,7,4,2,5,1,3,0]` |
| 2 | `pb` 9 | `[6,12]` | `[9,11,8,10,7,4,2,5,1,3,0]` |
| 2 | `pb` 6 | `[12]` | `[6,9,11,8,10,7,4,2,5,1,3,0]` |
| 3 | `pb` 12 | `[]` | `[12,6,9,11,8,10,7,4,2,5,1,3,0]` |

À la fin :

```text
b = [12 | 6, 9, 11, 8, 10, 7 | 4, 2, 5, 1, 3, 0]
```

Les barres verticales séparent les bandes :

```text
[12]                 bande 3 (partielle)
[6, 9, 11, 8, 10, 7] bande 2
[4, 2, 5, 1, 3, 0]   bande 1
```

### 7.9 Coût de la phase 1

- Il y a `n` `pb` au total, puisqu’on vide entièrement `a`.
- Il y a `P = O(√n)` passes.
- Chaque passe fait au plus `a->size <= n` itérations.
- Chaque itération fait un `pb` ou un `ra`.

Donc :

```text
O(n) par passe × O(√n) passes = O(n√n)
```

Les `ra` de chaque passe sont les éléments qui n’appartiennent pas encore à la
bande courante. Ils restent dans `a` et seront réexaminés à la passe suivante.

---

## 8. Étape 4 : remontée de `b` vers `a`

### 8.1 Code de `complex_pull`

```c
void complex_pull(t_stack *a, t_stack *b, t_geom *geom)
{
    int chunk_size;
    int chunk;
    int base;
    int mid;
    t_range range;

    chunk_size = geom->n / geom->div;
    if (chunk_size < 1)
        chunk_size = 1;
    chunk = geom->div;
    while (chunk >= 0)
    {
        base = chunk * chunk_size;
        mid = base + chunk_size / geom->fr;

        range.low = mid;
        range.high = base + chunk_size;
        pull_range(a, b, &range);

        range.low = base;
        range.high = mid;
        pull_range(a, b, &range);

        chunk--;
    }
    complex_align(a);
}
```

### 8.2 `chunk_size`

`chunk_size` est recalculé comme `n / div`, avec un minimum de `1`.

Pour `medium`, `n / div` est toujours au moins `1` (car `n >= 2` et
`div <= n`), mais le garde-fou protège le code partagé avec `complex`.

### 8.3 La boucle `chunk = div … 0`

La boucle couvre `div + 1` valeurs de `chunk` :

```text
chunk = div, div - 1, ..., 1, 0
```

Pour chaque `chunk`, on calcule :

```text
base = chunk * chunk_size
mid = base + chunk_size / fr
```

Avec `fr = 1` :

```text
mid = base + chunk_size
```

Les deux plages traitées sont :

```text
1. [mid, base + chunk_size) = [base + s, base + s) = plage vide
2. [base, mid)              = [base, base + s)     = bande entière
```

La première plage est vide, donc `pull_range` ne fait rien. La seconde traite la
bande `[base, base + s)`.

### 8.4 Correspondance avec la distribution

C’est un point souvent mal compris. Les numéros de `chunk` de la distribution et
ceux de `complex_pull` ne jouent pas le même rôle.

Distribution, passe `p` (à partir de `1`) :

```text
bande p = [ (p - 1) * s, p * s )
```

Pull, `chunk = c` :

```text
plage = [ c * s, (c + 1) * s )
```

Donc la plage du pull pour `chunk = c` correspond à la bande de distribution
`p = c + 1`.

Tableau pour `n = 13`, `div = 2`, `s = 6` :

| Distribution | Bande | Pull `chunk` | Plage |
| ---: | --- | ---: | --- |
| passe 1 | `[0, 6)` | 0 | `[0, 6)` |
| passe 2 | `[6, 12)` | 1 | `[6, 12)` |
| passe 3 | `[12, 13)` | 2 | `[12, 18)` |

Le pull traite donc les bandes dans l’ordre :

```text
chunk 2 : [12, 18)  -> bande 3
chunk 1 : [6, 12)   -> bande 2
chunk 0 : [0, 6)    -> bande 1
```

C’est exactement l’ordre décroissant.

Quand `n` est divisible par `div`, `chunk = div` donne une plage vide :

```text
n = 10, div = 2, s = 5
chunk 2 : [10, 15) -> vide
chunk 1 : [5, 10)  -> bande 2
chunk 0 : [0, 5)   -> bande 1
```

### 8.5 Pourquoi traiter les bandes de la plus grande à la plus petite ?

Deux raisons se combinent.

**Raison 1 : localité dans `b`.**

Après la distribution, `b` a la plus grande bande au sommet et la plus petite au
fond. En traitant les bandes de la plus grande à la plus petite, la bande
courante occupe le dessus de `b`. Les bandes plus petites restent au milieu.

Après quelques rotations `rb` ou `rrb`, les éléments de la bande courante
peuvent être répartis entre les deux extrémités de `b` : un préfixe au sommet et
un suffixe au fond. Les bandes plus petites occupent le bloc central, entre ces
deux parties (l’une des deux parties peut être vide). Ainsi, un élément de la
bande courante est toujours à au plus `s` rotations d’une des deux extrémités.

Si on traitait dans l’ordre inverse, il faudrait aller chercher la plus petite
bande au fond de `b`, derrière toutes les autres bandes, ce qui coûterait
`O(n)` rotations par élément.

**Raison 2 : localité dans `a`.**

Au début du traitement d’une bande `B`, les valeurs déjà présentes dans `a`
proviennent uniquement des bandes supérieures : elles sont donc toutes
strictement supérieures aux valeurs de `B`. Pendant le traitement, `a` contient
en plus les valeurs de `B` déjà insérées.

Les nouvelles valeurs de `B` s’insèrent donc dans une zone bien précise de `a` :
juste avant le bloc des valeurs plus grandes, et à l’intérieur du bloc des
valeurs de `B` déjà insérées. Cette zone est bornée par la taille de `B`, donc
par `O(s) = O(√n)`.

Au passage d’une bande à la suivante, le sommet de `a` est le dernier élément
inséré de la bande précédente, et le successeur de la première valeur de la
nouvelle bande est le minimum de `a`, qui appartient lui aussi à la bande
précédente. La distance entre les deux reste donc bornée par la taille d’une
bande, et non par la taille totale de `a`.

### 8.6 `pull_range`

```c
static void pull_range(t_stack *a, t_stack *b, t_range *range)
{
    int value;

    while (best_in_range(a, b, range, &value))
        complex_insert(a, b, value);
}
```

Tant qu’il existe dans `b` un élément dont le rang appartient à
`[range->low, range->high)`, on en choisit un et on l’insère dans `a`.

`best_in_range` renvoie `1` s’il a trouvé un candidat, `0` sinon.

### 8.7 `best_in_range`

```c
static int best_in_range(t_stack *a, t_stack *b, t_range *range, int *out)
{
    int i;
    int best;
    int score;
    int value;

    i = 0;
    best = 2147483647;
    *out = 0;
    while (i < b->size)
    {
        value = get_stack_element(b, i);
        if (value >= range->low && value < range->high)
        {
            score = candidate_score(a, b, value, i);
            if (score < best)
            {
                best = score;
                *out = value;
            }
        }
        i++;
    }
    return (best != 2147483647);
}
```

Points importants :

- On parcourt **tout `b`**, du sommet vers le fond.
- On ne considère que les valeurs dans `[low, high)`.
- On calcule un score pour chaque candidat.
- On garde le plus petit score.
- En cas d’égalité, on garde le **premier rencontré**, donc le plus proche du
  sommet de `b`.
- `2147483647` est `INT_MAX`, utilisé comme sentinelle « aucun candidat ».

### 8.8 `candidate_score`

```c
static int candidate_score(t_stack *a, t_stack *b, int value, int index)
{
    int cost_a;
    int cost_b;

    cost_a = get_cost(complex_target_index(a, value), a->size);
    cost_b = get_cost(index, b->size);
    return (complex_cost(cost_a, cost_b));
}
```

Le score est le coût combiné de :

- la rotation de `a` pour amener la position d’insertion au sommet ;
- la rotation de `b` pour amener `value` au sommet.

`index` est l’index de `value` dans `b`, déjà connu pendant le parcours.

### 8.9 Terminaison de la phase 2

À chaque appel de `complex_insert`, un élément est retiré de `b` et ajouté à
`a`. Donc `b->size` diminue strictement.

`pull_range` s’arrête quand plus aucun élément de `b` n’est dans la plage.

Chaque rang appartient à exactement une plage du pull :

- les plages `[c*s, (c+1)*s)` pour `c = div … 0` couvrent `[0, (div+1)*s)` ;
- comme `(div+1)*s >= n`, elles couvrent tous les rangs `0 … n-1`.

Donc chaque élément est inséré exactement une fois, et `b` finit vide.

---

## 9. L’heuristique de coût en détail

### 9.1 Trouver la position d’insertion : le successeur

Pour insérer `value` dans une pile `a` triée circulairement, on cherche le plus
petit élément de `a` **strictement supérieur** à `value`. C’est le
**successeur** de `value`.

On insère alors `value` juste avant ce successeur.

Exemple :

```text
a = [5, 7, 9, 2, 3]   (triée circulairement : 2,3,5,7,9)
value = 6
successeur = 7 (index 1)
on insère 6 avant 7
```

Après insertion, l’ordre circulaire est préservé :

```text
2, 3, 5, 6, 7, 9
```

### 9.2 `best_greater`

```c
static int best_greater(t_stack *a, int value)
{
    int idx;
    int best;
    int i;

    idx = -1;
    best = 2147483647;
    i = 0;
    while (i < a->size)
    {
        if (get_stack_element(a, i) > value
            && get_stack_element(a, i) < best)
        {
            best = get_stack_element(a, i);
            idx = i;
        }
        i++;
    }
    return (idx);
}
```

- `best` stocke la plus petite valeur supérieure trouvée.
- `idx` stocke son index.
- Si aucun élément n’est supérieur, `idx` reste `-1`.

### 9.3 `complex_target_index`

```c
int complex_target_index(t_stack *a, int value)
{
    int idx;

    idx = best_greater(a, value);
    if (idx == -1)
        idx = complex_min_index(a);
    return (idx);
}
```

Si `value` est plus grande que tout `a`, il n’y a pas de successeur. Dans une
pile triée circulairement, le successeur est alors le **minimum** de `a` : on
insère `value` juste avant le minimum, c’est-à-dire après le maximum.

Exemple :

```text
a = [2, 4, 6, 8]   (triée circulairement)
value = 9
successeur circulaire = 2 (le minimum)
on insère 9 avant 2
```

Si `a` est vide, `complex_min_index` renvoie `0` et `get_cost(0, 0)` vaut `0`.

### 9.4 `get_cost`

```c
int get_cost(int index, int size)
{
    if (size <= 1)
        return (0);
    if (index <= size / 2)
        return (index);
    return (index - size);
}
```

- `size <= 1` : rien à tourner, coût `0`.
- `index <= size / 2` : l’élément est dans la première moitié, on tourne vers le
  haut, coût positif `+index`.
- Sinon : l’élément est dans la seconde moitié, on tourne vers le bas, coût
  négatif `index - size`.

Le coût absolu est le nombre de rotations nécessaires.

### 9.5 `complex_cost`

```c
int complex_cost(int cost_a, int cost_b)
{
    int abs_a;
    int abs_b;

    abs_a = cost_a;
    if (abs_a < 0)
        abs_a = -abs_a;
    abs_b = cost_b;
    if (abs_b < 0)
        abs_b = -abs_b;
    if ((cost_a > 0 && cost_b > 0) || (cost_a < 0 && cost_b < 0))
    {
        if (abs_a > abs_b)
            return (abs_a);
        return (abs_b);
    }
    return (abs_a + abs_b);
}
```

Deux cas :

**Même signe** (les deux rotations vont dans le même sens) :

- on peut utiliser `rr` (positif) ou `rrr` (négatif) pour tourner les deux piles
  en même temps ;
- le coût est le maximum des deux valeurs absolues.

Exemple :

```text
cost_a = +3, cost_b = +5
-> rr trois fois, puis rb deux fois
-> 5 opérations
```

**Signes opposés ou l’un nul** :

- on ne peut pas combiner les rotations ;
- le coût est la somme des valeurs absolues.

Exemple :

```text
cost_a = +3, cost_b = -2
-> ra trois fois, rrb deux fois
-> 5 opérations
```

### 9.6 `rotate_ab`

```c
static void rotate_ab(t_stack *a, t_stack *b, int cost_a, int cost_b)
{
    while (cost_a > 0 && cost_b > 0)
    {
        rr(a, b);
        cost_a--;
        cost_b--;
    }
    while (cost_a < 0 && cost_b < 0)
    {
        rrr(a, b);
        cost_a++;
        cost_b++;
    }
    complex_rotate_a(a, cost_a);
    complex_rotate_b(b, cost_b);
}
```

Ordre des opérations :

1. tant que les deux coûts sont positifs : `rr` ;
2. tant que les deux coûts sont négatifs : `rrr` ;
3. rotations restantes de `a` avec `ra` ou `rra` ;
4. rotations restantes de `b` avec `rb` ou `rrb`.

Exemples :

```text
cost_a = +3, cost_b = +5
rr, rr, rr, rb, rb

cost_a = -4, cost_b = -2
rrr, rrr, rra, rra

cost_a = +3, cost_b = -2
ra, ra, ra, rrb, rrb

cost_a = 0, cost_b = -4
rrb, rrb, rrb, rrb
```

### 9.7 `complex_insert`

```c
void complex_insert(t_stack *a, t_stack *b, int value)
{
    int cost_a;
    int cost_b;

    cost_a = get_cost(complex_target_index(a, value), a->size);
    cost_b = get_cost(complex_find(b, value), b->size);
    rotate_ab(a, b, cost_a, cost_b);
    pa(a, b);
}
```

Étapes :

1. calculer la position d’insertion dans `a` et son coût ;
2. trouver `value` dans `b` et calculer son coût ;
3. effectuer les rotations ;
4. `pa` pour pousser `value` de `b` vers `a`.

Après les rotations, le successeur de `value` est au sommet de `a`. `pa` place
`value` au sommet, donc **juste avant** le successeur. L’ordre est préservé.

### 9.8 `complex_find`

```c
int complex_find(t_stack *s, int value)
{
    int i;

    i = 0;
    while (i < s->size)
    {
        if (get_stack_element(s, i) == value)
            return (i);
        i++;
    }
    return (0);
}
```

Comme les rangs sont uniques, `value` apparaît au plus une fois. Dans
`complex_insert`, on sait qu’elle est présente, donc la fonction trouve
toujours l’index correct.

### 9.9 `complex_align`

```c
void complex_align(t_stack *a)
{
    int index;
    int cost;

    index = complex_min_index(a);
    cost = get_cost(index, a->size);
    while (cost > 0)
    {
        ra(a);
        cost--;
    }
    while (cost < 0)
    {
        rra(a);
        cost++;
    }
}
```

À la fin de la remontée, `a` est triée circulairement. Le minimum n’est pas
forcément au sommet. `complex_align` le ramène au sommet avec la rotation la
moins chère. La pile est alors triée linéairement :

```text
a = [0, 1, 2, ..., n-1]
```

### 9.10 Pourquoi le glouton fonctionne bien ici

`best_in_range` choisit le candidat de score minimal, mais ce score est recalculé
après chaque insertion. C’est un glouton :

- il ne prévoit pas les insertions futures ;
- il choisit simplement l’insertion la moins chère maintenant.

Grâce au découpage en bandes, ce choix local reste borné :

- `cost_a` est borné par la taille de la bande courante ;
- `cost_b` est borné par la taille de la bande courante ;
- donc chaque insertion coûte `O(s)`.

C’est ce qui rend l’algorithme efficace malgré sa simplicité.

---

## 10. Exemple complet entièrement tracé (`n = 10`)

### 10.1 Données initiales

Entrée :

```text
a = [7, 2, 9, 0, 5, 1, 8, 3, 6, 4]
b = []
```

Les valeurs sont déjà les rangs `0 … 9`. La normalisation ne change donc rien.

Géométrie :

```text
n = 10
floor(sqrt(10)) = 3
div = max(2, 3/4) = 2
size = 10/2 = 5
P = ceil(10/5) = 2
```

Bandes :

```text
bande 1 : [0, 5)  -> rangs 0,1,2,3,4
bande 2 : [5,10)  -> rangs 5,6,7,8,9
```

### 10.2 Distribution

Passe 1 : limite `5`. Dans les tableaux suivants, `ra 7` signifie par exemple
« `ra`, avec `7` qui était au sommet ».

| Étape | Opération | `a` après | `b` après |
| ---: | --- | --- | --- |
| 0 | départ | `[7,2,9,0,5,1,8,3,6,4]` | `[]` |
| 1 | `ra` 7 | `[2,9,0,5,1,8,3,6,4,7]` | `[]` |
| 2 | `pb` 2 | `[9,0,5,1,8,3,6,4,7]` | `[2]` |
| 3 | `ra` 9 | `[0,5,1,8,3,6,4,7,9]` | `[2]` |
| 4 | `pb` 0 | `[5,1,8,3,6,4,7,9]` | `[0,2]` |
| 5 | `ra` 5 | `[1,8,3,6,4,7,9,5]` | `[0,2]` |
| 6 | `pb` 1 | `[8,3,6,4,7,9,5]` | `[1,0,2]` |
| 7 | `ra` 8 | `[3,6,4,7,9,5,8]` | `[1,0,2]` |
| 8 | `pb` 3 | `[6,4,7,9,5,8]` | `[3,1,0,2]` |
| 9 | `ra` 6 | `[4,7,9,5,8,6]` | `[3,1,0,2]` |
| 10 | `pb` 4 | `[7,9,5,8,6]` | `[4,3,1,0,2]` |

Passe 2 : limite `10`, tous les éléments restants sont poussés.

| Étape | Opération | `a` après | `b` après |
| ---: | --- | --- | --- |
| 11 | `pb` 7 | `[9,5,8,6]` | `[7,4,3,1,0,2]` |
| 12 | `pb` 9 | `[5,8,6]` | `[9,7,4,3,1,0,2]` |
| 13 | `pb` 5 | `[8,6]` | `[5,9,7,4,3,1,0,2]` |
| 14 | `pb` 8 | `[6]` | `[8,5,9,7,4,3,1,0,2]` |
| 15 | `pb` 6 | `[]` | `[6,8,5,9,7,4,3,1,0,2]` |

État final de la phase 1 :

```text
a = []
b = [6, 8, 5, 9, 7 | 4, 3, 1, 0, 2]
```

Les barres séparent les bandes :

```text
[6, 8, 5, 9, 7]  bande 2, au sommet
[4, 3, 1, 0, 2]  bande 1, au fond
```

### 10.3 Remontée de la bande `[5, 10)`

`chunk = 1`, plage `[5, 10)`. `a` est vide.

| # | Candidats (valeur, score, cost_a, cost_b) | Choix | Opérations | `a` après |
| ---: | --- | ---: | --- | --- |
| 1 | (6,0,0,0), (8,1,0,1), (5,2,0,2), (9,3,0,3), (7,4,0,4) | 6 | `pa` | `[6]` |
| 2 | (8,0,0,0), (5,1,0,1), (9,2,0,2), (7,3,0,3) | 8 | `pa` | `[8,6]` |
| 3 | (5,1,1,0), (9,1,1,1), (7,2,0,2) | 5 | `ra`, `pa` | `[5,6,8]` |
| 4 | (9,0,0,0), (7,2,-1,1) | 9 | `pa` | `[9,5,6,8]` |
| 5 | (7,1,-1,0) | 7 | `rra`, `pa` | `[7,8,9,5,6]` |

À la fin de cette bande, `a` est triée circulairement :

```text
5, 6, 7, 8, 9
```

mais le sommet est `7`.

### 10.4 Remontée de la bande `[0, 5)`

`chunk = 0`, plage `[0, 5)`.

| # | Candidats (valeur, score, cost_a, cost_b) | Choix | Opérations | `a` après |
| ---: | --- | ---: | --- | --- |
| 1 | (4,2,-2,0), (3,3,-2,1), (1,4,-2,2), (0,2,-2,-2), (2,2,-2,-1) | 4 | `rra`, `rra`, `pa` | `[4,5,6,7,8,9]` |
| 2 | (3,0,0,0), (1,1,0,1), (0,2,0,2), (2,1,0,-1) | 3 | `pa` | `[3,4,5,6,7,8,9]` |
| 3 | (1,0,0,0), (0,1,0,1), (2,1,0,-1) | 1 | `pa` | `[1,3,4,5,6,7,8,9]` |
| 4 | (0,0,0,0), (2,1,1,1) | 0 | `pa` | `[0,1,3,4,5,6,7,8,9]` |
| 5 | (2,2,2,0) | 2 | `ra`, `ra`, `pa` | `[2,3,4,5,6,7,8,9,0,1]` |

### 10.5 Alignement final

`a = [2,3,4,5,6,7,8,9,0,1]`.

Le minimum `0` est à l’index `8` dans une pile de taille `10`.

```text
get_cost(8, 10) = 8 - 10 = -2
```

On fait donc `rra` deux fois :

```text
[2,3,4,5,6,7,8,9,0,1]
-> [1,2,3,4,5,6,7,8,9,0]
-> [0,1,2,3,4,5,6,7,8,9]
```

### 10.6 Bilan des opérations

```text
pb  : 10
pa  : 10
ra  :  8
rra :  5
```

Total : `33` opérations.

Détail :

- `ra` en distribution : 5 ;
- `ra` en remontée : 1 (insertion de 5) + 2 (insertion de 2) = 3 ;
- `rra` en remontée : 1 (insertion de 7) + 2 (insertion de 4) = 3 ;
- `rra` d’alignement : 2.

---

## 11. Preuve de correction

### 11.1 Lemme 1 : la normalisation préserve l’ordre

Si `x < y`, alors `rank(x) < rank(y)`, car `rank` est l’index dans le tableau
trié.

Donc trier les rangs revient à trier les valeurs d’origine. La normalisation ne
change pas le problème.

### 11.2 Lemme 2 : la distribution partitionne les rangs

À la passe `p`, on retire les éléments restants dont le rang est `< p * s`.
Comme les passes précédentes ont retiré tous les rangs `< (p - 1) * s`, on
retire exactement la bande `[(p - 1) * s, p * s)`.

Les bandes sont disjointes et couvrent `[0, P * s)`, donc tous les rangs
`0 … n - 1`.

### 11.3 Lemme 3 : `b` contient les bandes en ordre décroissant

`pb` empile au sommet. La première bande poussée est la plus petite ; elle finit
au fond. La dernière bande poussée est la plus grande ; elle finit au sommet.

Donc, du sommet vers le fond, `b` contient les bandes de la plus grande à la plus
petite.

### 11.4 Lemme 4 : `a` reste triée circulairement

Initialement, `a` est vide, donc l’invariant est vrai.

Supposons `a` triée circulairement et insérons `value`.

`complex_target_index` trouve le successeur de `value` :

- le plus petit élément supérieur à `value`, s’il existe ;
- sinon le minimum de `a`.

On amène ce successeur au sommet, puis `pa` place `value` au sommet.

Donc `value` se retrouve exactement entre son prédécesseur et son successeur
dans l’ordre circulaire. L’ordre est préservé.

### 11.5 Lemme 5 : `complex_align` produit un tri linéaire

Si `a` est triée circulairement, alors en partant du minimum et en descendant,
on lit les valeurs dans l’ordre croissant.

`complex_align` amène le minimum au sommet. Après cela, `a` est triée
linéairement.

### 11.6 Théorème

À la fin de `sort_medium` :

- `b` est vide, car chaque élément de `b` est inséré exactement une fois ;
- `a` contient les `n` rangs `0 … n - 1`, triés linéairement ;
- les valeurs d’origine sont donc triées.

### 11.7 Terminaison

- `medium_chunks` : chaque passe retire au moins un élément ou termine. Comme
  `a->size` diminue strictement à chaque `pb` et que la boucle externe s’arrête
  quand `a->size == 0`, elle termine.
- `complex_pull` : chaque `complex_insert` retire un élément de `b`. Le nombre
  d’insertions est au plus `n`. La boucle sur `chunk` est finie. Donc tout
  termine.

---

## 12. Complexité

### 12.1 Deux complexités différentes

| Mesure | Complexité |
| --- | --- |
| Opérations push_swap émises | `O(n√n)` |
| Parcours de `b` par `best_in_range` | `O(n²)` comparaisons |
| Temps CPU total (avec les scores) | `O(n²√n)` dans le pire cas |
| Mémoire | `O(n)` |

Le sujet évalue les opérations push_swap. Les comparaisons internes ne sont pas
comptées.

### 12.2 Normalisation

```text
tri par insertion de la copie : O(n²)
recherches binaires           : O(n log n)
opérations push_swap          : 0
```

### 12.3 Phase 1

Il y a `P = O(√n)` passes. Chaque passe parcourt au plus `n` éléments. Chaque
itération émet un `pb` ou un `ra`.

```text
O(n) × O(√n) = O(n√n)
```

### 12.4 Phase 2

C’est la partie la plus intéressante.

Pour une insertion, le coût est :

```text
|cost_a| + |cost_b|   (ou max si même signe)
```

Majorons chaque terme.

**Coût dans `a`.**

Pendant le traitement d’une bande `B` de taille au plus `s` :

- `a` contient les bandes supérieures déjà insérées ;
- les valeurs de `B` déjà insérées forment un bloc contigu dans l’ordre
  circulaire ;
- pendant le traitement de `B`, le sommet de `a` est le dernier élément inséré,
  donc il appartient à ce bloc ;
- le successeur de `value` est soit dans ce bloc, soit juste après.

Au tout premier élément d’une bande `B`, `a` ne contient que les bandes
supérieures. Le sommet appartient alors à la bande précédente, et le successeur
de `value` est le minimum de `a`, qui appartient lui aussi à la bande
précédente. La distance reste donc bornée par la taille d’une bande.

Dans tous les cas, la distance circulaire entre le sommet et le successeur est
au plus `s`. Ainsi :

```text
|cost_a| <= s
```

**Coût dans `b`.**

Dans `b`, la bande courante occupe initialement le dessus de la pile. Après des
rotations, ses éléments peuvent être répartis entre un préfixe au sommet et un
suffixe au fond, les bandes plus petites formant le bloc central. Leur nombre
total est au plus `s`.

Donc l’index d’un élément de la bande est soit `< s`, soit `>= b->size - s`.
Dans les deux cas, la rotation la plus courte coûte au plus `s` :

```text
|cost_b| <= s
```

**Total.**

Chaque insertion coûte au plus `2s` rotations, plus un `pa`.

```text
O(s) par insertion × n insertions = O(n * s)
```

Or :

```text
s = n / div ≈ n / (√n / 4) = 4√n
```

Donc :

```text
O(n * s) = O(n√n)
```

### 12.5 Pourquoi le CPU coûte plus que `O(n²)`

Il faut distinguer deux niveaux.

**Niveau 1 : le parcours de `b`.**

Pour chaque insertion, `best_in_range` parcourt tout `b`. Il y a `n` insertions.

```text
n insertions × O(n) comparaisons = O(n²) comparaisons
```

**Niveau 2 : le calcul des scores.**

Pour chaque candidat de la plage courante, `candidate_score` appelle
`complex_target_index`, qui parcourt `a` en `O(n)`.

Combien de candidats sont évalués ? Pas `n` par insertion : seuls les éléments
de la bande courante sont dans la plage, et il y en a au plus `s = O(√n)`.

```text
O(s) candidats par insertion × O(n) pour le successeur × n insertions
= O(n² s)
= O(n²√n)
```

Donc :

```text
parcours de b seul       : O(n²)
CPU total avec les scores: O(n²√n)
opérations push_swap     : O(n√n)
```

Ces calculs ne produisent aucune opération push_swap. C’est du temps CPU, pas du
coût « opérations ».

Le point important est que le nombre de candidats réellement scorés par
insertion est borné par la taille de la bande, pas par la taille de `b`. C’est
encore une conséquence du découpage en bandes.

### 12.6 Mémoire

- `a` et `b` : `O(n)` chacun ;
- copie triée temporaire : `O(n)` ;
- pas de récursion, pas de matrice.

Donc `O(n)` mémoire.

### 12.7 Mesures pratiques

Le README indique environ :

| `n` | `--medium` |
| ---: | ---: |
| 100 | ~566 opérations |
| 500 | ~4374 opérations |
| 1000 | ~10490 opérations |

La borne `O(n√n)` est une borne supérieure théorique. En pratique, les
constantes sont bien plus petites, et `medium` est souvent proche de `complex`
pour les tailles courantes.

---

## 13. Cas limites et pièges

### 13.1 Entrée déjà triée

`main` calcule `disorder == 0` et ne lance pas le tri. Aucune opération n’est
émise, même avec `--medium`.

### 13.2 `n = 0` ou `n = 1`

`main` gère `n = 0` ; `sort_medium` retourne immédiatement si `a->size < 2`.

### 13.3 `n = 2`

```text
div = 2, s = 1
bandes : [0,1), [1,2)
```

La distribution fonctionne, puis le pull insère les deux valeurs. `medium` est
correct mais pas spécialement optimisé pour les très petites tailles.

### 13.4 `n` non divisible par `div`

La dernière bande est partielle. C’est géré par `P = div + 1` et par la plage
supplémentaire `chunk = div` dans `complex_pull`.

### 13.5 Plage vide dans `complex_pull`

Avec `fr = 1`, la première plage de chaque `chunk` est toujours vide. Ce n’est
pas un bug : `pull_range` appelle `best_in_range`, qui ne trouve aucun candidat
et retourne `0`.

### 13.6 Bande courante coupée dans `b`

Après des rotations `rb`, les éléments de la bande courante peuvent être
répartis entre le début et la fin de `b`. C’est pour cela que la preuve de
complexité parle d’éléments « au début ou à la fin », et pas seulement « au
sommet ».

### 13.7 `a` vide au premier `pa`

`complex_min_index` renvoie `0` et `get_cost(0, 0)` renvoie `0`. Le `pa`
fonctionne.

### 13.8 Valeurs négatives

La normalisation les transforme en rangs positifs. Aucun problème.

### 13.9 Doublons

Le parsing les refuse avant d’appeler le tri. `find_rank` suppose des valeurs
uniques.

### 13.10 Échec d’allocation

Si `build_sorted_copy` échoue, `normalize_stack` renvoie `0` et `sort_medium`
retombe sur `sort_simple`. Les valeurs d’origine sont intactes.

### 13.11 `get_cost` et la taille

`get_cost` utilise `size`, pas `size - 1`, parce que l’index va de `0` à
`size - 1` et que la rotation arrière d’un élément d’index `i` coûte
`size - i` rotations. Pour `i = size - 1`, cela donne `-1`, ce qui est correct :
un seul `rra`.

### 13.12 Pourquoi `b` n’est pas trié

La distribution ne trie pas l’intérieur des bandes. Elle ne fait que regrouper
les rangs par intervalle. L’ordre interne dépend du scan et des rotations. Le
tri fin est fait pendant la remontée.

### 13.13 Pourquoi le premier `pull_range` est vide

Parce que `fr = 1` donne `mid = base + chunk_size`, donc `[mid, high)` est
`[base + s, base + s)`, un intervalle vide.

### 13.14 `chunk` de distribution vs `chunk` de remontée

- Distribution : `chunk` est le numéro de passe, et la limite est
  `chunk * size`.
- Remontée : `chunk` est une borne supérieure de bande, et la plage est
  `[chunk * size, (chunk + 1) * size)`.

La remontée utilise un `chunk` de plus (`div`) pour couvrir la bande partielle
du haut.

---

## 14. Comparaison avec `simple` et `complex`

### 14.1 Tableau comparatif

| Critère | `simple` | `medium` | `complex` |
| --- | --- | --- | --- |
| Idée | extraction répétée du minimum | tri par bandes | bandes + insertion gloutonne |
| Normalisation | non | oui | oui |
| Distribution | non | bandes de largeur `n/div` | bandes + découpage `fr` |
| Remontée | `pa` en bloc | gloutonne par bande | gloutonne par sous-bande |
| `div` | — | `max(2, √n/4)` | 2, 3 ou 4 |
| `fr` | — | 1 | 3 ou 4 |
| Opérations | `O(n²)` | `O(n√n)` | `O(n log n)` visé |
| Usage | désordre faible | désordre moyen | désordre élevé |

### 14.2 Ce que change `fr`

Dans `complex`, `fr > 1` découpe chaque bande en deux sous-plages :

```text
[base, base + s/fr)      partie basse
[base + s/fr, base + s)  partie haute
```

Le presort place la partie basse au fond de `b`, et le pull traite la partie
haute puis la partie basse. Cela réduit encore les rotations nécessaires.

Dans `medium`, `fr = 1` :

```text
[base, base + s)         toute la bande
```

Il n’y a pas de sous-découpage. `medium` est donc la version « une bande = une
plage » de la remontée utilisée par `complex`.

### 14.3 Pourquoi `medium` est `O(n√n)` et pas `O(n log n)`

Avec `div ≈ √n/4`, il y a `O(√n)` bandes, donc `O(√n)` passes de distribution.
Chaque insertion coûte `O(√n)`. Le total est `O(n√n)`.

Pour atteindre `O(n log n)`, il faudrait un nombre de bandes constant et un
traitement plus fin, ce que fait `complex` avec `div` constant et `fr > 1`.

---

## 15. Vérification pratique

### 15.1 Compiler

```sh
make
```

### 15.2 Tester avec le checker

```sh
ARG="7 2 9 0 5 1 8 3 6 4"
./push_swap --medium $ARG | ./checker_linux $ARG
```

Résultat attendu :

```text
OK
```

### 15.3 Voir les statistiques

```sh
./push_swap --bench --medium 7 2 9 0 5 1 8 3 6 4 2>/tmp/bench.txt
cat /tmp/bench.txt
```

On peut vérifier que `b` est vide et que `a` est triée.

### 15.4 Vérifier le nombre d’opérations

Toujours avec `--bench`, le champ `total operations` donne le nombre
d’opérations émises. Pour l’exemple `n = 10` de ce document, la trace théorique
donne `33` opérations.

### 15.5 Tester les tailles limites

```sh
./push_swap --medium 2 1
./push_swap --medium 3 2 1
./push_swap --medium $(seq 1 100 | shuf)
./push_swap --medium $(seq 1 500 | shuf)
```

### 15.6 Vérifier le cas déjà trié

```sh
./push_swap --medium 1 2 3 4 5
```

Aucune sortie ne doit être produite.

---

## 16. Résumé final et fiche mémoire

### 16.1 Les formules

```text
div        = max(2, floor(sqrt(n)) / 4)
chunk_size = n / div
P          = ceil(n / chunk_size) = div ou div + 1
fr         = 1
```

### 16.2 Les cinq étapes

```text
1. Normaliser les valeurs en rangs 0..n-1.
2. Découper les rangs en bandes de largeur chunk_size.
3. Pousser les bandes de a vers b, de la plus petite à la plus grande.
4. Réinsérer les bandes de b vers a, de la plus grande à la plus petite.
5. Ramener le minimum au sommet de a.
```

### 16.3 Les invariants

- `a` est triée circulairement pendant toute la remontée.
- `b` contient les bandes restantes dans l’ordre décroissant, du sommet vers le
  fond.
- La bande courante est toujours proche du sommet de `b`.
- Les valeurs de la bande courante s’insèrent dans une zone de `a` de taille
  `O(chunk_size)`.

### 16.4 Les pièges à retenir

- `count = a->size` est capturé avant la passe : c’est essentiel.
- On ne teste que la borne supérieure `rang < chunk * size`.
- `chunk` commence à `1` en distribution.
- `complex_pull` boucle jusqu’à `chunk = 0` inclus.
- Avec `fr = 1`, la première plage du pull est vide.
- `get_cost` renvoie un coût **signé**.
- `complex_cost` prend le **max** si les signes sont identiques, la **somme**
  sinon.
- `pa` insère avant le successeur, pas après.
- `complex_align` est indispensable pour passer du tri circulaire au tri
  linéaire.
- Les comparaisons internes ne sont pas des opérations push_swap.

### 16.5 En une phrase

> `medium` range les valeurs par tranches de rangs, les empile dans `b` dans
> l’ordre décroissant, puis les réinsère une par une dans `a` en choisissant à
> chaque fois la rotation la moins coûteuse.
