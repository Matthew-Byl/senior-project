#!/bin/bash
#
# Run all the unit tests for this stage in the
#  Societies simulation.

echo "Compiling tests..."
make
echo "effort-curve"
./effort-curve
echo "gain-per-minute"
./gain-per-minute
echo "marginal-utility"
./marginal-utility
echo "utility"
./utility
