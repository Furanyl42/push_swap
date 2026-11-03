# Code expliqué ligne par ligne — tri `--complex`

Ce document explique **le code C du tri `--complex`**, ligne par ligne.

Pour chaque ligne ou groupe de lignes inséparables, il répond à quatre
questions :

- **Quoi ?** Que fait exactement l’instruction ?
- **Pourquoi ?** Pourquoi est-elle nécessaire ?
- **Comment ?** Quel mécanisme C ou algorithmique est utilisé ?
- **Piège ?** Quel invariant faut-il respecter pour que le code reste correct ?

Il complète `explication_complex.md`, qui présente l’algorithme, les preuves et
les exemples. Ici, on se concentre sur la lecture du code.

> Les numéros de ligne correspondent aux fichiers actuels du dépôt.
> Aucun fichier source n’a été modifié pour rédiger ce document.

---

## Sommaire

1. Périmètre, conventions et structures
2. Contexte d’appel
3. `sort_complex.c` : point d’entrée et géométrie
4. `complex_presort.c` : normalisation et presort
5. `complex_pull.c` : coût, sélection et remontée
6. `complex_insert.c` : rotations et insertion
7. `complex_move.c` : recherche et alignement
8. Helpers partagés
9. Opérations `push_swap` utilisées
10. Trace commentée `n = 10`
11. Invariants, pièges et cas limites
12. Checklist de compréhension et tests

---

## 0. Périmètre, conventions et structures

### 0.1 Fichiers couverts

Le chemin d’exécution de `--complex` est :

```text
main.c
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

Fichiers réellement lus :

- `main.c`, `parsing.c`, `sort_dispatcher.c` : contexte ;
- `sort_complex.c` : point d’entrée ;
- `complex_presort.c` : `normalize_stack`, `presort_one`,
  `complex_presort` ;
- `complex_pull.c` : `complex_cost`, `candidate_score`, `best_in_range`,
  `pull_range`, `complex_pull` ;
- `complex_insert.c` : rotations et `complex_insert` ;
- `complex_move.c` : `complex_find`, `complex_min_index`, `best_greater`,
  `complex_target_index`, `complex_align` ;
- `array_utils.c`, `sorts_utils.c` : copie triée et rang ;
- `utils.c` : `get_cost` ;
- `stack.c` : `get_stack_element` ;
- `ops_push.c`, `ops_rotate.c`, `ops_reverse.c` : opérations ;
- `push_swap.h` : structures et prototypes.

`--complex` n’utilise pas `sa`, `sb`, `ss`.

### 0.2 Conventions

- Une pile est écrite **du sommet vers le fond** :
  `a = [3, 1, 2]` signifie `3` au sommet, `2` au fond.
- L’index logique `0` est le sommet.
- Les rangs sont les entiers `0 … n - 1`, une fois la normalisation faite.
- Les intervalles sont semi-ouverts : `[low, high)` contient `low` mais pas
  `high`.
- Une **opération** est une ligne émise sur `stdout` (`pb`, `ra`, etc.).
- Une **comparaison** ou un calcul de coût n’est pas une opération.
- `n` est la taille initiale de `a`. Pendant le tri, `a->size` et `b->size`
  changent.
- Quand une ligne contient plusieurs effets, ils sont expliqués ensemble.
- Les accolades fermantes et les lignes vides sont mentionnées quand elles
  délimitent un bloc important ; elles n’ont pas d’effet algorithmique.
- Les lignes 1 à 12 de chaque fichier `.c` sont l’en-tête de norme 42, et la
  ligne 13 est le `#include "push_swap.h"`. Ce sont des lignes de boilerplate,
  sans logique de tri ; elles ne sont donc pas détaillées une par une.
- Les commentaires de banner et les includes de `push_swap.h` ne produisent
  aucune instruction exécutée dans l’algorithme.

### 0.3 Structures de données

#### `t_stack` — `push_swap.h`, lignes 35-43

```c
typedef struct s_stack
{
	int				*tab;
	int				size;
	int				capacity;
	int				head;
	int				quiet;
	t_counts		*counts;
}					t_stack;
```

| Ligne | Explication |
| ---: | --- |
| 37 | `tab` est le tableau physique des valeurs. |
| 38 | `size` est le nombre d’éléments réellement présents. |
| 39 | `capacity` est la taille allouée du tableau. Pour `a` et `b`, elle vaut `n`. |
| 40 | `head` est l’index physique du sommet logique. La pile est un tableau circulaire. |
| 41 | `quiet` vaut `0` en mode normal : chaque opération est écrite. Le checker le passe à `1`. |
| 42 | `counts` pointe vers les compteurs partagés par `a` et `b`. |

Le couple `(head, size)` décrit la partie utile du tableau. L’élément logique
d’index `i` est stocké à l’index physique :

```text
(head + i) % capacity
```

Cette représentation permet à `ra`, `rra`, `pa` et `pb` de ne déplacer qu’un
index, sans recopier tout le tableau.

#### `t_counts` — `push_swap.h`, lignes 20-33

Chaque champ compte une opération :

```text
sa, sb, ss, pa, pb, ra, rb, rr, rra, rrb, rrr
```

`--bench` additionne ces champs avec `total_ops`.

#### `t_range` — `push_swap.h`, lignes 59-63

```c
typedef struct s_range
{
	int				low;
	int				high;
}					t_range;
```

`t_range` représente un intervalle de **rangs** `[low, high)`. Il sert à
sélectionner les éléments de `b` qui appartiennent à la partie haute ou basse
d’une bande.

#### `t_geom` — `push_swap.h`, lignes 65-70

```c
typedef struct s_geom
{
	int				div;
	int				fr;
	int				n;
}					t_geom;
```

| Champ | Rôle |
| --- | --- |
| `div` | diviseur qui fixe le nombre de bandes et `chunk_size = n / div`. |
| `fr` | diviseur qui coupe chaque bande en partie basse et partie haute. |
| `n` | taille initiale de `a`, conservée pendant tout le tri. |

`geom.n` est important : après le presort, `a->size` vaut `0`, donc on ne peut
plus retrouver `n` à partir de `a`. `complex_pull` utilise `geom->n`.

---

## 1. Contexte d’appel

### 1.1 `parsing.c` : choix de la stratégie

Lignes 37-46 :

```c
if (ft_strncmp(argv[i], "--simple", 9) == 0)
	opts->strategy = STRAT_SIMPLE;
else if (ft_strncmp(argv[i], "--medium", 9) == 0)
	opts->strategy = STRAT_MEDIUM;
else if (ft_strncmp(argv[i], "--complex", 10) == 0)
	opts->strategy = STRAT_COMPLEX;
else if (ft_strncmp(argv[i], "--adaptive", 11) == 0)
	opts->strategy = STRAT_ADAPTIVE;
else if (ft_strncmp(argv[i], "--bench", 8) == 0)
	opts->bench = 1;
else if (argv[i][0] == '-' && argv[i][1] == '-')
	return (0);
```

| Ligne | Explication |
| ---: | --- |
| 37 | Si l’argument est exactement `--simple`, on force la stratégie simple. |
| 39 | `--medium` force `STRAT_MEDIUM`. |
| 41 | `--complex` force `STRAT_COMPLEX`. C’est le sélecteur qui nous intéresse. |
| 43 | `--adaptive` remet la stratégie adaptative. |
| 45 | `--bench` active les statistiques sur `stderr`. |
| 47 | Tout autre argument commençant par `--` est refusé. |

`parse_flags` parcourt les arguments dans l’ordre. Si plusieurs sélecteurs sont
présents, **le dernier gagne**, car chaque sélecteur écrase `opts->strategy`.

### 1.2 `sort_dispatcher.c` : résolution et dispatch

Lignes 15-24 :

```c
t_strategy	resolve_strategy(t_strategy requested, double disorder)
{
	if (requested != STRAT_ADAPTIVE)
		return (requested);
	if (disorder < 0.2)
		return (STRAT_SIMPLE);
	if (disorder < 0.5)
		return (STRAT_MEDIUM);
	return (STRAT_COMPLEX);
}
```

- Si une stratégie est forcée, elle est renvoyée telle quelle.
- En adaptatif :
  - `d < 0.2` : simple ;
  - `0.2 <= d < 0.5` : medium ;
  - `d >= 0.5` : complex.

Lignes 26-36 :

```c
void	sort_stacks(t_stack *a, t_stack *b, t_strategy strat)
{
	if (a->size < 2)
		return ;
	if (strat == STRAT_SIMPLE)
		sort_simple(a, b);
	else if (strat == STRAT_MEDIUM)
		sort_medium(a, b);
	else if (strat == STRAT_COMPLEX)
		sort_complex(a, b);
}
```

`a->size < 2` évite tout traitement inutile. Ensuite, `STRAT_COMPLEX` appelle
directement `sort_complex(a, b)`.

### 1.3 `main.c` : lancement conditionnel

Lignes 29-40 :

```c
init_counts(&counts);
a->counts = &counts;
b = init_stack(a->capacity, &counts);
if (!b)
	return (0);
disorder = compute_disorder(a);
strat = resolve_strategy(opts->strategy, disorder);
if (disorder > 0.0)
	sort_stacks(a, b, strat);
if (opts->bench)
	print_bench(&counts, disorder, opts->strategy, strat);
free_stack(b);
```

Points importants pour `complex` :

- `b` est alloué avec `a->capacity`, donc peut recevoir jusqu’à `n` éléments.
- `b` est vide au départ.
- `a` et `b` partagent les mêmes compteurs.
- Si le désordre vaut exactement `0`, `sort_stacks` n’est pas appelé :
  aucune opération n’est émise.
