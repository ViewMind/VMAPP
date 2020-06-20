#!/bin/bash 

# Switchting to bin directory.
cd ../../

OUTPUT="test_data/nback_rt_output" 

# Making sure the output does not exist
rm $OUTPUT 2> /dev/null

# Running the test. 
echo "RUNNING NBACK RT TEST ...."

# Rscript res/binding2.R test_data/binding2_bc.csv test_data/binding2_uc.csv $OUTPUT # > /dev/null 2>&1
Rscript res/nback_rt.R test_data/nbackrt.csv $OUTPUT







