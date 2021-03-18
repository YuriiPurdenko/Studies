# Ćwiczenia 31.03.2020

## Zadanie 1.6

Dane: $a_1, a_2, \dots, a_n$
Możemy usunąć $(a_i, a_j)$ jeśli $2 a_i \leq a_j$.

Rozważmy optimum:
$L$ - zbiór mniejszych elementów, które zostały usunięte
$R$ - zbiór większych elementów, które zostały usunięte

#### Lemat 1
Można założyć bez utraty ogólności, że w optymalnym rozwiązaniu
$L = \{a_1, a_2, \dots, a_k\}$
$R = \{a_{n-k+1}, a_{n-k+2}, \dots, a_n\}$
Dodatkowo $a_1$ jest w parze z $a_{n-k+1}$, $a_2$ z $a_{n-k+2}$ itd.

#### Dowód Lematu 1
Weźmy dowolne optymalne rozwiązanie $S = \{(a_i, a_j), \dots\}$

Krok 1. $\max(L) \leq \min(R)$
Załóżmy, że $a_i \in L$, $a_j \in R$ i $a_i > a_j$

Niech $a_k$ to jest element usuwany z $a_i$, $a_m$ niech będzie elementem usuwanym z $a_j$.

$(a_i, a_k)$ jest poprawną parą ($a_k \geq 2 a_i$), więc $(a_j, a_k)$ też jest.

W takim razie po zamianie $a_i$ i $a_j$, $a_i \in R$, a $a_j \in L$.

$(a_m, a_j)$ jest poprawną parą - $2 a_m \leq a_j \leq a_i$, w takim razie $(a_m, a_i)$ jest poprawną parą.

Możemy tak kontynuować, dopóki nie osiągniemy $\max(L) \leq \min(R)$.

Krok 2. $L = \{a_1, \dots a_k\}$
Załóżmy, że tak nie jest - $a_i \not\in L$  i $a_j \in L$ oraz $a_i < a_j$. 

Niech $a_k$ będzie elementem usuwanym z $a_j$ - w takim razie możemy zamienić $a_i$ z $a_j$

Powtarzamy, aż $L = \{a_1, \dots a_k\}$

Krok 3. $R = \{a_{n-k+1}, a_{n-k+2}, \dots, a_n\}$

Krok 4. $a_1$ jest połączony z $a_{n-k+1}$ itd. aż oń z w a on y

$a_1$ jest połączony z $a_{n-k+i}$.
Niech $j$ będzie takie, że $a_j$ jest połączony z $a_{n-k+1}$.

Połączmy $a_j$ z $a_{n-k+i}$ oraz $a_1$ z $a_{n-k+1}$.
$a_1 \leq a_j$
$2 a_j \leq a_{n-k+1}$
W takim razie $2 a_1 \leq a_{n-k+1}$.

$2 a_j \leq a_{n-k+1} \leq a_{n-k+i}$

Tak powtarzamy dla $a_2, a_3, \dots$.

#### Algorytm 1 
1. Dla $k$ od $n/2$ do 1:
    2. Jeśli $2 a_1 \leq a_{n-k+1}$, $2 a_2 \leq a_{n-k+2}$ itd. to zwróć k.

Złożoność czasowa: $n^2$

#### Algorytm 2
1. $S = \emptyset$
1. $i = 1$
2. $j = \frac{n}{2} + 1$
3. Dopóki $j \leq n$:
    4. Jeśli $2 a_i \leq a_j$ {
        5. Dodaj $(a_i, a_j)$ do $S$
        6. $i += 1$
    }
    7. $j = j+1$

Złożoność czasowa: $O(n)$
Złożoność pamięciowa: $O(n)$

Dowód poprawności:
Załóżmy, że optimum ($O$) usunie $k$ par.

Wystarczy, że pokażę, że mój algorytm usuwa $a_k$.

#### Lemat 2
Jeśli $a_i$ jest usunięte w optimum razem z $a_{n-k+i}$, to w moim algorytmie $a_i$ też jest usunięte. Co więcej, niech $a_j$ będzie elementem w parze z $a_i$. $a_j \leq a_{n-k+i}$.

#### Dowód
Dowód indukcyjny po i.

1. Baza. i = 1
$a_1$ zostało połączone z czymś mniejszym niż $a_{n-k+1}$, to koniec.
W przeciwnym razie algorytm połączy $a_1$ z $a_{n-k+1}$ czyli też dobrze.
2. Krok indukcyjny
Podobnie do bazy

## Zadanie 2.2

#### Algorytm

1. S - zbiór wszystkich odcinków
2. $T = \emptyset$
3. Dopóki $S \neq \emptyset$:
    4. Weź $I_j = (p_j, k_j)$, taki że $k_j$ jest najmniejszy wśród wszystkich odcinków w $S$
    5. Dodaj $I_j$ do $T$
    6. Usuwamy z $S$ wszystkie odcinki przecinające się z $I_j$
7. Zwróć $T$

Poprawność rozwiązania.

$S = \{I_1, I_2, \dots, I_k\}$

Rozpatrzmy optimum $O = \{J_1, J_2, \dots, J_m\}$.

#### Lemat
Dla każdego $i \leq m$, $I_i$ istnieje i kończy się nie później niż $J_i$.

#### Dowód indukcyjny po $i$

1. Baza i = 1 - jako pierwszy odcinek bierzemy ten, który się kończy najszybciej
2. Krok indukcyjny - zakładam, że założenie indukcyjne jest prawdziwe dla każdego $k \leq i$.
Po $i$ krokach $J_{i+1}$ znajduje się w $S$ (z założenia indukcyjnego).
W takim razie wybierzemy odcinek nie gorszy niż $J_{i+1}$.


Złożoność czasowa

Szczegóły implementacji
$S$ - posortowane po pierwszej współrzędnej
Kopiec odcinków posortowany po drugiej współrzędnej

Inicjalizacja $O(n \log n)$

Krok 4. czas stały - wyciągamy z kopca
Krok 6. $O(\log n)$ dla każdego usuwanego elementu
Krok 6. wykona się n razy

$O(n) + O(n \log n) = O(n \log n)$

-----------------

#### Ładniejsze rozwiązanie

$S$ posortowane po drugiej współrzędnej.
k = najdalszy koniec naszych odcinków

1. S - zbiór wszystkich odcinków
2. $T = \emptyset$
3. i = 1
4. $k = -\infty$ 
5. Dopóki $i < n$:
    6. Jeśli S(i) zaczyna się za po $k$ {
        7. Dodaj S(i) do T
        8. k = koniec S(i)
    }
7. Zwróć $T$


## Zadanie 2.3

Czy jest to rozwiązanie optymalne? Nie, $\frac{5}{121}$

$a_i, b_i$ - nasz ułamek po $i$ krokach algorytmu
($a_0, b_0 = a,b$).

Algorytm:
1. Dopóki $a > 0$:
    2. Weź najmniejsze $k$, takie że $1/k \leq \frac{a}{b}$
    3. $\frac{a}{b} := \frac{a}{b} - \frac{1}{k}$

#### Lemat
$a_{i+1} < a_i$

#### Dowód
Wiemy,że $\frac{1}{k-1} > \frac{a_i}{b_i}$.
$b_i > a_i (k-1)$
$b_i > a_i k - a_i$
$a_i > a_i k - b_i$

$\frac{a_i}{b_i} - \frac{1}{k} = \frac{a_i \dot k - b_i}{b_i k}$

$a_{i+1} = a_i \dot k - b_i < a_i$