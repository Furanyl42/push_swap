# Explication détaillée de l’algorithme `--complex`

Ce document explique la stratégie `--complex` de `push_swap`, aussi appelée
**chunked presort + cost-guided insertion** et souvent désignée dans la
communauté 42 sous le nom de **Turk-style**.

Il complète `explication_detail.md` (qui traite `--medium`) et
`code_complex.md` (qui explique le code ligne par ligne).

L’objectif est de répondre, pour chaque étape, à trois questions :

1. **Quoi ?** Que fait l’algorithme concrètement ?
2. **Pourquoi ?** Pourquoi ce découpage, ce `rb`, ce `fr`, ce glouton ?
3. **Comment ?** Comment prouver que ça termine, que c’est correct, et quelle
   complexité en déduire ?

---

## Sommaire

1. Rappels et place de `complex`
2. Concepts spécifiques à `complex`
3. Vue d’ensemble
4. Sélection de la stratégie
5. Normalisation
6. Géométrie : `div`, `fr`, `chunk_size`
7. Phase 1 : presort avec découpage haut/bas
8. Structure de `b` après le presort
9. Phase 2 : pull avec sous-plages
10. L’heuristique de coût
11. Exemple complet `n = 10`
12. Exemple de bande partielle `n = 13`
13. Preuve de correction
14. Complexité
15. Cas limites et pièges
16. Comparaison avec `medium` et `simple`
17. Vérification pratique
18. Résumé et fiche mémoire

---

## 0. Conventions et carte du code

### 0.1 Conventions

- Une pile est écrite **du sommet vers le fond** :
  `a = [3, 1, 2]` signifie `3` au sommet, `2` au fond.
- L’index logique `0` est le sommet.
- Les intervalles de rangs sont semi-ouverts : `[low, high)`.
- Une « opération » est une ligne émise sur `stdout` : `pb`, `ra`, etc.
- Une comparaison C n’est pas une opération `push_swap`.
- Les rangs vont de `0` à `n - 1`.

### 0.2 Fichiers utilisés par `complex`

| Fichier | Rôle |
| --- | --- |
| `main.c` | Calcule le désordre, lance le tri si nécessaire. |
| `parsing.c` | Reconnaît `--complex` et `--bench`. |
| `sort_dispatcher.c` | Choisit la stratégie et appelle `sort_complex`. |
| `sort_complex.c` | Géométrie fixe (`div`, `fr`) et point d’entrée. |
| `complex_presort.c` | `normalize_stack`, `presort_one`, `complex_presort`. |
| `array_utils.c` | Copie triée par tri par insertion. |
| `sorts_utils.c` | Recherche binaire du rang. |
| `complex_pull.c` | Pull, plages haut/bas, score, `complex_pull`. |
| `complex_insert.c` | Rotations combinées et `pa`. |
| `complex_move.c` | Successeur, minimum, alignement final. |
| `utils.c` | `get_cost`, coût signé. |
| `stack.c` | Accès logique au tableau circulaire. |
| `ops_push.c` | `pa`, `pb`. |
| `ops_rotate.c` | `ra`, `rb`, `rr`. |
| `ops_reverse.c` | `rra`, `rrb`, `rrr`. |

`complex` n’utilise pas `sa`, `sb`, `ss`.

### 0.3 Graphe d’appels

```text
main
└── run
    ├── compute_disorder
    ├── resolve_strategy
    └── sort_stacks
        └── sort_complex
            ├── normalize_stack
            │   ├── build_sorted_copy
            │   │   └── insertion_sort
            │   └── find_rank
            ├── complex_geometry
            ├── complex_presort
            │   └── presort_one
            │       ├── pb
            │       └── rb
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

---

## 1. Rappels et place de `complex`

### 1.1 Le problème `push_swap`

On a deux piles `a` et `b`. Au départ, `a` contient les valeurs et `b` est vide.
Il faut produire une suite d’opérations autorisées qui trie `a` du plus petit au
plus grand et vide `b`. Le coût mesuré est le nombre d’opérations émises.

Les opérations utiles à `complex` sont :

- `pb`, `pa` : push entre les piles ;
- `ra`, `rb`, `rr` : rotations vers le haut ;
- `rra`, `rrb`, `rrr` : rotations vers le bas.

### 1.2 Les trois stratégies

| Stratégie | Idée | Classe visée |
| --- | --- | --- |
| `simple` | extraction répétée du minimum | `O(n²)` |
| `medium` | bandes de largeur `n/div`, `div ≈ √n/4` | `O(n√n)` |
| `complex` | presort en bandes constantes + insertion gloutonne | `O(n log n)` visé |

`complex` est la stratégie la plus élaborée. Elle vise à la fois :

- peu de passes de distribution, grâce à un nombre **constant** de bandes ;
- peu de rotations pendant la remontée, grâce au découpage **haut/bas** de
  chaque bande et à l’heuristique de coût.

### 1.3 Sélection

Avec `--complex`, `parsing.c` positionne :

```c
opts->strategy = STRAT_COMPLEX;
```

En mode adaptatif, `resolve_strategy` choisit `complex` quand le désordre est
supérieur ou égal à `0.5`.

`main` ne lance le tri que si `disorder > 0.0`. Une pile déjà triée ne produit
donc aucune opération.

---

## 2. Concepts spécifiques à `complex`

### 2.1 Rang

Comme `medium`, `complex` normalise d’abord les valeurs en rangs `0 … n - 1`.

Exemple :

```text
valeurs : 42  -7  100  3
rangs   :  2   0    3  1
```

Trier les rangs revient à trier les valeurs.

### 2.2 Bande

Une bande est un intervalle de rangs de largeur `chunk_size`.

Avec `n = 10`, `div = 2`, `chunk_size = 5` :

```text
bande 0 : [0, 5)
bande 1 : [5, 10)
```

Avec `n = 13`, `div = 2`, `chunk_size = 6` :

```text
bande 0 : [0, 6)
bande 1 : [6, 12)
bande 2 : [12, 13)   bande partielle
```

### 2.3 Sous-bande haute et sous-bande basse

C’est la nouveauté de `complex` par rapport à `medium`.

Chaque bande est coupée en deux par le ratio `fr` :

```text
bande      : [base, base + s)
point coup : mid = base + s / fr

