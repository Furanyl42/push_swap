# Explication détaillée de l’algorithme `--medium`

## Vue d’ensemble

L’algorithme **medium** est la stratégie `--medium`. Il est implémenté principalement dans :

- `sort_medium.c` : normalisation, calcul des paramètres, découpage en seaux ;
- `complex_pull.c` / `complex_insert.c` / `complex_move.c` : remontée des seaux et insertion triée dans `a`.

C’est un **tri par seaux** (*chunk/bucket sort*) en deux grandes phases :

1. **Phase de distribution** : on découpe les valeurs en bandes de rangs, puis on pousse les bandes de `a` vers `b`, de la plus petite à la plus grande. Résultat : `b` contient les bandes empilées dans l’ordre décroissant, la plus grande au sommet.
2. **Phase de remontée** : on reprend les bandes de `b` de la plus grande à la plus petite, et on insère chaque valeur dans `a` à la bonne position, en choisissant à chaque fois l’élément le moins coûteux à déplacer.

Le tout repose sur une **normalisation préalable** : les valeurs réelles sont remplacées par leurs rangs `0 … n-1`.

---

## 1. Sélection de la stratégie

Avec `--medium`, `parsing.c` positionne `opts->strategy = STRAT_MEDIUM` (lignes 19 et 39-40).

En mode adaptatif, `resolve_strategy` choisit medium quand le désordre est compris entre `0.2` et `0.5` :

```c
if (disorder < 0.2)
    return (STRAT_SIMPLE);
if (disorder < 0.5)
    return (STRAT_MEDIUM);
return (STRAT_COMPLEX);
```

Puis `sort_stacks` appelle `sort_medium(a, b)` (`sort_dispatcher.c:32-33`).

À noter : `main` ne lance le tri que si `disorder > 0.0`. Un tableau déjà trié ne produit donc aucune opération, même avec `--medium`.

---

## 2. Prétraitement : normalisation des valeurs

`sort_medium` commence par :

```c
geom.n = a->size;
if (!normalize_stack(a))
{
    sort_simple(a, b);
    return ;
}
```

`normalize_stack` est dans `complex_presort.c:15` :

1. `build_sorted_copy(a)` copie le tableau puis le trie par **tri par insertion** ;
2. pour chaque valeur de `a`, `find_rank` fait une **recherche binaire** dans la copie triée et renvoie son rang ;
3. la valeur d’origine est remplacée par ce rang.

Exemple :

```text
valeurs : 42  -7  100  3
rangs   :  2   0    3  1
```

Pourquoi normaliser ?

- Les frontières des seaux deviennent indépendantes de l’amplitude réelle des valeurs.
- On travaille uniquement sur `0 … n-1`.
- Les comparaisons de bandes sont simples : `valeur < chunk * chunk_size`.

Coût : le tri par insertion de la copie est `O(n²)` dans le pire cas, avec `O(n)` mémoire temporaire. En cas d’échec d’allocation, le code retombe sur `sort_simple`.

---

## 3. Calcul de la géométrie

Toujours dans `sort_medium.c` :

```c
geom.div = floor_sqrt(geom.n) / 4;
if (geom.div < 2)
    geom.div = 2;
geom.fr = 1;
```

Donc :

```text
div        = max(2, floor(sqrt(n)) / 4)
chunk_size = n / div          (division entière, minimum 1)
fr         = 1
```

`floor_sqrt` calcule la racine carrée entière sans `sqrt()` ni flottants, en évitant les débordements grâce à la division :

```c
while ((root + 1) <= n / (root + 1))
    root++;
```

Quelques exemples :

| `n` | `floor(sqrt(n))` | `div` | `chunk_size` | Nombre approximatif de bandes |
|---:|---:|---:|---:|---:|
| 10 | 3 | 2 | 5 | 2 |
| 16 | 4 | 2 | 8 | 2 |
| 100 | 10 | 2 | 50 | 2 |
| 400 | 20 | 5 | 80 | 5 |
| 10 000 | 100 | 25 | 400 | 25 |

Le nombre de bandes est donc de l’ordre de `√n / 4`, et chaque bande contient environ `4√n` éléments. C’est bien la même classe asymptotique que `O(√n)` bandes, avec un facteur constant.

Le `fr = 1` est important : il désactive le découpage supplémentaire utilisé par la stratégie complex. Medium traite chaque bande comme **une seule plage**.

---

## 4. Phase 1 : distribution de `a` vers `b`

C’est `medium_chunks` (`sort_medium.c:25`) :

```c
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
```

### Principe

À chaque passe, on fixe une limite :

```text
limite = chunk * size
```

Puis on parcourt **tous les éléments encore présents dans `a`** :

- si le sommet de `a` a un rang strictement inférieur à la limite, on le pousse dans `b` avec `pb` ;
- sinon, on le laisse dans `a` en le faisant tourner avec `ra`.

La première passe prend donc les rangs `[0, size)`, la deuxième `[size, 2*size)`, etc.

### Pourquoi `count = a->size` ?

C’est un point subtil mais essentiel.

