setprecision(64)
import Pkg; Pkg.add("Plots")
import Pkg; Pkg.add("Polynomials")
import Pkg; Pkg.add("PyPlot")
using Plots
using Polynomials

function initarray(tab, val, k)
    for i in 1:1:k
        append!(tab,BigFloat(val))
    end
end

function nextbase(x, base1, base2, base3, a, b, k)
    c = BigFloat(0.0)
    d = BigFloat(0.0)
    e = BigFloat(0.0)
    
    for i in 1:1:k #wyliczanie wspolczynnikow c i d
        var = BigFloat(base2[i]) * BigFloat(base2[i])
        c += BigFloat(x[i]) * BigFloat(var)
        d += BigFloat(var)
        e += BigFloat(base1[i]) * BigFloat(base1[i])
    end
    #print("\nc:",c,"\nd:",d,"\n")
    c /= BigFloat(d)
    d /= BigFloat(e)
    append!(a, BigFloat(c))
    append!(b, BigFloat(d))
    #a[lvl] = c
    #b[lvl] = d
    
    for i in 1:1:k #obliczanie wartosci wielomianu z bazy w punktach xk
        base3[i] = (BigFloat(x[i]) - BigFloat(c)) * BigFloat(base2[i]) - BigFloat(d) * BigFloat(base1[i])
    end
    
end

function nextpolynomial(x, wiel, base, fun, alfa, k)
    a = BigFloat(0.0)
    b = BigFloat(0.0)
    c = BigFloat(0.0)
    for i in 1:1:k #wyliczanie wspolczynika alfa
        a += BigFloat(base[i]) * BigFloat(fun[i]) 
        b += BigFloat(base[i]) * BigFloat(base[i])
        #print("\n: ",tabb[i],"  ", fun[i])
    end
    # alfa[lvl] = a/b
    #print("\na: ",a)
    #print("\nb: ",b)
    append!(alfa, BigFloat(a/b))
    
    for i in 1:1:k #wyliczanie wartosci wielomianu optymalnego stopnia lvl w punktach xk
        wiel[i] += BigFloat(a/b) * BigFloat(base[i])
    end
end

function Clenshaw(alfa, a, b,res)
    n = length(alfa)
    vk = []
    vk1 = []
    vk2 = []
    append!(b,0) #gdyz w petli odowalmy sie do elemntu b_n+1
    initarray(vk, BigFloat(0), n)
    initarray(vk1, BigFloat(0), n)
    initarray(vk2, BigFloat(0), n)
    vk1[1] = BigFloat(alfa[n])
    for i in n - 1:-1:1
        vk[1] += BigFloat(alfa[i])
        for j in 1:1:n
            vk[j] -= BigFloat(a[i]) * BigFloat(vk1[j])
            vk[j] -= BigFloat(b[i+1]) * BigFloat(vk2[j])
            if j != n
                vk[j+1] += BigFloat(vk1[j])
            end
        end
        for j in 1:1:n
            vk2[j] = BigFloat(vk1[j])
            vk1[j] = BigFloat(vk[j])
            vk[j] = BigFloat(0.0)
        end
    end
    initarray(res, BigFloat(0), n)
    for j in 1:1:n
        res[j] = BigFloat(vk1[j])
    end
end

function optimalscalar(w, fun, k)
    sum = BigFloat(0.0)
    for i in 1:1:k
        #print((BigFloat(w[i]) - BigFloat(fun[i])) * (BigFloat(w[i]) - BigFloat(fun[i])), "a\n")
        sum += (BigFloat(w[i]) - BigFloat(fun[i])) * (BigFloat(w[i]) - BigFloat(fun[i]))
        #print(sum, "\n")
    end
    #print(sum)
    return BigFloat(sum)
end

function optimalenough(w, fun, delta, k)
    sum = optimalscalar(w, fun, k)
    if sum >= delta
        return false
    end
    return true
end 

