import matplotlib.image as mpimg
import matplotlib.pyplot as plt
import numpy as np

img = mpimg.imread('dogs.png')

print(img)

#img[:,:,0] = 0
red = np.where(img[:,:,0] > 0.7, 1., img[:,:,0])
img[:,:,0]

plt.imshow(img)
plt.show()


img = np.zeros((10,10,3))
for i in range(img.shape[0]):
    for j in range(img.shape[1]):
        img[i, j] = np.random.random(3)
        print(type(img[i,j][0]))

imgplot = plt.imshow(img)

plt.show()
mpimg.imsave('obraz.png',img)