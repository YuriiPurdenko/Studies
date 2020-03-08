#Wiktor Pilarczyk 308533 10.11.19
setprecision(64)
using Plots

function Clen1(a,b,c,w,x)
    n = length(w)
    wyn = Pdy = BigFloat(0)
    Py = BigFloat(a[1])
    Px = (BigFloat(a[2]) * BigFloat(x) - BigFloat(b[2])) * BigFloat(Py)
    Pdx = BigFloat(a[2]) * BigFloat(Py)
    for k in 2:n-1
        P = (BigFloat(a[k+1]) * BigFloat(x) - BigFloat(b[k+1])) * BigFloat(Px) - BigFloat(c[k+1]) * BigFloat(Py)
        Pd = BigFloat(a[k+1]) * BigFloat(Px) + (BigFloat(a[k+1]) * BigFloat(x) - BigFloat(b[k+1])) * BigFloat(Pdx) - BigFloat(c[k+1]) * BigFloat(Pdy)
        wyn += BigFloat(w[k]) * BigFloat(Pdx)
        Py = BigFloat(Px)
        Pdy = BigFloat(Pdx)
        Px = BigFloat(P)
        Pdx = BigFloat(Pd)
    end
    return BigFloat(wyn) + BigFloat(w[n]) * BigFloat(Pdx)
end

function Clen2(a,b,c,w,x)
    n = length(w)
    Bdy = BigFloat(0)
    By = BigFloat(w[n])
    Bx = BigFloat(w[n-1]) + (BigFloat(a[n]) * BigFloat(x) - BigFloat(b[n])) * BigFloat(By)
    Bdx = BigFloat(a[n]) * BigFloat(By)
    for k in n-2:-1:1
        B = BigFloat(w[k]) + (BigFloat(a[k+1]) * BigFloat(x) + BigFloat(b[k+1])) * BigFloat(Bx) - BigFloat(c[k+2]) * BigFloat(By)
        Bd = BigFloat(a[k+1]) * BigFloat(Bx) + (BigFloat(a[k+1]) * BigFloat(x) + BigFloat(b[k+1])) * BigFloat(Bdx) - BigFloat(c[k+2]) * BigFloat(Bdy)
        By = BigFloat(Bx)
        Bdy = BigFloat(Bdx)
        Bx = BigFloat(B)
        Bdx = BigFloat(Bd)
    end
    return BigFloat(a[1])*BigFloat(Bdx)
end

function Czeb(k,x)
    if x == 1 || x == -1
        return NAN
    end
    if(x > 1)
        pierw = BigFloat(sqrt(BigFloat(x) * BigFloat(x) - 1))
        return (BigFloat(k) * BigFloat(sinh(BigFloat(k) * BigFloat(acosh(BigFloat(x)))))) / BigFloat(pierw)
    end
    if(x < -1)
        pierw = BigFloat(sqrt(BigFloat(x) * BigFloat(x) - 1))
        if k % 2 == 0
            pierw *= -1
        end
        return (BigFloat(k) * BigFloat(sinh(BigFloat(k) * BigFloat(acosh(BigFloat(-x)))))) / BigFloat(pierw)
    end
    pierw = BigFloat(1 - sqrt(BigFloat(x) * BigFloat(x)))
    return (BigFloat(k) * BigFloat(sin(BigFloat(k) * BigFloat(acos(BigFloat(x)))))) / BigFloat(pierw)
    
end

a = [1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2]
b = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
c = [0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1]
start = 2
iter = 100
finish = 100000
ox = []
yCzeb = []
yClen1 = []
yClen2 = []
function blad(k,w)
    licz = 0
    sum1 = 0
    sum2 = 0
    print("Wyniki dla ")
    print(k)
    println(". wielomianu Czebyszwa:")
    for i in start:iter:finish
        wynclen1 = Clen1(a,b,c,w,i)
        wynclen2 = Clen2(a,b,c,w,i)
        wynczeb = Czeb(k,i)
        push!(ox,i)
        push!(yCzeb,wynczeb)
        push!(yClen1,wynclen1)
        push!(yClen2,wynclen2)
        print("x: ")
        print(i)
        print(" Czeb: ")
        println(wynczeb)
        print("Clen1: ")
        print(wynclen1)
        print(" BladClen1: ")
        print(wynclen1 - wynczeb)
        if(wynczeb != 0)
            licz += 1
            sum1 += abs((wynclen1 - wynczeb)/wynczeb)
            print(" BladWzgl: ")
            print(abs((wynclen1 - wynczeb)/wynczeb))
        end
        println()
        print("Clen2: ")
        print(wynclen2)
        print(" BladClen2: ")
        print(wynclen2 - wynczeb)
        if(wynczeb != 0)
            sum2 += abs((wynclen2 - wynczeb)/wynczeb)
            print(" BladWzgl: ")
            print(abs((wynclen2 - wynczeb)/wynczeb))
        end
        println()
    end
    print("Sredni błąd względny dla Clen1 wynosił: ")
    println(sum1/licz)
    print("Sredni błąd względny dla Clen2 wynosił: ")
    print(sum2/licz)
end

