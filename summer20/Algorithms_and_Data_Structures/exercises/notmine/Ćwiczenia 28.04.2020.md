# Ćwiczenia 28.04.2020

## Zadanie  3.8
::: info
Dane są trzy posortowane tablice $T_1, T_2$ i $T_3$. Celem jest znaleźć ich medianę.
:::
::: warning
- Będziemy numerować tablice od 1.
- Medianą $n$-elementowej tablicy jest element na $\lceil \frac{n}{2} \rceil$ pozycji.
:::

### Wariant dla dwóch tablic $T_1$ i $T_2$

$n$ - rozmiar $T_1$ i $T_2$
$x$ - mediana $T_1$ ($x = T_1[\lceil n/2 \rceil]$)
$y$ - mediana $T_2$ ($y = T_2[\lceil n/2 \rceil]$)

$x < y$

Elementów większych od $x$ jest: $\lfloor n/2 \rfloor + 1 + \lfloor n/2 \rfloor$ 

Od mediany większych elementów jest $n$

Stąd wiemy, że elementy na lewo od $x$ nie są medianą.
Podobnie, elementy na prawo od $y$ nie są medianą.

W takim razie usuwamy wszystkie elementy w tablicy $T_1$ mniejsze od $x$ i $\lceil n/2 \rceil - 1$ największych elementów z tablicy $T_2$.

Uwaga: tak naprawdę nie usuwamy elementów, tylko zmienamy indeksy początku i końca tablicy.

W każdym kroku zmniejszamy każdą z tablic o połowę, więc jest ich logarytmicznie wiele.

### Wariant dla trzech tablic

$n, m, k$ - długości odpowiednio $T_1, T_2$ i $T_3$
$x, y, z$ - mediany $T_1, T_2$ i $T_3$
Załóżmy, że $x < y < z$

Policzmy elementy większe od $x$:
$\lfloor n/2 \rfloor + \lfloor m/2 \rfloor + 1 + \lfloor k/2 \rfloor + 1$

Od wspólnej mediany większych elementów $\lfloor \frac{n+m+k}{2} \rfloor$

Stąd możemy usunąć $x$ i wszystkie elementy mniejsze od niego w $T_1$.

Możemy usunąć $z$ i wszystkie elementy większe od niego w $T_3$.


```
Dopóki T_1, T_2 i T_3 są niepusty:
    x, y, z - mediany T_1, T_2 i T_3
    n, m, k - długości T_1, T_2 i T_3
    Jeśli x < y < z:
        Jeśli n < k:
            Z T_1 usuwamy pierwsze (n+1)/2 elementów (dzielenie całkowite)
            Z T_3 usuwamy ostatnie (n+1)/2 elementów
        W p. p.:
            Z T_1 usuwamy pierwsze (k+1)/2 elementów (dzielenie całkowite)
            Z T_3 usuwamy ostatnie (k+1)/2 elementów
    Pozostałe przypadki symetryczne
Potem kontynuujemy algorytm dla dwóch tablic
```

Czas działania: $O(\log n)$
Złożoność pamięciowa: $O(1)$ dodatkowej pamięci

## Zadanie 4.1
::: info
Dana tablica $A \geq 0$ o wymiarach $n \times m$. Znaleźć drogę przez tablicę wykorzystującą ruchy w prawo (również po przekątnej) oraz w górę i w dół minimalizującą sumę pól.
:::

$S$ - tablica $n \times m$ przechowująca w polu $i, j$ długość najkrótszej drogi do tego pola

Liczymy dla każdej kolumny długość najkrószej drogi dwa razy - najpierw przy założeniu, że nie możemy iść w górę, a potem przy założeniu, że nie możemy iść w dół.

```
Dla i = 1 to m:
    Dla j = 1 to n:
        S[j][i] = min(S[j-1][i], S[j-1][i-1], S[j][i-1], S[j+1][i-1])
    Dla j = n to 1:
        S[j][i] = min(S[j][i], S[j+1][i])

Zwróć minimum z ostatniej kolumny
```

