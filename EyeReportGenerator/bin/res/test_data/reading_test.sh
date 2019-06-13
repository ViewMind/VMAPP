#!/bin/bash 

# Switchting to bin directory.
cd ../../

Rscript res/reading.R test_data/reading_data_1.csv tempvar test_data/reading_output_1
Rscript res/reading.R test_data/reading_data_2.csv tempvar test_data/reading_output_2
Rscript res/reading.R test_data/reading_data_3.csv tempvar test_data/reading_output_3