function findoptimal(x, fun, delta, res)
    k = length(x)
    base1 = [] #obliczenie dwóch pierwszych elementów bazy
    base2 = []
    base3 = []
    a = []
    b = [BigFloat(0.0)]
    alfa = []
    w = []
    initarray(base1, BigFloat(1.0), k)
    initarray(base2, BigFloat(0.0), k)
    initarray(base3, BigFloat(0.0), k)
    var1 = BigFloat(0.0)
    
    for i in 1:1:k #wyliczanie a1
        var1 += BigFloat(x[i])
    end
    append!(a,BigFloat(var1/k))
    
    var1 = BigFloat(0.0)
    for i in 1:1:k
        var1 += BigFloat(fun[i])
    end
    append!(alfa, BigFloat(var1/k))
    initarray(w, BigFloat(alfa[1]), k)
    if optimalenough(w, fun ,delta, k)
        Clenshaw(alfa, a, b, res)
        return
    end
    
    for i in 1:1:k #wyliczanie drugie elementu bazy
        base2[i] = x[i] - a[1]
    end
    
    nextpolynomial(x, w, base2, fun, alfa, k)
    #print("\base2: ",base2)
    #print("\nalfa[1]: ",alfa[1])
    #print("\nalfa[2]: ",alfa[2])
    #print("\na[1]: ",a[1])
    while !optimalenough(w, fun ,delta, k)
        nextbase(x, base1, base2, base3, a, b, k)
        for i in 1:1:k
            base1[i] = BigFloat(base2[i])
            base2[i] = BigFloat(base3[i])
        end
        nextpolynomial(x, w, base2, fun, alfa, k)
    end
    #print(alfa,"\n", a,"\n", b,"\n")
    Clenshaw(alfa, a, b, res)
end

function findnthoptimal(x, fun, n, res) # funkcja znajdujaca n - ty wiel optymalny
    k = length(x)
    base1 = [] #obliczenie dwóch pierwszych elementów bazy
    base2 = []
    base3 = []
    a = []
    b = [BigFloat(0.0)]
    alfa = []
    w = []
    initarray(base1, BigFloat(1.0), k)
    initarray(base2, BigFloat(0.0), k)
    initarray(base3, BigFloat(0.0), k)
    var1 = BigFloat(0.0)
    
    for i in 1:1:k #wyliczanie a1
        var1 += BigFloat(x[i])
    end
    append!(a,BigFloat(var1/k))
    
    var1 = BigFloat(0.0)
    for i in 1:1:k
        var1 += BigFloat(fun[i])
    end
    append!(alfa, BigFloat(var1/k))
    initarray(w, BigFloat(alfa[1]), k)
    if optimalenough(w, fun, 4.9406564584124654e-320, k) || n == 0
        Clenshaw(alfa, a, b, res)
        return
    end
    n -= 1
    
    for i in 1:1:k #wyliczanie drugie elementu bazy
        base2[i] = x[i] - a[1]
    end
    
    nextpolynomial(x, w, base2, fun, alfa, k)
    #print("\base2: ",base2)
    #print("\nalfa[1]: ",alfa[1])
    #print("\nalfa[2]: ",alfa[2])
    #print("\na[1]: ",a[1])
    while !optimalenough(w, fun, 4.9406564584124654e-320, k) && n > 0
        n -= 1
        nextbase(x, base1, base2, base3, a, b, k)
        for i in 1:1:k
            base1[i] = BigFloat(base2[i])
            base2[i] = BigFloat(base3[i])
        end
        nextpolynomial(x, w, base2, fun, alfa, k)
    end
    #print(alfa,"\n", a,"\n", b,"\n")
    Clenshaw(alfa, a, b, res)
end

function scalar(x, i, y, j) #Oblicza ilocznyn skalarny oraz podnosci elementy do potęgi i/j
    sum = BigFloat(0)
    for g in 1:1:length(x)
        a = BigFloat(1)
        b = BigFloat(1)
        for h in 1:1:i
            a *= x[g]
        end
        for h in 1:1:j
            b *= y[g]
        end
        sum += a*b
    end
    return sum
end
            