partie basse : [base, mid)       largeur ≈ s / fr
partie haute : [mid, base + s)   largeur ≈ s - s / fr
```

Avec `fr = 3`, la partie basse fait environ `1/3` de la bande et la partie haute
environ `2/3`.

Avec `fr = 4`, la partie basse fait environ `1/4` et la partie haute `3/4`.

Exemple `n = 10`, `div = 2`, `s = 5`, `fr = 3` :

```text
bande 1 : [5, 10)
mid     = 5 + 5/3 = 6
basse   : [5, 6)   -> rang 5
haute   : [6, 10)  -> rangs 6, 7, 8, 9
```

### 2.4 `b` comme un double-ended queue

Après le presort, `b` a une structure très particulière :

```text
sommet de b
  parties hautes des bandes, de la plus grande à la plus petite
  parties basses des bandes, de la plus petite à la plus grande
fond de b
```

Autrement dit, pour les bandes `0 … P-1` :

```text
b = H_{P-1}, H_{P-2}, ..., H_0, L_0, L_1, ..., L_{P-1}
```

où `H_c` est la partie haute de la bande `c` et `L_c` sa partie basse.

Cette structure est expliquée en détail dans la section 8.

### 2.5 Tri circulaire

Pendant la remontée, `a` est triée **au sens circulaire** :

```text
a = [7, 8, 9, 5, 6]   -> si on commence à 5 : 5, 6, 7, 8, 9
```

À la fin, `complex_align` ramène le minimum au sommet pour obtenir un tri
linéaire.

### 2.6 Coût signé

Pour amener l’index `i` au sommet d’une pile de taille `size` :

```text
i <= size / 2 : coût = +i      -> ra / rb
sinon         : coût = i - size -> rra / rrb
```

Exemple `size = 5` :

| index | coût | opération |
| ---: | ---: | --- |
| 0 | 0 | aucune |
| 1 | +1 | `ra`/`rb` |
| 2 | +2 | `ra`/`rb` |
| 3 | -2 | `rra`/`rrb` |
| 4 | -1 | `rra`/`rrb` |

### 2.7 Heuristique gloutonne

`complex` ne calcule pas une solution optimale globale. Il choisit à chaque
insertion le candidat de score minimal :

```text
score = coût combiné des rotations dans a et dans b
```

C’est un glouton. Il est efficace parce que le presort place les plages actives
aux extrémités de `b`.

---

## 3. Vue d’ensemble

### 3.1 Les cinq étapes

1. **Normaliser** les valeurs en rangs `0 … n - 1`.
2. **Choisir la géométrie** :
   - `div` ∈ {2, 3, 4} selon `n` ;
   - `fr` ∈ {3, 4} selon `n` ;
   - `chunk_size = n / div`.
3. **Presort** : pousser les bandes de `a` vers `b`, de la plus petite à la plus
   grande, en envoyant la partie basse de chaque bande au fond de `b` avec `rb`.
4. **Pull** : reprendre les bandes de la plus grande à la plus petite, et pour
   chaque bande traiter d’abord la partie haute, puis la partie basse, en
   insérant chaque élément au meilleur coût.
5. **Aligner** le minimum de `a` au sommet.

### 3.2 Schéma

```text
Départ :
a = [valeurs mélangées]       b = []

Après normalisation :
a = [rangs mélangés]          b = []

Après presort :
a = []                        b = [H_last, ..., H_0, L_0, ..., L_last]

Après pull :
a = [triée circulairement]    b = []

Après alignement :
a = [0, 1, 2, ..., n-1]       b = []
```

### 3.3 Pseudo-code

```text
sort_complex(a, b):
    si taille(a) < 2: retour
    n = taille(a)

    si normalize_stack(a) échoue:
        sort_simple(a, b)
        retour

    complex_geometry(n, &div, &fr)
    complex_presort(a, b, div, fr)

    geom = { div, fr, n }
    complex_pull(a, b, geom)
