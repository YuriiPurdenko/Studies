import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

x = 500
y = 500
direction = 0
matr = np.matrix(np.zeros((x, y)))
arr = np.array([int(x/2), int(y/2), 0])
# polozenie (x,y) i kierunek ruchu 0 - E, 1 - N, 2 - W, 3 - S

def nextstep(matr, arr):
    if arr[2] == 0:
        arr[0] += 1
    if arr[2] == 1:
        arr[1] += 1
    if arr[2] == 2:
        arr[0] -= 1
    if arr[2] == 3:
        arr[1] -= 1
    if matr[arr[0], arr[1]] == 0:
        arr[2] = (arr[2] + 1) % 4
        matr[arr[0], arr[1]] = 1
    else:
        arr[2] = (arr[2] + 3) % 4
        matr[arr[0], arr[1]] = 0


fig = plt.figure()
im = plt.imshow(matr, vmax=1)


def animate(i):
    nextstep(matr, arr)
    im.set_data(matr)
    return im,


anim = FuncAnimation(fig, animate, frames=2000000, interval=0, blit=True)
plt.show()