`count` est capturé **avant** la passe. Même si `ra` réorganise la pile, la boucle fait exactement `count` itérations. Chaque élément présent au début de la passe est donc examiné exactement une fois.

Sans ce `count`, le fait de faire tourner `a` pourrait faire examiner plusieurs fois les mêmes éléments ou en oublier.

### Empilement dans `b`

`pb` place l’élément au **sommet** de `b`.

Donc :

- la première bande poussée, la plus petite, se retrouve au fond de `b` ;
- la bande suivante est poussée au-dessus ;
- la dernière bande, la plus grande, se retrouve au sommet.

Après la phase 1, `b` ressemble donc à :

```text
sommet de b
  bande la plus grande
  ...
  bande intermédiaire
  bande la plus petite
fond de b
```

Chaque bande est contiguë dans `b`, mais **l’ordre interne d’une bande n’est pas trié** : il dépend de l’ordre de scan et des rotations.

### Exemple avec `n = 13`

```text
sqrt(13) = 3
div = max(2, 3/4) = 2
chunk_size = 13/2 = 6
```

Les passes utilisent les limites :

```text
passe 1 : v < 6    -> rangs 0..5
passe 2 : v < 12   -> rangs 6..11
passe 3 : v < 18   -> rang 12
```

On obtient trois bandes :

```text
[0,6)  [6,12)  [12,13)
```

Dans `b`, du sommet vers le fond :

```text
[12]  puis  [6..11]  puis  [0..5]
```

### Coût de la phase 1

Chaque passe parcourt au plus `a->size` éléments. Il y a environ `n / chunk_size = div ≈ √n/4` passes.

Donc :

```text
O(n) par passe × O(√n) passes = O(n√n)
```

Les `pb` sont au total exactement `n`, puisqu’on vide entièrement `a`.

---

## 5. Phase 2 : remontée de `b` vers `a`

`sort_medium` appelle ensuite :

```c
complex_pull(a, b, &geom);
```

avec `geom.fr = 1`. C’est le même code que la stratégie complex, mais avec un comportement simplifié.

### La boucle de `complex_pull`

```c
chunk_size = geom->n / geom->div;
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
```

Avec `fr = 1` :

```text
mid = base + chunk_size
```

Donc le premier `pull_range` est :

```text
[base + chunk_size, base + chunk_size)
```

C’est une plage **vide**, il ne fait rien.

Le second `pull_range` est :

```text
[base, base + chunk_size)
```

C’est la bande complète.

Medium traite donc **une bande entière à la fois**, de la plus grande vers la plus petite :

```text
chunk = div   -> bande du haut, éventuellement partielle
chunk = div-1 -> bande suivante
...
chunk = 0     -> plus petite bande
```

Le `chunk = div` peut correspondre à une bande vide lorsque `n` est divisible par `div`, ou à la bande partielle du haut sinon. Dans les deux cas, les plages restent alignées avec celles de `medium_chunks`.

### `pull_range`

```c
static void pull_range(t_stack *a, t_stack *b, t_range *range)
{
    int value;

    while (best_in_range(a, b, range, &value))
        complex_insert(a, b, value);
}
```

Tant qu’il reste dans `b` un élément dont le rang appartient à `[low, high)`, on en choisit un et on l’insère dans `a`.

---

## 6. Choix de l’élément à insérer : heuristique de coût

`best_in_range` parcourt tous les éléments de `b`. Pour chaque élément dont la valeur est dans la plage, il calcule un score avec `candidate_score`, puis garde le plus petit score.

### `complex_target_index`

```c
int complex_target_index(t_stack *a, int value)
{
    idx = best_greater(a, value);
    if (idx == -1)
        idx = complex_min_index(a);
    return idx;
}
```

`best_greater` renvoie l’index du **plus petit élément de `a` strictement supérieur à `value`**.

- Si un tel élément existe, on insère `value` juste avant lui.
- Si `value` est plus grand que tout `a`, on renvoie l’index du minimum de `a`.

Pourquoi le minimum dans ce second cas ? Parce que la pile est circulaire : insérer avant le minimum revient à insérer après le maximum, ce qui préserve l’ordre croissant circulaire.

Si `a` est vide, `complex_min_index` renvoie `0` et le coût est nul.

### `get_cost`

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

Cela donne le nombre de rotations **signé** le plus court pour amener l’index au sommet :

- index dans la première moitié : coût positif, donc `ra` / `rb` ;
- index dans la seconde moitié : coût négatif, donc `rra` / `rrb`.

Exemple pour `size = 4` :

| index | coût | opération |
|---:|---:|---|
| 0 | 0 | aucune |
| 1 | +1 | `ra` une fois |
| 2 | +2 | `ra` deux fois |
| 3 | -1 | `rra` une fois |

### `complex_cost`

```c
if ((cost_a > 0 && cost_b > 0) || (cost_a < 0 && cost_b < 0))
{
    if (abs_a > abs_b)
        return (abs_a);
    return (abs_b);
}
return (abs_a + abs_b);
```

- Si les deux rotations vont dans le **même sens**, on peut utiliser `rr` ou `rrr` pour les faire en même temps. Le coût est donc le maximum des deux valeurs absolues.
- Si elles vont dans des sens **opposés**, on ne peut pas les combiner : le coût est la somme.