```

---

## 4. Sélection de la stratégie

`--complex` force `STRAT_COMPLEX`.

En mode adaptatif :

```text
désordre < 0.2  -> simple
0.2 <= d < 0.5  -> medium
d >= 0.5        -> complex
```

Pourquoi `complex` pour un désordre élevé ?

- Un désordre élevé signifie beaucoup d’inversions : les stratégies naïves
  multiplient les rotations.
- `complex` fait peu de passes de presort (nombre constant de bandes), puis
  utilise le coût pour choisir les insertions.
- Il est plus stable que `medium` sur les grandes entrées très mélangées.

---

## 5. Normalisation

### 5.1 Principe

`normalize_stack` remplace chaque valeur par son rang.

Exemple :

```text
valeurs : 42  -7  100  3
rangs   :  2   0    3  1
```

### 5.2 Mécanisme

1. `build_sorted_copy` copie `a` et trie la copie par insertion.
2. `find_rank` fait une recherche binaire de chaque valeur dans la copie triée.
3. `normalize_stack` remplace la valeur par son rang, en place.

Coût :

```text
tri par insertion : O(n²) CPU
recherches binaires : O(n log n) CPU
opérations push_swap : 0
mémoire temporaire : O(n)
```

En cas d’échec d’allocation, `sort_complex` retombe sur `sort_simple`.

La normalisation ne produit aucune opération `push_swap`. Elle simplifie les
comparaisons de bandes.

---

## 6. Géométrie : `div`, `fr`, `chunk_size`

### 6.1 `complex_geometry`

```c
static void complex_geometry(int n, int *div, int *fr)
{
    if (n < 300)
    {
        *div = 2;
        *fr = 3;
    }
    else if (n < 450)
    {
        *div = 3;
        *fr = 3;
    }
    else if (n < 800)
    {
        *div = 4;
        *fr = 3;
    }
    else
    {
        *div = 4;
        *fr = 4;
    }
}
```

Tableau :

| Taille `n` | `div` | `fr` | Taille de bande `s ≈ n/div` | Partie basse |
| --- | ---: | ---: | ---: | ---: |
| `n < 300` | 2 | 3 | `n/2` | `s/3` |
| `300 ≤ n < 450` | 3 | 3 | `n/3` | `s/3` |
| `450 ≤ n < 800` | 4 | 3 | `n/4` | `s/3` |
| `n ≥ 800` | 4 | 4 | `n/4` | `s/4` |

### 6.2 Pourquoi un nombre constant de bandes ?

`medium` utilise `div ≈ √n/4`, donc beaucoup de bandes et beaucoup de passes.

`complex` utilise `div` constant (2, 3 ou 4). Le presort ne fait donc qu’un
nombre constant de passes :

```text
P = ceil(n / s) = div ou div + 1 ≤ 5
```

C’est ce qui rend le presort `O(n)` en opérations.

Le prix à payer est que chaque bande est plus large (`s = O(n)`). Le découpage
haut/bas et l’heuristique de coût compensent pendant le pull.

### 6.3 Pourquoi `fr = 3` ou `4` ?

`fr` contrôle la taille de la partie basse :

```text
partie basse ≈ s / fr
partie haute ≈ s - s / fr
```

- `fr = 3` : basse `1/3`, haute `2/3`.
- `fr = 4` : basse `1/4`, haute `3/4`.

Un `fr` plus grand rend la partie basse plus petite. Pour les grandes entrées,
`fr = 4` réduit le nombre d’éléments envoyés au fond de `b`, ce qui limite les
rotations `rb` et facilite le pull.

Les seuils `300`, `450`, `800` sont des réglages empiriques : ils donnent de
bons nombres d’opérations sur les tailles classiques du sujet.

### 6.4 `chunk_size`

```c
s = n / div;
```

Division entière. La dernière bande peut être partielle.

Exemples :

| `n` | `div` | `s` | Bandes |
| ---: | ---: | ---: | --- |
| 100 | 2 | 50 | `[0,50)`, `[50,100)` |
| 101 | 2 | 50 | `[0,50)`, `[50,100)`, `[100,101)` |
| 300 | 3 | 100 | `[0,100)`, `[100,200)`, `[200,300)` |
| 500 | 4 | 125 | `[0,125)`, …, `[375,500)` |
| 1000 | 4 | 250 | `[0,250)`, …, `[750,1000)` |

Le nombre de bandes est :

```text
P = ceil(n / s)
```

soit `div` ou `div + 1`.

---

## 7. Phase 1 : presort avec découpage haut/bas

### 7.1 Code

```c
static void presort_one(t_stack *a, t_stack *b, int limit, int sink)
{
    if (get_stack_element(a, 0) < limit)
    {
        pb(a, b);
        if (get_stack_element(b, 0) < sink)
            rb(b);
    }
    else
        ra(a);
}

void complex_presort(t_stack *a, t_stack *b, int div, int fr)
{
    int chunk_size;
    int chunk;
    int count;
    int i;

    chunk_size = a->size / div;
    if (chunk_size < 1)
        chunk_size = 1;
    chunk = 0;
    while (a->size > 0)
    {
        count = a->size;
        i = 0;
        while (i < count)
        {
            presort_one(a, b, (chunk + 1) * chunk_size,
                chunk * chunk_size + chunk_size / fr);
            i++;
        }
        chunk++;
    }
}
```

### 7.2 Variables

| Variable | Rôle |
| --- | --- |
| `chunk_size` | largeur de bande `s = n / div`. |
| `chunk` | numéro de bande courant, à partir de `0`. |
| `count` | nombre d’éléments au début de la passe. |
| `i` | compteur d’itérations. |
| `limit` | borne supérieure de la bande : `(chunk + 1) * s`. |
| `sink` | frontière haut/bas : `chunk * s + s / fr`. |

### 7.3 La passe de presort

Pour la bande `chunk`, on parcourt les `count` éléments présents au début de la
passe.

À chaque étape, on regarde le sommet de `a` :

- si `sommet < limit`, l’élément appartient à la bande courante :
  - on le pousse dans `b` avec `pb` ;
  - si sa valeur est `< sink`, il appartient à la **partie basse** : on le fait
    descendre au fond de `b` avec `rb` ;
  - sinon, il reste au sommet de `b` (partie haute) ;
- sinon, l’élément appartient à une bande future : on le fait tourner dans `a`
  avec `ra`.

### 7.4 Pourquoi seulement `sommet < limit` ?

Au début de la passe `chunk`, toutes les bandes précédentes
`[0, chunk * s)` ont déjà été retirées.

Donc tout élément restant a un rang `>= chunk * s`. La condition
`sommet < (chunk + 1) * s` signifie donc exactement :

```text
chunk * s <= rang < (chunk + 1) * s
```

C’est la bande courante. Inutile de tester la borne inférieure.

### 7.5 Pourquoi `chunk` commence à `0` ?

`medium` commence à `1` parce que sa limite est `chunk * size`.

`complex` commence à `0` parce que sa limite est `(chunk + 1) * size` :

```text
chunk = 0 -> limite = size       -> bande [0, size)
chunk = 1 -> limite = 2 * size   -> bande [size, 2*size)
...
```

### 7.6 Le rôle de `count`

Comme dans `medium`, `count = a->size` est capturé avant la passe. La boucle
interne fait exactement `count` itérations.

Chaque élément présent au début de la passe est examiné exactement une fois :

- soit il est poussé dans `b` (`pb`) ;
- soit il est remis au fond de `a` (`ra`).

Sans ce `count`, les rotations pourraient faire examiner plusieurs fois les
mêmes éléments ou en oublier.

### 7.7 Le rôle de `sink` et de `rb`

Après `pb`, le sommet de `b` est l’élément qui vient d’être poussé. On peut donc
tester directement :

```c
if (get_stack_element(b, 0) < sink)
    rb(b);
```

Si l’élément est dans la partie basse de la bande, `rb` le fait passer du sommet
au fond de `b`.

Pourquoi veut-on cela ?

Parce que le pull traitera plus tard :

1. la partie haute de la bande (au sommet de `b`) ;
2. puis la partie basse (au fond de `b`).

En envoyant les parties basses au fond dès le presort, on prépare cette
structure.

### 7.8 Exemple de presort `n = 10`

Entrée :

```text
a = [7, 2, 9, 0, 5, 1, 8, 3, 6, 4]
b = []
```

Géométrie :

```text
n = 10
div = 2
fr = 3
s = 5
s/fr = 1
```

Bandes :

```text
bande 0 : [0, 5)
  basse : [0, 1)  -> rang 0
  haute : [1, 5)  -> rangs 1, 2, 3, 4

