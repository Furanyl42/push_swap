# Code expliqué ligne par ligne — tri `--medium`

Ce document explique **le code source du tri `medium`**, ligne par ligne, avec
pour chaque ligne :

- ce qu’elle fait ;
- pourquoi elle est là ;
- comment elle fonctionne ;
- les pièges ou invariants associés.

Il complète `explication_detail.md`, qui explique l’algorithme et les concepts.
Ici, on se concentre sur le code C.

---

## Sommaire

1. Les structures de données
2. Contexte d’appel (`main.c`, `sort_dispatcher.c`)
3. `sort_medium.c` : cœur de l’algorithme
4. Normalisation (`normalize_stack`, tri par insertion, recherche du rang)
5. `complex_pull.c` : remontée et score
6. `complex_insert.c` : rotations et insertion
7. `complex_move.c` : recherche et alignement
8. `utils.c` : `get_cost`
9. `stack.c` : accès logique
10. `ops_push.c` : `pb` et `pa`
11. `ops_rotate.c` : `ra`, `rb`, `rr`
12. `ops_reverse.c` : `rra`, `rrb`, `rrr`
13. `ops_swap.c` : pourquoi `medium` ne l’utilise pas
14. Trace détaillée d’une insertion
15. Checklist pour vérifier sa compréhension

---

## 0. Périmètre et conventions

### 0.1 Fichiers couverts

Le chemin d’exécution de `--medium` est :

