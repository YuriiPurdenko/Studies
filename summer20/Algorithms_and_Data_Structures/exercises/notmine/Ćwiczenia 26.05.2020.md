# Ćwiczenia 26.05.2020

## Zadanie 6.1
::: info
Podaj nierekurencyjną implementację Quicksort
:::

#### Implementacja nierekurencyjna, ale z dodatkową pamięcią

```
T - tablica do posortowania
S - stos
S.insert((1,n))
While S nie jest pusty:
    l, r = S.pop
    If r - l jest duże:
        k = ustaw_piwot(T, l, r)
        // k - indeks piwota
        S.push((k+1, r))
        S.push((l+1, k-1))
    Else:
        Posortuj(T, l, r)
```

#### Implementacja nierekurencyjna w miejscu
```
T - tablica do posortowania
T[n+1] = infinity
l = 1
r = n
Powtarzaj:
    if r - l jest małe:
        Posortuj(T[l...r])
        l = r + 2
        r = r + 3
        while T[r] < T[l] and r <= n:
            r += 1
        swap(T[l], T[r])
        r -= 1
    else:
        k = ustaw_piwot(T, l, r)
        // k - indeks piwota
        swap(T[k+1], T[r+1])
        r = k - 1
```


## Zadanie 6.5
::: info
Oszacuj oczekiwany czas działania Algorytmu Hoare'a.
:::
:::warning
Jako zadanie domowe.
:::

## Zadanie 6.6
::: info
Zaimplementuj złączalną kolejkę priorytetową, wykorzystując następujący warunek:
$$ h(lewy\ syn) \geq h(prawy\ syn) $$,
gdzie $h(v)$ oznacza odległość do najbliższego pustego wskaźnika.
:::

::: warning
Będziemy znajdować minimum.
:::

#### Lemat 
W drzewie binarnym z warunkiem jak w zadaniu, liść najbliżej korzenia ma głębokość co najwyżej $O(\log n)$

Operacje:
- insert
- min
- delete_min
- join

```
Procedure insert(T1, v):
    Stwórz drzewo T2 zawierające tylko v
    join(T1, T2)
```
```
Procedure delete_min(T1):
    join(T1.left, T1.right)
```

W wierzchołku przechowujemy:
- value
- left, right - wskaźniki na lewe i prawe poddrzewo
- h - wartość funkcji z zadania

```
Procedure join(T1, T2):
    If T1.value < T2.value:
        If T1.left = NULL
            T1.left = T2
        Else If T1.right = NULL
            T1.right = T2
        Else
            T1.right = join(T1.right, T2)
        If T1.left.h < T1.right.h:
            swap(T1.left, T1.right)
            T1.h = T1.right.h + 1
        return T1
    Else:
        Podobnie
```

Złożoność czasowa join: $log(|T1|) + log(|T2|)$

## Zadanie 6.7
:::info
Zmodyfikuj drzewa AVL, tak, żeby operacje $następnik(v)$ i $poprzednik(v)$ wykonywały się w czasie stałym.
:::

W każdym węźle zapamiętujemy poprzednika i następnika.

Operacja delete(v):
    v.prev.next = v.next
    v.next.prev = v.prev

Operacja insert(v):
    Najpierw trzeba znaleźć następnika lub poprzednika v -> w (załóżmy, że jest to poprzednik)
    w.next.prev = v
    v.next = w.next
    w.next = v
    v.prev = w

## Zadanie 6.8
:::info
Napisz procedurę Split(T,k) rozdzielającą drzewo AVL na dwa drzewa o kluczach odpowiednio mniejszych i większych od k.
:::
:::warning
~~Jako zadanie domowe~~ Jednak nie.
:::

## Zadanie 6.9
:::info 
Czy w drzewach AVL potrzebne są dwa bity w każdym wierzchołku do pamiętania współczynnika zrównoważenia?
:::

#### Obserwacja
Jeśli któreś z dzieci wierzchołka jest NULLem, to nie musimy pamiętać współczynnika zrównoważenia.

Zamiast trzymać dwa bity u siebie, trzymamy po dwa bity u swoich dzieci.