- `sort_complex` peut donc supposer `b->size == 0`.

---

## 2. `sort_complex.c` : point d’entrée et géométrie

### 2.1 Code complet

```c
static void	complex_geometry(int n, int *div, int *fr)
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

void	sort_complex(t_stack *a, t_stack *b)
{
	t_geom	geom;

	geom.n = a->size;
	if (geom.n < 2)
		return ;
	if (!normalize_stack(a))
	{
		sort_simple(a, b);
		return ;
	}
	complex_geometry(geom.n, &geom.div, &geom.fr);
	complex_presort(a, b, geom.div, geom.fr);
	complex_pull(a, b, &geom);
}
```

### 2.2 `complex_geometry`, lignes 15-37

| Ligne | Explication |
| ---: | --- |
| 15 | Signature : reçoit `n` et deux pointeurs de sortie `div` et `fr`. |
| 16 | Ouverture du corps. |
| 17 | Premier cas : `n < 300`. |
| 18 | Ouverture du bloc. |
| 19 | `div = 2` : deux bandes principales. |
| 20 | `fr = 3` : la partie basse fait environ `1/3` de la bande. |
| 21 | Fermeture du bloc. |
| 22 | Sinon, si `n < 450`. |
| 23 | Ouverture. |
| 24 | `div = 3` : trois bandes. |
| 25 | `fr = 3`. |
| 26 | Fermeture. |
| 27 | Sinon, si `n < 800`. |
| 28 | Ouverture. |
| 29 | `div = 4` : quatre bandes. |
| 30 | `fr = 3`. |
| 31 | Fermeture. |
| 32 | Sinon, donc `n >= 800`. |
| 33 | Ouverture. |
| 34 | `div = 4`. |
| 35 | `fr = 4` : partie basse plus petite, environ `1/4`. |
| 36 | Fermeture. |
| 37 | Fermeture de la fonction. |

Les seuils sont testés dans l’ordre. Par exemple, `n = 299` entre dans le
premier cas ; `n = 300` dans le deuxième ; `n = 800` dans le quatrième.

Tableau récapitulatif :

| `n` | `div` | `fr` | `chunk_size = n / div` |
| ---: | ---: | ---: | ---: |
| `n < 300` | 2 | 3 | `n / 2` |
| `300 <= n < 450` | 3 | 3 | `n / 3` |
| `450 <= n < 800` | 4 | 3 | `n / 4` |
| `n >= 800` | 4 | 4 | `n / 4` |

Pourquoi un nombre de bandes constant ? Parce que le presort fait une passe par
bande. Avec `div <= 4`, il y a au plus `5` passes, donc `O(n)` opérations de
presort. `medium`, au contraire, utilise `O(sqrt(n))` bandes.

### 2.3 `sort_complex`, lignes 39-54

| Ligne | Explication |
| ---: | --- |
| 39 | Signature publique appelée par `sort_stacks`. |
| 40 | Ouverture. |
| 41 | Déclare la structure de géométrie. |
| 43 | Mémorise la taille initiale dans `geom.n`. |
| 44 | Teste si la pile a moins de deux éléments. |
| 45 | Dans ce cas, rien à trier : retour immédiat. |
| 46 | Tente de normaliser `a`. `normalize_stack` renvoie `0` en cas d’échec d’allocation. |
| 47 | Ouverture du bloc d’échec. |
| 48 | Repli sur `sort_simple`, qui sait trier les valeurs d’origine. |
| 49 | Retour après le repli. |
| 50 | Fermeture du bloc. |
| 51 | Calcule `div` et `fr` à partir de `geom.n`. |
| 52 | Lance le presort de `a` vers `b`. |
| 53 | Lance la remontée de `b` vers `a`, puis l’alignement final. |
| 54 | Fermeture. |

Pourquoi `geom.n` est-il capturé avant la normalisation ? Parce que la
normalisation ne change pas la taille, mais surtout parce que le presort vide
`a`. Après le presort, `a->size == 0` ; `geom.n` conserve donc la taille
d’origine pour `complex_pull`.

Pourquoi le repli sur `sort_simple` ? `normalize_stack` alloue un tableau
temporaire. Si `malloc` échoue, on ne peut pas calculer les rangs. Mieux vaut
trier les valeurs originales avec une stratégie qui n’a pas besoin de cette
allocation.

---

## 3. `complex_presort.c` : normalisation et presort

### 3.1 `normalize_stack`, lignes 15-33

```c
int	normalize_stack(t_stack *a)
{
	int	*sorted;
	int	index;
	int	i;

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
}
```

| Ligne | Explication |
| ---: | --- |
| 15 | Signature : normalise `a` en place et renvoie `1` en cas de succès. |
| 16 | Ouverture. |
| 17 | `sorted` recevra une copie triée des valeurs. |
| 18 | `index` sera l’index **physique** dans le tableau circulaire. |
| 19 | `i` est l’index **logique**, de `0` à `a->size - 1`. |
| 21 | Construit une copie triée par ordre croissant. |
| 22 | Vérifie l’échec d’allocation. |
| 23 | En cas d’échec, renvoie `0` sans modifier `a`. |
| 24 | Initialise le parcours logique. |
| 25 | Parcourt tous les éléments de `a`. |
| 26 | Ouverture. |
| 27 | Convertit l’index logique `i` en index physique : `(head + i) % capacity`. |
| 28 | Remplace la valeur par son rang dans la copie triée. |
| 29 | Passe à l’élément suivant. |
| 30 | Fermeture de la boucle. |
| 31 | Libère la copie triée : elle ne sert plus. |
| 32 | Succès. |
| 33 | Fermeture. |

Détail crucial de la ligne 28 : en C, le membre de droite est évalué avant
l’affectation. Donc `a->tab[index]` à droite est **l’ancienne valeur**. On
cherche son rang, puis on écrit ce rang dans la même case. On ne perd pas la
valeur avant de l’avoir cherchée.

Exemple :

```text
valeurs : 42  -7  100  3
copie triée : -7  3  42  100
rangs : 2  0  3  1
```

Pourquoi normaliser ? Les bandes sont des intervalles de rangs. Sans
normalisation, il faudrait calculer des bornes dépendant du minimum et du
maximum, et les écarts entre valeurs rendraient les bandes irrégulières. Avec
les rangs, chaque bande contient exactement le même nombre d’éléments, à la
dernière bande près.

Coût :

- `build_sorted_copy` : `O(n)` copies + `O(n²)` tri par insertion ;
- `find_rank` : `O(log n)` par élément, donc `O(n log n)` ;
- total CPU : `O(n²)` ;
- aucune opération `push_swap` ;
- mémoire temporaire : `O(n)`.

### 3.2 `presort_one`, lignes 35-45

```c
static void	presort_one(t_stack *a, t_stack *b, int limit, int sink)
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
```

| Ligne | Explication |
| ---: | --- |
| 35 | Signature : traite un seul élément du sommet de `a`. |
| 36 | Ouverture. |
| 37 | Si le sommet de `a` est strictement inférieur à `limit`. |
| 38 | Ouverture. |
| 39 | L’élément appartient à la bande courante : on le pousse dans `b`. |
| 40 | Après `pb`, le sommet de `b` est l’élément poussé. On teste s’il appartient à la partie basse. |
| 41 | Si oui, `rb` l’envoie au fond de `b`. |
| 42 | Fermeture. |
| 43 | Sinon, l’élément appartient à une bande future. |
| 44 | On le remet au fond de `a` avec `ra`. |
| 45 | Fermeture. |

Pourquoi seulement `sommet < limit` et pas aussi `sommet >= base` ? Parce que
les bandes précédentes ont déjà été retirées. Au début de la passe `chunk`, tout
élément restant a un rang au moins égal à `chunk * chunk_size`. Donc la seule
borne utile est la borne supérieure.

Pourquoi tester `b` après `pb` plutôt que la valeur d’origine ? Parce que `pb`
place exactement la valeur poussée au sommet de `b`. Le test est donc équivalent
et évite de garder une variable temporaire. C’est un choix de concision.

Pourquoi `rb` pour les valeurs basses ? Le pull traitera plus tard la partie
haute au sommet de `b`, puis la partie basse au fond. En envoyant les valeurs
basses au fond dès le presort, on prépare cette structure.

### 3.3 `complex_presort`, lignes 47-70

