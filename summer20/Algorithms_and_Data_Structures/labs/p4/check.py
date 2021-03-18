import sys
import math as m

def dist(p1, p2):
    a = p2[0] - p1[0]
    b = a**2
    c = p2[1] - p1[1]
    d = c**2
    e = b+d
    f = m.sqrt(e)
    return f


def per(p1, p2, p3):
    return dist(p1, p2)+dist(p2, p3)+dist(p1, p3)

eps = 0.0000000001

def check(x):
    tout = "out/"+str(x)+".out"
    mout = "mine/"+str(x)+".out"
    ft = open(tout,"r")
    twynik = int(ft.read())

    fm = open(mout, "r")
    mwynik = int(fm.read())

    print("Test",x,":")
    print("best:",twynik,"  yours:",mwynik)
    if abs(twynik-mwynik) == 0:
        print("OK\n")
    else:
        print("Check your answer. Press Enter to continue.")
        input()

i = int(sys.argv[1])
check(i)
