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


def pierwsze_imperatywna(n):
    odp = []
    for i in range(2, n + 1):
        if prime(i):
            odp.append(i)
    return odp


def pierwsze_skladana(n):
    return [i for i in range(2, n + 1) if not
            any(i % j == 0 for j in range(2, i))]


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


def pierwsze_yield():
    lista = []
    a = 2
    while True:
        yield a
        lista.append(a)
        a += 1
        ok = False
        while not(ok):
            ok = True
            for i in lista:
                if a % i == 0:
                    ok = False
                    break
            if not(ok):
                a += 1


def pierwsze_iterator(n):
    prim = pierwsze_yield()
    odp = []
    x = next(prim)
    while x <= n:
        odp.append(x)
        x = next(prim)
    return odp


print(pierwsze_iterator(200))

x = "from __main__ import pierwsze_"
v = "pierwsze_"
imp10 = round(timeit.timeit(v + "imperatywna(10)", setup=x + "imperatywna", number = 100), 4)
imp100 = round(timeit.timeit(v + "imperatywna(100)", setup=x + "imperatywna", number = 100), 4)
imp500 = round(timeit.timeit(v + "imperatywna(500)", setup=x + "imperatywna", number = 100), 4)
fun10 = round(timeit.timeit(v + "funkcyjna(10)", setup=x + "funkcyjna", number = 100), 4)
fun100 = round(timeit.timeit(v + "funkcyjna(100)", setup=x + "funkcyjna", number = 100), 4)
fun500 = round(timeit.timeit(v + "funkcyjna(500)", setup=x + "funkcyjna", number = 100), 4)
skl10 = round(timeit.timeit(v + "skladana(10)", setup=x + "skladana", number = 100), 4)
skl100 = round(timeit.timeit(v + "skladana(100)", setup=x + "skladana", number = 100), 4)
skl500 = round(timeit.timeit(v + "skladana(500)", setup=x + "skladana", number = 100), 4)
ite10 = round(timeit.timeit(v + "iterator(10)", setup=x + "iterator", number = 100), 4)
ite100 = round(timeit.timeit(v + "iterator(100)", setup=x + "iterator", number = 100), 4)
ite500 = round(timeit.timeit(v + "iterator(500)", setup=x + "iterator", number = 100), 4)

y = " funkcyjna | skladana  | iterator"
print("    | imperatywna |" + y)
print(" 10 |   ", imp10, "  |  ", fun10, " |  ", skl10, " |  ", ite10, sep=" ")
print("100 |   ", imp100, "  |  ", fun100, " |  ", skl100, " |  ", ite100, sep=" ")
print("500 |   ", imp500, "  |  ", fun500, " |  ", skl500, " |  ", ite500, sep=" ")
