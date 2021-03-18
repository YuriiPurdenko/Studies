# Ćwiczenia 20.05.2020

## Zadanie 5.1
:::info
Pokaż, że problem znajdowania otoczki wypukłej nie może być rozwiązany w modelu drzew decyzyjnych.
:::

![](https://i.imgur.com/eprVhCk.png)

$v_1 = (11,1)$
$v_2 = (14,2)$
$v_3 = (15,5)$
$v_4 = (12, 4)$

$w_1 = (11,1)$
$w_2 = (12.5,3.5)$
$w_3 = (15,5)$
$w_4 = (12,4)$

Wyniki porównań są takie same, a rozwiązania inne.


## Zadanie 5.2
:::info
Dane są $n$ liczby rzeczywiste. Problemem jest rozstrzygnięcie, czy wśród nich są trzy takie, które się sumują do 0. Pokaż, że ten problem wymaga czasu co najmniej $O(n\log n)$ w modelu liniowych drzew decyzyjnych.
:::


#### Liniowe drzewa decyzyjne
Porównujemy kombinacje liniowe danych na wejściu.

Wszystkich możliwych danych na wejściu jest $R^n$

Dla danego wierzchołka $v$, $S(v) \subseteq R^n$ oznacza dane wejściowe, które trafiają do $v$.

#### Lemat 
Dla każdego $v$, $S(v)$ jest wypukły.
(Jeśli $x, y\in S(v)$, to $\forall \alpha\in(0,1) \ \alpha x + (1-\alpha)y \in S(v)$ )


Zakładam, że $n$ jest parzyste.

$1,3,\dots, n-1,2n,-(2n+2),-(2n+4),\dots,-(2n+n-2)$
(Rozwiązaniem dla tej instancji jest NIE).

Tworzymy $\frac{n}{2}$ instacji poprzez permutowanie pierwszych $\frac{n}{2}$ pozycji i otrzymujemy instacje: $P_1, P_2, \dots, P_{\frac{n}{2}!}$

#### Lemat
Każda z tych instancji musi być w różnym liściu.

#### Dowód
Załóżmy, że $P_i$ i $P_j$ są w jednym liściu (odpowiedzią dla tego liścia jest NIE).

Niech $k$ będzie współrzędną, na której $P_i$ i $P_j$ się  różnią. Załóżmy bez utraty ogólności, że $P_j(k) > P_i(k)$

Z Lematu o wypukłości, niech $p$ będzie instancją należącą do tego liścia, która na $k$-tej współrzędnej ma liczbę parzystą.
(Formalnie: $p = \frac{P_j(k) - P_i(k) - 1}{P_j(k) - P_i(k)}P_i + \frac{1}{P_j(k) - P_i(k)}P_j$)
Łatwo zauważyć, że: $p(k)=P_i(k) + 1$.

W takim razie algorytm dla instacji $p$ zwraca NIE. Ale poprawna odpowiedź to TAK, bo $p(k) + 2n + (- (2n+p(k)))=0$.



Mamy co najmniej $\frac{n}{2}!$ liści. 

Jeśli wysokość to $h$, to liczba liści to $3^h$.

W takim razie $3^h > \frac{n}{2}!$, a więc $h> O(n\log n)$.

$n! \approx (\frac{n}{e})^n$ (wzór Stirlinga)
$n^n = 3^{\log_3 n \cdot n}$
$n! \approx 3 ^{(\log_3 n  - log_3 e)\cdot n}$

$3^h >3 ^{(\log_3 n  - log_3 e)\cdot n}$

$h > (\log_3 n  - log_3 e)\cdot n = n\log n - O(n) = O(n\log n)$


## Zadanie 5.4
:::info
Pokaż, że potrzeba co najmniej $2n - 1$ porównań, żeby scalić dwa $n$-elementowe ciągi.
:::

Bazowy zestaw:
$x_0 = 1,3,5, \dots, 2n-1$
$y_0 = 2,4,6, \dots, 2n$

Wynik: $1,2,3, \dots, 2n$

$x_1 = 2,3,5,7,\dots, 2n-1$
$y_1 = 1,4,6,8,\dots, 2n$

Tworzymy $2n-1$ zestawów poprzez zamianę $i$ i $i+1$ miejscami.

$x_2 = 1,2,5,7,\dots 2n-1$
$y_2 = 3,4,6,8,\dots 2n$

$x_3 = 1,4,5,7,\dots$
$y_3 = 2,3,6,8,\dots$

Dopóki co najmniej $2$ zestawy są zgodne z tym co odpowiadaliśmy, to algorytm nie może się zakończyć.

Schemat odpowiadania: odpowiadamy zgodnie z bazowym zestawem.

#### Lemat 
Jedno zapytanie algorytmu usuwa co najwyżej jeden zestaw danych.
#### Dowód
Co najwyżej jeden zestaw ma inny wynik porównania niż bazowy zestaw.

Skoro każde porównanie wyklucza co najwyżej jeden zestaw, to potrzeba co najmniej $2n-1$ porównań.