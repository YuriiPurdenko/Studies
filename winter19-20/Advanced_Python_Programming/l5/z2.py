import itertools

v1 = 'KIOTO'
v2 = 'OSAKA'
v3 = 'TOKIO'


def alphadir(permu):
    x = y = z = 0
    for i in v1:
        x += permu[ord(i) - ord('A')]
        x *= 10
    x /= 10
    for i in v2:
        y += permu[ord(i) - ord('A')]
        y *= 10
    y /= 10
    for i in v3:
        z += permu[ord(i) - ord('A')]
        z *= 10
    z /= 10
    return (x + y) == z


for i in itertools.product([1, 2, 3, 4, 5, 6, 7, 8, 9, 0], repeat=26):
    if alphadir(i):
        x = 0
        for j in i:
            print(chr(ord('A') + x), j, sep=' - ')
            x += 1
        break
