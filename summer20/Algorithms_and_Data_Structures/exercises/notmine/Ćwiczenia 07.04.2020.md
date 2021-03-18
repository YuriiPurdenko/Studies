# Ćwiczenia 07.04.2020

## Zadanie 2.7

:::info
Mamy dwie maszyny i $n$ zadań. Każde zadanie ma dwie wartości $a_i$ i $b_i$ - czas wykonywania odpowiednio na maszynie $A$ i $B$. Każde zadanie musi być wykonane najpierw na maszynie $A$, a potem na $B$. Zadaniem jest uszegerować zadania tak, żeby zminimalizować czas zakończenia ostatniego zadania na maszynie $B$. 
:::

:::warning
Zakładam, że $\forall i \ a_i \neq b_i$
:::


![](https://i.imgur.com/7dEAUra.png)

$X_i$ - czas czekania przed zadaniem $b_{\sigma(i)}$ na maszynie $B$

#### Obserwacje:
- Wykonujemy na obu maszynach zadania w tej samej kolejności
- Cel: zminimalizować $\sum X_i$

$\sum_{i=1}^n a_i = \sum_{i=1}^n X_i + \sum_{i=1}^{n-1} b_i$
$\sum_{i=1}^n X_i = \sum_{i=1}^n a_i - \sum_{i=1}^{n-1} b_i$

![](https://i.imgur.com/VHrwyzb.png)

$\sum_{i=1}^n X_i = \sum_{i=1}^5 a_i - \sum_{i=1}^{4} b_i$


Jeśli $k$ - maksymalna liczba taka, że $X_k \neq 0$, to wtedy:

$$\sum_{i=1}^n X_i = \sum_{i=1}^k a_i - \sum_{i=1}^{k-1} b_i$$


$$\sum_{i=1}^n X_i = \max_{k\in \{1,\dots,n\}} (\sum_{i=1}^k a_i - \sum_{i=1}^{k-1} b_i)$$

$$S_k = \sum_{i=1}^k a_i - \sum_{i=1}^{k-1} b_i$$

$$\sum_{i=1}^n X_i = \max_{k} S_k $$

---

Jeśli $i$ i $j$ zadanie są obok siebie - które wykonać jako pierwsze.

$k$ - pozycja pierwszego z nich

Zmieniamy jedynie $S_k$ i $S_{k+1}$

Oznaczenia:
- $S_k^i$ - $S_k$ jeśli $i$ jest wcześniejszym zadaniem.
- $S = \sum_{l=1}^{k-1} a_l - \sum_{l=1}^{k-1} b_l$

$S_k^i = \sum_{l=1}^k a_l - \sum_{l=1}^{k-1} b_l = S + a_i$
$S_k^j = \sum_{l=1}^k a_l - \sum_{l=1}^{k-1} b_l = S + a_j$
$S_{k+1}^i = S + a_i + a_j - b_i$
$S_{k+1}^j = S + a_i + a_j - b_j$

i przed j, jeśli $\max(S_k^i, S_{k+1}^i) < \max(S_k^j, S_{k+1}^j)$

$$ \max(S + a_i, S+ a_i + a_j - b_i) < \max(S + a_j, S + a_j + a_i - b_j) $$
Odejmijmy od obu stron $S$ i $a_i + a_j$

$$\max(-a_j, -b_i) < \max(-a_i, -b_j)$$
$$\min(a_j, b_i) < \min(a_i, b_j)$$


Zadanie $i$ jest przed $j$ jeśli $\min(a_j, b_i) < \min(a_i, b_j)$.

Relacja $\preceq$, $i \preceq j$ wtedy i tylko wtedy $\min(a_j, b_i) < \min(a_i, b_j)$.

Cel: $\preceq$ zachodzi dla wszystkich zadań, nie tylko sąsiadujących.

#### Lemat
$\preceq$ jest przechodnia.

#### Dowód
Weźmy trzy zadania, i, j, k.
Pokażemy $i \preceq j$ i $j \preceq k$ to $i \preceq k$.

Czyli wiemy, że:
$\min(a_j, b_i) < \min(a_i, b_j)$
$\min(a_k, b_j) < \min(a_j, b_k)$

Chcemy pokazać, że:
$\min(a_k, b_i) < \min(a_i, b_k)$

1. $b_j \leq a_k$ 
$b_j < \min(a_j, b_k)$
$b_j < a_j, b_j < b_k$

a) $b_i \leq a_j$ $b_i < b_j < b_k$
$b_i < a_i$

$b_i < \min(a_i, b_k)$

b) $a_j < b_i$
$a_j < b_j < a_j$

