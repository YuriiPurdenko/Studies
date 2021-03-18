# Ćwiczenia 12.05.2020

## Zadanie 4.6
:::info
Dla ciągów $x$ i $y$ znaleźć najdłuższy wspólny podciąg, który:
- zawiera podciąg "egzamin"
- nie zawiera podciągu "egzamin"
- zawiera podsłowo "egzamin"
- nie zawiera podsłowa "egzamin"
:::

#### Zwykły najdłuższy wspólny podciąg
Obliczamy tablicę $T$, gdzie $T[i][j]$ oznacza długość najdłuższego wspólnego podciągu prefiksów $x[1...i]$ oraz $y[1...j]$

$$
T[i][j] = \begin{cases}
    \max(T[i-1][j], T[j-1][i]), \ & \text{jeśli } x[i] \neq y[j] \\
    T[i-1][j-1] + 1, \ & \text{jeśli } x[i] = y[j]
\end{cases}
$$

#### Zawiera podciąg "egzamin"
$T_0$ - najdłuższy wspólny podciąg
$T_1$ - najdłuższy wspólny podciąg zawierający literę e

$$
T_1[i][j] =
\begin{cases}
    \max(T_1[i-1][j], T_1[j-1][i]), \ & \text{jeśli } x[i] \neq y[j] \\
    T_1[i-1][j-1] + 1, & \text{jeśli } x[i] = y[j] \neq ``e``\\
    T_0[i-1][j-1] + 1, & \text{jeśli } x[i] = y[j] = ``e``
\end{cases}
$$

$T_2$ - najdłuższy wspólny podciąg zawierający podciąg "eg"

$$
T_2[i][j] =
\begin{cases}
    \max(T_2[i-1][j], T_2[j-1][i]), \ & \text{jeśli } x[i] \neq y[j] \\
    T_2[i-1][j-1] + 1, & \text{jeśli } x[i] = y[j] \neq `g`\\
    T_1[i-1][j-1] + 1, & \text{jeśli } x[i] = y[j] = `g`
\end{cases}
$$

$T_3$ - najdłuższy wspólny podciąg zawierający podciąg "egz"
$T_7$ - najdłuższy wspólny podciąg zawierający podciąg "egzamin"

Złożoność czasowa: $O(n^2)$
Złożoność pamięciowa: $O(n^2)$

#### Nie zawiera podciągu "egzamin"
$T_1$ - najdłuższy wspólny podciąg bez litery e
$T_2$ - najdłuższy wspólny podciąg bez podciągu "eg"
$T_7$ - najdłuższy wspólny podciąg bez podciągu "egzamin"

$$
T_2[i][j] =
\begin{cases}
    \max(T_2[i-1][j], T_2[j-1][i]), \ & \text{jeśli } x[i] \neq y[j] \\
    T_2[i-1][j-1] + 1, & \text{jeśli } x[i] = y[j] \neq g\\
    \max(T_1[i-1][j-1] + 1, T_2[i-1][j-1]),  &\text{jeśli } x[i] = y[j] = g
\end{cases}
$$

Złożoność czasowa: $O(n^2)$
Złożoność pamięciowa: $O(n^2)$

#### Zawiera podsłowo "egzamin"
$T_0$ - najdłuższy wspólny podciąg
$T_1$ - najdłuższy wspólny podciąg kończący się na literze $e$

$$
T_1[i][j] = 
\begin{cases}
    \max(T_1[i-1][j], T_1[j-1][i]), \ & \text{jeśli } x[i] \neq y[j] \\
    T_1[i-1][j-1], & \text{jeśli } x[i] = y[j] \neq e \\
    T_0[i-1][j-1] + 1, & \text{jeśli } x[i] = y[j] = e
\end{cases}
$$

$T_2$ - najdłuższy wspólny podciąg kończący się słowem "eg"
$T_7$ - najdłuższy wspólny podciąg kończący się słowem "egzamin"

$T_8$ - najdłuższy wspólny podciąg zawierający podsłowo egzamin

