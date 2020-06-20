#!/bin/bash 

# Switchting to bin directory.
cd ../../

OUTPUT="test_data/reading_es_output" 

# Making sure the output does not exist
rm $OUTPUT 2> /dev/null

# Running the test. 
echo "RUNNING READING ES TEST ...."

Rscript res/reading_es.R test_data/reading_es.csv $OUTPUT