2. $a_k < b_j$
$a_k < a_j$, $a_k < b_k$

a) $a_j < b_i$, $a_j < a_i$
$a_k < a_j < a_i$

b) $b_i < a_j$

$b_i < a_i$
$a_k < b_k$

$\min(a_k, b_i) < \min(a_i,b_k)$


Wiemy, że dla wszystkich zadań zachodzi ta relacja.

Algorytm: Posortować zadania względem relacji $\preceq$

Złożoność czasowa: $O(n\log n)$ porównań.



## Zadanie 2.8

:::info
Mamy trzy monety $1 < a < b$. Kiedy algorytm zachłanny jest optymalny?
:::

#### Oznaczenia
- Rozwiązaniem jest wektor trzyelementowy $(x,y,z)$, gdzie $x$ oznacza liczbę użytych monet $b$, $y$ liczbę użytych monet $a$, a $z$ liczbę użytych monet 1.
- $G(w)$ - rozwiązanie zachłanne dla kwoty $w$
- $O(w)$ - rozwiązanie optymalne dla kwoty $w$
- $R$ - rozwiązanie, $|R|$ - liczba monet używanych w rozwiązaniu $R$


#### Obserwacje:
- Algorytm zachłanny zwraca największe leksykograficznie rozwiązanie
- $O(w)$ - największe leksykograficznie optymalne rozwiązanie

$O(w) \neq G(w)$ -> algorytm zachłanny nie jest optymalny

Niech $w$ będzie minimalnym kontrprzykładem. Wtedy:
- $G(w)$ wykorzystuje więcej monet $b$ niż $O(w)$
- $O(w)$ wykorzystuje więcej monet $a$ niż $G(w)$
- $O(w)$ i $G(w)$ wykorzystują różne monety
- $G(w)$ wykorzystuje monetę $1$ (czyli w konsekwencji $O(w)$)
- $G(w)$ wykorzystuje monetę $b$ i $1$, a $O(w)$ wykorzystuje monetę $a$.
- $w$ jest wielokrotnością $a$

#### Lemat 1
Jeśli $w-a \geq b$, to $w$ nie jest minimalnym kontrprzykładem.

#### Lemat 2 
Jeśli dla dowolnego $w$ $O(w) nie wykorzystuje monety $b$, to dla każdej mniejszej kwoty $w' < w$ $O(w')$ też nie wykorzystuje monety $b.

#### Dowód Lematu 2
Załóżmy nie wprost, że dla pewnego $w' < w$ $O(w')$ wykorzystuje monetę $b$.

$R$ - najlepsze rozwiązanie dla $w'$, które jest zdominowane przez $O(w)$. W szczególności, $R$ nie wykorzystuje monety $b$.

$T = O(w) - R$ - jest poprawnym rozwiązaniem dla $w - w'$

$|O(w)| = |R| + |T|$

$Z = O(w') + T$

Wiemy, że $Z$ jest większe leksykograficznie niż $O(w)$.

$|Z| = |O(w')| + |T| \leq |R| + |T| = |O(w)|$

$O(w)$ nie jest największym leksykograficznie rozwiązaniem optymalnym - sprzeczność.

#### Dowód Lematu 1
Załózmy nie wprost, że $w$ jest minimalnym kontrprzykładem i $w-a \geq b$.

$O(w)$ nie wykorzystuje monety $b$.
$G(w-a)$ korzysta z monety $b$.
$O(w-a)$ nie korzysta z monety $b$.
$O(w-a) \neq G(w-a)$

Czyli $w-a$ jest mniejszym kontrprzykładem.


Jeśli istnieje minimalny kontrprzykład $w$, to:
- $w$ jest wielokrotnością $a$
- $w \geq b$
- $w - a < b$

Czyli minimalnym kontrprzykładem jest najmniejsza wielokrotność $a$ niemniejsza od $b$ --- $w = \lceil \frac{b}{a} \rceil a$


Algorytm: sprawdzić czy $w = \lceil \frac{b}{a} \rceil a$ jest kontrprzykładem
Wystarczy porównać rozwiązanie: $G(w) = (1,0, w-b)$ oraz $(0, w/a, 0)$