#!/bin/bash

# Get the parameter from the command line
param=$1

for i in {1..100}
do
   ./o_solution global $param global/GlobalTest.csv global/LUTTest.csv
done