```text
main.c
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

Les fichiers réellement lus par `medium` sont :

- `sort_dispatcher.c`
- `main.c` (contexte)
- `sort_medium.c`
- `complex_presort.c` (pour `normalize_stack`)
- `array_utils.c`
- `sorts_utils.c`
- `complex_pull.c`
- `complex_insert.c`
- `complex_move.c`
- `utils.c`
- `stack.c`
- `ops_push.c`
- `ops_rotate.c`
- `ops_reverse.c`

`ops_swap.c` n’est pas utilisé par `medium`, mais on explique brièvement pourquoi.

### 0.2 Conventions de lecture

- Les numéros de ligne correspondent aux fichiers actuels du dépôt.
- Une ligne peut être expliquée seule ou avec les lignes qui forment une même
  instruction C.
- `a` et `b` sont des piles ; le sommet est l’index logique `0`.
- « opération » signifie une ligne émise sur `stdout` (`pb`, `ra`, etc.).
- « coût CPU » signifie une comparaison ou un calcul qui ne produit pas
  d’opération.

---

## 1. Les structures de données

### 1.1 `t_stack`

```c
35  typedef struct s_stack
36  {
37      int             *tab;
38      int             size;
39      int             capacity;
40      int             head;
41      int             quiet;
42      t_counts        *counts;
43  }                   t_stack;
```

| Ligne | Explication |
| ---: | --- |
| 37 | `tab` est le tableau physique qui stocke les valeurs. Sa taille allouée est `capacity`. |
| 38 | `size` est le nombre d’éléments réellement présents dans la pile. Il peut être inférieur à `capacity`. |
| 39 | `capacity` est la taille maximale du tableau. Pour `a` et `b`, elle vaut `n`, la taille initiale de l’entrée. |
| 40 | `head` est l’index physique du sommet logique. C’est le point de départ du tableau circulaire. |
| 41 | `quiet` contrôle l’affichage : si `quiet == 0`, les opérations sont écrites sur `stdout` ; si `quiet == 1`, elles ne sont pas écrites. Les compteurs sont quand même incrémentés. |
| 42 | `counts` pointe vers la structure de comptage partagée par `a` et `b`. Chaque opération incrémente le compteur correspondant. |

### 1.2 `t_geom`

```c
65  typedef struct s_geom
66  {
67      int             div;
68      int             fr;
69      int             n;
70  }                   t_geom;
```

| Ligne | Explication |
| ---: | --- |
| 67 | `div` est le nombre de découpes / le diviseur utilisé pour calculer la largeur de bande. |
| 68 | `fr` est le ratio de découpage utilisé par `complex_pull`. `medium` le fixe à `1`. |
| 69 | `n` est la taille initiale de la pile. Elle ne change pas : c’est le nombre total d’éléments à trier. |

### 1.3 `t_range`

```c
59  typedef struct s_range
60  {
61      int             low;
62      int             high;
63  }                   t_range;
```

`[low, high)` est un intervalle semi-ouvert de rangs. Un élément est dans la
plage si `low <= value && value < high`.

### 1.4 `t_counts`

`t_counts` contient un compteur par opération :

```text
sa, sb, ss, pa, pb, ra, rb, rr, rra, rrb, rrr
```

Chaque opération incrémente son compteur. `total_ops` les additionne pour
`--bench`.

---

## 2. Contexte d’appel

### 2.1 `main.c` : `run`

```c
22  static int run(t_stack *a, t_opts *opts)
23  {
24      t_stack        *b;
25      t_counts       counts;
26      double         disorder;
27      t_strategy     strat;
28
29      init_counts(&counts);
30      a->counts = &counts;
31      b = init_stack(a->capacity, &counts);
32      if (!b)
33          return (0);
34      disorder = compute_disorder(a);
35      strat = resolve_strategy(opts->strategy, disorder);
36      if (disorder > 0.0)
37          sort_stacks(a, b, strat);
38      if (opts->bench)
39          print_bench(&counts, disorder, opts->strategy, strat);
40      free_stack(b);
41      return (1);
42  }
```

| Ligne | Explication |
| ---: | --- |
| 24 | `b` est la deuxième pile, vide au départ. |
| 25 | `counts` stocke les compteurs d’opérations. |
| 26 | `disorder` recevra le taux de désordre. |
| 27 | `strat` recevra la stratégie résolue. |
| 29 | Remet tous les compteurs à zéro. |
| 30 | `a` pointe vers les compteurs. |
| 31 | Alloue `b` avec la même capacité que `a`. C’est indispensable : `b` doit pouvoir contenir jusqu’à `n` éléments. `b` partage les mêmes compteurs. |
| 32-33 | Si l’allocation échoue, `run` signale l’échec. |
| 34 | Calcule le désordre avec la formule des paires inversées. |
| 35 | Si `--medium` est forcé, renvoie `STRAT_MEDIUM` ; sinon choisit selon le désordre. |
| 36-37 | Ne trie que si la pile n’est pas déjà triée. C’est pourquoi une entrée triée ne produit aucune opération. |
| 38-39 | En mode `--bench`, affiche les statistiques sur `stderr`. |
| 40 | Libère `b`. |
| 41 | Succès. |

### 2.2 `sort_dispatcher.c` : `resolve_strategy` et `sort_stacks`

```c
15  t_strategy resolve_strategy(t_strategy requested, double disorder)
16  {
17      if (requested != STRAT_ADAPTIVE)
18          return (requested);
19      if (disorder < 0.2)
20          return (STRAT_SIMPLE);
21      if (disorder < 0.5)
22          return (STRAT_MEDIUM);
23      return (STRAT_COMPLEX);
24  }
```

| Ligne | Explication |
| ---: | --- |
| 15 | Prend la stratégie demandée et le désordre. |
| 17-18 | Si l’utilisateur a forcé une stratégie (`--simple`, `--medium`, `--complex`), on la respecte. |
| 19-20 | Désordre faible : `simple`. |
| 21-22 | Désordre moyen : `medium`. |
| 23 | Désordre élevé : `complex`. |

```c
26  void sort_stacks(t_stack *a, t_stack *b, t_strategy strat)
27  {
28      if (a->size < 2)
29          return ;
30      if (strat == STRAT_SIMPLE)
31          sort_simple(a, b);
32      else if (strat == STRAT_MEDIUM)
33          sort_medium(a, b);
34      else if (strat == STRAT_COMPLEX)
35          sort_complex(a, b);
36  }
```

| Ligne | Explication |
| ---: | --- |
| 28-29 | Une pile vide ou à un élément est déjà triée. |
| 30-31 | Dispatch vers `simple`. |
| 32-33 | Dispatch vers `medium`, le sujet de ce document. |
| 34-35 | Dispatch vers `complex`. |

---

## 3. `sort_medium.c` — cœur de l’algorithme

### 3.1 `floor_sqrt`

```c
15  static int floor_sqrt(int n)
16  {
17      int root;
18
19      root = 0;
20      while ((root + 1) <= n / (root + 1))
21          root++;
22      return (root);
23  }
```

| Ligne | Explication |
| ---: | --- |
| 15 | `static` : fonction privée au fichier. `n` est la taille de la pile, donc `n >= 2` quand elle est appelée. |
| 17 | Déclare le candidat de racine. |
| 19 | On commence à `0`. |
| 20 | Teste si le candidat suivant, `root + 1`, a son carré inférieur ou égal à `n`. La division entière `n / (root + 1)` évite la multiplication `(root + 1) * (root + 1)`, donc évite un débordement. Pour `a > 0`, `a <= n / a` équivaut à `a * a <= n`. |
| 21 | Si le candidat suivant est valide, on l’accepte. |
| 22 | Quand la condition échoue, `root` est le plus grand entier dont le carré est `<= n` : c’est `floor(sqrt(n))`. |
| 23 | Fin de fonction. |

Exemple :

```text
n = 13
root=0 : 1 <= 13/1=13 -> root=1
root=1 : 2 <= 13/2=6  -> root=2
root=2 : 3 <= 13/3=4  -> root=3
root=3 : 4 <= 13/4=3  -> faux
retour 3
```

### 3.2 `medium_chunks`

```c
25  static void medium_chunks(t_stack *a, t_stack *b, int div)
26  {
27      int size;
28      int chunk;
29      int count;
30      int i;
31
32      size = a->size / div;
33      if (size < 1)
34          size = 1;
35      chunk = 1;
36      while (a->size > 0)
37      {
38          count = a->size;
39          i = 0;
40          while (i < count)
41          {
42              if (get_stack_element(a, 0) < chunk * size)
43                  pb(a, b);
44              else
45                  ra(a);
46              i++;
47          }
48          chunk++;
49      }
50  }
```

| Ligne | Explication |
| ---: | --- |
| 25 | Fonction privée qui distribue `a` vers `b`. `div` est la géométrie calculée par `sort_medium`. |
| 27 | `size` sera la largeur de bande `s`. |
| 28 | `chunk` est le numéro de passe / la borne courante. |
| 29 | `count` est le nombre d’éléments au début d’une passe. |
| 30 | `i` compte les itérations de la passe. |
| 32 | Calcule `s = n / div` avec division entière. `a->size` vaut `n` au début de la distribution. |
| 33-34 | Garde-fou : une largeur de bande ne peut pas être nulle. Sinon `chunk * size` vaudrait toujours `0` et rien ne serait poussé. |
| 35 | On commence à `1`, car `chunk = 0` donnerait une limite `0` et une boucle infinie. |
| 36 | Continue tant qu’il reste des éléments dans `a`. |
| 38 | **Point crucial** : capture la taille avant la passe. La boucle interne fera exactement `count` itérations, même si des `pb` réduisent `a` ou si des `ra` la réorganisent. |
| 39 | Réinitialise le compteur d’itérations. |
| 40 | Chaque élément présent au début de la passe sera examiné exactement une fois. |
| 42 | Regarde le sommet de `a`. Si son rang est strictement inférieur à `chunk * size`, il appartient à la bande courante. On ne teste pas la borne inférieure car les bandes précédentes ont déjà été retirées. |
| 43 | `pb` retire le sommet de `a` et le pousse au sommet de `b`. |
| 44-45 | Sinon, l’élément n’est pas dans la bande courante : `ra` le met au fond de `a` pour libérer le sommet. |
| 46 | Itération suivante. |
| 48 | Passe à la bande suivante. |
| 49-50 | Fin des boucles et de la fonction. |

#### Pourquoi `count` est indispensable

Sans `count`, la condition de boucle serait `i < a->size`, qui change pendant la
passe. On pourrait :

- examiner plusieurs fois le même élément après des rotations ;
- ou en oublier.

Avec `count`, on traite la passe comme une file circulaire de `count` éléments :
chaque élément est soit poussé (`pb`), soit remis au fond (`ra`), mais il est
examiné une seule fois.

#### Pourquoi seulement `rang < chunk * size`

Au début de la passe `chunk`, tous les rangs `< (chunk - 1) * size` ont été
retirés lors des passes précédentes. Donc tout élément restant a un rang
`>= (chunk - 1) * size`. La condition `rang < chunk * size` signifie donc :

```text
(chunk - 1) * size <= rang < chunk * size
```

C’est exactement la bande courante.

### 3.3 `sort_medium`

```c
52  void sort_medium(t_stack *a, t_stack *b)
53  {
54      t_geom  geom;
55
56      if (a->size < 2)
57          return ;
58      geom.n = a->size;
59      if (!normalize_stack(a))
60      {
61          sort_simple(a, b);
62          return ;
63      }
64      geom.div = floor_sqrt(geom.n) / 4;
65      if (geom.div < 2)
66          geom.div = 2;
67      geom.fr = 1;
68      medium_chunks(a, b, geom.div);
69      complex_pull(a, b, &geom);
70  }
```

| Ligne | Explication |
| ---: | --- |
| 52 | Point d’entrée de la stratégie medium. |
| 54 | Déclare la géométrie. |
| 56-57 | Une pile de taille 0 ou 1 est déjà triée : rien à faire. |
| 58 | Sauvegarde `n`. La taille ne change pas ensuite, mais on en a besoin pour `complex_pull`. |
| 59 | Normalise `a` : remplace les valeurs par leurs rangs. Si l’allocation de la copie triée échoue, renvoie `0`. |
| 60-63 | En cas d’échec de normalisation, on retombe sur `sort_simple`, qui n’a pas besoin de rangs. `return` évite de continuer avec des valeurs non normalisées. |
| 64 | `div = floor(sqrt(n)) / 4`, division entière. |
| 65-66 | `div` minimum `2` : évite une seule bande géante. |
| 67 | `fr = 1` : medium ne découpe pas les bandes en sous-plages. |
| 68 | Phase 1 : distribution de `a` vers `b`. |
| 69 | Phase 2 : remontée de `b` vers `a`, puis alignement final. `complex_pull` appelle `complex_align` à la fin. |
| 70 | Fin. |

---

## 4. Normalisation

> `medium` n’appelle pas `complex_presort` ni `presort_one`, qui sont réservés à
> la stratégie `complex`. Il utilise seulement `normalize_stack`, qui se trouve
> dans le même fichier.

### 4.1 `normalize_stack` dans `complex_presort.c`

```c
15  int normalize_stack(t_stack *a)
16  {
17      int *sorted;
18      int index;
19      int i;
20
21      sorted = build_sorted_copy(a);
22      if (!sorted)
23          return (0);
24      i = 0;
25      while (i < a->size)
26      {
27          index = (a->head + i) % a->capacity;
28          a->tab[index] = find_rank(sorted, a->size, a->tab[index]);
29          i++;
30      }
31      free(sorted);
32      return (1);
33  }
```

| Ligne | Explication |
| ---: | --- |
| 15 | Renvoie `1` en cas de succès, `0` en cas d’échec d’allocation. |
| 17 | `sorted` recevra une copie triée des valeurs d’origine. |
| 18 | `index` sera l’index physique dans `a->tab`. |
| 19 | `i` est l’index logique. |
| 21 | Construit la copie triée. |
| 22-23 | Si `malloc` échoue, on renvoie `0` sans avoir modifié `a`. C’est important pour le fallback `sort_simple`. |
| 24 | Parcourt les éléments dans l’ordre logique. |
| 25 | Boucle sur les `a->size` éléments. |
| 27 | Convertit l’index logique `i` en index physique : `(head + i) % capacity`. |
| 28 | Remplace la valeur physique par son rang. La partie droite est évaluée avec l’ancienne valeur de `a->tab[index]`, puis le résultat est stocké. `sorted` n’est jamais modifié, donc les rangs restent corrects. |
| 29 | Élément suivant. |
| 31 | Libère la copie triée. |
| 32 | Succès. |
| 33 | Fin. |

### 4.2 `insertion_sort` dans `array_utils.c`

```c
15  static void insertion_sort(int *array, int size)
16  {
17      int i;
18      int j;
19      int tmp;
20
21      i = 1;
22      while (i < size)
23      {
24          tmp = array[i];
25          j = i - 1;
26          while (j >= 0 && array[j] > tmp)
27          {
28              array[j + 1] = array[j];
29              j--;
30          }
31          array[j + 1] = tmp;
32          i++;
33      }
34  }
```

| Ligne | Explication |
| ---: | --- |
| 15 | Tri par insertion privé. |
| 17-19 | Variables de boucle et valeur temporaire. |
| 21 | On considère que `array[0]` est déjà trié. |
| 22 | Parcourt les éléments à insérer. |
| 24 | Sauvegarde l’élément courant. |
| 25 | `j` démarre sur le dernier élément de la partie triée. |
| 26 | Tant qu’on n’est pas sorti du tableau et que l’élément à gauche est plus grand que `tmp`, il faut le décaler. |
| 28 | Décale l’élément vers la droite. |
| 29 | Recule. |
| 31 | Insère `tmp` à sa place. |
| 32 | Élément suivant. |
| 33-34 | Fin. |

Propriétés :

- tri **en place** ;
- **stable** : les éléments égaux gardent leur ordre relatif ;
- `O(n²)` dans le pire cas ;
- pas d’allocation supplémentaire.

Exemple :

```text
[42, -7, 100, 3]

