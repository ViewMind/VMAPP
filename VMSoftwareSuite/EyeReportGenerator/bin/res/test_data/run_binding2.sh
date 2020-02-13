#!/bin/bash 

# Switchting to bin directory.
cd ../../

REFERENCE="res/test_data/binding2_output_reference"
OUTPUT="test_data/binding2_output" 

# Making sure the output does not exist
rm $OUTPUT 2> /dev/null

# Running the test. 
echo "RUNNING BINDING 2 TEST ...."

# if [[ ! -f $REFERENCE ]]; then
#    echo "FAILED: Could not find reference file $REFERENCE"
#    exit
# fi

Rscript res/binding2.R test_data/binding2_bc.csv test_data/binding2_uc.csv $OUTPUT # > /dev/null 2>&1

# 
# Adding the rest of the path.
# OUTPUT="res/$OUTPUT"
# 
# if [[ ! -f $OUTPUT ]]; then
#    echo "FAILED: Could not find output file $OUTPUT"
#    exit
# fi
# 
# DIFF=$(diff $REFERENCE $OUTPUT)
# 
# if [[ $DIFF != "" ]]; then
#    echo "FAILED"
# else 
#    echo "PASSED"
# fi
