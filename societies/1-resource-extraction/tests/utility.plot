#!/usr/bin/gnuplot
 
set title "Utility"
set xlabel "Units of Resource"
set ylabel "Utility"
plot "utility.out" title "" with lines
pause -1 "Exit"