function nextstandard(alpha, x, fun, k)
    M = Array{Float64,2}(UndefInitializer(), k+1, k+1)
    f = Array{Float64,2}(UndefInitializer(), k+1, 1)
    a = Array{Float64,2}(UndefInitializer(), k+1, 1)
    for i in 0:1:k
        f[i+1,1] = BigFloat(scalar(fun, 1, x, i))
        for j in 0:1:k
            M[i+1,j+1] = BigFloat(scalar(x,j,x,i)) #Zakładam, że macierz jest odwracalna
        end
    end
    #print("\nNextstandard:",M)
    #print("\nNextstandard:",inv(M),"\n")
    a = inv(M)*f
    for i in 0:1:k
        append!(alpha,a[i+1][1])
    end
end

function calculatepoly(alfa, x, w)
    for i in 1:1:length(x)
        wart = BigFloat(alfa[length(alfa)])
        for j in length(alfa)-1:-1:1
            wart *= BigFloat(x[i])
            wart += BigFloat(alfa[j])
        end
        append!(w, wart)
    end
end                          

function findstandardpoly(x, fun, delta, result)
    deg = 0
    res = []
    w = []
    nextstandard(res, x, fun, deg)
    calculatepoly(res, x, w)
    while !optimalenough(w, fun ,delta, length(x))
        deg += 1
        res = []
        w = []
        nextstandard(res, x, fun, deg)
        calculatepoly(res, x, w)
    end
    for i in 1:1:length(res)
        append!(result, res[i])
    end
end

function findnthstandardpoly(x, fun, n, result)
    deg = 0
    res = []
    w = []
    nextstandard(res, x, fun, deg)
    calculatepoly(res, x, w)
    while !optimalenough(w, fun ,4.9406564584124654e-30, length(x)) && n > 0
       #print("\nHalo\n", res)
        n -= 1
        deg += 1
        res = []
        w = []
        nextstandard(res, x, fun, deg)
        calculatepoly(res, x, w)
    end
    for i in 1:1:length(res)
        append!(result, res[i])
    end
end

function comparepoly(x,fun, n)
    if n >= length(x) - 1
        n = length(x) - 1
    end
    
    wiel = Array{Float64,1}(UndefInitializer(), 0)
    res = Array{Float64,1}(UndefInitializer(), 0)
    findnthstandardpoly(x,fun, n, wiel)
    calculatepoly(wiel, x, res)
    #print(res)
    a = BigFloat(optimalscalar(res, fun, length(fun)))
    #print(wiel)
    
    wiel = Array{Float64,1}(UndefInitializer(), 0)
    res = Array{Float64,1}(UndefInitializer(), 0)
    findnthoptimal(x, fun, n, wiel)
    calculatepoly(wiel, x, res)
    b = optimalscalar(res, fun, length(fun))
    
    return (a,b)
end
    

x = [0,10,20,30,40,80,90,95]
fun = [68.0, 67.1, 66.4, 65.6, 64.6, 61.8, 61, 60]

#przyklad z listy 7 zad 8
for i in 0:1:7
    a,b = comparepoly(x, fun, i)
    print(abs(b-a), " ", abs((b-a)/min(b,a)), "\n")
end

x = [-1.00, -0.75, -0.5, -0.25, 0, 0.25, 0.5, 0.75, 1.0]
fun = [-0.2209, 0.3259, 0.8826, 1.4392, 2.003, 2.5645, 3.1334, 3.7061, 4.2836]
#przyklad z treści zadania 
for i in 0:1:8
    a,b = comparepoly(x, fun, i)
    print(abs(b-a), " ", abs((b-a)/min(b,a)), "\n")
end

x = [1,2,3,4,5]
fun = [1,32,243,1024,3125]
#funkcja x^5 dla punktów równoodległych
for i in 0:1:4
    a,b = comparepoly(x, fun, i)
    print(abs(b-a), " ", abs((b-a)/min(b,a)), "\n")
end

x = [0.1,0.2,0.3,3,5]
fun = [0.00001,0.00032,0.00243,243,3125]
#funkcja x^5
for i in 0:1:4
    a,b = comparepoly(x, fun, i)
    print(abs(b-a), " ", abs((b-a)/min(b,a)), "\n")
end