bande 1 : [5, 10)
  basse : [5, 6)  -> rang 5
  haute : [6, 10) -> rangs 6, 7, 8, 9
```

Passe `chunk = 0`, `limit = 5`, `sink = 1` :

| Étape | Opération | `a` après | `b` après |
| ---: | --- | --- | --- |
| 0 | départ | `[7,2,9,0,5,1,8,3,6,4]` | `[]` |
| 1 | `ra` 7 | `[2,9,0,5,1,8,3,6,4,7]` | `[]` |
| 2 | `pb` 2 | `[9,0,5,1,8,3,6,4,7]` | `[2]` |
| 3 | `ra` 9 | `[0,5,1,8,3,6,4,7,9]` | `[2]` |
| 4 | `pb` 0 | `[5,1,8,3,6,4,7,9]` | `[0,2]` |
| 5 | `rb` | `[5,1,8,3,6,4,7,9]` | `[2,0]` |
| 6 | `ra` 5 | `[1,8,3,6,4,7,9,5]` | `[2,0]` |
| 7 | `pb` 1 | `[8,3,6,4,7,9,5]` | `[1,2,0]` |
| 8 | `ra` 8 | `[3,6,4,7,9,5,8]` | `[1,2,0]` |
| 9 | `pb` 3 | `[6,4,7,9,5,8]` | `[3,1,2,0]` |
| 10 | `ra` 6 | `[4,7,9,5,8,6]` | `[3,1,2,0]` |
| 11 | `pb` 4 | `[7,9,5,8,6]` | `[4,3,1,2,0]` |

Après la bande 0 :

```text
b = [4, 3, 1, 2, 0]
```

Interprétation :

```text
[4, 3, 1, 2]  partie haute de la bande 0
[0]           partie basse de la bande 0, au fond
```

Passe `chunk = 1`, `limit = 10`, `sink = 6` :

| Étape | Opération | `a` après | `b` après |
| ---: | --- | --- | --- |
| 12 | `pb` 7 | `[9,5,8,6]` | `[7,4,3,1,2,0]` |
| 13 | `pb` 9 | `[5,8,6]` | `[9,7,4,3,1,2,0]` |
| 14 | `pb` 5 | `[8,6]` | `[5,9,7,4,3,1,2,0]` |
| 15 | `rb` | `[8,6]` | `[9,7,4,3,1,2,0,5]` |
| 16 | `pb` 8 | `[6]` | `[8,9,7,4,3,1,2,0,5]` |
| 17 | `pb` 6 | `[]` | `[6,8,9,7,4,3,1,2,0,5]` |

Après le presort :

```text
b = [6, 8, 9, 7 | 4, 3, 1, 2 | 0 | 5]
```

Les barres séparent :

```text
H1 = [6, 8, 9, 7]  partie haute de la bande 1
H0 = [4, 3, 1, 2]  partie haute de la bande 0
L0 = [0]           partie basse de la bande 0
L1 = [5]           partie basse de la bande 1, au fond
```

On retrouve la structure annoncée :

```text
b = H1, H0, L0, L1
```

### 7.9 Coût du presort

- Il y a exactement `n` `pb`.
- Il y a au plus `n` `rb` (un par élément de partie basse, et `rb` peut être
  no-op si `b` n’a qu’un élément).
- Il y a `P` passes, avec `P ≤ div + 1 ≤ 5`.
- Chaque passe fait au plus `n` itérations.

Donc le presort émet `O(n)` opérations.

C’est la grande différence avec `medium`, où `P = O(√n)` et le presort coûte
`O(n√n)`.

---

## 8. Structure de `b` après le presort

### 8.1 Notation

Pour chaque bande `c` :

```text
H_c = partie haute = [c*s + s/fr, (c+1)*s)
L_c = partie basse = [c*s, c*s + s/fr)
```

Les bandes sont traitées dans l’ordre croissant `c = 0, 1, ..., P-1`.

### 8.2 Effet de `pb` et `rb`

- Un élément de `H_c` est poussé au sommet de `b` et **reste** au sommet.
- Un élément de `L_c` est poussé au sommet puis envoyé au fond par `rb`.

Comme les bandes sont traitées de `0` à `P-1` :

- les `H_c` s’empilent au sommet, donc dans l’ordre décroissant des bandes :
  `H_{P-1}, H_{P-2}, ..., H_0` ;
- les `L_c` s’empilent au fond, donc dans l’ordre croissant des bandes :
  `L_0, L_1, ..., L_{P-1}`.

D’où :

```text
b = H_{P-1}, H_{P-2}, ..., H_0, L_0, L_1, ..., L_{P-1}
```

### 8.3 Pourquoi cette structure est idéale pour le pull

Quand on traite la bande `c` (de la plus grande à la plus petite) :

- toutes les bandes supérieures ont déjà été retirées ;
- il reste dans `b` les bandes `0 … c` ;
- `H_c` est au **sommet** de `b` ;
- `L_c` est au **fond** de `b`.

Ainsi, la partie haute est accessible par des `rb` (coût positif) et la partie
basse par des `rrb` (coût négatif). Les deux sont aux extrémités, jamais au
milieu.

C’est exactement ce que le pull attend :

```text
chunk = c : d’abord H_c (sommet), puis L_c (fond)
```

### 8.4 Exemple `n = 13` avec bande partielle

Géométrie :

```text
n = 13
div = 2
fr = 3
s = 6
s/fr = 2
```

Bandes :

```text
bande 0 : [0, 6)
  L0 = [0, 2)  -> rangs 0, 1
  H0 = [2, 6)  -> rangs 2, 3, 4, 5

bande 1 : [6, 12)
  L1 = [6, 8)  -> rangs 6, 7
  H1 = [8, 12) -> rangs 8, 9, 10, 11

bande 2 : [12, 13)
  L2 = [12, 14) -> rang 12
  H2 = [14, 18) -> vide
