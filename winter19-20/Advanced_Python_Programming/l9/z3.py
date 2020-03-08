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


def checking(img, ba, orginial):
    ok = 0
    for i in range(img.shape[0]):
        for j in range(img.shape[1]):
            if ok > len(ba):
                break
            if ok:
                if img[i, j, 0] != original[i, j, 0]:
                    if ba[ok] == 0:
                        print("ZLE")
                else:
                    if ba[ok]:
                        print("ZLE")
            if img[i, j, 0] != original[i, j, 0]:
                ok = 1


def codeintopicture(img, original, ba):
    start = random.randint(0, img.shape[0]*img.shape[0] - len(ba) - 1)
    i = int(start / img.shape[1])
    j = int(start % img.shape[1])

    if img[i, j][0] == 1:
        img[i, j][0] = np.nextafter(img[i, j][0], 0)
    else:
        img[i, j][0] = np.nextafter(img[i, j][0], 1)
    j += 1

    for i in range(len(ba)):
        if j >= img.shape[1]:
            j = 0
            i += 1
        if ba[i]:
            if img[i, j][0] == 1:
                img[i, j][0] = np.nextafter(img[i, j][0], 0)
            else:
                img[i, j][0] = np.nextafter(img[i, j][0], 1)
        j += 1

    if j >= img.shape[1]:
        j = 0
        i += 1
    if img[i, j][0] == 1:
        img[i, j][0] = np.nextafter(img[i, j][0], 0)
    else:
        img[i, j][0] = np.nextafter(img[i, j][0], 1)


text = "tekst do zakodowania"
picture = "dogs.png"
safe = "picture.png"
img = mpimg.imread(picture)
original = mpimg.imread(picture)
ba = ''.join(format(ord(i), 'b') for i in text)

codeintopicture(img, original, ba)

plotimages(img, original)

mpimg.imsave(safe, img)

checking(img, ba, original)
