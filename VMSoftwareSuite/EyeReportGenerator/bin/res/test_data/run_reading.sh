#!/bin/bash 

# Switchting to bin directory.
cd ../../

REFERENCE="res/test_data/reading_output_reference_1"
OUTPUT="test_data/reading_output_1" 

# Making sure the output does not exist
rm $OUTPUT 2> /dev/null

# Running the test. 
echo "RUNNING READING TEST 1...."

if [[ ! -f $REFERENCE ]]; then
   echo "FAILED: Could not find reference file $REFERENCE"
   exit
fi

Rscript res/reading.R test_data/reading_data_1.csv tempvar $OUTPUT 
