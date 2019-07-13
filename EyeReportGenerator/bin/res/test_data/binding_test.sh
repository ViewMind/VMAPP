#!/bin/bash 

# Switchting to bin directory.
cd ../../

Rscript res/binding.R test_data/test_bc.csv test_data/test_uc.csv test_data/binding_output