```

Après le presort, on obtient :

```text
b = [9, 11, 8, 10 | 4, 2, 5, 3 | 0, 1 | 7, 6 | 12]
```

Soit :

```text
H1 = [9, 11, 8, 10]
H0 = [4, 2, 5, 3]
L0 = [0, 1]
L1 = [7, 6]
L2 = [12]
```

La structure est bien :

```text
H1, H0, L0, L1, L2
```

La bande 2 est partielle et entièrement basse : `H2` est vide, `L2 = {12}`.
Le pull traitera donc `chunk = 2` avec une partie haute vide, puis `L2`, avant
de passer à `chunk = 1`.

---

## 9. Phase 2 : pull avec sous-plages

### 9.1 Code de `complex_pull`

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

### 9.2 Les deux plages

Pour `chunk = c` :

```text
base = c * s
mid  = c * s + s / fr

plage haute : [mid, base + s) = [c*s + s/fr, (c+1)*s)
plage basse : [base, mid)     = [c*s, c*s + s/fr)
```

Le code traite d’abord la plage haute, puis la plage basse.

Avec `fr = 3` :

```text
haute ≈ 2/3 de la bande
basse ≈ 1/3 de la bande
```

Avec `fr = 4` :

```text
haute ≈ 3/4 de la bande
basse ≈ 1/4 de la bande
```

### 9.3 Correspondance avec le presort

Presort, bande `c` :

```text
L_c = [c*s, c*s + s/fr)
H_c = [c*s + s/fr, (c+1)*s)
```

Pull, `chunk = c` :

```text
haute : [c*s + s/fr, (c+1)*s) = H_c
basse : [c*s, c*s + s/fr)     = L_c
```

Le pull traite donc exactement `H_c` puis `L_c`.

### 9.4 Pourquoi commencer à `chunk = div` ?

Le presort traite les bandes `0 … P-1`, avec `P = ceil(n/s)`, donc `P` vaut
`div` ou `div + 1`.

Le pull commence à `chunk = div` pour couvrir une éventuelle bande partielle
supplémentaire :

- si `n` est divisible par `div`, `chunk = div` donne des plages vides ;
- sinon, `chunk = div` couvre la bande partielle du haut.

Exemple `n = 101`, `div = 2`, `s = 50`, `fr = 3` :

```text
bande 2 : [100, 101)
mid = 100 + 50/3 = 116

haute : [116, 150) -> vide
basse : [100, 116) -> rang 100
```

La bande partielle est donc entièrement basse.

### 9.5 Pourquoi traiter les bandes de la plus grande à la plus petite ?

Parce que le presort a empilé les parties hautes dans l’ordre décroissant :

```text
H_{P-1}, H_{P-2}, ..., H_0
```

En traitant `chunk = div, div-1, ..., 0`, on retire toujours `H_c` au sommet,
puis `L_c` au fond. Les bandes plus petites ne gênent pas : elles sont au
milieu de `b`, entre les parties hautes et les parties basses.

Si on traitait dans l’ordre croissant, il faudrait aller chercher les parties
hautes des grandes bandes au milieu de `b`, ce qui coûterait beaucoup plus de
rotations.

### 9.6 `pull_range`

```c
static void pull_range(t_stack *a, t_stack *b, t_range *range)
{
    int value;

    while (best_in_range(a, b, range, &value))
        complex_insert(a, b, value);
}
```

Tant qu’il reste dans `b` un élément de la plage, on choisit le meilleur
candidat et on l’insère dans `a`.

### 9.7 `best_in_range`

`best_in_range` parcourt tout `b`, filtre les valeurs de `[low, high)`, calcule
un score pour chacune et garde le plus petit score.

En cas d’égalité, il garde le premier candidat rencontré, donc le plus proche du
sommet de `b`.

### 9.8 Terminaison

Chaque `complex_insert` retire un élément de `b`.

Les plages haute et basse de toutes les bandes sont disjointes et couvrent tous
les rangs `0 … n - 1`.

Donc chaque élément est inséré exactement une fois, et `b` finit vide.

---

## 10. L’heuristique de coût

### 10.1 Position d’insertion dans `a`

Pour insérer `value` dans `a` triée circulairement, on cherche son
**successeur** : le plus petit élément de `a` strictement supérieur à `value`.

Si aucun élément n’est supérieur, le successeur circulaire est le **minimum** de
`a`.

On insère `value` juste avant ce successeur.

Exemple :

```text
a = [5, 7, 9, 2, 3]   (circulaire : 2, 3, 5, 7, 9)
value = 6
successeur = 7
on insère 6 avant 7
```

### 10.2 `get_cost`

```text
coût = +index              si index <= size/2
coût = index - size        sinon
```

Le signe indique la direction :

- positif : `ra` / `rb` ;
- négatif : `rra` / `rrb`.

### 10.3 `complex_cost`

Deux cas :

- **même signe** : on peut combiner avec `rr` ou `rrr`, coût =
  `max(|cost_a|, |cost_b|)` ;
- **signes opposés ou l’un nul** : coût = `|cost_a| + |cost_b|`.

Exemples :

```text
cost_a=+3, cost_b=+5 -> 5   (rr x3 + rb x2)
cost_a=-4, cost_b=-2 -> 4   (rrr x2 + rra x2)
cost_a=+3, cost_b=-2 -> 5   (ra x3 + rrb x2)
cost_a=0,  cost_b=-4 -> 4   (rrb x4)
```

### 10.4 `rotate_ab`

Ordre :

1. `rr` tant que les deux coûts sont positifs ;
2. `rrr` tant que les deux coûts sont négatifs ;
3. rotations restantes de `a` avec `ra`/`rra` ;
4. rotations restantes de `b` avec `rb`/`rrb`.

### 10.5 `complex_insert`

```c
cost_a = get_cost(complex_target_index(a, value), a->size);
cost_b = get_cost(complex_find(b, value), b->size);
rotate_ab(a, b, cost_a, cost_b);
pa(a, b);
```

Après les rotations, le successeur est au sommet de `a`. `pa` place `value`
juste au-dessus, donc juste avant le successeur. L’ordre circulaire est
préservé.

### 10.6 Pourquoi le glouton est efficace ici

Les plages actives sont aux extrémités de `b` :

- `H_c` au sommet : coût `rb` petit ;
- `L_c` au fond : coût `rrb` petit.

Le glouton choisit parmi tous les éléments de la plage celui dont le coût total
est minimal. Il a donc souvent le choix entre un élément proche du sommet et un
élément proche du fond, ce qui évite de traverser le milieu de `b`.

C’est ce qui rend `complex` performant malgré des bandes larges.

---

## 11. Exemple complet `n = 10`

### 11.1 Données

```text
a = [7, 2, 9, 0, 5, 1, 8, 3, 6, 4]
b = []
```

Géométrie :

```text
n = 10
div = 2
fr = 3
s = 5
s/fr = 1
```

Bandes :

```text
bande 0 : [0, 5)
  L0 = [0, 1)  -> rang 0
  H0 = [1, 5)  -> rangs 1, 2, 3, 4

