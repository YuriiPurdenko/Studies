import timeit
import functools


def doskonale(n):
    odp = 0
    x = int(n/2) + 1
    for i in range(1, x):
        if n % i == 0:
            odp += i
        if odp > n:
            return False
    return odp == n


listadosk = [i for i in range(2, 10000) if doskonale(i)]


def doskonala(n):
    if n in listadosk:
        return True
    return False


def doskonale_imperatywna(n):
    odp = []
    for i in range(2, n + 1):
        if doskonale(i):
            odp.append(i)
    return odp


mysetup = "from __main__ import doskonale_"
dosk = "doskonale_"
print(timeit.timeit(dosk + "imperatywna(500)", setup=mysetup + "imperatywna"))
print(doskonale_imperatywna(500))


def doskonale_skladana(n):
    return [i for i in range(2, n + 1) if i == sum(
            [j for j in range(1, i-1) if i % j == 0])]


print(timeit.timeit("doskonale_skladana(500)", setup=mysetup + "skladana"))
print(doskonale_skladana(500))


def doskonale_funkcyjna(n):
    def recursiv(n, i, acc):
        if i == n:
            return functools.reduce(lambda a, b: a + b, acc)
        if n % i == 0:
            return recursiv(n, i+1, acc+[i])
        return recursiv(n, i+1, acc)

    def rec(n):
        if recursiv(n, 1, []) == n:
            return True
        return False

    return list(filter(rec, range(2, n + 1)))


print(timeit.timeit("doskonale_funkcyjna(500)", setup=mysetup + "funkcyjna"))
print(doskonale_funkcyjna(500))