$$
T_8[i][j] = 
\begin{cases}
\max(T_8[i-1][j], T_8[j-1][i]), \ & \text{jeśli } x[i] \neq y[j] \\
\max(T_8[i-1][j-1] + 1, T_7[i][j]), &\text{jeśli } x[i] = y[j]
\end{cases}
$$

Złożoność czasowa: $O(n^2)$
Złożoność pamięciowa: $O(n^2)$

#### Nie zawiera podsłowa "egzamin"
$T_1$ - najdłuższy wspólny podciąg nie zawierający podsłowa "egzamin" i niekończący się na e
$T_2$ - najdłuższy wspólny podciąg nie zawierający podsłowa "egzamin" i niekończący się podsłowem "eg"
$T_3$ - najdłuższy wspólny podciąg nie zawierający podsłowa "egzamin" i niekończący się podsłowem "egz"
$T_7$ - najdłuższy wspólny podciąg nie zawierający podsłowa "egzamin"

$$
T_1[i][j] = 
\begin{cases}
\max(T_1[i-1][j], T_1[j-1][i]), \ & \text{jeśli } x[i] \neq y[j] \\
T_1[i-1][j-1], & \text{jeśli } x[i] = y[j] = e \\
\max(T_6[i-1][j-1] + 1, T_1[i-1][j-1]), & \text{jeśli } x[i] = y[j] = n \\
T_7[i-1][j-1] + 1 &\text{jeśli } x[i] = y[j] \not\in \{e,n\}
\end{cases}
$$

$$
T_2[i][j] = 
\begin{cases}
\max(T_2[i-1][j], T_1[j-1][i]), \ & \text{jeśli } x[i] \neq y[j] \\
\max(T_1[i-1][j-1] + 1, T_2[i-1][j-1]), & \text{jeśli } x[i] = y[j] = g \\
\max(T_6[i-1][j-1] + 1, T_2[i-1][j-1]), & \text{jeśli } x[i] = y[j] = n \\
T_7[i-1][j-1] + 1 &\text{jeśli } x[i] = y[j] \not\in \{g,n\}
\end{cases}
$$

Złożoność czasowa: $O(n^2)$
Złożoność pamięciowa: $O(n^2)$

## Zadanie 4.7
:::info
Dane są liczby $a_1, \dots, a_n \in [-C, C]$. Czy można je podzielić na trzy podzbiory o równej sumie.
:::

Niech $S = \sum_{i=1}^n a_i$

$T[i][j][k]$ - czy można podzielić liczby $a_1, \dots, a_n$ na trzy podzbiory, których suma to odpowiednio $i$, $j$ i $k$

Wystarczy:
$T[i][j]$ - czy można podzielić liczby $a_1, \dots, a_n$ na trzy podzbiory, których suma to odpowiednio $i$, $j$ i $S - i - j$

Rozpatrzmy liczbę $a_k$ - jeśli $T[i][j] = 1$, to $T[i+a_k][j] = 1$

```
T[0][0] = 1
for k = 1 to n:
    if a_k < 0:
        for i = -Cn to Cn
            for j = -Cn to Cn
                if T[i][j] = 1 then
                    T[i + a_k][j] = 1
                    T[i][j + a_k] = 1
    else:
        for i = Cn to -Cn
            for j = Cn to -Cn
                if T[i][j] = 1 then
                    T[i + a_k][j] = 1
                    T[i][j + a_k] = 1
Zwróć T[S/3][S/3]
```

Złożoność pamięciowa: $Cn \cdot Cn = O(n^2)$
Złożoność czasowa: $n \cdot 2 Cn \cdot 2 Cn = O(n^3)$

## Zadanie 4.8

W każdej kolumnie jest 8 możliwości.

$c[i][j]$ - suma pól w $i$-tej kolumnie pokrytych $j$-tym ustawieniem kamyków.

$T[n][8]$
$T[i][j]$ - najlepsze możliwe ustawienie w pierwszych $i$ kolumnach, takie że w ostatniej kolumnie jest $j$-te ustawienie