bande 1 : [5, 10)
  L1 = [5, 6)  -> rang 5
  H1 = [6, 10) -> rangs 6, 7, 8, 9
```

### 11.2 Presort

Le détail est dans la section 7.8. Résultat :

```text
a = []
b = [6, 8, 9, 7 | 4, 3, 1, 2 | 0 | 5]
```

Soit :

```text
H1 = [6, 8, 9, 7]
H0 = [4, 3, 1, 2]
L0 = [0]
L1 = [5]
```

### 11.3 Pull de la bande 1

`chunk = 1`, `base = 5`, `mid = 6`.

#### Partie haute `H1 = [6, 10)`

| # | Candidats `(valeur, score, cost_a, cost_b)` | Choix | Opérations | `a` après |
| ---: | --- | ---: | --- | --- |
| 1 | `(6,0,0,0)`, `(8,1,0,1)`, `(9,2,0,2)`, `(7,3,0,3)` | 6 | `pa` | `[6]` |
| 2 | `(8,0,0,0)`, `(9,1,0,1)`, `(7,2,0,2)` | 8 | `pa` | `[8,6]` |
| 3 | `(9,1,1,0)`, `(7,1,0,1)` | 9 | `ra`, `pa` | `[9,6,8]` |
| 4 | `(7,1,-1,0)` | 7 | `rra`, `pa` | `[7,8,9,6]` |

Détail de l’étape 3 : `9` et `7` ont tous deux un score de `1`. `best_in_range`
garde le premier rencontré, `9`, qui est plus proche du sommet de `b`.

#### Partie basse `L1 = [5, 6)`

| # | Candidat | Choix | Opérations | `a` après |
| ---: | --- | ---: | --- | --- |
| 5 | `(5,1,-1,-1)` | 5 | `rrr`, `pa` | `[5,6,7,8,9]` |

`cost_a = -1` et `cost_b = -1` : les deux rotations vont vers le bas, donc
`rrr` les combine. C’est un exemple concret d’utilisation de `rrr`.

### 11.4 Pull de la bande 0

`chunk = 0`, `base = 0`, `mid = 1`.

#### Partie haute `H0 = [1, 5)`

| # | Candidats `(valeur, score, cost_a, cost_b)` | Choix | Opérations | `a` après |
| ---: | --- | ---: | --- | --- |
| 1 | `(4,0,0,0)`, `(3,1,0,1)`, `(1,2,0,2)`, `(2,2,0,-2)` | 4 | `pa` | `[4,5,6,7,8,9]` |
| 2 | `(3,0,0,0)`, `(1,1,0,1)`, `(2,2,0,2)` | 3 | `pa` | `[3,4,5,6,7,8,9]` |
| 3 | `(1,0,0,0)`, `(2,1,0,1)` | 1 | `pa` | `[1,3,4,5,6,7,8,9]` |
| 4 | `(2,1,1,0)` | 2 | `ra`, `pa` | `[2,3,4,5,6,7,8,9,1]` |

#### Partie basse `L0 = [0, 1)`

| # | Candidat | Choix | Opérations | `a` après |
| ---: | --- | ---: | --- | --- |
| 5 | `(0,1,-1,0)` | 0 | `rra`, `pa` | `[0,1,2,3,4,5,6,7,8,9]` |

### 11.5 Alignement

Le minimum `0` est déjà au sommet. `complex_align` ne fait rien.

### 11.6 Bilan

```text
pb  : 10
pa  : 10
ra  :  7
rb  :  2
rra :  2
rrr :  1
```

Total : `32` opérations.

Détail des `ra` :

- presort : 5 ;
- pull : 1 (insertion de 9) + 1 (insertion de 2) = 2.

Détail des `rb` :

- presort : 2 (rang 0 et rang 5).

Détail des `rra` :

- pull : 1 (insertion de 7) + 1 (insertion de 0) = 2.

Détail des `rrr` :

- pull : 1 (insertion de 5).

---

## 12. Exemple de bande partielle `n = 13`

On reprend l’exemple de la section 8.4.

```text
n = 13
div = 2
fr = 3
s = 6
s/fr = 2
```

Bandes :

```text
bande 0 : L0 = {0,1}, H0 = {2,3,4,5}
bande 1 : L1 = {6,7}, H1 = {8,9,10,11}
bande 2 : L2 = {12},  H2 = {}
```

Après presort :

```text
b = [9, 11, 8, 10 | 4, 2, 5, 3 | 0, 1 | 7, 6 | 12]
```

Pull :

```text
chunk = 2 :
  haute [14,18) -> vide
  basse [12,14) -> {12}

chunk = 1 :
  haute [8,12) -> H1
  basse [6,8)  -> L1

chunk = 0 :
  haute [2,6)  -> H0
  basse [0,2)  -> L0
