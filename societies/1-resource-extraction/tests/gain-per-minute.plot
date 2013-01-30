#!/usr/bin/gnuplot
 
set title "Gain Per Minute"
set xlabel "Units of Resource"
set ylabel "Gain Per Minute"
plot "gain-per-minute.out" title "" with lines
pause -1 "Exit"