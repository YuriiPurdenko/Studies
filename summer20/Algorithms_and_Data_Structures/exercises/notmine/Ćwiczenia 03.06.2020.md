# Ćwiczenia 03.06.2020

## Zadanie 6.2

Mamy:
- korzeń
- na pierwszym mamy $n/4$ wierzchołków
- na drugim poziomie mamy $3n/4$ wierzchołków

Wyróżnijmy wierzchołek $v$ na pierwszym poziomie. $v$ ma $n/2$ synów. Pozostałe wierzchołki mają jednego syna z drugiego poziomu.

Etykieta wierzchołka $v$ ma długość $n/2$. Pozostałe wierzchołki o długości $1$.

Koszt sortowania: $\Theta((N+k)l_{max})$
W naszym przypadku: $N = n/4$
$k = 1$
$l_{max} = n/2$
Koszt: $\Theta(n^2)$

## Zadanie 6.3
:::info
Jak sprawdzić izomorfizm drzew nieukorzenionych?
:::

### Wierzchołki centralne

Promień Grafu:
$$r(G) = \min_{v\in V(G)} \max_{u\in V(G)} d(u,v)$$
Wierzchołek grafu, to wierzchołek, który minimalizuje promień grafu.

#### Lemat
W drzewie jest 1 lub 2 wierzchołki centralne.
#### Idea Dowodu
Usuwamy liście, co nie zmienia zbioru wierzchołków centralnych, dopóki nie zostaną 1 lub 2 wierzchołki.

Idea Algorytmu:
Usuwamy warstwami liście, dopóki nie zostaną 1 lub 2 wierzchołki.

```
T1, T2 - drzewa
S1 - wierzchołki centralne drzewa T1
S2 - wierzchołki centralne drzewa T2
If |S1| != |S2|:
    Return 'nieizomorficzne'
If |S1| == |S2| == 1:
    Ukorzeń T1 w jego wierzchołku centralnym
    Tak samo T2
    Sprawdź izomorfizm drzew ukorzenionych
If |S1| == |S2| == 2:
    Sprawdź wszytkie kombinacje wierzchołków centralnych 
```

Złożoność: $O(n)$


## Zadanie 6.4
:::info
Sortowanie w miejscu rekordów o kluczach ze zbioru $\{1,2,3\}$.
:::

Utrzymujemy taki format tablicy:
1,1,1,1,2,2,2,2,2,_,_,_,_,_,_,3,3,3,3,3,3

```
T - tablica do posortowania
i,j,k
i - pierwszy indeks za 1
j - pierwszy indeks za 2
k - indeks gdzie się zaczynają 3
i = 0
j = 0
k = n
while j < k:
    if T[j] = 1:
        swap(T[i], T[j])
        i += 1
        j += 1
    if T[j] = 2:
        j += 1
    if T[j] = 3:
        swap(T[i], T[k-1])
        k -= 1
```

Złożoność czasowa: $O(n)$

## Zadanie 6.8
:::info
Procedura Split na drzewach AVL.
:::
:::warning
Jako zadanie domowe.
:::