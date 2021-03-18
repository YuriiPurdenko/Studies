# Ćwiczenia 20.04

## Zadanie 2.4
Drzewo jako listy sąsiedztwa
```
D[v] - stopnie wierzchołków
L - lista liści
M - pusta lista
k/2 razy powtórz:                                             (1)
    dla każdego wierzchołka v w L:                            (2)
        pokoloruj v
        dla każdego wierzchołka w w liście sąsiedztwa v:      (3)
            D[w] -= 1
            Jeśli D[w] = 1:
                dodaj w do M
    L = M
    M = pusta lista
```

Pętla (1) wykona się $O(n)$
Pętla (2) wykona się $O(n)$
Pętla (3) co najwyżej dwa razy dla każdej krawędzi, czyli $O(n)$

$O(n)$

## Zadanie 2.9

::: info
Kodowanie Huffmana
:::
:::info
Niech $c: V \rightarrow \mathcal{R_+}$ będzie wagami wierzchołków, a $d(v)$ długośią ścieżki od $v$ do korzenia. Wtedy:
$$ EL(T) = \sum_{v - liść\in T} c(v) \cdot d(v) $$

Dla zbioru $W = \{w_1, \dots, w_n\}$ znaleźć drzewo $T$ w którym każde $w_i$ jest wagą dokładnie jednego liścia, a $EL(T)$ jest najmniejsze wśród wszystkich takich drzew.
:::

```
Tworzymy dla każdego w_i wierzchołek (c(v_i) = w_i) i wrzucamy je do kolejki priorytetowej Q
Dopóki |Q| > 1:
    Niech u,v - dwa pierwsze wierzchołki z Q
    Tworzymy wierzchołek w i ustawiamy u i v jako dzieci w
    c(w) = c(u) + c(v) 
    Wrzucamy w do Q
```

Złożoność pamięciowa $O(n)$
Złożoność czasowa $O(n \log n)$

#### Lemat 1 
Niech $w_i$ i $w_j$ będą najmniejszymi wagami w $W$. Istnieje optymalne rozwiązanie, w którym $w_i$ i $w_j$ są braćmi.
#### Dowód
Niech $T$ będzie rozwiązaniem optymalnym.
Weźmy te dwa wierzchołki $w_k$ i $w_m$, które są braćmi i są na najniższym poziomie drzewa $T$.

Możemy zamienić $w_k$ i $w_i$ miejscami oraz $w_m$ i $w_j$ i stworzyliśmy drzewo $T'$. $EL(T') \leq EL(T)$.

$d_T(w_k) \geq d_T(w_i)$
$w_i \leq w_k$

$EL(T') = EL(T) - w_i d_T(w_i) - w_k d_T(w_k) + w_i d_T(w_k) + w_k d_T(w_i) \\ = EL(T) + w_i(d_T(w_k) - d_T(w_i) + w_k(d_T(w_i) - d_T(w_k)) \\ = EL(T) + (d_T(w_k) - d_T(w_i)) (w_i - w_k)$

$d_T(w_k) - d_T(w_i) \geq 0$
$w_i - w_k \leq 0$

$EL(T) + (d_T(w_k) - d_T(w_i)) (w_i - w_k) \leq EL(T)$

#### Lemat 2
Drzewo zwrócone przez algorytm jest optymalne.
#### Dowód indukcyjny po n
1) Baza: n = 1, lub n = 2 - istnieje tylko jedno takie drzewo więc jest optymalne
2) Krok: załóżmy, że dla każdego $k\leq n$ algorytm zwraca optymalne rozwiązanie.

$w_i, w_j$ - najmniejsze wagi
Rozpatrzmy $W' = W \setminus \{w_i, w_j\} \cup \{w_i + w_j\}$
$P$ - rozwiązanie zwrócone przez algorytm dla $W$
$P'$ - rozwiązanie zwrócone przez algorytm dla $W$