```c
void	complex_presort(t_stack *a, t_stack *b, int div, int fr)
{
	int	chunk_size;
	int	chunk;
	int	count;
	int	i;

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

| Ligne | Explication |
| ---: | --- |
| 47 | Signature : `a` source, `b` destination, `div` et `fr` issus de la géométrie. |
| 48 | Ouverture. |
| 49 | Largeur d’une bande. |
| 50 | Numéro de la bande courante. |
| 51 | Nombre d’éléments présents au début d’une passe. |
| 52 | Compteur d’itérations de la boucle interne. |
| 54 | Calcule `chunk_size = n / div` avec division entière. |
| 55 | Si la division donne `0` (petite taille), on force au moins `1`. |
| 56 | Évite une largeur de bande nulle et une boucle infinie. |
| 57 | La première bande est la bande `0`. |
| 58 | Tant que `a` contient des éléments. |
| 59 | Ouverture de la passe. |
| 60 | Mémorise la taille **avant** la passe. |
| 61 | Réinitialise le compteur interne. |
| 62 | Fait exactement `count` itérations. |
| 63 | Ouverture. |
| 64 | Appelle `presort_one` avec la borne supérieure de la bande : `(chunk + 1) * chunk_size`. |
| 65 | Calcule la frontière haut/bas : `chunk * chunk_size + chunk_size / fr`. |
| 66 | Incrémente le compteur d’itérations. |
| 67 | Fermeture de la boucle interne. |
| 68 | Passe à la bande suivante. |
| 69 | Fermeture de la boucle externe. |
| 70 | Fermeture. |

Pourquoi `chunk` commence à `0` alors que `medium_chunks` commence à `1` ?
Parce que la limite utilisée ici est `(chunk + 1) * chunk_size`, pas
`chunk * chunk_size` :

```text
chunk = 0 -> limite = chunk_size       -> bande [0, chunk_size)
chunk = 1 -> limite = 2 * chunk_size   -> bande [chunk_size, 2*chunk_size)
```

Pourquoi `count = a->size` avant la boucle interne ? Parce que `pb` réduit
`a->size`. Si on écrivait `while (i < a->size)`, la borne changerait à chaque
`pb` et on risquerait de sauter des éléments. Le snapshot `count` garantit que
chaque élément présent au début de la passe est examiné exactement une fois.

Pourquoi la boucle interne ne risque-t-elle pas d’appeler `presort_one` sur une
pile vide ? Avant l’itération `i`, au plus `i` éléments ont été poussés. Donc
`a->size >= count - i`. Pour `i <= count - 1`, `a->size >= 1`. Le dernier appel
est donc sûr.

Combien de passes ? Soit `s = chunk_size`. La dernière bande non vide est
`ceil(n / s) - 1`. Donc :

```text
P = ceil(n / s) = div ou div + 1
```

Avec `div <= 4`, `P <= 5`. Chaque passe examine au plus `n` éléments. Le presort
émet exactement `n` opérations `pb`, plus au plus `n` opérations `rb`, et un
nombre d’opérations `ra` inférieur à `n * P`. C’est donc `O(n)` opérations,
avec une constante liée à `P`.

Pourquoi les `rb` ne sont-ils pas toujours comptés ? `rb` ne fait rien si `b`
contient moins de deux éléments. L’appel est alors un no-op : aucune ligne
n’est écrite et le compteur `rb` n’est pas incrémenté.

---

## 4. `complex_pull.c` : coût, sélection et remontée

### 4.1 `complex_cost`, lignes 15-33

```c
int	complex_cost(int cost_a, int cost_b)
{
	int	abs_a;
	int	abs_b;

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

| Ligne | Explication |
| ---: | --- |
| 15 | Signature : coût signé dans `a` et coût signé dans `b`. |
| 16 | Ouverture. |
| 17 | Valeur absolue du coût dans `a`. |
| 18 | Valeur absolue du coût dans `b`. |
| 20 | Copie `cost_a`. |
| 21 | Teste la négativité. |
| 22 | Rend positif. |
| 23 | Copie `cost_b`. |
| 24 | Teste la négativité. |
| 25 | Rend positif. |
| 26 | Détecte si les deux rotations vont dans le **même sens**. |
| 27 | Ouverture. |
| 28 | Compare les amplitudes. |
| 29 | Renvoie la plus grande si `a` est plus loin. |
| 30 | Sinon renvoie celle de `b`. |
| 31 | Fermeture. |
| 32 | Signes opposés ou coût nul : les rotations ne peuvent pas être fusionnées. |
| 33 | Fermeture. |

Règle :

```text
même signe strictement positif ou strictement négatif :
    coût = max(|cost_a|, |cost_b|)

sinon :
    coût = |cost_a| + |cost_b|
```

Exemples :

| `cost_a` | `cost_b` | Coût | Pourquoi |
| ---: | ---: | ---: | --- |
| `+3` | `+5` | 5 | trois `rr`, puis deux `rb` |
| `-4` | `-2` | 4 | deux `rrr`, puis deux `rra` |
| `+3` | `-2` | 5 | pas de fusion possible |
| `0` | `-4` | 4 | quatre `rrb` |
| `0` | `0` | 0 | rien à faire |

Le cas `cost_a == 0` n’est pas dans la branche « même signe » : zéro n’est ni
strictement positif ni strictement négatif. Le résultat `abs_a + abs_b` reste
correct.

### 4.2 `candidate_score`, lignes 35-43

```c
static int	candidate_score(t_stack *a, t_stack *b, int value, int index)
{
	int	cost_a;
	int	cost_b;

	cost_a = get_cost(complex_target_index(a, value), a->size);
	cost_b = get_cost(index, b->size);
	return (complex_cost(cost_a, cost_b));
}
```

| Ligne | Explication |
| ---: | --- |
| 35 | Signature : évalue un candidat `value` situé à l’index `index` dans `b`. |
| 36 | Ouverture. |
| 37 | Coût signé pour amener la position d’insertion au sommet de `a`. |
| 38 | Coût signé pour amener le candidat au sommet de `b`. |
| 40 | Cherche l’index du successeur de `value` dans `a`, puis convertit cet index en coût signé. |
| 41 | Convertit l’index du candidat dans `b` en coût signé. |
| 42 | Combine les deux coûts. |
| 43 | Fermeture. |

`complex_target_index` parcourt `a` pour trouver le successeur. C’est un coût
CPU `O(|a|)` par candidat. `get_cost` est `O(1)`.

### 4.3 `best_in_range`, lignes 45-70

```c
static int	best_in_range(t_stack *a, t_stack *b, t_range *range, int *out)
{
	int	i;
	int	best;
	int	score;
	int	value;

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

| Ligne | Explication |
| ---: | --- |
| 45 | Signature : cherche le meilleur candidat de `b` dans `range`, et l’écrit dans `*out`. |
| 46 | Ouverture. |
| 47 | Index logique dans `b`. |
| 48 | Meilleur score trouvé. |
| 49 | Score du candidat courant. |
| 50 | Valeur du candidat courant. |
| 52 | Part du sommet de `b`. |
| 53 | Sentinelle `INT_MAX` : aucun score réel ne peut l’atteindre. |
| 54 | Initialise la sortie à `0` ; cette valeur ne sera utilisée que si la fonction renvoie `1`. |
| 55 | Parcourt toute la pile `b`. |
| 56 | Ouverture. |
| 57 | Lit l’élément logique d’index `i`. |
| 58 | Filtre : garde uniquement les rangs de `[low, high)`. |
| 59 | Ouverture. |
| 60 | Calcule le score du candidat. |
| 61 | Compare au meilleur score courant. |
| 62 | Ouverture. |
| 63 | Met à jour le meilleur score. |
| 64 | Mémorise la valeur gagnante. |
| 65 | Fermeture. |
| 66 | Fermeture du filtre. |
| 67 | Index suivant. |
| 68 | Fermeture de la boucle. |
| 69 | Renvoie `1` si au moins un candidat a été trouvé. |
| 70 | Fermeture. |

En cas d’égalité de score, le test strict `<` conserve le **premier** candidat
rencontré. Comme le parcours part de l’index `0`, cela favorise l’élément le
plus proche du sommet de `b`. Ce n’est pas indispensable à la correction, mais
c’est un choix déterministe et souvent économique.

Pourquoi `best != 2147483647` plutôt qu’un booléen séparé ? Parce que la
sentinelle sert à la fois de score initial et de témoin de « aucun candidat ».
Aucun score réel ne peut valoir `INT_MAX` : les coûts sont au plus `n`, donc
leur somme est bien inférieure.

### 4.4 `pull_range`, lignes 72-78

```c
static void	pull_range(t_stack *a, t_stack *b, t_range *range)
{
	int	value;

	while (best_in_range(a, b, range, &value))
		complex_insert(a, b, value);
}
```

| Ligne | Explication |
| ---: | --- |
| 72 | Signature : vide de `b` tous les éléments de `range`, vers `a`. |
| 73 | Ouverture. |
| 74 | Recevra la valeur choisie. |
| 76 | Tant qu’il existe un élément de `range` dans `b`… |
| 77 | … on l’insère dans `a` au meilleur coût. |
| 78 | Fermeture. |

La boucle termine car chaque `complex_insert` retire exactement un élément de
`b`. La plage est finie et ne reçoit aucun nouvel élément. Après au plus
`b->size` insertions, `best_in_range` renvoie `0`.

### 4.5 `complex_pull`, lignes 80-105

```c
void	complex_pull(t_stack *a, t_stack *b, t_geom *geom)
{
	int		chunk_size;
	int		chunk;
	int		base;
	int		mid;
	t_range	range;

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

| Ligne | Explication |
| ---: | --- |
| 80 | Signature : remonte `b` vers `a` selon la géométrie. |
| 81 | Ouverture. |
| 82 | Largeur de bande recalculée. |
| 83 | Numéro de bande, du plus grand au plus petit. |
| 84 | Début de la bande courante. |
| 85 | Frontière entre partie basse et partie haute. |
| 86 | Intervalle courant. |
| 88 | Recalcule `chunk_size = n / div`. |
| 89 | Protection contre une largeur nulle. |
| 90 | Force `1` dans ce cas. |
| 91 | Commence à `chunk = div`, pour couvrir une éventuelle bande partielle. |
| 92 | Descend jusqu’à `chunk = 0`. |
| 93 | Ouverture. |
| 94 | `base = chunk * chunk_size`. |
| 95 | `mid = base + chunk_size / fr`. |
| 96 | La première plage commence à `mid`. |
| 97 | Elle finit à `base + chunk_size` (exclu). |
| 98 | Traite la partie **haute** en premier. |
| 99 | La seconde plage commence à `base`. |
| 100 | Elle finit à `mid` (exclu). |
| 101 | Traite la partie **basse** ensuite. |
| 102 | Bande précédente. |
| 103 | Fermeture. |
| 104 | Aligne le minimum de `a` au sommet. |
| 105 | Fermeture. |

Pourquoi commencer à `chunk = div` et non à `P - 1` ? Parce que le code ne
calcule pas `P`. Il sait seulement que `P` vaut `div` ou `div + 1`. En
commençant à `div`, il couvre toujours l’éventuelle bande partielle. Si `n` est
divisible par `div`, les deux plages de `chunk = div` sont vides et les appels
ne font rien.

Exemple `n = 13`, `div = 2`, `chunk_size = 6`, `fr = 3` :

```text
chunk = 2 : base = 12, mid = 14
  haute : [14, 18) -> vide
  basse : [12, 14) -> rang 12

chunk = 1 : base = 6, mid = 8
  haute : [8, 12) -> rangs 8, 9, 10, 11
  basse : [6, 8)  -> rangs 6, 7

chunk = 0 : base = 0, mid = 2
  haute : [2, 6)  -> rangs 2, 3, 4, 5
  basse : [0, 2)  -> rangs 0, 1
```

Pourquoi traiter la partie haute avant la partie basse ? Parce que le presort a
placé les parties hautes au sommet de `b` et les parties basses au fond. Pour la
bande `chunk = c`, la partie haute est immédiatement accessible par `rb`, tandis
que la partie basse est au fond, accessible par `rrb`. En traitant la partie
haute d’abord, on retire ce qui est au sommet avant d’aller chercher le fond.

Pourquoi les bandes sont-elles traitées de la plus grande à la plus petite ?
Parce que le presort a empilé les parties hautes dans l’ordre :

```text
H_{P-1}, H_{P-2}, ..., H_0
```

En commençant par la plus grande, on retire toujours `H_c` au sommet. Si on
commençait par la plus petite, il faudrait traverser les parties hautes des
bandes supérieures pour atteindre `H_0`.

---

## 5. `complex_insert.c` : rotations et insertion

### 5.1 `complex_rotate_a`, lignes 15-27

```c
static void	complex_rotate_a(t_stack *a, int cost)
{
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

| Ligne | Explication |
| ---: | --- |
| 15 | Signature : applique un coût signé à `a`. |
| 16 | Ouverture. |
| 17 | Tant que le coût est positif. |
| 18 | Ouverture. |
| 19 | Rotation vers le haut : le sommet part au fond. |
| 20 | Décrémente le coût restant. |
| 21 | Fermeture. |
| 22 | Tant que le coût est négatif. |
| 23 | Ouverture. |
| 24 | Rotation vers le bas : le fond remonte au sommet. |
| 25 | Incrémente le coût vers zéro. |
| 26 | Fermeture. |
| 27 | Fermeture. |

Un coût positif signifie « le successeur est dans la première moitié » : on
monte les éléments avec `ra`. Un coût négatif signifie « il est dans la seconde
moitié » : on descend avec `rra`.

### 5.2 `complex_rotate_b`, lignes 29-41

```c
static void	complex_rotate_b(t_stack *b, int cost)
{
	while (cost > 0)
	{
		rb(b);
		cost--;
	}
	while (cost < 0)
	{
		rrb(b);
		cost++;
	}
}
```

| Ligne | Explication |
| ---: | --- |
| 29 | Signature : applique un coût signé à `b`. |
| 30 | Ouverture. |
| 31 | Tant que le coût est positif. |
| 32 | Ouverture. |
| 33 | Rotation vers le haut de `b`. |
| 34 | Décrémente. |
| 35 | Fermeture. |
| 36 | Tant que le coût est négatif. |
| 37 | Ouverture. |
| 38 | Rotation vers le bas de `b`. |
| 39 | Incrémente. |
| 40 | Fermeture. |
| 41 | Fermeture. |

C’est exactement le même principe que pour `a`, avec les opérations de `b`.

### 5.3 `rotate_ab`, lignes 43-59

```c
static void	rotate_ab(t_stack *a, t_stack *b, int cost_a, int cost_b)
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

| Ligne | Explication |
| ---: | --- |
| 43 | Signature : combine les rotations de `a` et `b`. |
| 44 | Ouverture. |
| 45 | Tant que les deux coûts sont positifs. |
| 46 | Ouverture. |
| 47 | `rr` fait tourner les deux piles en une seule opération. |
| 48 | Décrémente le coût de `a`. |
| 49 | Décrémente le coût de `b`. |
| 50 | Fermeture. |
| 51 | Tant que les deux coûts sont négatifs. |
| 52 | Ouverture. |
| 53 | `rrr` fait tourner les deux piles vers le bas. |
| 54 | Incrémente le coût de `a`. |
| 55 | Incrémente le coût de `b`. |
| 56 | Fermeture. |
| 57 | Termine les rotations restantes de `a`. |
| 58 | Termine les rotations restantes de `b`. |
| 59 | Fermeture. |

Pourquoi commencer par les rotations communes ? Parce que `rr` et `rrr`
consomment une seule opération pour deux rotations. Si on faisait d’abord les
rotations individuelles, on perdrait cette économie.

Pourquoi les deux boucles `while` suffisent-elles ? Après la première boucle,
au moins un des deux coûts n’est plus strictement positif. Après la seconde, au
moins un des deux n’est plus strictement négatif. Les deux coûts ne peuvent donc
pas encore avoir le même signe non nul : les appels individuels traitent le
reste. Si les signes sont opposés dès le départ, aucune boucle commune ne
s’exécute.

### 5.4 `complex_insert`, lignes 61-70

```c
void	complex_insert(t_stack *a, t_stack *b, int value)
{
	int	cost_a;
	int	cost_b;

	cost_a = get_cost(complex_target_index(a, value), a->size);
	cost_b = get_cost(complex_find(b, value), b->size);
	rotate_ab(a, b, cost_a, cost_b);
	pa(a, b);
}
```

| Ligne | Explication |
| ---: | --- |
| 61 | Signature : insère `value` de `b` vers `a`. |
| 62 | Ouverture. |
| 63 | Coût signé dans `a`. |
| 64 | Coût signé dans `b`. |
| 66 | Trouve le successeur de `value` dans `a`, puis son coût. |
| 67 | Trouve `value` dans `b`, puis son coût. |
| 68 | Applique les rotations, en fusionnant `rr`/`rrr` si possible. |
| 69 | Pousse le sommet de `b` sur `a`. |
| 70 | Fermeture. |

Pourquoi `complex_target_index(a, value)` donne-t-il la bonne position ? Dans
une pile triée circulairement, `value` doit être inséré juste **avant** son
successeur, c’est-à-dire le plus petit élément strictement supérieur. Si aucun
élément n’est supérieur, le successeur circulaire est le minimum.

Après les rotations, le successeur est au sommet de `a`. `pa` place `value`
au-dessus de lui :

```text
avant : a = [successeur, ...]
après : a = [value, successeur, ...]
```

L’ordre circulaire est donc préservé.

Pourquoi `complex_find` est-il nécessaire ? Parce que `value` est connue, mais
sa position dans `b` ne l’est pas. `complex_find` la retrouve par recherche
linéaire. Le coût est `O(|b|)`.

Pourquoi ne pas utiliser directement le coût calculé par `best_in_range` ?
`best_in_range` renvoie seulement la valeur, pas les coûts. `complex_insert`
recalcule les deux coûts. C’est redondant mais simple. Entre les deux appels,
`a` et `b` n’ont pas changé, donc les coûts sont identiques.

---

## 6. `complex_move.c` : recherche et alignement

### 6.1 `complex_find`, lignes 15-27

```c
int	complex_find(t_stack *s, int value)
{
	int	i;

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

| Ligne | Explication |
| ---: | --- |
| 15 | Signature : cherche `value` dans la pile `s`. |
| 16 | Ouverture. |
| 17 | Index logique. |
| 19 | Part du sommet. |
| 20 | Parcourt la pile. |
| 21 | Ouverture. |
| 22 | Compare l’élément courant à `value`. |
| 23 | Si égal, renvoie l’index. |
| 24 | Sinon, avance. |
| 25 | Fermeture. |
| 26 | Valeur non trouvée : renvoie `0` par défaut. |
| 27 | Fermeture. |

Le `return (0)` final est un choix de robustesse. Dans le flux normal,
`complex_insert` est appelé avec une valeur qui vient d’être trouvée dans `b`
par `best_in_range`, donc `value` existe. Les rangs étant uniques, il n’y a
aucune ambiguïté.

### 6.2 `complex_min_index`, lignes 29-43

```c
int	complex_min_index(t_stack *a)
{
	int	idx;
	int	i;

	idx = 0;
	i = 1;
	while (i < a->size)
	{
		if (get_stack_element(a, i) < get_stack_element(a, idx))
			idx = i;
		i++;
	}
	return (idx);
}
```

| Ligne | Explication |
| ---: | --- |
| 29 | Signature : renvoie l’index du minimum de `a`. |
| 30 | Ouverture. |
| 31 | Index du minimum courant. |
| 32 | Index de parcours. |
| 34 | Suppose que le sommet est le minimum initial. |
| 35 | Commence la comparaison à l’index `1`. |
| 36 | Parcourt le reste de la pile. |
| 37 | Ouverture. |
| 38 | Compare l’élément courant au minimum courant. |
| 39 | Met à jour l’index si nécessaire. |
| 40 | Avance. |
| 41 | Fermeture. |
| 42 | Renvoie l’index du minimum. |
| 43 | Fermeture. |

Si `a` est vide, la fonction renvoie `0` sans lire d’élément. C’est important
pour le premier `pa` : `a->size == 0`, `best_greater` renvoie `-1`, puis
`complex_target_index` appelle `complex_min_index`, qui renvoie `0`. Ensuite
`get_cost(0, 0)` renvoie `0` grâce au test `size <= 1`. Aucun accès invalide
n’est fait.

### 6.3 `best_greater`, lignes 45-65

```c
static int	best_greater(t_stack *a, int value)
{
	int	idx;
	int	best;
	int	i;

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

| Ligne | Explication |
| ---: | --- |
| 45 | Signature : cherche le plus petit élément de `a` strictement supérieur à `value`. |
| 46 | Ouverture. |
| 47 | Index du meilleur candidat. |
| 48 | Meilleure valeur trouvée. |
| 49 | Index de parcours. |
| 51 | `-1` signifie « aucun élément supérieur ». |
| 52 | Sentinelle `INT_MAX`. |
| 53 | Part du sommet. |
| 54 | Parcourt `a`. |
| 55 | Ouverture. |
| 56 | Premier critère : l’élément doit être strictement supérieur à `value`. |
| 57 | Second critère : il doit être plus petit que le meilleur trouvé jusqu’ici. |
| 58 | Ouverture. |
| 59 | Met à jour la meilleure valeur. |
| 60 | Mémorise son index. |
| 61 | Fermeture. |
| 62 | Avance. |
| 63 | Fermeture. |
| 64 | Renvoie l’index, ou `-1`. |
| 65 | Fermeture. |

Cet algorithme est un **minimum conditionnel** : parmi les éléments supérieurs à
`value`, on garde le plus petit. C’est exactement le successeur dans l’ordre
trié.

Pourquoi `get_stack_element(a, i)` est-il appelé plusieurs fois ? Pour la
lisibilité. Le compilateur peut souvent réutiliser la valeur, mais le code
source ne stocke pas de variable temporaire. Ce n’est pas un problème de
correction.

### 6.4 `complex_target_index`, lignes 67-75

```c
int	complex_target_index(t_stack *a, int value)
{
	int	idx;

	idx = best_greater(a, value);
	if (idx == -1)
		idx = complex_min_index(a);
	return (idx);
}
```

| Ligne | Explication |
| ---: | --- |
| 67 | Signature : renvoie l’index devant lequel insérer `value`. |
| 68 | Ouverture. |
| 69 | Index du successeur. |
| 71 | Cherche le plus petit élément supérieur. |
| 72 | Si aucun élément n’est supérieur… |
| 73 | … le successeur circulaire est le minimum. |
| 74 | Renvoie l’index cible. |
| 75 | Fermeture. |

Exemple :

```text
a = [5, 7, 9, 2, 3]   (ordre circulaire : 2, 3, 5, 7, 9)
value = 6
best_greater = 7, index 1
```

On insère `6` avant `7`. Si `value = 10`, aucun élément n’est supérieur : on
insère avant le minimum `2`. L’ordre circulaire devient `2, 3, 5, 7, 9, 10`,
puis l’alignement final ramènera `2` au sommet.

### 6.5 `complex_align`, lignes 77-94

```c
void	complex_align(t_stack *a)
{
	int	index;
	int	cost;

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

| Ligne | Explication |
| ---: | --- |
| 77 | Signature : aligne le minimum de `a` au sommet. |
| 78 | Ouverture. |
| 79 | Index du minimum. |
| 80 | Coût signé pour l’amener au sommet. |
| 82 | Trouve le minimum. |
| 83 | Convertit son index en coût. |
| 84 | Tant que le coût est positif. |
| 85 | Ouverture. |
| 86 | Monte les éléments. |
| 87 | Décrémente. |
| 88 | Fermeture. |
| 89 | Tant que le coût est négatif. |
| 90 | Ouverture. |
| 91 | Descend les éléments. |
| 92 | Incrémente. |
| 93 | Fermeture. |
| 94 | Fermeture. |

Pendant le pull, `a` est triée **circulairement**, pas forcément linéairement.
Par exemple :

```text
a = [7, 8, 9, 5, 6]
```

Si on commence à `5`, l’ordre est `5, 6, 7, 8, 9`, mais le sommet n’est pas le
minimum. `complex_align` ramène `5` au sommet et produit :

```text
a = [5, 6, 7, 8, 9]
```

C’est la dernière étape obligatoire pour obtenir un tri linéaire.

---

## 7. Helpers partagés

### 7.1 `get_cost` — `utils.c`, lignes 15-22

```c
int	get_cost(int index, int size)
{
	if (size <= 1)
		return (0);
	if (index <= size / 2)
		return (index);
	return (index - size);
}
```

| Ligne | Explication |
| ---: | --- |
| 15 | Signature : index logique et taille de la pile. |
| 16 | Ouverture. |
| 17 | Une pile de taille `0` ou `1` n’a pas besoin de rotation. |
| 18 | Renvoie `0`. |
| 19 | Si l’index est dans la première moitié, rotation vers le haut. |
| 20 | Coût positif égal à l’index. |
| 21 | Sinon, rotation vers le bas : coût négatif `index - size`. |
| 22 | Fermeture. |

Table pour `size = 5` :

| Index | Condition | Coût | Opération |
| ---: | --- | ---: | --- |
| 0 | `0 <= 2` | 0 | aucune |
| 1 | `1 <= 2` | +1 | `ra` / `rb` |
| 2 | `2 <= 2` | +2 | `ra` / `rb` |
| 3 | `3 > 2` | -2 | `rra` / `rrb` |
| 4 | `4 > 2` | -1 | `rra` / `rrb` |

Pour `size` pair, l’index exactement au milieu a deux coûts de même amplitude.
Le test `<=` choisit la rotation vers le haut. Ce choix est arbitraire mais
déterministe.

### 7.2 `get_stack_element` — `stack.c`, lignes 15-20

```c
int	get_stack_element(t_stack *s, int index)
{
	if (index < 0 || index >= s->size)
		return (0);
	return (s->tab[(s->head + index) % s->capacity]);
}
```

| Ligne | Explication |
| ---: | --- |
| 15 | Signature : lit l’élément logique d’index `index`. |
| 16 | Ouverture. |
| 17 | Vérifie que l’index est dans la partie utile. |
| 18 | En cas d’index invalide, renvoie `0` au lieu de lire hors limites. |
| 19 | Convertit l’index logique en index physique circulaire. |
| 20 | Fermeture. |

Le `% capacity` gère le rebouclage. Exemple avec `capacity = 5`, `head = 3` :

```text
index logique 0 -> physique 3
index logique 1 -> physique 4
index logique 2 -> physique 0
index logique 3 -> physique 1
index logique 4 -> physique 2
```

Le retour `0` sur index invalide masque les erreurs mais évite les accès
mémoire interdits. Le code appelant doit néanmoins respecter les bornes.

### 7.3 `build_sorted_copy` — `array_utils.c`, lignes 36-52

```c
int	*build_sorted_copy(t_stack *a)
{
	int	*copy;
	int	i;

	copy = malloc(sizeof(int) * a->size);
	if (!copy)
		return (NULL);
	i = 0;
	while (i < a->size)
	{
		copy[i] = get_stack_element(a, i);
		i++;
	}
	insertion_sort(copy, a->size);
	return (copy);
}
```

| Ligne | Explication |
| ---: | --- |
| 36 | Signature : renvoie un nouveau tableau trié. |
| 37 | Ouverture. |
| 38 | Pointeur vers la copie. |
| 39 | Index. |
| 41 | Alloue `n` entiers. |
| 42 | Vérifie l’échec. |
| 43 | Renvoie `NULL`. |
| 44 | Part de `0`. |
| 45 | Parcourt la pile. |
| 46 | Ouverture. |
| 47 | Copie l’élément logique dans le tableau linéaire. |
| 48 | Avance. |
| 49 | Fermeture. |
| 50 | Trie la copie. |
| 51 | Renvoie la copie triée. |
| 52 | Fermeture. |

La copie est **linéaire** : elle ne conserve pas la structure circulaire. C’est
voulu, car `insertion_sort` et `find_rank` travaillent sur un tableau ordinaire.

#### `insertion_sort` — `array_utils.c`, lignes 15-34

```c
static void	insertion_sort(int *array, int size)
{
	int	i;
	int	j;
	int	tmp;

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
}
```

| Ligne | Explication |
| ---: | --- |
| 15 | Signature : trie `array` sur place. |
| 16 | Ouverture. |
| 17 | Index de l’élément à insérer. |
| 18 | Index de comparaison vers la gauche. |
| 19 | Valeur temporaire. |
| 21 | Le premier élément est déjà trié. |
| 22 | Parcourt les éléments à partir de l’index `1`. |
| 23 | Ouverture. |
| 24 | Sauvegarde l’élément courant. |
| 25 | Commence juste à sa gauche. |
| 26 | Tant qu’on est dans le tableau et que la valeur à gauche est plus grande. |
| 27 | Ouverture. |
| 28 | Décale la valeur vers la droite. |
| 29 | Recule. |
| 30 | Fermeture. |
| 31 | Place `tmp` à sa position finale. |
| 32 | Élément suivant. |
| 33 | Fermeture. |
| 34 | Fermeture. |

Tri par insertion : `O(n²)` dans le pire cas, `O(n)` si le tableau est déjà
trié. Pour `push_swap`, la taille reste modérée et cette méthode évite une
dépendance à `qsort`.

### 7.4 `find_rank` — `sorts_utils.c`, lignes 15-34

```c
int	find_rank(int *sorted, int n, int value)
{
	int	lo;
	int	hi;
	int	mid;

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
}
```

| Ligne | Explication |
| ---: | --- |
| 15 | Signature : cherche `value` dans `sorted`. |
| 16 | Ouverture. |
| 17 | Borne basse. |
| 18 | Borne haute. |
| 19 | Milieu. |
| 21 | Commence au début. |
| 22 | Finit au dernier index. |
| 23 | Tant que l’intervalle est non vide. |
| 24 | Ouverture. |
| 25 | Calcule le milieu. |
| 26 | Si la valeur est trouvée… |
| 27 | … renvoie son index, qui est son rang. |
| 28 | Si la valeur du milieu est trop petite… |
| 29 | … cherche à droite. |
| 30 | Sinon… |
| 31 | … cherche à gauche. |
| 32 | Fermeture. |
| 33 | Valeur absente : renvoie `0`. |
| 34 | Fermeture. |

La recherche binaire est correcte parce que `sorted` est trié. Elle est en
`O(log n)`. Les valeurs sont uniques, donc il n’y a pas de doublon d’index.

Le `return (0)` final ne devrait jamais être atteint dans le flux normal :
`value` vient de `a`, et `sorted` contient exactement les mêmes valeurs.

---

## 8. Opérations `push_swap` utilisées

### 8.1 `pb` — `ops_push.c`, lignes 15-30

```c
void	pb(t_stack *a, t_stack *b)
{
	int	value;

	if (a->size == 0)
		return ;
	value = a->tab[a->head];
	a->head = (a->head + 1) % a->capacity;
	a->size--;
	b->head = (b->head - 1 + b->capacity) % b->capacity;
	b->tab[b->head] = value;
	b->size++;
	a->counts->pb++;
	if (!a->quiet)
		write(1, "pb\n", 3);
}
```

| Ligne | Explication |
| ---: | --- |
| 15 | Signature : pousse le sommet de `a` sur `b`. |
| 16 | Ouverture. |
| 17 | Valeur à déplacer. |
| 19 | Si `a` est vide, rien à faire. |
| 20 | Retour silencieux : aucune opération comptée. |
| 21 | Lit le sommet de `a`. |
| 22 | Avance `head` de `a` : l’ancien sommet est retiré. |
| 23 | Diminue la taille de `a`. |
| 24 | Recule `head` de `b` pour libérer une case au sommet. |
| 25 | Écrit la valeur au nouveau sommet de `b`. |
| 26 | Augmente la taille de `b`. |
| 27 | Incrémente le compteur `pb`. |
| 28 | Si l’affichage n’est pas coupé… |
| 29 | … écrit `pb\n` sur `stdout`. |
| 30 | Fermeture. |

`b->head` recule avec `(head - 1 + capacity) % capacity` pour éviter un index
négatif en C. La valeur `+ capacity` garantit un résultat positif avant le
modulo.

### 8.2 `pa` — `ops_push.c`, lignes 32-47

```c
void	pa(t_stack *a, t_stack *b)
{
	int	value;

	if (b->size == 0)
		return ;
	value = b->tab[b->head];
	b->head = (b->head + 1) % b->capacity;
	b->size--;
	a->head = (a->head - 1 + a->capacity) % a->capacity;
	a->tab[a->head] = value;
	a->size++;
	a->counts->pa++;
	if (!a->quiet)
		write(1, "pa\n", 3);
}
```

| Ligne | Explication |
| ---: | --- |
| 32 | Signature : pousse le sommet de `b` sur `a`. |
| 33 | Ouverture. |
| 34 | Valeur à déplacer. |
| 36 | Si `b` est vide, rien à faire. |
| 37 | Retour silencieux. |
| 38 | Lit le sommet de `b`. |
| 39 | Avance `head` de `b`. |
| 40 | Diminue la taille de `b`. |
| 41 | Recule `head` de `a` pour libérer le sommet. |
| 42 | Écrit la valeur au sommet de `a`. |
| 43 | Augmente la taille de `a`. |
| 44 | Incrémente `pa`. |
| 45 | Si l’affichage est actif… |
| 46 | … écrit `pa\n`. |
| 47 | Fermeture. |

`pa` est la seule opération qui fait passer un élément de `b` à `a` dans
`complex`. Elle est toujours appelée après les rotations, quand le successeur
est au sommet de `a` et `value` au sommet de `b`.

### 8.3 `ra`, `rb`, `rr` — `ops_rotate.c`, lignes 15-71

#### `ra`

```c
void	ra(t_stack *a)
{
	int	value;
	int	tail;

	if (a->size < 2)
		return ;
	value = a->tab[a->head];
	tail = (a->head + a->size) % a->capacity;
	a->tab[tail] = value;
	a->head = (a->head + 1) % a->capacity;
	a->counts->ra++;
	if (!a->quiet)
		write(1, "ra\n", 3);
}
```

| Ligne | Explication |
| ---: | --- |
| 15 | Signature : rotation vers le haut de `a`. |
| 16 | Ouverture. |
| 17 | Valeur du sommet. |
| 18 | Index physique de la queue. |
| 20 | Une pile de taille `0` ou `1` est invariante par rotation. |
| 21 | Retour silencieux : pas de compteur. |
| 22 | Lit le sommet. |
| 23 | Calcule la case après le dernier élément. |
| 24 | Copie le sommet à la queue. |
| 25 | Avance le sommet. |
| 26 | Compteur `ra`. |
| 27 | Si affichage actif… |
| 28 | … écrit `ra\n`. |
| 29 | Fermeture. |

#### `rb`

Même logique sur `b` (lignes 31-45) :

| Ligne | Explication |
| ---: | --- |
| 31 | Signature : rotation vers le haut de `b`. |
| 32 | Ouverture. |
| 33 | Valeur du sommet. |
| 34 | Index de la queue. |
| 36 | Si `b` a moins de deux éléments, rien à faire. |
| 37 | Retour. |
| 38 | Lit le sommet. |
| 39 | Calcule la queue. |
| 40 | Copie à la queue. |
| 41 | Avance `head`. |
| 42 | Compteur `rb`. |
| 43 | Si affichage actif… |
| 44 | … écrit `rb\n`. |
| 45 | Fermeture. |

#### `rr`

```c
void	rr(t_stack *a, t_stack *b)
{
	int	value;
	int	tail;

	if (a->size < 2 && b->size < 2)
		return ;
	if (a->size >= 2)
	{
		value = a->tab[a->head];
		tail = (a->head + a->size) % a->capacity;
		a->tab[tail] = value;
		a->head = (a->head + 1) % a->capacity;
	}
	if (b->size >= 2)
	{
		value = b->tab[b->head];
		tail = (b->head + b->size) % b->capacity;
		b->tab[tail] = value;
		b->head = (b->head + 1) % b->capacity;
	}
	a->counts->rr++;
	if (!a->quiet)
		write(1, "rr\n", 3);
}
```

| Ligne | Explication |
| ---: | --- |
| 47 | Signature : rotation vers le haut des deux piles. |
| 48 | Ouverture. |
| 49 | Valeur temporaire. |
| 50 | Index de queue. |
| 52 | Si les deux piles ont moins de deux éléments, rien à faire. |
| 53 | Retour : aucune opération comptée. |
| 54 | Si `a` peut tourner… |
| 55 | Ouverture. |
| 56 | Lit le sommet de `a`. |
| 57 | Calcule la queue de `a`. |
| 58 | Copie à la queue. |
| 59 | Avance `head` de `a`. |
| 60 | Fermeture. |
| 61 | Si `b` peut tourner… |
| 62 | Ouverture. |
| 63 | Lit le sommet de `b`. |
| 64 | Calcule la queue de `b`. |
| 65 | Copie à la queue. |
| 66 | Avance `head` de `b`. |
| 67 | Fermeture. |
| 68 | Incrémente `rr`. |
| 69 | Si affichage actif… |
| 70 | … écrit `rr\n`. |
| 71 | Fermeture. |

Détail important : `rr` est **une seule opération**, même si une des deux piles
a moins de deux éléments et ne bouge pas. Le compteur `rr` est incrémenté dès
qu’au moins une pile peut tourner. Cela ne pose pas de problème de correction,
car une rotation sur une pile de taille `1` est sans effet.

### 8.4 `rra`, `rrb`, `rrr` — `ops_reverse.c`, lignes 15-71

#### `rra`

```c
void	rra(t_stack *a)
{
	int	value;
	int	new_head;

	if (a->size < 2)
		return ;
	value = a->tab[(a->head + a->size - 1) % a->capacity];
	new_head = (a->head - 1 + a->capacity) % a->capacity;
	a->tab[new_head] = value;
	a->head = new_head;
	a->counts->rra++;
	if (!a->quiet)
		write(1, "rra\n", 4);
}
```

| Ligne | Explication |
| ---: | --- |
| 15 | Signature : rotation vers le bas de `a`. |
| 16 | Ouverture. |
| 17 | Valeur de la queue. |
| 18 | Nouvel index du sommet. |
| 20 | Si `a` a moins de deux éléments, rien à faire. |
| 21 | Retour. |
| 22 | Lit la queue : `head + size - 1`. |
| 23 | Calcule la case juste avant le sommet. |
| 24 | Écrit la queue à la nouvelle position du sommet. |
| 25 | Met à jour `head`. |
| 26 | Compteur `rra`. |
| 27 | Si affichage actif… |
| 28 | … écrit `rra\n`. |
| 29 | Fermeture. |

#### `rrb`

Même logique sur `b` (lignes 31-45) :

| Ligne | Explication |
| ---: | --- |
| 31 | Signature : rotation vers le bas de `b`. |
| 32 | Ouverture. |
| 33 | Valeur de la queue. |
| 34 | Nouvel index de sommet. |
| 36 | Si `b` a moins de deux éléments, rien à faire. |
| 37 | Retour. |
| 38 | Lit la queue. |
| 39 | Calcule la case précédente. |
| 40 | Écrit à la nouvelle tête. |
| 41 | Met à jour `head`. |
| 42 | Compteur `rrb`. |
| 43 | Si affichage actif… |
| 44 | … écrit `rrb\n`. |
| 45 | Fermeture. |

#### `rrr`

```c
void	rrr(t_stack *a, t_stack *b)
{
	int	value;
	int	new_head;

	if (a->size < 2 && b->size < 2)
		return ;
	if (a->size >= 2)
	{
		value = a->tab[(a->head + a->size - 1) % a->capacity];
		new_head = (a->head - 1 + a->capacity) % a->capacity;
		a->tab[new_head] = value;
		a->head = new_head;
	}
	if (b->size >= 2)
	{
		value = b->tab[(b->head + b->size - 1) % b->capacity];
		new_head = (b->head - 1 + b->capacity) % b->capacity;
		b->tab[new_head] = value;
		b->head = new_head;
	}
	a->counts->rrr++;
	if (!a->quiet)
		write(1, "rrr\n", 4);
}
```

| Ligne | Explication |
| ---: | --- |
| 47 | Signature : rotation vers le bas des deux piles. |
| 48 | Ouverture. |
| 49 | Valeur temporaire. |
| 50 | Nouvel index de sommet. |
| 52 | Si les deux piles ont moins de deux éléments, rien à faire. |
| 53 | Retour. |
| 54 | Si `a` peut tourner… |
| 55 | Ouverture. |
| 56 | Lit la queue de `a`. |
| 57 | Calcule la case précédente. |
| 58 | Écrit à la nouvelle tête. |
| 59 | Met à jour `head`. |
| 60 | Fermeture. |
| 61 | Si `b` peut tourner… |
| 62 | Ouverture. |
| 63 | Lit la queue de `b`. |
| 64 | Calcule la case précédente. |
| 65 | Écrit à la nouvelle tête. |
| 66 | Met à jour `head`. |
| 67 | Fermeture. |
| 68 | Compteur `rrr`. |
| 69 | Si affichage actif… |
| 70 | … écrit `rrr\n`. |
| 71 | Fermeture. |

Comme `rr`, `rrr` est une seule opération même si une pile ne bouge pas.

### 8.5 `sa`, `sb`, `ss`

`complex` ne les utilise jamais. Ils ne servent qu’à échanger les deux premiers
éléments d’une pile, ce qui n’est pas nécessaire dans une stratégie
d’insertion par rotations et `pa`.

---

## 9. Trace commentée `n = 10`

### 9.1 Données et géométrie

```text
a = [7, 2, 9, 0, 5, 1, 8, 3, 6, 4]
b = []
n = 10
div = 2
fr = 3
chunk_size = 5
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

Le pull commencera à `chunk = 2` : `base = 10`, `mid = 11`, plages vides.

### 9.2 Presort

Passe `chunk = 0`, `limit = 5`, `sink = 1` :

| # | Opération | `a` après | `b` après |
| ---: | --- | --- | --- |
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

Passe `chunk = 1`, `limit = 10`, `sink = 6` :

| # | Opération | `a` après | `b` après |
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

Groupes :

```text
H1 = [6, 8, 9, 7]
H0 = [4, 3, 1, 2]
L0 = [0]
L1 = [5]
```

La structure est bien `H1, H0, L0, L1`.

### 9.3 Pull

#### `chunk = 2`

```text
base = 10, mid = 11
haute : [11, 15) -> vide
basse : [10, 11) -> vide
```

Aucune insertion.

#### `chunk = 1`, partie haute `[6, 10)`

| # | Candidats `(valeur, score, cost_a, cost_b)` | Choix | Opérations | `a` après |
| ---: | --- | ---: | --- | --- |
| 18 | `(6,0,0,0)`, `(8,1,0,1)`, `(9,2,0,2)`, `(7,3,0,3)` | 6 | `pa` | `[6]` |
| 19 | `(8,0,0,0)`, `(9,1,0,1)`, `(7,2,0,2)` | 8 | `pa` | `[8,6]` |
| 20-21 | `(9,1,1,0)`, `(7,1,0,1)` | 9 | `ra`, `pa` | `[9,6,8]` |
| 22-23 | `(7,1,-1,0)` | 7 | `rra`, `pa` | `[7,8,9,6]` |

À l’étape 20, `9` et `7` ont le même score `1`. `best_in_range` garde le
premier rencontré, `9`, car `b = [9, 7, ...]`.

#### `chunk = 1`, partie basse `[5, 6)`

| # | Candidat | Choix | Opérations | `a` après |
| ---: | --- | ---: | --- | --- |
| 24-25 | `(5,1,-1,-1)` | 5 | `rrr`, `pa` | `[5,6,7,8,9]` |

Ici `cost_a = -1` et `cost_b = -1` : `rotate_ab` combine les deux rotations
avec `rrr`.

#### `chunk = 0`, partie haute `[1, 5)`

| # | Candidats `(valeur, score, cost_a, cost_b)` | Choix | Opérations | `a` après |
| ---: | --- | ---: | --- | --- |
| 26 | `(4,0,0,0)`, `(3,1,0,1)`, `(1,2,0,2)`, `(2,2,0,-2)` | 4 | `pa` | `[4,5,6,7,8,9]` |
| 27 | `(3,0,0,0)`, `(1,1,0,1)`, `(2,2,0,2)` | 3 | `pa` | `[3,4,5,6,7,8,9]` |
| 28 | `(1,0,0,0)`, `(2,1,0,1)` | 1 | `pa` | `[1,3,4,5,6,7,8,9]` |
| 29-30 | `(2,1,1,0)` | 2 | `ra`, `pa` | `[2,3,4,5,6,7,8,9,1]` |

#### `chunk = 0`, partie basse `[0, 1)`

| # | Candidat | Choix | Opérations | `a` après |
| ---: | --- | ---: | --- | --- |
| 31-32 | `(0,1,-1,0)` | 0 | `rra`, `pa` | `[0,1,2,3,4,5,6,7,8,9]` |

### 9.4 Alignement

Le minimum `0` est déjà au sommet. `complex_align` ne fait rien.

### 9.5 Bilan

Séquence complète :

```text
ra pb ra pb rb ra pb ra pb ra pb
pb pb pb rb pb pb
pa pa ra pa rra pa rrr pa pa pa pa ra pa rra pa
```

Compteurs :

| Opération | Nombre |
| --- | ---: |
| `pb` | 10 |
| `pa` | 10 |
| `ra` | 7 |
| `rb` | 2 |
| `rra` | 2 |
| `rrr` | 1 |
| **Total** | **32** |

Vérification avec le code :

- 10 `pb` : un par élément, dans `complex_presort` ;
- 10 `pa` : un par élément, dans `complex_insert` ;
- 2 `rb` : rangs `0` et `5`, dans `presort_one` ;
- 7 `ra` : 5 dans le presort (`7`, `9`, `5`, `8`, `6`) et 2 dans le pull
  (insertion de `9` et de `2`) ;
- 2 `rra` : insertions de `7` et de `0` ;
- 1 `rrr` : insertion de `5`.

### 9.6 Vérification du résultat

Après le pull, `a` est triée circulairement. Ici, elle est même déjà linéaire :

```text
a = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
b = []
```

Le checker doit répondre `OK`.

---

## 10. Invariants, pièges et cas limites

### 10.1 Invariants principaux

1. **Rangs uniques.** Le parsing refuse les doublons. `find_rank` peut donc
   renvoyer un rang sans ambiguïté.
2. **`a` est triée circulairement pendant le pull.** Chaque insertion place
   `value` juste avant son successeur.
3. **`H_c` est au sommet de `b` quand on traite la bande `c`.**
4. **`L_c` est au fond de `b` quand on traite la bande `c`.**
5. **Chaque élément est inséré exactement une fois.** Les plages haute et basse
   des bandes sont disjointes et couvrent `0 … n - 1`.
6. **`b` finit vide.** Chaque `complex_insert` retire un élément.
7. **Le minimum de `a` est ramené au sommet par `complex_align`.**

### 10.2 Pièges de lecture

#### `chunk` commence à `0`

`complex_presort` utilise `(chunk + 1) * chunk_size`. Donc `chunk = 0`
correspond bien à `[0, chunk_size)`. Ne pas confondre avec `medium_chunks`, qui
utilise `chunk * size` et commence à `1`.

#### `count` est un snapshot

`count = a->size` est pris avant la boucle interne. `a->size` diminue à chaque
`pb`. La boucle doit quand même faire `count` itérations pour examiner tous les
éléments présents au début de la passe.

#### `rb` peut être un no-op

Dans `presort_one`, le premier élément basse poussé dans un `b` vide déclenche
un appel à `rb`, mais `rb` retourne immédiatement car `b->size < 2`. Aucune
opération n’est émise et le compteur n’est pas incrémenté.

#### `complex_pull` commence à `div`

Ce n’est pas `P - 1`, car `P` n’est pas calculé. `chunk = div` couvre la bande
partielle éventuelle. Si `n` est divisible par `div`, les plages sont vides.

#### `get_cost` et le milieu

Pour une taille paire, l’index `size / 2` a deux chemins de même longueur. Le
code choisit `ra`/`rb` à cause de `<=`. Ce n’est pas un bug.

#### `best_in_range` et les égalités

Le test `score < best` garde le premier candidat. Le résultat dépend donc de
l’ordre des éléments dans `b`, mais reste déterministe.

#### `complex_find` renvoie `0` si absent

Dans le flux normal, la valeur existe. Si on modifiait le code pour appeler
`complex_insert` avec une valeur absente, on risquerait de faire tourner `b`
vers le sommet puis de pousser une autre valeur. C’est une hypothèse implicite
du code.

#### `complex_min_index` sur pile vide

Elle renvoie `0` sans accès. C’est utilisé au premier `pa`, quand `a` est vide.
`get_cost(0, 0)` renvoie `0`, donc aucun problème.

#### `best_greater` sur valeur maximale

Si `value` est le rang `n - 1`, aucun élément n’est supérieur. Le successeur
circulaire est le minimum, et `value` est inséré juste avant lui, donc à la fin
de l’ordre croissant.

#### `complex_target_index` sur pile vide

`best_greater` renvoie `-1`, `complex_min_index` renvoie `0`, `get_cost(0,0)`
renvoie `0`. Le premier `pa` n’a donc besoin d’aucune rotation.

#### `pa` après les rotations

L’ordre est important : on tourne d’abord `a` pour amener le successeur au
sommet, on tourne `b` pour amener la valeur au sommet, puis on fait `pa`. Si on
faisait `pa` avant, la valeur serait insérée au mauvais endroit.

#### `rr` et `rrr` sont des opérations uniques

Ils comptent pour une opération même s’ils ne font tourner qu’une seule pile
parce que l’autre a moins de deux éléments. C’est conforme aux règles de
`push_swap`.

#### Le CPU n’est pas le nombre d’opérations

`best_in_range` parcourt `b` pour chaque insertion : `O(n²)` comparaisons au
total. Pour chaque candidat de la plage, `candidate_score` appelle
`complex_target_index`, qui parcourt `a` en `O(n)`. Le pire cas CPU peut donc
être majoré par `O(n³)`. Ce n’est pas la métrique du sujet, qui compte les
opérations émises.

#### La borne `O(n log n)` est un objectif

Le README présente `complex` comme visant `O(n log n)`. Le code ne contient pas
de preuve formelle de cette borne dans le pire cas : le pull peut, en théorie,
coûter `O(n²)` opérations si chaque insertion traverse une grande partie de
`b`. En pratique, la structure du presort et le glouton donnent une croissance
quasi linéaire, mesurée autour de `5,7 n` à `9,4 n` sur les tailles testées.

### 10.3 Cas limites

| Cas | Comportement |
| --- | --- |
| `n < 2` | `sort_complex` retourne immédiatement. |
| Entrée déjà triée | `main` ne lance pas le tri ; aucune opération. |
| `normalize_stack` échoue | Repli sur `sort_simple`. |
| `chunk_size == 0` | Garde `if (chunk_size < 1) chunk_size = 1`. |
| Partie basse vide (`chunk_size / fr == 0`) | Aucun `rb` au presort ; le pull traite tout comme partie haute. |
| Bande partielle | `chunk = div` la couvre ; une des deux plages peut être vide. |
| `a` vide au premier `pa` | `complex_min_index` renvoie `0`, coût `0`. |
| `b` vide | `best_in_range` renvoie `0`, la boucle s’arrête. |
| Doublons | Refusés au parsing ; les rangs sont uniques. |
| `b` non trié après presort | Normal : le presort ne trie pas l’intérieur des bandes. |
| `rb` sur pile de taille 1 | No-op, pas de compteur. |

---

## 11. Checklist de compréhension

Pour vérifier que le code est compris, on doit pouvoir répondre à ces questions
sans relire le document :

1. Pourquoi `sort_complex` mémorise-t-il `geom.n` avant le presort ?
2. Pourquoi `normalize_stack` est-il nécessaire avant les bandes ?
3. Pourquoi `complex_presort` commence-t-il à `chunk = 0` ?
4. Pourquoi `count = a->size` est-il pris avant la boucle interne ?
5. Pourquoi `presort_one` fait-il `rb` après `pb` pour les valeurs basses ?
6. Pourquoi la structure de `b` est-elle `H_last, ..., H_0, L_0, ..., L_last` ?
7. Pourquoi `complex_pull` commence-t-il à `chunk = div` ?
8. Pourquoi traite-t-il la partie haute avant la partie basse ?
9. Pourquoi `best_in_range` garde-t-il le premier candidat en cas d’égalité ?
10. Pourquoi `complex_cost` utilise-t-il `max` pour des signes identiques ?
11. Pourquoi `rotate_ab` fait-il `rr`/`rrr` avant les rotations individuelles ?
12. Pourquoi `complex_target_index` renvoie-t-il le minimum quand aucun
    élément n’est supérieur ?
13. Pourquoi `pa` insère-t-il avant le successeur ?
14. Pourquoi `complex_align` est-il nécessaire même si `a` est triée
    circulairement ?
15. Pourquoi `complex_find` peut-il renvoyer `0` sans casser le flux normal ?
16. Pourquoi le CPU peut-il être `O(n³)` alors que le nombre d’opérations est
    bien plus faible ?
17. Pourquoi `rb` peut-il être appelé sans émettre d’opération ?
18. Pourquoi le code n’utilise-t-il jamais `sa`/`sb`/`ss` ?

---

## 12. Tests et vérification pratique

### 12.1 Compiler

```sh
make
```

### 12.2 Vérifier le résultat

```sh
ARG="7 2 9 0 5 1 8 3 6 4"
./push_swap --complex $ARG | ./checker_linux $ARG
```

Attendu :

```text
OK
```

### 12.3 Vérifier le nombre d’opérations de la trace

```sh
ARG="7 2 9 0 5 1 8 3 6 4"
./push_swap --complex $ARG | wc -l
```

Attendu pour cette entrée : `32`.

### 12.4 Vérifier les compteurs

```sh
ARG="7 2 9 0 5 1 8 3 6 4"
./push_swap --bench --complex $ARG 2>/tmp/bench_complex.txt
cat /tmp/bench_complex.txt
```

On doit retrouver :

```text
pb: 10
pa: 10
ra: 7
rb: 2
rra: 2
rrr: 1
```

### 12.5 Tester les seuils de géométrie

```sh
./push_swap --bench --complex $(seq 1 299 | shuf)
./push_swap --bench --complex $(seq 1 300 | shuf)
./push_swap --bench --complex $(seq 1 449 | shuf)
./push_swap --bench --complex $(seq 1 450 | shuf)
./push_swap --bench --complex $(seq 1 799 | shuf)
./push_swap --bench --complex $(seq 1 800 | shuf)
```

Observer `div` et `fr` dans la sortie de bench ou instrumenter temporairement
`complex_geometry` (sans modifier la version finale).

### 12.6 Tester les tailles limites

```sh
./push_swap --complex 2 1
./push_swap --complex 1 2 3
./push_swap --complex 3 1 2
./push_swap --complex 2 1 3
```

Vérifier avec le checker et avec `--bench`.

### 12.7 Tester une entrée déjà triée

```sh
./push_swap --complex 1 2 3 4 5 | wc -l
```

Attendu : `0`.

### 12.8 Tester une entrée inversée

```sh
./push_swap --complex 5 4 3 2 1 | ./checker_linux 5 4 3 2 1
```

Attendu : `OK`.

---

## 13. Résumé en une page

### 13.1 Les fonctions du parcours

| Fonction | Rôle | Fichier |
| --- | --- | --- |
| `sort_complex` | Point d’entrée, géométrie, phases | `sort_complex.c` |
| `normalize_stack` | Remplace les valeurs par leurs rangs | `complex_presort.c` |
| `complex_presort` | Distribue `a` en bandes dans `b` | `complex_presort.c` |
| `presort_one` | Traite un élément du presort | `complex_presort.c` |
| `complex_pull` | Remonte les bandes de `b` vers `a` | `complex_pull.c` |
| `pull_range` | Vide une plage de `b` | `complex_pull.c` |
| `best_in_range` | Choisit le meilleur candidat d’une plage | `complex_pull.c` |
| `candidate_score` | Calcule le score d’un candidat | `complex_pull.c` |
| `complex_cost` | Combine deux coûts signés | `complex_pull.c` |
| `complex_insert` | Insère une valeur dans `a` | `complex_insert.c` |
| `rotate_ab` | Fusionne les rotations | `complex_insert.c` |
| `complex_target_index` | Trouve la position d’insertion | `complex_move.c` |
| `best_greater` | Trouve le successeur | `complex_move.c` |
| `complex_min_index` | Trouve le minimum | `complex_move.c` |
| `complex_align` | Ramène le minimum au sommet | `complex_move.c` |
| `get_cost` | Convertit un index en coût signé | `utils.c` |
| `get_stack_element` | Accès logique circulaire | `stack.c` |

### 13.2 Les formules

```text
chunk_size = n / div

bande c      : [c * chunk_size, (c + 1) * chunk_size)
partie basse : [c * chunk_size, c * chunk_size + chunk_size / fr)
partie haute : [c * chunk_size + chunk_size / fr, (c + 1) * chunk_size)

coût d’un index i dans une pile de taille s :
    +i        si i <= s / 2
    i - s     sinon

coût combiné :
    max(|ca|, |cb|)   si ca et cb ont le même signe non nul
    |ca| + |cb|       sinon
```

### 13.3 Les cinq étapes

```text
1. Normaliser les valeurs en rangs 0 … n-1.
2. Choisir div et fr selon n.
3. Presort : bandes croissantes, partie basse au fond de b avec rb.
4. Pull : bandes décroissantes, partie haute puis partie basse,
   candidat de score minimal.
5. Aligner le minimum de a au sommet.
```

### 13.4 La phrase à retenir

> `complex` normalise les valeurs, les répartit en quelques bandes larges,
> envoie la partie basse de chaque bande au fond de `b`, puis réinsère les
> bandes de la plus grande à la plus petite en choisissant à chaque fois
> l’insertion la moins coûteuse ; un dernier alignement ramène le minimum au
> sommet.
