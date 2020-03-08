import timeit


def sito(n):
    lista = list([0 for x in range(n+1)])
    for i in range(2, n):
        if i * i > n:
            break
        if lista[i] == 0:
            x = i * i
            while x <= n:
                lista[x] = 1
                x += i
    return lista


def prime(n):
    for i in range(2, n):
        if n % i == 0:
            return False
    return True


def pierwsze_yield():
    lista = []
    a = 2
    ok = True
    while True:
        yield lista
        for i in lista:
            if a % i == 0:
                ok = False
                break
        if ok:
            lista.append(a)
        a += 1
        ok = True


odp = pierwsze_yield()

for i in range(5):
    print(odp)


def pierwsze_imperatywna(n):
    odp = []
    for i in range(2, n + 1):
        if prime(i):
            odp.append(i)
    return odp


x = "from __main__ import pierwsze_"
print(timeit.timeit("pierwsze_imperatywna(20)", setup=x + "imperatywna"))
print(pierwsze_imperatywna(20))


def pierwsze_skladana(n):
    return [i for i in range(2, n + 1) if not
            any(i % j == 0 for j in range(2, i))]


print(timeit.timeit("pierwsze_skladana(20)", setup=x + "skladana"))
print(pierwsze_skladana(20))


def pierwsze_funkcyjna(n):
    def recursiv(m, i):
        if i == m:
            return True
        if m % i == 0:
            return False
        return recursiv(m, i+1)

    def rec(m):
        return recursiv(m, 2)

    return list(filter(rec, range(2, n + 1)))


print(timeit.timeit("pierwsze_funkcyjna(20)", setup=x + "funkcyjna"))
print(pierwsze_funkcyjna(20))
