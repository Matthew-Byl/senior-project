#!/bin/bash

./choose_thread_tester > choose_thread_tester_raw.out
sort -n choose_thread_tester_raw.out | uniq -c | sed "s/\s*\([0-9]*\) \([0-9]*\)/\2 \1/" > choose_thread_tester.out
./choose_thread_tester.plot