w2 = [0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
blad(2,w2)

y = [yCzeb,yClen1, yClen2]
plot(ox,y,color = ["red" "green" "orange"],seriesalpha = 0.55, linewidth=3.5, title="Wielomian Czebyszewa st. 2", label=["Czeb" "Clen1" "Clen2"])

ox = []
yCzeb = []
yClen1 = []
yClen2 = []
w20 = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
blad(20,w20)

y = [yCzeb,yClen1, yClen2]
plot(ox,y,color = ["red" "green" "orange"],seriesalpha = 0.55, linewidth=3.5, title="Wielomian Czebyszewa st. 20", label=["Czeb" "Clen1" "Clen2"])

ox = []
yCzeb = []
yClen1 = []
yClen2 = []
w39 = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1]
blad(39,w39)

start = 0.95
iter = -0.001
finish = -0.95
y = [yCzeb,yClen1, yClen2]
plot(ox,y,color = ["red" "green" "orange"],seriesalpha = 0.55, linewidth=3.5, title="Wielomian Czebyszewa st. 39", label=["Czeb" "Clen1" "Clen2"])

ox = []
yCzeb = []
yClen1 = []
yClen2 = []
w2 = [0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
blad(2,w2)

y = [yCzeb,yClen1, yClen2]
plot(ox,y,color = ["red" "green" "orange"],seriesalpha = 0.55, linewidth=3.5, title="Wielomian Czebyszewa st. 2", label=["Czeb" "Clen1" "Clen2"])

ox = []
yCzeb = []
yClen1 = []
yClen2 = []
w20 = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
blad(20,w20)

y = [yCzeb,yClen1, yClen2]
plot(ox,y,color = ["red" "green" "orange"],seriesalpha = 0.55, linewidth=3.5, title="Wielomian Czebyszewa st. 20", label=["Czeb" "Clen1" "Clen2"])

ox = []
yCzeb = []
yClen1 = []
yClen2 = []
w39 = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1]
blad(39,w39)

start = -2
iter = -100
finish = -100000
y = [yCzeb,yClen1, yClen2]
plot(ox,y,color = ["red" "green" "orange"],seriesalpha = 0.55, linewidth=3.5, title="Wielomian Czebyszewa st. 39", label=["Czeb" "Clen1" "Clen2"])

ox = []
yCzeb = []
yClen1 = []
yClen2 = []
w2 = [0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
blad(2,w2)

y = [yCzeb,yClen1, yClen2]
plot(ox,y,color = ["red" "green" "orange"],seriesalpha = 0.55, linewidth=3.5, title="Wielomian Czebyszewa st. 2", label=["Czeb" "Clen1" "Clen2"])

ox = []
yCzeb = []
yClen1 = []
yClen2 = []
w20 = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
blad(20,w20)

y = [yCzeb,yClen1, yClen2]
plot(ox,y,color = ["red" "green" "orange"],seriesalpha = 0.55, linewidth=3.5, title="Wielomian Czebyszewa st. 20", label=["Czeb" "Clen1" "Clen2"])

ox = []
yCzeb = []
yClen1 = []
yClen2 = []
w39 = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1]
blad(39,w39)

y = [yCzeb,yClen1, yClen2]
plot(ox,y,color = ["red" "green" "orange"],seriesalpha = 0.55, linewidth=3.5, title="Wielomian Czebyszewa st. 39", label=["Czeb" "Clen1" "Clen2"])

function wiel(x)
    return ((BigFloat(x) * BigFloat(5) + BigFloat(16)) * BigFloat(x) - BigFloat(3)) * BigFloat(x) * BigFloat(x)
end

ww = [0,1/8,1,1/16,1/2,1/16]
start = -100050
iter = 100
finish = 100050
ox = []
yWiel = []
yClen1 = []
yClen2 = []
function blad2(w)
    licz = 0
    sum1 = 0
    sum2 = 0
    print("Wyniki dla pochodnej wielomianu:")
    for i in start:iter:finish
        wynclen1 = Clen1(a,b,c,w,i)
        wynclen2 = Clen2(a,b,c,w,i)
        wyn = wiel(i)
        push!(ox,i)
        push!(yWiel,wyn)
        push!(yClen1,wynclen1)
        push!(yClen2,wynclen2)
        print("x: ")
        print(i)
        print(" wiel:")
        println(wyn)
        print("Clen1: ")
        print(wynclen1)
        print(" BladClen1: ")
        print(wynclen1 - wyn)
        if(wyn != 0)
            licz += 1
            sum1 += abs((wynclen1 - wyn)/wyn)
            print(" BladWzgl: ")
            print(abs((wynclen1 - wyn)/wyn))
        end
        println()
        print("Clen2: ")
        print(wynclen2)
        print(" BladClen2: ")
        print(wynclen2 - wyn)
        if(wyn != 0)
            sum2 += abs((wynclen2 - wyn)/wyn)
            print(" BladWzgl: ")
            print(abs((wynclen2 - wyn)/wyn))
        end
        println()
    end
    print("Sredni błąd względny dla Clen1 wynosił: ")
    println(sum1/licz)
    print("Sredni błąd względny dla Clen2 wynosił: ")
    print(sum2/licz)
end

blad2(ww)

y = [yWiel,yClen1, yClen2]
plot(ox,y,color = ["red" "green" "orange"],seriesalpha = 0.55, linewidth=3.5, title="Wielomian", label=["Czeb" "Clen1" "Clen2"])

start = -1
iter = 0.001
finish = 1
ox = []
yWiel = []
yClen1 = []
yClen2 = []
blad2(ww)

y = [yWiel,yClen1, yClen2]
plot(ox,y,color = ["red" "green" "orange"],seriesalpha = 0.55, linewidth=3.5, title="Wielomian", label=["Czeb" "Clen1" "Clen2"])