```

On voit que la bande partielle 2 est entièrement basse : son `H2` est vide.
Le pull la traite quand même, mais le premier `pull_range` ne trouve rien.

---

## 13. Preuve de correction

### 13.1 Lemme 1 : normalisation

La normalisation conserve l’ordre relatif. Trier les rangs revient à trier les
valeurs.

### 13.2 Lemme 2 : le presort partitionne les rangs

À la passe `chunk = c`, les bandes `< c` ont déjà été retirées. Donc tout
élément restant a un rang `>= c*s`.

La condition `rang < (c+1)*s` sélectionne exactement :

```text
c*s <= rang < (c+1)*s
```

C’est la bande `c`. Les bandes sont disjointes et couvrent `0 … P*s - 1`, donc
tous les rangs.

Chaque élément est poussé exactement une fois dans `b`.

### 13.3 Lemme 3 : structure de `b`

Pour chaque bande `c` :

- `H_c` reste au sommet de `b` ;
- `L_c` est envoyé au fond par `rb`.

Comme les bandes sont traitées dans l’ordre croissant :

```text
b = H_{P-1}, ..., H_0, L_0, ..., L_{P-1}
```

### 13.4 Lemme 4 : `H_c` et `L_c` sont accessibles aux extrémités

Quand on traite la bande `c`, toutes les bandes `> c` ont été retirées.

Il reste :

```text
H_c, H_{c-1}, ..., H_0, L_0, ..., L_c
```

Donc :

- `H_c` est au sommet ;
- `L_c` est au fond.

Les deux plages sont aux extrémités de `b`.

### 13.5 Lemme 5 : `a` reste triée circulairement

On insère `value` avant son successeur :

- le plus petit élément supérieur, s’il existe ;
- sinon le minimum.

Après les rotations, le successeur est au sommet, et `pa` place `value`
juste au-dessus. Donc `value` s’insère entre son prédécesseur et son successeur
dans l’ordre circulaire.

### 13.6 Lemme 6 : `complex_align` finalise

Si `a` est triée circulairement, ramener le minimum au sommet produit un tri
linéaire.

### 13.7 Théorème

À la fin :

- chaque élément de `b` a été inséré exactement une fois ;
- `b` est vide ;
- `a` contient les rangs `0 … n - 1` triés ;
- les valeurs d’origine sont triées.

### 13.8 Terminaison

- `complex_presort` : chaque passe `pb` au moins un élément ou termine. Comme
  `a->size` diminue et que la boucle s’arrête à `0`, elle termine.
- `complex_pull` : chaque `complex_insert` retire un élément de `b`. Les
  plages sont finies et disjointes. Donc `b` finit vide.

---

## 14. Complexité

### 14.1 Presort

```text
P = ceil(n / s) = div ou div + 1 ≤ 5
chaque passe : O(n) itérations
opérations : O(n)
```

C’est le point fort de `complex` par rapport à `medium` : le nombre de passes
est constant.

### 14.2 Pull : ce qui est visé, ce qui est prouvé, ce qui est mesuré

Il faut être précis ici.

**Ce qui est certain :**

- Il y a `n` insertions.
- Chaque insertion peut coûter jusqu’à `O(s) = O(n)` rotations dans le pire cas,
  car une plage active peut avoir une taille `O(n)`.
- On peut donc majorer grossièrement le pull par `O(n²)` opérations.

**Ce que le sujet vise :**

- la classe `O(n log n)`.

**Ce qui est mesuré :**

Le README donne environ :

| `n` | `--complex` | ratio |
| ---: | ---: | ---: |
| 100 | ~568 | ~5.7 n |
| 500 | ~3986 | ~8.0 n |
| 1000 | ~9406 | ~9.4 n |

La croissance observée est donc quasi linéaire, avec une constante qui augmente
lentement. C’est cohérent avec l’objectif `O(n log n)`, mais le code ne contient
pas de preuve formelle de cette borne.

**Pourquoi c’est bien mieux que `O(n²)` en pratique :**

- les plages actives sont aux extrémités de `b` ;
- le glouton choisit souvent un élément proche du sommet ou du fond ;
- les rotations traversent rarement le milieu de `b` ;
- le découpage haut/bas réduit la taille des plages actives.

### 14.3 CPU

Le nombre d’opérations push_swap n’est pas le temps CPU.

`best_in_range` parcourt tout `b` pour chaque insertion, soit `O(n²)`
comparaisons. De plus, pour chaque candidat de la plage, `candidate_score`
appelle `complex_target_index`, qui parcourt `a` en `O(n)`.

Avec `s = O(n)` pour `complex`, on peut majorer grossièrement le CPU par
`O(n³)` dans le pire cas. Ce n’est pas la métrique du sujet.

### 14.4 Mémoire

```text
piles a et b       : O(n)
copie triée        : O(n)
pas de récursion
```

Donc `O(n)` mémoire.

---

## 15. Cas limites et pièges

### 15.1 Entrée déjà triée

`main` ne trie pas si `disorder == 0`. Aucune opération.

### 15.2 `n < 2`

`sort_complex` retourne immédiatement.

### 15.3 Petites tailles

Pour `n < 300` :

```text
div = 2
fr = 3
s = n / 2
```

Si `s / fr == 0`, la partie basse est vide. Le presort ne fait aucun `rb`, et le
pull traite toute la bande comme partie haute.

Exemple `n = 3` : `s = 1`, `s/fr = 0`, donc `L_c` est vide pour toutes les
bandes. Le comportement reste correct : le pull insère les rangs un par un.

### 15.4 Bande partielle

Si `n` n’est pas divisible par `div`, la dernière bande est partielle. Elle peut
être :

- entièrement haute ;
- entièrement basse ;
- coupée entre les deux.

Le pull commence à `chunk = div` pour la couvrir.

### 15.5 `rb` no-op

`rb` ne fait rien si `b` a moins de deux éléments. Dans `presort_one`, le
premier élément basse poussé dans un `b` vide déclenche un appel à `rb`, mais
cet appel est un no-op : aucune opération n’est émise et le compteur `rb` n’est
pas incrémenté.

C’est un détail important pour les traces et les compteurs.

### 15.6 Partie haute ou basse vide

- Si `s/fr == 0`, `L_c` est vide.
- Si la bande partielle est plus petite que `s/fr`, elle est entièrement basse.
- Si elle commence après `mid`, elle est entièrement haute.

`pull_range` gère les plages vides : `best_in_range` renvoie `0` et la boucle
s’arrête immédiatement.

### 15.7 `a` vide au premier `pa`

`complex_min_index` renvoie `0` et `get_cost(0, 0)` vaut `0`.

### 15.8 Doublons

Le parsing les refuse. Les rangs sont uniques.

### 15.9 Échec d’allocation

`normalize_stack` renvoie `0` et `sort_complex` retombe sur `sort_simple`.

### 15.10 `b` n’est pas trié

Le presort ne trie pas l’intérieur des bandes. Il ne fait que regrouper les
rangs et séparer haut/bas. Le tri fin est fait pendant le pull.

### 15.11 Les parties basses ne sont pas dans l’ordre décroissant

Dans `b`, les parties basses sont dans l’ordre croissant :

```text
L_0, L_1, ..., L_{P-1}
```

Donc `L_{P-1}` est tout au fond. Quand on traite les bandes de la plus grande à
la plus petite, `L_c` devient le fond au moment où on en a besoin. C’est
pourquoi il est accessible par `rrb`.

### 15.12 Pourquoi `rr` / `rrr` sont possibles

Les coûts `cost_a` et `cost_b` peuvent avoir le même signe. `rotate_ab` combine
alors les rotations avec `rr` ou `rrr`, ce qui économise des opérations.

Exemple de `rrr` dans l’exemple `n = 10` : insertion de `5`, avec
`cost_a = -1` et `cost_b = -1`.

---

## 16. Comparaison avec `medium` et `simple`

### 16.1 Tableau

| Critère | `simple` | `medium` | `complex` |
| --- | --- | --- | --- |
| Idée | extraction du minimum | bandes `√n` | bandes constantes + haut/bas |
| Normalisation | non | oui | oui |
| `div` | — | `max(2, √n/4)` | 2, 3 ou 4 |
| `fr` | — | 1 | 3 ou 4 |
| Presort | non | `medium_chunks` | `complex_presort` |
| `rb` au presort | non | non | oui, pour les parties basses |
| Structure de `b` | bandes décroissantes | bandes décroissantes | `H` décroissants + `L` croissants |
| Pull | `pa` en bloc | une plage par bande | deux plages par bande |
| Presort ops | — | `O(n√n)` | `O(n)` |
| Pull visé | — | `O(n√n)` | `O(n log n)` |
| Mesuré `n=1000` | ~128020 | ~10490 | ~9406 |

### 16.2 Différences essentielles

**`medium`** :

- beaucoup de bandes (`≈ √n/4`) ;
- presort plus coûteux (`O(n√n)`) ;
- une seule plage par bande (`fr = 1`) ;
- `b` contient les bandes dans l’ordre décroissant.

**`complex`** :

- peu de bandes (2 à 4) ;
- presort peu coûteux (`O(n)`) ;
- deux sous-plages par bande (`fr = 3` ou `4`) ;
- les parties basses sont envoyées au fond avec `rb` ;
- `b` est structuré en `H` décroissants puis `L` croissants ;
- pull plus fin, avec un objectif `O(n log n)`.

### 16.3 Pourquoi `medium` n’utilise pas `complex_presort`

Si on utilisait `complex_presort` avec `fr = 1`, la condition
`b top < sink = (chunk + 1) * s` serait toujours vraie pour un élément de la
bande. Chaque élément serait donc envoyé au fond avec `rb`, ce qui donnerait une
structure différente de celle attendue par `medium`.

`medium` possède donc sa propre distribution, `medium_chunks`, sans `rb`.

---

## 17. Vérification pratique

### 17.1 Compiler

```sh
make
```

### 17.2 Vérifier avec le checker

```sh
ARG="7 2 9 0 5 1 8 3 6 4"
./push_swap --complex $ARG | ./checker_linux $ARG
```

Résultat attendu :

```text
OK
```

### 17.3 Bench

```sh
./push_swap --bench --complex 7 2 9 0 5 1 8 3 6 4 2>/tmp/bench.txt
cat /tmp/bench.txt
```

### 17.4 Comparer les stratégies

```sh
for s in simple medium complex; do
    ./push_swap --bench --$s $(seq 1 100 | shuf) 2>&1 >/dev/null | grep "total operations"
