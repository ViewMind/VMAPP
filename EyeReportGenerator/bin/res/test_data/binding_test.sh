#!/bin/bash 

# Switchting to bin directory.
cd ../../

Rscript res/binding.R test_data/mendez_bc.csv test_data/mendez_uc.csv test_data/binding_output
