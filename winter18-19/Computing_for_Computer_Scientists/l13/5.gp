set  title "Prognoza␣ocen␣z␣PWI"
set  boxwidth  0.6
set  style  fill  solid
set  yrange  [0:45]
plot "oceny.dat" using 2:xtic (1)  with  boxes