#funkcja zwracajaca tablice wartosci w zadanych punktach dla x^(x * sin(x))
function calculfunc(points, res)
    n = length(points)
    for i in 1:1:n
        append!(res,BigFloat(points[i]^BigFloat(points[i] * sin(points[i]))))
    end
end
#funkcja zwracajaca tablice wartosci w zadanych punktach dla sin(x)
function calculfunc2(points, res)
    n = length(points)
    for i in 1:1:n
        append!(res,BigFloat(sin(points[i])))
    end
end

x = [1,1.5,2,2.5,3,3.5,4,4.5,5]
func = Array{Float64,1}(UndefInitializer(), 0)
calculfunc(x,func)
#funkcja x ^(x * sin(x))
for i in 0:1:8
    a,b = comparepoly(x, func, i)
    print(abs(b-a), " ", abs((b-a)/min(b,a)), "\n")
end

x = [-12.56, -12.55, 12.54, 12.55, 12.56]
func = Array{Float64,1}(UndefInitializer(), 0)
calculfunc2(x,func)
#funkcja sin(x) dla punktów różnoodległych
for i in 0:1:4
    a,b = comparepoly(x, func, i)
    print(abs(b-a), " ", abs((b-a)/min(b,a)), "\n")
end

x = [0,10,20,30,40,80,90,95]
fun = [68.0, 67.1, 66.4, 65.6, 64.6, 61.8, 61, 60]

#przyklad z listy 7 zad X
wiel = Array{Float64,1}(UndefInitializer(), 0)
wiel1 = Array{Float64,1}(UndefInitializer(), 0) 
wiel2 = Array{Float64,1}(UndefInitializer(), 0) 
wiel3 = Array{Float64,1}(UndefInitializer(), 0)
findoptimal(x,fun, 100, wiel)
findoptimal(x,fun, 1, wiel1)
findoptimal(x,fun, 0.27, wiel2)
findoptimal(x,fun, 0.1, wiel3)

print("wspolczynniki otrzymanego wielomianu:",wiel,"\n")
print("wspolczynniki otrzymanego wielomianu:",wiel1,"\n")
print("wspolczynniki otrzymanego wielomianu:",wiel2,"\n")
print("wspolczynniki otrzymanego wielomianu:",wiel3,"\n")
pyplot()
p = Poly(wiel)
p1 = Poly(wiel1)
p2 = Poly(wiel2)
p3 = Poly(wiel3)
xs = range(x[1], stop=x[length(x)], length=10000)
plot(xs, [p.(xs), p1.(xs), p2.(xs), p3.(xs)],seriesalpha = 0.55, linewidth=1.3, title="Wielomian optymalny", label=[ "w_0" "w_1" "w_2" "w_4"])
scatter!(x, fun, label="f")

x = [-1.00, -0.75, -0.5, -0.25, 0, 0.25, 0.5, 0.75, 1.0]
fun = [-0.2209, 0.3259, 0.8826, 1.4392, 2.003, 2.5645, 3.1334, 3.7061, 4.2836]
#przyklad z treści zadania 
wiel = Array{Float64,1}(UndefInitializer(), 0)
wiel1 = Array{Float64,1}(UndefInitializer(), 0) 
wiel2 = Array{Float64,1}(UndefInitializer(), 0) 
wiel3 = Array{Float64,1}(UndefInitializer(), 0)
findoptimal(x,fun, 10, wiel)
findoptimal(x,fun, 0.0001, wiel1)
findoptimal(x,fun, 0.000015, wiel2)
findoptimal(x,fun, 0.00001, wiel3)

print("wspolczynniki otrzymanego wielomianu w:",wiel,"\n")
print("wspolczynniki otrzymanego wielomianu w1:",wiel1,"\n")
print("wspolczynniki otrzymanego wielomianu w2:",wiel2,"\n")
print("wspolczynniki otrzymanego wielomianu w3:",wiel3,"\n")
pyplot()
p = Poly(wiel)
p1 = Poly(wiel1)
p2 = Poly(wiel2)
p3 = Poly(wiel3)
xs = range(x[1], stop=x[length(x)], length=10000)
plot(xs, [p.(xs), p1.(xs), p2.(xs), p3.(xs)],seriesalpha = 0.55, linewidth=1.3, title="Wielomian optymalny", label=["w_2" "w_3" "w_4" "w_5"])
scatter!(x, fun, label="f")

