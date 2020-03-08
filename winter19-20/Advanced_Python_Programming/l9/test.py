import matplotlib.image as mpimg
import matplotlib.pyplot as plt
import numpy as np
import random

print(random.randint(0,3)) 
print(random.randint(0,3))
print(random.randint(0,3))
print(random.randint(0,3))
print(random.randint(0,3))



def checking(img, ba, orginial):
    ok = 0
    for i in range(img.shape[0]):
        for j in range(img.shape[1]):
            if ok > len(ba):
                print("HALO")
                if img[i, j, 0] != original[i, j, 0] or img[i, j, 1] != original[i, j, 1] or img[i, j, 2] != original[i, j, 2]:
                    print("ZLE")
                continue
            if ok:
                print("HALO")
                if img[i, j, 0] != original[i, j, 0] or img[i, j, 1] != original[i, j, 1] or img[i, j, 2] != original[i, j, 2]:
                    if ba[ok - 1] == 0:
                        print("ZLE")
                else:
                    if ba[ok - 1]:
                        print("ZLE")
                ok += 1
            if img[i, j, 0] != original[i, j, 0] or img[i, j, 1] != original[i, j, 1] or img[i, j, 2] != original[i, j, 2]:
                print("HALO")
                ok = 1