using Plots
using Printf
pyplot()
f(x) = sin.(x)cos.(x)+x^(sin.(x))

x = LinRange(0.0, 8*pi, 1000);
plot(x, f.(x), color="blue", linewidth=2.5, linestyle=:dash, 
    title="\$f(x)=\\sin(x) cos(x)+x^{sin.(x)}\$", legend=false, xlabel="\$x\$")

savefig("wykres.png")