$$
    T[i][j] = \max_{k - \text{ustawienie zgodne z j-tym ustawieniem}}(T[i-1][k] + c[i][j])
$$

Złożoność pamięciowa: $O(n)$
Złożoność czasowa: $O(n)$

## Zadanie 4.9
:::info
Dane są odcinki pomiędzy równoległymi prostymi $l'$ i $l''$. Znaleźć 
- największy zbiór nieprzecinających się odcinków
- liczbę takich zbiorów.
:::
![](https://i.imgur.com/TsB5iN0.png)

Mamy dwa odcinki $p_1$ od $(x_1, y_1)$ do $(x_2, y_2)$ oraz $p_2$ od $(x_3, y_1)$ do $(x_4,y_2)$.

$p_1$ i $p_2$ się przecinają wtedy i tylko wtedy $x_1 < x_3$ oraz $x_2 > x_4$ lub $x_1 > x_3$ oraz $x_2 < x_4$.

Zapominamy o konkretnych wartościach $x$

Przykładowo odcinek $(1,6)$

Dane z przykładu:
$(1,6), (2,4), (3,7), (4,1), (5,8), (6,5), (7,2), (8,9), (9,3)$

Rozpatrzmy jakieś rozwiązanie $S$. Posortujmy $S$ względem pierwszej współrzędnej. Wtedy druga współrzędna też jest rosnąca.

$6, 4, 7, 1, 8, 5, 2, 9, 3$

Czyli to jest po prostu najdłuższy rosnący podciąg.

Mamy dany ciąg $\sigma$
Jedna z możliwości: najdłuższy wspólny podciąg $\sigma$ i ciągu $1,2,3,\dots,n$
Czas $O(n^2)$


Przechodzimy po ciągu $\sigma$ i dla każdego elementu  liczymy najdłuższy rosnący podciąg kończący się w tym elemencie.

Struktura: dla każdego $i$  trzymamy liczby $\sigma_j$, takie że najdłuższy wspólny podciąg kończący się w $\sigma_j$ ma długość $i$

1: 6, 4, 1
2: 7, 5, 2
3: 8, 3
4: 9

Trzymamy tę strukturę jako tablica list.

#### Lemat
Każda lista w tej strukturze jest malejąca.

```
x - ciąg wejściowy
S - struktura jak wyżej
Dla i = 1 do n:
    Znajdź największe j, takie że S[j] zawiera liczbę mniejszą od x[i]    (i)
    Dodaj x[i] do S[j+1]
```

(i) - wykonujemy za pomocą binary search

Złożoność czasowa: $O(n\log n)$
Złożoność pamięciowa: $O(n)$


#### Liczba najdłuższych rosnących podciągów

Przykład: $6, 4, 7, 1, 8, 5, 2, 9, 3$

1: (6,1), (4,1), (1,1)
2: (7,2), (5,2), (2,1)
3: (8,2), (3,1)
4: (9,2)


$p[i]$ - wskaźnik w $i$-tej liście
$p[i]$ - największy element w $i$-tej liście mniejszy od ostatniego elementu w $i+1$-tej liście

$p[1] = (1,1)$
$p[2] = (2,1)$
$p[3] = (8,2)$

$s[i]$ - liczba rosnących podciągów długości $i$, które się kończą liczbą co najwyżej $p[i]$


```
x - ciąg wejściowy
S - struktura jak wyżej
p[n] - wskaźniki
s[n] - jak wyżej
Dla i = 1 do n:
    Znajdź największe j, takie że S[j] zawiera liczbę mniejszą od x[i]    (i)
    Dopóki p[j][0] > x[i]:        (ii)
        s[j] -= p[j][1]
        p[j] = p[j].next
    Dodaj do S[j+1] krotkę (x[i], s[j])
    s[j+1] = s[j+1] + s[j]
```

Złożoność pamięciowa: $O(n)$
Złożoność czasowa: $O(n\log n)$


## Zadanie 4.10
::: info
Znaleźć triangulację wielokąta, która minimalizuje długość najdłuższej przekątnej
:::
:::warning
Jako zadanie domowe
:::