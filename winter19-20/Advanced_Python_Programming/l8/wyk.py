import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation


x = np.arange(15)
y = np.zeros((4,5,6))
z = np.array([[3.1415, 2.7182, 1.6180], [4.135, 1.660, 12.56]])

a = z + 2.5
b = z * 2.5

plt.plot(x,x)
plt.show()

x = np.random.randint(1,10, size=1000)
plt.hist(10)
plt.show()

fig = plt.figure()
ax = plt.axes(xlim = (-2,2), ylim = (-2,2))

xdata, ydata = [],[]
line, = ax.plot([],[])

def init():
    line.set_data([],[])
    return line

def animate(i):
    t = 0.01*i
    x = np.sin(t + np.pi/2.0)
    y = np.sin(t)
    xdata.append(x)
    ydata.append(y)
    line.setdata(xdata, ydata)
    return line,

ani = FuncAnimation(fig, animate, init_func=init,frames=500, interval=50, blit=True)
plt.show()