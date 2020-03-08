# mozliwe ze nextafter nie dziala :(
import matplotlib.image as mpimg
import matplotlib.pyplot as plt
import numpy as np
import random


def plotimages(img, original):
    f = plt.figure()
    f.add_subplot(1, 2, 1)  # nrows, nclos, index
    plt.imshow(img)
    f.add_subplot(1, 2, 2)
    plt.imshow(original)
    plt.show(block=True)


def codeintopicture(img, original, ba):

    if img.shape[0]*img.shape[0] < len(ba):
        print("ERR: YOUR IMAGE IS TO SMALL")
        return

    start = random.randint(0, img.shape[0]*img.shape[0] - len(ba) - 1)
    i = int(start / img.shape[1])
    j = int(start % img.shape[1])
    color = random.randint(0, 2)

    if img[i, j][color] == 1:
        img[i, j][color] = np.nextafter(img[i, j][color], 0)
    else:
        img[i, j][color] = np.nextafter(img[i, j][color], 1)
    j += 1

    for x in range(len(ba)):
        color = random.randint(0, 2)
        if j >= img.shape[1]:
            j = 0
            i += 1
        if ba[x]:
            if img[i, j][color] == 1:
                img[i, j][color] = np.nextafter(img[i, j][color], 0)
            else:
                img[i, j][color] = np.nextafter(img[i, j][color], 1)
        j += 1

    color = random.randint(0, 2)
    if j >= img.shape[1]:
        j = 0
        i += 1
    if img[i, j][color] == 1:
        img[i, j][color] = np.nextafter(img[i, j][color], 0)
    else:
        img[i, j][color] = np.nextafter(img[i, j][color], 1)


text = "tekst do zakodowania"
picture = "dogs.png"
safe = "picture.png"
img = mpimg.imread(picture)
original = mpimg.imread(picture)
ba = ''.join(format(ord(i), 'b') for i in text)

codeintopicture(img, original, ba)

plotimages(img, original)

mpimg.imsave(safe, img)
