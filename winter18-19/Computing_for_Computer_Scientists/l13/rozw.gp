set title "Prognoza ocen z PWI po liście 10"
set yrange [0:18]
set style data histogram
set style histogram clustered gap 1
set style fill solid border -1
set xtics rotate out


set datafile separator ';'

plot for [i=2:7]'oceny.csv' using i:xtic(1) ti col
