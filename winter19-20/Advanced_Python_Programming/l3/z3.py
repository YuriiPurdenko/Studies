def pierwiastek(n):
    y = 0
    i = 0
    while (y <= n):
        i += 1
        y += (2*i-1)
    return i-1


for i in range(1, 10):
    print(pierwiastek(i))