### `complex_insert`

```c
cost_a = get_cost(complex_target_index(a, value), a->size);
cost_b = get_cost(complex_find(b, value), b->size);
rotate_ab(a, b, cost_a, cost_b);
pa(a, b);
```

`rotate_ab` :

- utilise `rr` tant que `cost_a > 0` et `cost_b > 0` ;
- utilise `rrr` tant que les deux sont négatifs ;
- termine les rotations restantes séparément avec `ra`/`rra` et `rb`/`rrb` ;
- puis `pa` amène la valeur de `b` vers `a`.

---

## 7. Pourquoi l’ordre reste correct

L’invariant est : **`a` reste triée au sens circulaire**.

Quand on insère `value` :

- on trouve son successeur dans `a` (le plus petit élément supérieur) ;
- on amène ce successeur au sommet ;
- `pa` place `value` juste au-dessus de lui.

Donc `value` s’insère exactement entre son prédécesseur et son successeur. L’ordre croissant circulaire est préservé.

À la fin, `complex_align(a)` trouve l’index du minimum et le ramène au sommet avec `ra` ou `rra`. La pile `a` est alors triée de façon linéaire, du plus petit au plus grand.

---

## 8. Pourquoi traiter les bandes de la plus grande à la plus petite ?

Parce que cela localise les insertions.

Quand on traite une bande `B`, toutes les valeurs déjà dans `a` proviennent :

- des bandes plus grandes, donc strictement supérieures à toutes les valeurs de `B` ;
- éventuellement des valeurs de `B` déjà insérées.

Ainsi, les valeurs de `B` s’insèrent toutes dans la même zone de `a` : juste avant le bloc des bandes supérieures. Ce bloc courant grandit au fur et à mesure, mais sa taille reste au plus `chunk_size = O(√n)`.

De même, dans `b`, les éléments de la bande courante sont encore proches du sommet : ils occupent au plus `chunk_size` positions, réparties entre le début et la fin de `b` après d’éventuelles rotations. Donc `get_cost` pour `b` reste lui aussi en `O(√n)`.

C’est ce qui donne le `O(n√n)` en nombre d’opérations émises : chaque élément coûte `O(√n)` rotations maximum, et il y a `n` éléments.

---

## 9. Complexité réelle et nuance importante

### En opérations `push_swap`

```text
Normalisation          : O(n²) à cause du tri par insertion de la copie
Phase 1 (distribution) : O(n√n)
Phase 2 (remontée)     : O(n√n)
Mémoire                : O(n) pour les piles + O(n) pour la copie triée
```

La phase 1 est en `O(n√n)` car il y a environ `√n/4` passes de `O(n)`.

La phase 2 est en `O(n√n)` car chaque élément est inséré avec au plus `O(√n)` rotations dans `a` et `O(√n)` dans `b`.

### Nuance : les comparaisons internes

`best_in_range` parcourt **tout `b`** pour chaque élément inséré. Cela représente `O(n²)` comparaisons au total dans le pire cas.

Mais ces comparaisons ne sont **pas** des opérations `push_swap` : elles ne produisent ni `ra`, ni `rb`, ni `pb`, ni `pa`. La complexité demandée par le sujet porte sur le nombre d’opérations émises, pas sur le temps CPU des comparaisons.

Autrement dit :

```text
opérations push_swap : O(n√n)
comparaisons C       : O(n²)
```

### Le rôle de `fr = 1`

Dans la stratégie complex, `fr` vaut `3` ou `4` et permet de découper chaque bande en deux sous-plages pour raffiner les insertions.

Pour medium, `fr = 1` :

- `mid` vaut `base + chunk_size` ;
- la première plage est vide ;
- la seconde plage est la bande entière.

Medium est donc la version « une bande = une plage » de la remontée utilisée par complex.

---

## 10. Résumé pas à pas

Pour `n` valeurs :

1. Remplacer chaque valeur par son rang `0 … n-1`.
2. Calculer `div = max(2, floor(sqrt(n))/4)` et `chunk_size = n/div`.
3. Répéter pour `chunk = 1, 2, 3, …` :
   - parcourir les `a->size` éléments restants ;
   - pousser dans `b` ceux dont le rang est `< chunk * chunk_size` ;
   - faire tourner les autres avec `ra`.
4. À la fin, `b` contient les bandes de la plus grande à la plus petite.
5. Pour chaque bande, de la plus grande à la plus petite :
   - chercher dans `b` l’élément de la bande qui a le coût d’insertion minimal ;
   - calculer sa position d’insertion dans `a` via son successeur ;
   - faire les rotations `ra`/`rra`, `rb`/`rrb`, `rr`/`rrr` nécessaires ;
   - `pa` pour l’insérer.
6. Ramener le minimum de `a` au sommet avec `complex_align`.

En une phrase : **medium range les valeurs par tranches de rangs, les empile dans `b` dans l’ordre décroissant, puis les réinsère une par une dans `a` en choisissant à chaque fois la rotation la moins coûteuse.**
