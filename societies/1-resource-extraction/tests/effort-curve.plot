#!/usr/bin/gnuplot
 
set title "Resource Effort Curve"
set xlabel "Experience with resource"
set ylabel "Minutes to extract one unit of resource"
plot "effort-curve.out" title "" with lines
pause -1 "Exit"