Niech $T$ - drzewo optymalne dla $W$
Z Lematu 1. wiemy, że $w_i$ i $w_j$ są braćmi. Usuwamy z $T$ $w_i$ i $w_j$, a jakow wagę ich ojca ustawiamy $w_i + w_j$. Niech to drzewo nazywa się $T'$.

$EL(T') \geq EL(P')$

$EL(T) = EL(T') + w_i + w_j$
$EL(P) = EL(P') + w_i + w_j$

$EL(P) \leq EL(T)$


## Zadanie 3.2
```
Procedure MaxMin(S:set)
    if |S| = 1 then return {a_1,a_1}
    else
        if |S| = 2 then return (max(a_1,a_2), min(a_1,a_2))
        else
            podziel S na dwa równoliczne (z dokładnością do jednego elementu) podzbiory S_1 S_2
            (max1,min1) <- MaxMin(S_1)
            (max2,min2) <- MaxMin(S_2)
            return (max(max1,max2), min(min1,min2))
```

Oznaczenia:
$T(n)$ - liczba porównań wykonywanych przez algorytm MaxMin

$T(n) = T(\lfloor \frac{n}{2} \rfloor) + T(\lceil \frac{n}{2}\rceil) + 2$

#### Lemat
Zapiszmy $n = 2^i + k$, gdzie $k < 2^i$. Wtedy
$$T(n) =  \begin{cases} 
      \frac{3}{2} 2^i - 2 + 2k & \text{jeśli } k\leq 2^{i-1} \\
      2 \cdot 2^{i} - 2 + k & \text{jeśli } k > 2^{i-1} 
   \end{cases}
$$

#### Dowód indukcyjny po n
Baza indukcji - pominięta 
Weźmy $n = 2^i + k$.
1. $k \leq 2^{i-1}$
$\lceil \frac{n}{2} \rceil = 2^{i-1} + \lceil \frac{k}{2} \rceil$
$\lceil \frac{k}{2} \rceil \leq 2^{i-2}$
$T(n) = T(\lfloor \frac{n}{2} \rfloor) + T(\lceil \frac{n}{2}\rceil) + 2 = \frac{3}{2} 2^{i-1} - 2 + 2\lfloor \frac{k}{2} \rfloor + \frac{3}{2} 2^{i-1} - 2 + 2\lceil \frac{k}{2} \rceil + 2 \\= \frac{3}{2} 2^i - 2 + 2k$
2. $k > 2^{i-1}$ - Podobnie

$$ G(n) = T(n) - \lceil \frac{3}{2} n - 2 \rceil $$

$n = 2^i + k$
$$ G(n) = T(n) - \lceil \frac{3}{2} 2^i + \frac{3}{2} k - 2 \rceil = T(n) - \frac{3}{2} 2^i + 2 - \lceil \frac{3}{2} k \rceil$$

$$G(n) = \begin{cases}
    \lfloor \frac{k}{2} \rfloor & \text{jeśli } k\leq 2^{i-1} \\
    2^{i - 1} - \lceil \frac{k}{2} \rceil & \text{jeśli } k > 2^{i-1} 
    \end{cases}
$$

1 podpunkt
$G(n) = 0$, kiedy $\lfloor \frac{k}{2} \rfloor = 0$ lub $\lceil \frac{k}{2} \rceil = 2^{i-1}$.

Dla ustalonego $i$, $G(n) = 0$ gdy $k = 0$, lub $k = 1$, lub $k = 2^i - 1$.

2 podpunkt
Ustalmy $i$
$G(n)$ jest największe dla $k = 2^{i-1}$ i wynosi
$G(n) = 2^{i-2}$

$n = 2^i + k = 2^i + 2^{i-1}$
$G(n) = \frac{n}{6}$

3 podpunkt
```
Procedure MaxMin(S:set)
    if |S| = 1 then return {a_1,a_1}
    else
        if |S| = 2 then return (max(a_1,a_2), min(a_1,a_2))
        else
            jeśli |S| jest parzysta, a |S|/2 jest nieparzysta:
                podziel S na podzbiory S_1, S_2 o mocach odpowiednio |S|/2 - 1 i |S|/2 + 1
            else:
                podziel S na dwa równoliczne (z dokładnością do jednego elementu) podzbiory S_1 S_2
            (max1,min1) <- MaxMin(S_1)
            (max2,min2) <- MaxMin(S_2)
            return (max(max1,max2), min(min1,min2))
```

## Zadanie 3.3
::: info
Mamy dane dwa zbiory wypukłe $S_1$ i $S_2$. Mamy znaleźć najmniejszy zbiór wypukły zawierający te dwa zbiory.
:::

![](https://i.imgur.com/NbZorBy.png)


Przechowujemy otoczki wypukłe jako listę wierzchołków zgodnie z ruchem wskazówek zegara.

![](https://i.imgur.com/qQKAdhK.png)

```
Procedure PolaczZbiory(S1, S2):
    x,y - odcinek nieprzecinający się z S1 i S2 (x - najbardziej na prawo wierzchołek S1)
    Powtarzaj:
        Niech u będzie poprzednikiem x
        Jeśli u - x - y skręca w lewo:
            x = u
        Niech v - następnik y
        Jeśli x - y - v skręca w lewo:
            y = v
    Dopóki nie dojdzie do żadnej zmiany x i y
    
    Zrób to samo dla dolnego odcinka
```

Złożoność czasowa: $O(n)$
Złożoność pamięciowa: $O(1)$ dodatkowej pamięci

#### Lemat 1
Nie ma mniejszego zbioru wypukłego zawierającego $S_1$ i $S_2$
#### Szkic Dowodu
Znalezione odcinki należą do najmniejszego zbioru wypukłego, a więc wszystko pomiędzy nimi też. Więc nie może istnieć mniejszy zbiór wypukły.

#### Lemat 3
Jeśli w wielokącie wszystkie kąty są mniejsze niż 180, to ten wielokąt jest wypukły.

#### Lemat 2
Zwrócony zbiór jest zbiorem wypukłym
#### Dowód
Kąty przy niebieskich krawędziach są mniejsze niż 180 stopni, bo w przeciwnym razie spełniony byłby warunek z pętli. Więc z Lematu 3. jest to zbiór wypukły.

---

Mając dane wierzchołki $x, y, v$ jak sprawdzić czy ścieżka $x - y - v$ skręca w lewo. Inaczej mówiąc pytamy się o kąt przy y między odcinkami $(y,x)$ i $(y,v)$.

Niech $a = (y,x)$, $b = (y,v)$. Wtedy:
$$ |a \times b| = |a| |b| sin \alpha$$
gdzie $\alpha$ jest kątem między a i b.


## Zadanie 3.7
::: info
Macierz A rozmiaru $n × n$ nazywamy macierzą Toeplitza, jeśli jej elementy spełniają równanie $A[i, j] = A[i − 1, j − 1]$ dla $2 ≤ i, j ≤ n$.
:::

### Podpunkt a
Wystarczy pamiętać pierwszy wiersz i pierwszą kolumnę - $2n - 1$ wartości. Możemy dodać $2n - 1$ wartości w czasie $O(n)$.

### Podpunkt b

Macierz Toeplitza jest następującej postaci:
$$\begin{bmatrix}
A & B \\
C & A
\end{bmatrix}$$

$$\begin{bmatrix}
A & B \\
C & A
\end{bmatrix} \cdot
\begin{bmatrix}
u \\
v
\end{bmatrix} = 
\begin{bmatrix}
A\cdot u + B \cdot v + (Av - Av)\\
C\cdot u + A \cdot v + (Au - Au)
\end{bmatrix} = 
\begin{bmatrix}
A(u + v) + (B - A)v \\
A(v + u) + (C - A)u
\end{bmatrix}$$

Czyli musimy obliczyć:
$A(u+v)$, $(B-A)v$ oraz $(C-A)u$

Czas działania tego algorytmu to:
$T(n) = 3T(n/2) + O(n) = O(n^{\log_2 3})$
Złożoność pamięciowa: $O(n)$