done
```

### 17.5 Tester les seuils de géométrie

```sh
./push_swap --bench --complex $(seq 1 299 | shuf)
./push_swap --bench --complex $(seq 1 300 | shuf)
./push_swap --bench --complex $(seq 1 449 | shuf)
./push_swap --bench --complex $(seq 1 450 | shuf)
./push_swap --bench --complex $(seq 1 799 | shuf)
./push_swap --bench --complex $(seq 1 800 | shuf)
```

On doit voir `div` et `fr` changer aux seuils.

---

## 18. Résumé et fiche mémoire

### 18.1 Les formules

```text
div = 2 si n < 300
      3 si 300 <= n < 450
      4 si 450 <= n < 800
      4 si n >= 800

fr  = 3 si n < 800
      4 si n >= 800

s = n / div
P = ceil(n / s) = div ou div + 1
```

### 18.2 Les cinq étapes

```text
1. Normaliser en rangs 0..n-1.
2. Calculer div, fr, s.
3. Presort : bandes croissantes, partie basse au fond avec rb.
4. Pull : bandes décroissantes, partie haute puis partie basse.
5. Aligner le minimum au sommet de a.
```

### 18.3 Structure de `b`

```text
b = H_{P-1}, ..., H_0, L_0, ..., L_{P-1}
```

### 18.4 Invariants

- `a` est triée circulairement pendant le pull.
- `H_c` est au sommet quand on traite la bande `c`.
- `L_c` est au fond quand on traite la bande `c`.
- Chaque rang appartient à exactement une plage haute ou basse.
- Chaque élément est inséré exactement une fois.

### 18.5 Pièges à retenir

- `complex_presort` commence à `chunk = 0`, `medium_chunks` à `1`.
- `presort_one` envoie la partie basse au fond avec `rb`.
- `rb` peut être un no-op si `b` a moins de deux éléments.
- `complex_pull` commence à `chunk = div`.
- `fr = 1` rendrait la partie haute vide ; `complex` utilise `fr = 3` ou `4`.
- Les parties basses sont dans l’ordre croissant dans `b`, pas décroissant.
- `complex_cost` combine les rotations de même signe avec `rr`/`rrr`.
- `pa` insère avant le successeur.
- `complex_align` est nécessaire pour passer du tri circulaire au tri linéaire.
- Le CPU n’est pas le nombre d’opérations.

### 18.6 En une phrase

> `complex` range les rangs en quelques bandes larges, envoie la partie basse de
> chaque bande au fond de `b`, puis réinsère les bandes de la plus grande à la
> plus petite en traitant le haut puis le bas, en choisissant à chaque fois
> l’insertion la moins coûteuse.
