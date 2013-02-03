#!/usr/bin/gnuplot
 
# set style data histogram
set yrange [0:]
plot "choose_thread_tester.out" with boxes notitle
pause -1 "Exit"