0a1
> # 308533 Wiktor Pilarczyk
2,4d2
< import Pkg; Pkg.add("Plots")
< import Pkg; Pkg.add("Polynomials")
< import Pkg; Pkg.add("PyPlot")
89c87
< function optimalscalar(w, fun, k)
---
> function optimalenough(w, fun, delta, k)
92d89
<         #print((BigFloat(w[i]) - BigFloat(fun[i])) * (BigFloat(w[i]) - BigFloat(fun[i])), "a\n")
94d90
<         #print(sum, "\n")
96,101d91
<     #print(sum)
<     return BigFloat(sum)
< end
< 
< function optimalenough(w, fun, delta, k)
<     sum = optimalscalar(w, fun, k)
159,317d148
< function findnthoptimal(x, fun, n, res) # funkcja znajdujaca n - ty wiel optymalny
<     k = length(x)
<     base1 = [] #obliczenie dwóch pierwszych elementów bazy
<     base2 = []
<     base3 = []
<     a = []
<     b = [BigFloat(0.0)]
<     alfa = []
<     w = []
<     initarray(base1, BigFloat(1.0), k)
<     initarray(base2, BigFloat(0.0), k)
<     initarray(base3, BigFloat(0.0), k)
<     var1 = BigFloat(0.0)
<     
<     for i in 1:1:k #wyliczanie a1
<         var1 += BigFloat(x[i])
<     end
<     append!(a,BigFloat(var1/k))
<     
<     var1 = BigFloat(0.0)
<     for i in 1:1:k
<         var1 += BigFloat(fun[i])
<     end
<     append!(alfa, BigFloat(var1/k))
<     initarray(w, BigFloat(alfa[1]), k)
<     if optimalenough(w, fun, 4.9406564584124654e-320, k) || n == 0
<         Clenshaw(alfa, a, b, res)
<         return
<     end
<     n -= 1
<     
<     for i in 1:1:k #wyliczanie drugie elementu bazy
<         base2[i] = x[i] - a[1]
<     end
<     
<     nextpolynomial(x, w, base2, fun, alfa, k)
<     #print("\base2: ",base2)
<     #print("\nalfa[1]: ",alfa[1])
<     #print("\nalfa[2]: ",alfa[2])
<     #print("\na[1]: ",a[1])
<     while !optimalenough(w, fun, 4.9406564584124654e-320, k) && n > 0
<         n -= 1
<         nextbase(x, base1, base2, base3, a, b, k)
<         for i in 1:1:k
<             base1[i] = BigFloat(base2[i])
<             base2[i] = BigFloat(base3[i])
<         end
<         nextpolynomial(x, w, base2, fun, alfa, k)
<     end
<     #print(alfa,"\n", a,"\n", b,"\n")
<     Clenshaw(alfa, a, b, res)
< end
< 
< function scalar(x, i, y, j) #Oblicza ilocznyn skalarny oraz podnosci elementy do potęgi i/j
<     sum = BigFloat(0)
<     for g in 1:1:length(x)
<         a = BigFloat(1)
<         b = BigFloat(1)
<         for h in 1:1:i
<             a *= x[g]
<         end
<         for h in 1:1:j
<             b *= y[g]
<         end
<         sum += a*b
<     end
<     return sum
< end
<             
< function nextstandard(alpha, x, fun, k)
<     M = Array{Float64,2}(UndefInitializer(), k+1, k+1)
<     f = Array{Float64,2}(UndefInitializer(), k+1, 1)
<     a = Array{Float64,2}(UndefInitializer(), k+1, 1)
<     for i in 0:1:k
<         f[i+1,1] = BigFloat(scalar(fun, 1, x, i))
<         for j in 0:1:k
<             M[i+1,j+1] = BigFloat(scalar(x,j,x,i)) #Zakładam, że macierz jest odwracalna
<         end
<     end
<     #print("\nNextstandard:",M)
<     #print("\nNextstandard:",inv(M),"\n")
<     a = inv(M)*f
<     for i in 0:1:k
<         append!(alpha,a[i+1][1])
<     end
< end
< 
< function calculatepoly(alfa, x, w)
<     for i in 1:1:length(x)
<         wart = BigFloat(alfa[length(alfa)])
<         for j in length(alfa)-1:-1:1
<             wart *= BigFloat(x[i])
<             wart += BigFloat(alfa[j])
<         end
<         append!(w, wart)
<     end
< end                          
< 
< function findstandardpoly(x, fun, delta, result)
<     deg = 0
<     res = []
<     w = []
<     nextstandard(res, x, fun, deg)
<     calculatepoly(res, x, w)
<     while !optimalenough(w, fun ,delta, length(x))
<         deg += 1
<         res = []
<         w = []
<         nextstandard(res, x, fun, deg)
<         calculatepoly(res, x, w)
<     end
<     for i in 1:1:length(res)
<         append!(result, res[i])
<     end
< end
< 
< function findnthstandardpoly(x, fun, n, result)
<     deg = 0
<     res = []
<     w = []
<     nextstandard(res, x, fun, deg)
<     calculatepoly(res, x, w)
<     while !optimalenough(w, fun ,4.9406564584124654e-30, length(x)) && n > 0
<        #print("\nHalo\n", res)
<         n -= 1
<         deg += 1
<         res = []
<         w = []
<         nextstandard(res, x, fun, deg)
<         calculatepoly(res, x, w)
<     end
<     for i in 1:1:length(res)
<         append!(result, res[i])
<     end
< end
< 
< function comparepoly(x,fun, n)
<     if n >= length(x) - 1
<         n = length(x) - 1
<     end
<     
<     wiel = Array{Float64,1}(UndefInitializer(), 0)
<     res = Array{Float64,1}(UndefInitializer(), 0)
<     findnthstandardpoly(x,fun, n, wiel)
<     calculatepoly(wiel, x, res)
<     #print(res)
<     a = BigFloat(optimalscalar(res, fun, length(fun)))
<     #print(wiel)
<     
<     wiel = Array{Float64,1}(UndefInitializer(), 0)
<     res = Array{Float64,1}(UndefInitializer(), 0)
<     findnthoptimal(x, fun, n, wiel)
<     calculatepoly(wiel, x, res)
<     b = optimalscalar(res, fun, length(fun))
<     
<     return (a,b)
< end
<     
< 
320,386d150
< 
< #przyklad z listy 7 zad 8
< for i in 0:1:7
<     a,b = comparepoly(x, fun, i)
<     print(abs(b-a), " ", abs((b-a)/min(b,a)), "\n")
< end
< 
< x = [-1.00, -0.75, -0.5, -0.25, 0, 0.25, 0.5, 0.75, 1.0]
< fun = [-0.2209, 0.3259, 0.8826, 1.4392, 2.003, 2.5645, 3.1334, 3.7061, 4.2836]
< #przyklad z treści zadania 
< for i in 0:1:8
<     a,b = comparepoly(x, fun, i)
<     print(abs(b-a), " ", abs((b-a)/min(b,a)), "\n")
< end
< 
< x = [1,2,3,4,5]
< fun = [1,32,243,1024,3125]
< #funkcja x^5 dla punktów równoodległych
< for i in 0:1:4
<     a,b = comparepoly(x, fun, i)
<     print(abs(b-a), " ", abs((b-a)/min(b,a)), "\n")
< end
< 
< x = [0.1,0.2,0.3,3,5]
< fun = [0.00001,0.00032,0.00243,243,3125]
< #funkcja x^5
< for i in 0:1:4
<     a,b = comparepoly(x, fun, i)
<     print(abs(b-a), " ", abs((b-a)/min(b,a)), "\n")
< end
< 
< #funkcja zwracajaca tablice wartosci w zadanych punktach dla x^(x * sin(x))
< function calculfunc(points, res)
<     n = length(points)
<     for i in 1:1:n
<         append!(res,BigFloat(points[i]^BigFloat(points[i] * sin(points[i]))))
<     end
< end
< #funkcja zwracajaca tablice wartosci w zadanych punktach dla sin(x)
< function calculfunc2(points, res)
<     n = length(points)
<     for i in 1:1:n
<         append!(res,BigFloat(sin(points[i])))
<     end
< end
< 
< x = [1,1.5,2,2.5,3,3.5,4,4.5,5]
< func = Array{Float64,1}(UndefInitializer(), 0)
< calculfunc(x,func)
< #funkcja x ^(x * sin(x))
< for i in 0:1:8
<     a,b = comparepoly(x, func, i)
<     print(abs(b-a), " ", abs((b-a)/min(b,a)), "\n")
< end
< 
< x = [-12.56, -12.55, 12.54, 12.55, 12.56]
< func = Array{Float64,1}(UndefInitializer(), 0)
< calculfunc2(x,func)
< #funkcja sin(x) dla punktów różnoodległych
< for i in 0:1:4
<     a,b = comparepoly(x, func, i)
<     print(abs(b-a), " ", abs((b-a)/min(b,a)), "\n")
< end
< 
< x = [0,10,20,30,40,80,90,95]
< fun = [68.0, 67.1, 66.4, 65.6, 64.6, 61.8, 61, 60]
< 
393c157
< findoptimal(x,fun, 1, wiel1)
---
> findoptimal(x,fun, 10, wiel1)
484a249,256
> #funkcja zwracajaca tablice wartosci w zadanych punktach dla x^(x * sin(x))
> function calculfunc(points, res)
>     n = length(points)
>     for i in 1:1:n
>         append!(res,BigFloat(points[i]^BigFloat(points[i] * sin(points[i]))))
>     end
> end
> 
535a308,315
> 
> #funkcja zwracajaca tablice wartosci w zadanych punktach dla sin(x)
> function calculfunc2(points, res)
>     n = length(points)
>     for i in 1:1:n
>         append!(res,BigFloat(sin(points[i])))
>     end
> end