x = [1,2,3,4,5]
fun = [1,32,243,1024,3125]
#funkcja x^5 dla punktów równoodległych
wiel = Array{Float64,1}(UndefInitializer(), 0)
wiel1 = Array{Float64,1}(UndefInitializer(), 0) 
wiel2 = Array{Float64,1}(UndefInitializer(), 0) 
wiel3 = Array{Float64,1}(UndefInitializer(), 0)
findoptimal(x,fun, 1800000, wiel)
findoptimal(x,fun, 140000, wiel1)
findoptimal(x,fun, 1900, wiel2)
findoptimal(x,fun, 0.000001, wiel3)

print("wspolczynniki otrzymanego wielomianu:",wiel,"\n")
print("wspolczynniki otrzymanego wielomianu:",wiel1,"\n")
print("wspolczynniki otrzymanego wielomianu:",wiel2,"\n")
print("wspolczynniki otrzymanego wielomianu:",wiel3,"\n")
pyplot()
p = Poly(wiel)
p1 = Poly(wiel1)
p2 = Poly(wiel2)
p3 = Poly(wiel3)
xs = range(x[1], stop=x[length(x)], length=10000)
plot(xs, [p.(xs), p1.(xs), p2.(xs), p3.(xs)],seriesalpha = 0.55, linewidth=1.5, title="Wielomian optymalny", label=["w_1" "w_2" "w_3" "w_4"])
scatter!(x, fun, label="f")

x = [0.1,0.2,0.3,3,5]
fun = [0.00001,0.00032,0.00243,243,3125]
#funkcja x^5
wiel = Array{Float64,1}(UndefInitializer(), 0)
wiel1 = Array{Float64,1}(UndefInitializer(), 0) 
wiel2 = Array{Float64,1}(UndefInitializer(), 0) 
wiel3 = Array{Float64,1}(UndefInitializer(), 0)
findoptimal(x,fun, 1800000, wiel)
findoptimal(x,fun, 10000, wiel1)
findoptimal(x,fun, 1, wiel2)
findoptimal(x,fun, 0.000001, wiel3)

print("wspolczynniki otrzymanego wielomianu:",wiel,"\n")
print("wspolczynniki otrzymanego wielomianu:",wiel1,"\n")
print("wspolczynniki otrzymanego wielomianu:",wiel2,"\n")
print("wspolczynniki otrzymanego wielomianu:",wiel3,"\n")
pyplot()
p = Poly(wiel)
p1 = Poly(wiel1)
p2 = Poly(wiel2)
p3 = Poly(wiel3)
xs = range(x[1], stop=x[length(x)], length=10000)
plot(xs, [p.(xs), p1.(xs), p2.(xs), p3.(xs)],seriesalpha = 0.55, linewidth=1.5, title="Wielomian optymalny", label=["w_1" "w_2" "w_3" "w_4"])
scatter!(x, fun, label="f")

x = [1,1.5,2,2.5,3,3.5,4,4.5,5]
func = Array{Float64,1}(UndefInitializer(), 0)
calculfunc(x,func)
#funkcja x ^(x * sin(x))
wiel = Array{Float64,1}(UndefInitializer(), 0)
wiel1 = Array{Float64,1}(UndefInitializer(), 0) 
wiel2 = Array{Float64,1}(UndefInitializer(), 0) 
wiel3 = Array{Float64,1}(UndefInitializer(), 0)
findoptimal(x,func, 19, wiel)
findoptimal(x,func, 12, wiel1)
findoptimal(x,func, 9, wiel2)
findoptimal(x,func, 3, wiel3)