#### Obserwacja
Weźmy dowolną najkrótszą drogę. W żadnej kolumnie nie wykonuje zarówno ruchów w górę i w dół.
#### Dowód
W przeciwnym razie znajdźmy ten moment kiedy idzie najpierw w górę, a potem w dół i usuńmy te dwa ruchy, w efekcie poprawiając drogę.

Złożoność pamięciowa: $O(n)$
Złożoność czasowa: $O(nm)$

## Znajdowanie najtańszej drogi (a nie kosztu jak wyżej)
Zapamiętujemy z której komórki przyszliśmy. Na koniec wracamy się po tablicy.

Złożoność pamięciowa: $O(nm)$

## Zadanie 4.2
Jako zadanie domowe

## Zadanie 4.3
::: info
Dany jest zbiór $A = \{a, b, c\}$ oraz relacja $\odot$. Czy można postawić nawiasy w ciągu $x_1 \odot x_2 \odot \dots \odot x_n$ tak, aby wynik wynosił $a$?
:::

Załóżmy, że ostatnią wykonujemy operację między $x_i$ a $x_{i+1}$. Jeśli znamy zbiór możliwych wyników dla $x_1 \odot \dots \odot x_i$ oraz dla $x_{i+1} \odot \dots \odot x_n$ to możemy wyliczyć dla $x_1 \odot \dots \odot x_n$.

Co musimy policzyć: dla każdego $1 \leq i \leq j \leq n$ musimy wyliczyć możliwe wyniki dla $x_i \odot \dots \odot x_j$. Będziemy je przechowywać w tablicy $T$ (wymiary $n\times n$).

$T[i][j]$ - zbiór

```
Chcemy wyliczyć T[i][j]
Dla k = i do j-1:
    Dla x w T[i][k]:
        Dla y w T[k+1][j]:
            T[i][j].add(x * y)
```
Ta procedura trwa $O(j)$

```
Dla j = 1 do n:
    Dla i = 1 do n - j + 1:
        Wylicz T[i][i+j-1]
```

Złożoność obliczeniowa: $O(n^2 \cdot n ) = O(n^3)$
Złożoność pamięciowa: $O(n^2)$

## Zadanie 4.4

$c$ - funkcja wagowa na krawędziach grafu

Stan - w pionki są w $v_i$ i $v_j$ - źle.

Stan - jeden pionek jest w $v_i$, a drugi musi być albo w tym wierzchołku, albo w którymś z sąsiednich.

$T$ - o wymiarach $n \times 3$

$T[i][0]$ - jeden pionek w $v_i$, a drugi w $v_{i-1}$
$T[i][1]$ - jeden pionek w $v_i$, a drugi w $v_{i}$
$T[i][2]$ - jeden pionek w $v_i$, a drugi w $v_{i+1}$
($T[i][0] = T[i-1][2]$)

```
Chcemy wyliczyć T[i][0]
Dla k = 1 do i-1:
    T[i][0] = min(T[i][0], T[k][2] + c(k,i) + c(k+1, k+2) + c(k+2, k+3) + ... + c(i-2, i-1))
```

Możemy spamiętać na początku: c(k+1, k+2) + c(k+2, k+3) + ... + c(i-2, i-1) - $O(n^2)$

Czas działania liczenia T[i][0] to $O(i)$

```
Dla i = 1 do n:
    Dla j=0 do 2:
        Wylicz T[i][j]
```

Złożoność czasowa: $O(n^2)$
Złożoność pamięciowa: $O(n^2)$

#### Poprawianie złożoności pamięciowej

```
s = 0
Chcemy wyliczyć T[i][0]
Dla k = i-1 do 0:
    s = s + c(k+1, k+2)
    T[i][0] = min(T[i][0], T[k][2] + c(k,i) + s)
```

Złożoność pamięciowa: $O(n)$