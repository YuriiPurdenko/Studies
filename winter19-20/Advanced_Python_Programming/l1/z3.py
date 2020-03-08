def romb(n):
    sp = ' '
    krz = '#'
    wyp = 1
    for i in range(1, n):
        wyp = (n-i)*sp
        wyp += '#'
        wyp += (2*(i-1))*krz
        print(wyp)
    wyp = (2*n-1)*krz
    print(wyp)
    for i in range(n-1, 0, -1):
        wyp = (n-i)*sp
        wyp += '#'
        wyp += (2*(i-1))*krz
        print(wyp)


for i in range(1, 5):
    romb(i)