i=1, tmp=-7  : [-7, 42, 100, 3]
i=2, tmp=100 : [-7, 42, 100, 3]
i=3, tmp=3   : [-7, 3, 42, 100]
```

### 4.3 `build_sorted_copy` dans `array_utils.c`

```c
36  int *build_sorted_copy(t_stack *a)
37  {
38      int *copy;
39      int i;
40
41      copy = malloc(sizeof(int) * a->size);
42      if (!copy)
43          return (NULL);
44      i = 0;
45      while (i < a->size)
46      {
47          copy[i] = get_stack_element(a, i);
48          i++;
49      }
50      insertion_sort(copy, a->size);
51      return (copy);
52  }
```

| Ligne | Explication |
| ---: | --- |
| 36 | Renvoie un nouveau tableau trié, ou `NULL`. |
| 38 | Pointeur vers la copie. |
| 39 | Index logique. |
| 41 | Alloue `a->size` entiers. |
| 42-43 | Échec d’allocation : retour `NULL`. |
| 44 | Commence au sommet logique. |
| 45 | Parcourt tous les éléments. |
| 47 | Copie l’élément logique `i` dans `copy[i]`. `get_stack_element` gère le tableau circulaire. |
| 48 | Élément suivant. |
| 50 | Trie la copie. |
| 51 | Renvoie la copie triée. |
| 52 | Fin. |

### 4.4 `find_rank` dans `sorts_utils.c`

```c
15  int find_rank(int *sorted, int n, int value)
16  {
17      int lo;
18      int hi;
19      int mid;
20
21      lo = 0;
22      hi = n - 1;
23      while (lo <= hi)
24      {
25          mid = (lo + hi) / 2;
26          if (sorted[mid] == value)
27              return (mid);
28          if (sorted[mid] < value)
29              lo = mid + 1;
30          else
31              hi = mid - 1;
32      }
33      return (0);
34  }
```

| Ligne | Explication |
| ---: | --- |
| 15 | Recherche `value` dans `sorted` et renvoie son index, qui est son rang. |
| 17-19 | Bornes et milieu de la recherche binaire. |
| 21 | Début du tableau. |
| 22 | Fin du tableau. |
| 23 | Tant que l’intervalle de recherche est non vide. |
| 25 | Calcule le milieu. |
| 26-27 | Valeur trouvée : son index est son rang. |
| 28-29 | La valeur cherchée est à droite. |
| 30-31 | La valeur cherchée est à gauche. |
| 33 | Sécurité : ne devrait jamais arriver si `value` est présente. |
| 34 | Fin. |

Complexité : `O(log n)` par appel.

---

## 5. `complex_pull.c` — remontée et score

### 5.1 `complex_cost`

```c
15  int complex_cost(int cost_a, int cost_b)
16  {
17      int abs_a;
18      int abs_b;
19
20      abs_a = cost_a;
21      if (abs_a < 0)
22          abs_a = -abs_a;
23      abs_b = cost_b;
24      if (abs_b < 0)
25          abs_b = -abs_b;
26      if ((cost_a > 0 && cost_b > 0) || (cost_a < 0 && cost_b < 0))
27      {
28          if (abs_a > abs_b)
29              return (abs_a);
30          return (abs_b);
31      }
32      return (abs_a + abs_b);
33  }
```

| Ligne | Explication |
| ---: | --- |
| 15 | Calcule le coût combiné de deux rotations signées. |
| 17-18 | Variables pour les valeurs absolues. |
| 20-22 | Calcule `|cost_a|` sans appeler `abs`. |
| 23-25 | Calcule `|cost_b|`. |
| 26 | Détecte si les deux rotations vont dans le **même sens** : toutes deux positives (`ra`/`rb`) ou toutes deux négatives (`rra`/`rrb`). |
| 28-30 | Même sens : on peut utiliser `rr` ou `rrr`, donc le coût est le **maximum** des deux valeurs absolues. |
| 32 | Signes opposés ou l’un nul : on ne peut pas combiner, donc le coût est la **somme**. |
| 33 | Fin. |

Exemples :

```text
cost_a=+3, cost_b=+5 -> 5   (rr x3 + rb x2)
cost_a=-4, cost_b=-2 -> 4   (rrr x2 + rra x2)
cost_a=+3, cost_b=-2 -> 5   (ra x3 + rrb x2)
cost_a=0,  cost_b=-4 -> 4   (rrb x4)
```

### 5.2 `candidate_score`

```c
35  static int candidate_score(t_stack *a, t_stack *b, int value, int index)
36  {
37      int cost_a;
38      int cost_b;
39
40      cost_a = get_cost(complex_target_index(a, value), a->size);
41      cost_b = get_cost(index, b->size);
42      return (complex_cost(cost_a, cost_b));
43  }
```

| Ligne | Explication |
| ---: | --- |
| 35 | Calcule le score d’un candidat `value` situé à l’index `index` dans `b`. |
| 37-38 | Coûts signés dans `a` et dans `b`. |
| 40 | Trouve la position d’insertion dans `a` (le successeur), puis convertit cet index en coût signé. |
| 41 | Convertit l’index de `value` dans `b` en coût signé. |
| 42 | Combine les deux coûts. |
| 43 | Fin. |

### 5.3 `best_in_range`

```c
45  static int best_in_range(t_stack *a, t_stack *b, t_range *range, int *out)
46  {
47      int i;
48      int best;
49      int score;
50      int value;
51
52      i = 0;
53      best = 2147483647;
54      *out = 0;
55      while (i < b->size)
56      {
57          value = get_stack_element(b, i);
58          if (value >= range->low && value < range->high)
59          {
60              score = candidate_score(a, b, value, i);
61              if (score < best)
62              {
63                  best = score;
64                  *out = value;
65              }
66          }
67          i++;
68      }
69      return (best != 2147483647);
70  }
```

| Ligne | Explication |
| ---: | --- |
| 45 | Cherche le meilleur candidat de `b` dans `[low, high)`. `out` recevra sa valeur. |
| 47 | Index de parcours de `b`. |
| 48 | Meilleur score trouvé. |
| 49 | Score du candidat courant. |
| 50 | Valeur du candidat courant. |
| 52 | Commence au sommet de `b`. |
| 53 | `2147483647` est `INT_MAX`, utilisé comme « infiniment grand ». |
| 54 | Initialise la sortie ; la valeur ne sera utilisée que si un candidat est trouvé. |
| 55 | Parcourt tout `b`, du sommet vers le fond. |
| 57 | Récupère la valeur logique à l’index `i`. |
| 58 | Ne considère que les valeurs de la plage courante. |
| 60 | Calcule le score du candidat. |
| 61 | Garde le meilleur score. En cas d’égalité, on ne rentre pas dans le `if`, donc le **premier** candidat rencontré (le plus proche du sommet) gagne. |
| 63-64 | Mémorise le score et la valeur. |
| 67 | Candidat suivant. |
| 69 | Renvoie `1` si au moins un candidat a été trouvé, `0` sinon. |
| 70 | Fin. |

### 5.4 `pull_range`

```c
72  static void pull_range(t_stack *a, t_stack *b, t_range *range)
73  {
74      int value;
75
76      while (best_in_range(a, b, range, &value))
77          complex_insert(a, b, value);
78  }
```

| Ligne | Explication |
| ---: | --- |
| 72 | Insère tous les éléments de `b` dont le rang est dans `range`. |
| 74 | Recevra la valeur choisie. |
| 76 | Tant qu’il reste un candidat dans la plage, on continue. |
| 77 | Insère le candidat choisi. Le score est recalculé au tour suivant, car `a` a changé. |
| 78 | Fin. |

### 5.5 `complex_pull`

```c
80  void complex_pull(t_stack *a, t_stack *b, t_geom *geom)
81  {
82      int chunk_size;
83      int chunk;
84      int base;
85      int mid;
86      t_range range;
87
88      chunk_size = geom->n / geom->div;
89      if (chunk_size < 1)
90          chunk_size = 1;
91      chunk = geom->div;
92      while (chunk >= 0)
93      {
94          base = chunk * chunk_size;
95          mid = base + chunk_size / geom->fr;
96
97          range.low = mid;
98          range.high = base + chunk_size;
99          pull_range(a, b, &range);
100
101         range.low = base;
102         range.high = mid;
103         pull_range(a, b, &range);
104
105         chunk--;
106     }
107     complex_align(a);
108 }
```

| Ligne | Explication |
| ---: | --- |
| 80 | Remonte toutes les bandes de `b` vers `a`. |
| 82 | Largeur de bande. |
| 83 | Numéro de la borne supérieure de bande. |
| 84 | Début de la bande. |
| 85 | Point de découpage interne. |
| 86 | Plage courante. |
| 88 | Recalcule `s = n / div`. Pour medium, c’est la même valeur que dans `medium_chunks`. |
| 89-90 | Garde-fou contre une largeur nulle. |
| 91 | Commence à `div`, pour couvrir la bande partielle du haut. Si `n` est divisible par `div`, cette plage sera vide. |
| 92 | Boucle jusqu’à `chunk = 0` inclus. |
| 94 | `base = chunk * s`. |
| 95 | `mid = base + s / fr`. Avec `fr = 1`, `mid = base + s`. |
| 97-99 | Première plage `[mid, base + s)`. Avec `fr = 1`, elle est vide : `[base+s, base+s)`. `pull_range` ne fait rien. |
| 101-103 | Seconde plage `[base, mid)`. Avec `fr = 1`, c’est la bande entière `[base, base+s)`. |
| 105 | Passe à la bande inférieure. |
| 107 | Aligne le minimum de `a` au sommet. |
| 108 | Fin. |

#### Correspondance des plages

Distribution, passe `p` :

```text
[ (p-1)*s, p*s )
```

Pull, `chunk = c` :

```text
[ c*s, (c+1)*s )
```

Donc `chunk = c` correspond à la bande `p = c + 1`.

Pour `n = 13`, `div = 2`, `s = 6` :

| Distribution | Bande | Pull `chunk` | Plage |
| ---: | --- | ---: | --- |
| passe 1 | `[0,6)` | 0 | `[0,6)` |
| passe 2 | `[6,12)` | 1 | `[6,12)` |
| passe 3 | `[12,13)` | 2 | `[12,18)` |

Le pull traite donc `chunk = 2`, puis `1`, puis `0` : les bandes de la plus
grande à la plus petite.

---

## 6. `complex_insert.c` — rotations et insertion

### 6.1 `complex_rotate_a`

```c
15  static void complex_rotate_a(t_stack *a, int cost)
16  {
17      while (cost > 0)
18      {
19          ra(a);
20          cost--;
21      }
22      while (cost < 0)
23      {
24          rra(a);
25          cost++;
26      }
27  }
```

| Ligne | Explication |
| ---: | --- |
| 15 | Applique un coût signé à `a`. |
| 17-21 | Coût positif : `ra` autant de fois que nécessaire, en décrémentant. |
| 22-26 | Coût négatif : `rra` autant de fois que nécessaire, en incrémentant jusqu’à `0`. |
| 27 | Fin. |

### 6.2 `complex_rotate_b`

```c
29  static void complex_rotate_b(t_stack *b, int cost)
30  {
31      while (cost > 0)
32      {
33          rb(b);
34          cost--;
35      }
36      while (cost < 0)
37      {
38          rrb(b);
39          cost++;
40      }
41  }
```

| Ligne | Explication |
| ---: | --- |
| 29 | Applique un coût signé à `b`. |
| 31-35 | Coût positif : `rb` autant de fois que nécessaire. |
| 36-40 | Coût négatif : `rrb` autant de fois que nécessaire. |
| 41 | Fin. |

Même principe que `complex_rotate_a`, mais avec `b` : `rb` pour un coût positif,
`rrb` pour un coût négatif.

### 6.3 `rotate_ab`

```c
43  static void rotate_ab(t_stack *a, t_stack *b, int cost_a, int cost_b)
44  {
45      while (cost_a > 0 && cost_b > 0)
46      {
47          rr(a, b);
48          cost_a--;
49          cost_b--;
50      }
51      while (cost_a < 0 && cost_b < 0)
52      {
53          rrr(a, b);
54          cost_a++;
55          cost_b++;
56      }
57      complex_rotate_a(a, cost_a);
58      complex_rotate_b(b, cost_b);
59  }
```

| Ligne | Explication |
| ---: | --- |
| 43 | Applique les deux rotations en combinant ce qui peut l’être. |
| 45-50 | Tant que les deux coûts sont positifs, on utilise `rr`, qui tourne les deux piles en une seule opération. |
| 51-56 | Tant que les deux coûts sont négatifs, on utilise `rrr`. |
| 57 | Termine les rotations restantes de `a` avec `ra` ou `rra`. |
| 58 | Termine les rotations restantes de `b` avec `rb` ou `rrb`. |
| 59 | Fin. |

Pourquoi cet ordre ? Parce que `rr` et `rrr` ne sont possibles que si les deux
piles doivent tourner dans le même sens. Dès qu’un des deux coûts atteint `0`
ou change de signe, on ne peut plus combiner.

### 6.4 `complex_insert`

```c
61  void complex_insert(t_stack *a, t_stack *b, int value)
62  {
63      int cost_a;
64      int cost_b;
65
66      cost_a = get_cost(complex_target_index(a, value), a->size);
67      cost_b = get_cost(complex_find(b, value), b->size);
68      rotate_ab(a, b, cost_a, cost_b);
69      pa(a, b);
70  }
```

| Ligne | Explication |
| ---: | --- |
| 61 | Insère `value` de `b` vers `a`. |
| 63-64 | Coûts signés. |
| 66 | Trouve la position d’insertion dans `a` (le successeur) et son coût. |
| 67 | Trouve `value` dans `b` et son coût. |
| 68 | Effectue les rotations nécessaires, en combinant `rr`/`rrr`. |
| 69 | `pa` pousse `value` au sommet de `a`. Après les rotations, le successeur est au sommet, donc `value` se place juste avant lui. |
| 70 | Fin. |

---

## 7. `complex_move.c` — recherche et alignement

### 7.1 `complex_find`

```c
15  int complex_find(t_stack *s, int value)
16  {
17      int i;
18
19      i = 0;
20      while (i < s->size)
21      {
22          if (get_stack_element(s, i) == value)
23              return (i);
24          i++;
25      }
26      return (0);
27  }
```

| Ligne | Explication |
| ---: | --- |
| 15 | Cherche l’index logique de `value` dans `s`. |
| 17 | Index de parcours. |
| 19 | Commence au sommet. |
| 20 | Parcourt toute la pile. |
| 22-23 | Valeur trouvée : renvoie son index. |
| 24 | Sinon, continue. |
| 26 | Sécurité : ne devrait pas arriver dans `medium`, car `value` vient de `b`. |
| 27 | Fin. |

### 7.2 `complex_min_index`

```c
29  int complex_min_index(t_stack *a)
30  {
31      int idx;
32      int i;
33
34      idx = 0;
35      i = 1;
36      while (i < a->size)
37      {
38          if (get_stack_element(a, i) < get_stack_element(a, idx))
39              idx = i;
40          i++;
41      }
42      return (idx);
43  }
```

| Ligne | Explication |
| ---: | --- |
| 29 | Renvoie l’index du minimum de `a`. |
| 31 | Index du minimum courant. |
| 32 | Index de parcours. |
| 34 | On suppose le premier élément minimum. |
| 35 | Commence au deuxième. |
| 36 | Parcourt le reste. |
| 38-39 | Si un élément est plus petit, il devient le nouveau minimum. |
| 40 | Élément suivant. |
| 42 | Renvoie l’index. |
| 43 | Fin. |

Si `a` est vide, la boucle ne s’exécute pas et la fonction renvoie `0`. C’est
utilisé au premier `pa` de la remontée.

### 7.3 `best_greater`

```c
45  static int best_greater(t_stack *a, int value)
46  {
47      int idx;
48      int best;
49      int i;
50
51      idx = -1;
52      best = 2147483647;
53      i = 0;
54      while (i < a->size)
55      {
56          if (get_stack_element(a, i) > value
57              && get_stack_element(a, i) < best)
58          {
59              best = get_stack_element(a, i);
60              idx = i;
61          }
62          i++;
63      }
64      return (idx);
65  }
```

| Ligne | Explication |
| ---: | --- |
| 45 | Cherche le plus petit élément de `a` strictement supérieur à `value`. |
| 47 | Index du meilleur candidat. |
| 48 | Meilleure valeur trouvée. |
| 49 | Index de parcours. |
| 51 | `-1` signifie « aucun successeur ». |
| 52 | `INT_MAX` comme valeur initiale « infiniment grande ». |
| 53 | Commence au sommet. |
| 54 | Parcourt toute la pile. |
| 56-57 | L’élément doit être strictement supérieur à `value` et plus petit que le meilleur courant. |
| 59-60 | Mémorise la valeur et l’index. |
| 62 | Élément suivant. |
| 64 | Renvoie `-1` si aucun élément n’est supérieur. |
| 65 | Fin. |

### 7.4 `complex_target_index`

```c
67  int complex_target_index(t_stack *a, int value)
68  {
69      int idx;
70
71      idx = best_greater(a, value);
72      if (idx == -1)
73          idx = complex_min_index(a);
74      return (idx);
75  }
```

| Ligne | Explication |
| ---: | --- |
| 67 | Renvoie l’index devant lequel insérer `value`. |
| 69 | Index du successeur. |
| 71 | Cherche le plus petit élément supérieur. |
| 72-73 | S’il n’y en a pas, `value` est plus grande que tout `a` : son successeur circulaire est le minimum. |
| 74 | Renvoie l’index. |
| 75 | Fin. |

### 7.5 `complex_align`

```c
77  void complex_align(t_stack *a)
78  {
79      int index;
80      int cost;
81
82      index = complex_min_index(a);
83      cost = get_cost(index, a->size);
84      while (cost > 0)
85      {
86          ra(a);
87          cost--;
88      }
89      while (cost < 0)
90      {
91          rra(a);
92          cost++;
93      }
94  }
```

| Ligne | Explication |
| ---: | --- |
| 77 | Ramène le minimum de `a` au sommet. |
| 79 | Index du minimum. |
| 80 | Coût signé pour l’amener au sommet. |
| 82 | Trouve le minimum. |
| 83 | Convertit son index en coût. |
| 84-88 | Coût positif : `ra`. |
| 89-93 | Coût négatif : `rra`. |
| 94 | Fin. |

Après `complex_align`, `a` est triée linéairement :

```text
a = [0, 1, 2, ..., n-1]
```

---

## 8. `utils.c` — `get_cost`

```c
15  int get_cost(int index, int size)
16  {
17      if (size <= 1)
18          return (0);
19      if (index <= size / 2)
20          return (index);
21      return (index - size);
22  }
```

| Ligne | Explication |
| ---: | --- |
| 15 | Convertit un index en coût signé de rotation. |
| 17-18 | Une pile de taille 0 ou 1 n’a rien à tourner. |
| 19-20 | Si l’index est dans la première moitié, on tourne vers le haut : `ra`/`rb`, coût positif `+index`. En cas d’égalité à `size / 2`, on choisit cette direction. |
| 21 | Sinon, on tourne vers le bas : `rra`/`rrb`, coût négatif `index - size`. |
| 22 | Fin. |

Exemple pour `size = 4` :

```text
index 0 -> 0
index 1 -> +1
index 2 -> +2
index 3 -> -1
```

Exemple pour `size = 5` :

```text
index 0 -> 0
index 1 -> +1
index 2 -> +2
index 3 -> -2
index 4 -> -1
```

---

## 9. `stack.c` — accès logique

```c
15  int get_stack_element(t_stack *s, int index)
16  {
17      if (index < 0 || index >= s->size)
18          return (0);
19      return (s->tab[(s->head + index) % s->capacity]);
20  }
```

| Ligne | Explication |
| ---: | --- |
| 15 | Renvoie l’élément logique d’index `index`. |
| 17-18 | Garde-fou : un index hors de la pile renvoie `0`. Cela évite un accès mémoire invalide. |
| 19 | Convertit l’index logique en index physique : `(head + index) % capacity`. Le modulo gère le tableau circulaire. |
| 20 | Fin. |

Exemple :

```text
capacity = 8, head = 6, size = 4
index logique 0 -> physique 6
index logique 1 -> physique 7
index logique 2 -> physique 0
index logique 3 -> physique 1
```

---

## 10. `ops_push.c` — `pb` et `pa`

### 10.1 `pb`

```c
15  void pb(t_stack *a, t_stack *b)
16  {
17      int value;
18
19      if (a->size == 0)
20          return ;
21      value = a->tab[a->head];
22      a->head = (a->head + 1) % a->capacity;
23      a->size--;
24      b->head = (b->head - 1 + b->capacity) % b->capacity;
25      b->tab[b->head] = value;
26      b->size++;
27      a->counts->pb++;
28      if (!a->quiet)
29          write(1, "pb\n", 3);
30  }
```

| Ligne | Explication |
| ---: | --- |
| 15 | Pousse le sommet de `a` au sommet de `b`. |
| 17 | Valeur à déplacer. |
| 19-20 | Si `a` est vide, rien à faire. |
| 21 | Récupère la valeur du sommet de `a`. |
| 22 | Avance le sommet de `a` : l’élément suivant devient le nouveau sommet. |
| 23 | `a` perd un élément. |
| 24 | Recule le sommet de `b` pour faire de la place au-dessus de l’ancien sommet. Le `+ b->capacity` évite un index négatif avant le modulo. |
| 25 | Écrit la valeur au nouveau sommet de `b`. |
| 26 | `b` gagne un élément. |
| 27 | Incrémente le compteur `pb`. |
| 28-29 | Si la pile n’est pas silencieuse, écrit `pb\n` sur `stdout`. |
| 30 | Fin. |

Point important : `b` doit avoir une `capacity` suffisante. C’est le cas car
`b` est allouée avec `a->capacity`, soit `n`.

### 10.2 `pa`

```c
32  void pa(t_stack *a, t_stack *b)
33  {
34      int value;
35
36      if (b->size == 0)
37          return ;
38      value = b->tab[b->head];
39      b->head = (b->head + 1) % b->capacity;
40      b->size--;
41      a->head = (a->head - 1 + a->capacity) % a->capacity;
42      a->tab[a->head] = value;
43      a->size++;
44      a->counts->pa++;
45      if (!a->quiet)
46          write(1, "pa\n", 3);
47  }
```

| Ligne | Explication |
| ---: | --- |
| 32 | Pousse le sommet de `b` au sommet de `a`. |
| 34 | Valeur à déplacer. |
| 36-37 | Si `b` est vide, rien à faire. |
| 38 | Récupère la valeur du sommet de `b`. |
| 39 | Avance le sommet de `b`. |
| 40 | `b` perd un élément. |
| 41 | Recule le sommet de `a` pour faire de la place au-dessus de l’ancien sommet. |
| 42 | Écrit la valeur au nouveau sommet de `a`. |
| 43 | `a` gagne un élément. |
| 44 | Incrémente le compteur `pa`. |
| 45-46 | Affiche `pa` si la pile n’est pas silencieuse. |
| 47 | Fin. |

C’est `pa` qui réalise l’insertion : après les rotations, le successeur est au
sommet de `a`, et `pa` place `value` juste au-dessus, donc juste avant lui dans
l’ordre du haut vers le fond.

---

## 11. `ops_rotate.c` — `ra`, `rb`, `rr`

### 11.1 `ra`

```c
15  void ra(t_stack *a)
16  {
17      int value;
18      int tail;
19
20      if (a->size < 2)
21          return ;
22      value = a->tab[a->head];
23      tail = (a->head + a->size) % a->capacity;
24      a->tab[tail] = value;
25      a->head = (a->head + 1) % a->capacity;
26      a->counts->ra++;
27      if (!a->quiet)
28          write(1, "ra\n", 3);
29  }
```

| Ligne | Explication |
| ---: | --- |
| 15 | Rotation vers le haut de `a`. |
| 17 | Valeur du sommet. |
| 18 | Index physique de la nouvelle queue. |
| 20-21 | Une pile de taille 0 ou 1 ne change pas. |
| 22 | Récupère le sommet. |
| 23 | Calcule l’index juste après le dernier élément : `(head + size) % capacity`. |
| 24 | Écrit le sommet à cet emplacement, donc il devient le nouveau fond. |
| 25 | Avance le sommet : l’ancien deuxième élément devient le nouveau sommet. |
| 26 | Compteur. |
| 27-28 | Affichage. |
| 29 | Fin. |

### 11.2 `rb`

```c
31  void rb(t_stack *b)
32  {
33      int value;
34      int tail;
35
36      if (b->size < 2)
37          return ;
38      value = b->tab[b->head];
39      tail = (b->head + b->size) % b->capacity;
40      b->tab[tail] = value;
41      b->head = (b->head + 1) % b->capacity;
42      b->counts->rb++;
43      if (!b->quiet)
44          write(1, "rb\n", 3);
45  }
```

| Ligne | Explication |
| ---: | --- |
| 31 | Rotation vers le haut de `b`. |
| 33 | Valeur du sommet. |
| 34 | Index physique de la nouvelle queue. |
| 36-37 | Une pile de taille 0 ou 1 ne change pas. |
| 38 | Récupère le sommet de `b`. |
| 39 | Calcule l’index juste après le dernier élément. |
| 40 | Écrit le sommet au fond. |
| 41 | Avance le sommet de `b`. |
| 42 | Compteur `rb`. |
| 43-44 | Affichage. |
| 45 | Fin. |

Même logique que `ra`, mais appliquée à `b`.

### 11.3 `rr`

```c
47  void rr(t_stack *a, t_stack *b)
48  {
49      int value;
50      int tail;
51
52      if (a->size < 2 && b->size < 2)
53          return ;
54      if (a->size >= 2)
55      {
56          value = a->tab[a->head];
57          tail = (a->head + a->size) % a->capacity;
58          a->tab[tail] = value;
59          a->head = (a->head + 1) % a->capacity;
60      }
61      if (b->size >= 2)
62      {
63          value = b->tab[b->head];
64          tail = (b->head + b->size) % b->capacity;
65          b->tab[tail] = value;
66          b->head = (b->head + 1) % b->capacity;
67      }
68      a->counts->rr++;
69      if (!a->quiet)
70          write(1, "rr\n", 3);
71  }
```

| Ligne | Explication |
| ---: | --- |
| 47 | Effectue `ra` et `rb` en une seule opération. |
| 49-50 | Variables temporaires. |
| 52-53 | Si les deux piles ont moins de deux éléments, rien à faire. |
| 54-60 | Si `a` a au moins deux éléments, effectue la rotation de `a`. |
| 61-67 | Si `b` a au moins deux éléments, effectue la rotation de `b`. |
| 68 | Incrémente le compteur `rr`. |
| 69-70 | Affiche `rr`. |
| 71 | Fin. |

Nuance : `rr` incrémente le compteur même si une seule des deux piles a
réellement tourné (parce que l’autre avait moins de deux éléments). Dans
`medium`, `rotate_ab` n’appelle `rr` que lorsque les deux coûts sont positifs,
donc les deux piles tournent réellement.

---

## 12. `ops_reverse.c` — `rra`, `rrb`, `rrr`

### 12.1 `rra`

```c
15  void rra(t_stack *a)
16  {
17      int value;
18      int new_head;
19
20      if (a->size < 2)
21          return ;
22      value = a->tab[(a->head + a->size - 1) % a->capacity];
23      new_head = (a->head - 1 + a->capacity) % a->capacity;
24      a->tab[new_head] = value;
25      a->head = new_head;
26      a->counts->rra++;
27      if (!a->quiet)
28          write(1, "rra\n", 4);
29  }
```

| Ligne | Explication |
| ---: | --- |
| 15 | Rotation vers le bas de `a`. |
| 17 | Valeur du fond. |
| 18 | Nouvel index du sommet. |
| 20-21 | Rien à faire pour moins de deux éléments. |
| 22 | Récupère le dernier élément logique : index `head + size - 1`. |
| 23 | Calcule l’index physique juste avant le sommet actuel. |
| 24 | Écrit le fond à cet emplacement : il devient le nouveau sommet. |
| 25 | Met à jour `head`. |
| 26 | Compteur. |
| 27-28 | Affichage. |
| 29 | Fin. |

### 12.2 `rrb`

```c
31  void rrb(t_stack *b)
32  {
33      int value;
34      int new_head;
35
36      if (b->size < 2)
37          return ;
38      value = b->tab[(b->head + b->size - 1) % b->capacity];
39      new_head = (b->head - 1 + b->capacity) % b->capacity;
40      b->tab[new_head] = value;
41      b->head = new_head;
42      b->counts->rrb++;
43      if (!b->quiet)
44          write(1, "rrb\n", 4);
45  }
```

| Ligne | Explication |
| ---: | --- |
| 31 | Rotation vers le bas de `b`. |
| 33 | Valeur du fond. |
| 34 | Nouvel index du sommet. |
| 36-37 | Rien à faire pour moins de deux éléments. |
| 38 | Récupère le dernier élément logique de `b`. |
| 39 | Calcule l’index physique juste avant le sommet actuel. |
| 40 | Écrit le fond au nouvel emplacement du sommet. |
| 41 | Met à jour `head`. |
| 42 | Compteur `rrb`. |
| 43-44 | Affichage. |
| 45 | Fin. |

Même logique que `rra`, mais pour `b`.

### 12.3 `rrr`

```c
47  void rrr(t_stack *a, t_stack *b)
48  {
49      int value;
50      int new_head;
51
52      if (a->size < 2 && b->size < 2)
53          return ;
54      if (a->size >= 2)
55      {
56          value = a->tab[(a->head + a->size - 1) % a->capacity];
57          new_head = (a->head - 1 + a->capacity) % a->capacity;
58          a->tab[new_head] = value;
59          a->head = new_head;
60      }
61      if (b->size >= 2)
62      {
63          value = b->tab[(b->head + b->size - 1) % b->capacity];
64          new_head = (b->head - 1 + b->capacity) % b->capacity;
65          b->tab[new_head] = value;
66          b->head = new_head;
67      }
68      a->counts->rrr++;
69      if (!a->quiet)
70          write(1, "rrr\n", 4);
71  }
```

| Ligne | Explication |
| ---: | --- |
| 47 | Effectue `rra` et `rrb` en une seule opération. |
| 49-50 | Variables temporaires. |
| 52-53 | Si les deux piles ont moins de deux éléments, rien à faire. |
| 54-60 | Rotation inverse de `a` si possible. |
| 61-67 | Rotation inverse de `b` si possible. |
| 68 | Compteur `rrr`. |
| 69-70 | Affichage. |
| 71 | Fin. |

Comme pour `rr`, le compteur est incrémenté même si une seule pile tourne
réellement. Dans `medium`, `rotate_ab` n’appelle `rrr` que lorsque les deux
coûts sont négatifs, donc les deux piles tournent réellement.

---

## 13. `ops_swap.c` — pourquoi `medium` ne l’utilise pas

`medium` n’appelle jamais `sa`, `sb` ou `ss`.

Pourquoi ? Parce que son algorithme est basé sur :

- des rotations (`ra`, `rra`, `rb`, `rrb`, `rr`, `rrr`) ;
- des push (`pb`, `pa`).

Il n’a pas besoin d’échanger deux éléments adjacents. Les swaps sont utilisés
par d’autres stratégies ou par des optimisations locales, mais pas ici.

---

## 14. Trace détaillée d’une insertion

Reprenons l’exemple `n = 10`, `div = 2`, `chunk_size = 5`.

Après la distribution :

```text
a = []
b = [6, 8, 5, 9, 7, 4, 3, 1, 0, 2]
```

On traite la bande `[5, 10)`, `chunk = 1`.

Première insertion : `a` est vide.

```text
best_in_range(a, b, [5,10), &value)
```

Il parcourt `b` :

```text
i=0 : value=6, score = 0
i=1 : value=8, score = 1
i=2 : value=5, score = 2
i=3 : value=9, score = 3
i=4 : value=7, score = 4
```

Le meilleur est `6`, score `0`.

`complex_insert(a, b, 6)` :

```text
cost_a = get_cost(complex_target_index(a, 6), a->size)
```

`a` est vide, donc `best_greater` renvoie `-1`, `complex_min_index` renvoie
`0`, `get_cost(0, 0)` renvoie `0`.

```text
cost_b = get_cost(complex_find(b, 6), b->size)
```

`6` est à l’index `0` de `b`, donc `get_cost(0, 10) = 0`.

Aucune rotation. `pa` :

```text
a = [6]
b = [8, 5, 9, 7, 4, 3, 1, 0, 2]
```

Deuxième insertion : `a = [6]`.

`best_in_range` parcourt `b`. Les candidats de `[5,10)` sont `8, 5, 9, 7` :

```text
8 : target index 0 (min de a), cost_a = 0, cost_b = 0 -> score 0
5 : target index 0 (6),      cost_a = 0, cost_b = 1 -> score 1
9 : target index 0 (min),    cost_a = 0, cost_b = 2 -> score 2
7 : target index 0 (min),    cost_a = 0, cost_b = 3 -> score 3
```

On choisit `8`. `pa` :

```text
a = [8, 6]
b = [5, 9, 7, 4, 3, 1, 0, 2]
```

`a` est triée circulairement : `6, 8`.

Troisième insertion : `a = [8, 6]`.

Candidats de `[5,10)` dans `b` :

```text
5 : target = index 1 (6), cost_a = +1, cost_b = 0 -> score 1
9 : target = index 0 (6, minimum), cost_a = 0, cost_b = 1 -> score 1
7 : target = index 0 (8), cost_a = 0, cost_b = 2 -> score 2
```

Il y a égalité entre `5` et `9` à score `1`.

`best_in_range` garde le **premier rencontré** : `5` est à l’index `0` de `b`,
`9` à l’index `1`. Donc `5` gagne.

`complex_insert(a, b, 5)` :

```text
cost_a = get_cost(complex_target_index(a, 5), 2)
complex_target_index(a, 5) = index de 6 = 1
get_cost(1, 2) = +1