print("wspolczynniki otrzymanego wielomianu:",wiel,"\n")
print("wspolczynniki otrzymanego wielomianu:",wiel1,"\n")
print("wspolczynniki otrzymanego wielomianu:",wiel2,"\n")
print("wspolczynniki otrzymanego wielomianu:",wiel3,"\n")
pyplot()
p = Poly(wiel)
p1 = Poly(wiel1)
p2 = Poly(wiel2)
p3 = Poly(wiel3)
xs = range(x[1], stop=x[length(x)], length=10000)
plot(xs, [p.(xs), p1.(xs), p2.(xs), p3.(xs)],seriesalpha = 0.55, linewidth=1.5, title="Wielomian optymalny", label=["w_0" "w_1" "w_2" "w_3"])
scatter!(x, func, label="f")

x = [1,1.125,1.253,1.3999,1.4,4.76324,4.8234,4.9456,5]
func = Array{Float64,1}(UndefInitializer(), 0)
calculfunc(x,func)
#funkcja x ^(x * sin(x))
wiel = Array{Float64,1}(UndefInitializer(), 0)
wiel1 = Array{Float64,1}(UndefInitializer(), 0) 
wiel2 = Array{Float64,1}(UndefInitializer(), 0) 
wiel3 = Array{Float64,1}(UndefInitializer(), 0)
findoptimal(x,func, 5, wiel)
findoptimal(x,func, 0.6, wiel1)
findoptimal(x,func, 0.2, wiel2)
findoptimal(x,func, 0.02, wiel3)

print("wspolczynniki otrzymanego wielomianu:",wiel,"\n")
print("wspolczynniki otrzymanego wielomianu:",wiel1,"\n")
print("wspolczynniki otrzymanego wielomianu:",wiel2,"\n")
print("wspolczynniki otrzymanego wielomianu:",wiel3,"\n")
pyplot()
p = Poly(wiel)
p1 = Poly(wiel1)
p2 = Poly(wiel2)
p3 = Poly(wiel3)
xs = range(x[1], stop=x[length(x)], length=10000)
plot(xs, [p.(xs), p1.(xs), p2.(xs), p3.(xs)],seriesalpha = 0.55, linewidth=1.5, title="Wielomian optymalny", label=["w_0" "w_1" "w_2" "w_3"])
scatter!(x, func, label="f")

x = [-12.56, -6.28, 0, 6.28, 12.56]
func = Array{Float64,1}(UndefInitializer(), 0)
calculfunc2(x,func)
#funkcja sin(x) dla punktów równoodległych
wiel = Array{Float64,1}(UndefInitializer(), 0)
wiel1 = Array{Float64,1}(UndefInitializer(), 0)
findoptimal(x,func, 0.0002, wiel)
findoptimal(x,func, 0.0000000000001, wiel1)

print("wspolczynniki otrzymanego wielomianu:",wiel,"\n")
print("wspolczynniki otrzymanego wielomianu:",wiel1,"\n")
pyplot()
p = Poly(wiel)
p1 = Poly(wiel1)
xs = range(x[1], stop=x[length(x)], length=10000)
plot(xs, [p.(xs), p1.(xs)],seriesalpha = 0.55, linewidth=1.5, title="Wielomian optymalny", label=["w_0" "w_1"])
scatter!(x, func, label="f")

x = [-12.56, -12.55, 12.54, 12.55, 12.56]
func = Array{Float64,1}(UndefInitializer(), 0)
calculfunc2(x,func)
#funkcja sin(x) dla punktów różnoodległych
wiel = Array{Float64,1}(UndefInitializer(), 0)
wiel1 = Array{Float64,1}(UndefInitializer(), 0)
wiel2 = Array{Float64,1}(UndefInitializer(), 0)
findoptimal(x,func, 0.1, wiel)
findoptimal(x,func, 0.001, wiel1)

print("wspolczynniki otrzymanego wielomianu:",wiel,"\n")
print("wspolczynniki otrzymanego wielomianu:",wiel1,"\n")
pyplot()
p = Poly(wiel)
p1 = Poly(wiel1)
xs = range(x[1], stop=x[length(x)], length=10000)
plot(xs, [p.(xs), p1.(xs)],seriesalpha = 0.55, linewidth=1.5, title="Wielomian optymalny", label=["w_0" "w_1"])
scatter!(x, func, label="f")


