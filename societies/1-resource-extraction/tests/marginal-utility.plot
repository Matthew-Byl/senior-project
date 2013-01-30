#!/usr/bin/gnuplot
 
set title "Marginal Utility"
set xlabel "Units of Resource"
set ylabel "Marginal Utility"
plot "marginal-utility.out" title "" with lines
pause -1 "Exit"