cost_b = get_cost(complex_find(b, 5), 8)
complex_find(b, 5) = 0
get_cost(0, 8) = 0
```

`rotate_ab` fait donc un `ra` :

```text
a = [6, 8]
b = [5, 9, 7, 4, 3, 1, 0, 2]
```

Puis `pa` :

```text
a = [5, 6, 8]
b = [9, 7, 4, 3, 1, 0, 2]
```

`5` est bien inséré avant son successeur `6`. L’ordre circulaire est préservé.

Cette trace montre les points essentiels :

- le score dépend de la position du successeur dans `a` et de la position du
  candidat dans `b` ;
- le glouton recalcule les scores après chaque insertion ;
- l’égalité est résolue par l’ordre de parcours de `b` ;
- `pa` insère avant le successeur, pas après.

---

## 15. Checklist pour vérifier sa compréhension

1. Pourquoi `medium_chunks` capture-t-il `count = a->size` avant la boucle ?
2. Pourquoi ne teste-t-il que `rang < chunk * size` et pas la borne
   inférieure ?
3. Pourquoi `chunk` commence-t-il à `1` dans la distribution ?
4. Pourquoi `complex_pull` commence-t-il à `chunk = div` et pas `div - 1` ?
5. Pourquoi la première plage de `complex_pull` est-elle vide avec `fr = 1` ?
6. Pourquoi les bandes sont-elles traitées de la plus grande à la plus petite ?
7. Pourquoi `complex_target_index` renvoie-t-il l’index du minimum quand aucun
   élément n’est supérieur à `value` ?
8. Pourquoi `get_cost` renvoie-t-il un coût négatif pour la seconde moitié ?
9. Pourquoi `complex_cost` prend-il le maximum quand les signes sont identiques ?
10. Pourquoi `pa` insère-t-il avant le successeur ?
11. Pourquoi `a` est-elle seulement triée circulairement pendant la remontée ?
12. Pourquoi `complex_align` est-il nécessaire à la fin ?
13. Pourquoi `best_in_range` parcourt-il tout `b` à chaque insertion ?
14. Pourquoi cela ne change-t-il pas la complexité en opérations push_swap ?
15. Pourquoi `b` n’est-il pas trié après la phase de distribution ?

Si tu peux répondre à ces quinze questions sans regarder le code, tu as compris
l’essentiel du tri `medium`.
