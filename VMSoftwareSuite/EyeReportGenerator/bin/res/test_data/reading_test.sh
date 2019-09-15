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

Rscript res/reading.R test_data/reading_data_1.csv tempvar  $OUTPUT > /dev/null 2>&1

# Adding the rest of the path.
OUTPUT="res/$OUTPUT"

if [[ ! -f $OUTPUT ]]; then
   echo "FAILED: Could not find output file $OUTPUT"
   exit
fi

DIFF=$(diff $REFERENCE $OUTPUT)

if [[ $DIFF != "" ]]; then
   echo "FAILED"
   exit
else 
   echo "PASSED"
fi

####################

REFERENCE="res/test_data/reading_output_reference_2"
OUTPUT="test_data/reading_output_2" 

# Making sure the output does not exist
rm $OUTPUT 2> /dev/null

# Running the test. 
echo "RUNNING READING TEST 2...."

if [[ ! -f $REFERENCE ]]; then
   echo "FAILED: Could not find reference file $REFERENCE"
   exit
fi

Rscript res/reading.R test_data/reading_data_2.csv tempvar  $OUTPUT > /dev/null 2>&1

# Adding the rest of the path.
OUTPUT="res/$OUTPUT"

if [[ ! -f $OUTPUT ]]; then
   echo "FAILED: Could not find output file $OUTPUT"
   exit
fi

DIFF=$(diff $REFERENCE $OUTPUT)

if [[ $DIFF != "" ]]; then
   echo "FAILED"
   exit
else 
   echo "PASSED"
fi

####################

REFERENCE="res/test_data/reading_output_reference_3"
OUTPUT="test_data/reading_output_3" 

# Making sure the output does not exist
rm $OUTPUT 2> /dev/null

# Running the test. 
echo "RUNNING READING TEST 3...."

if [[ ! -f $REFERENCE ]]; then
   echo "FAILED: Could not find reference file $REFERENCE"
   exit
fi

Rscript res/reading.R test_data/reading_data_3.csv tempvar  $OUTPUT > /dev/null 2>&1

# Adding the rest of the path.
OUTPUT="res/$OUTPUT"

if [[ ! -f $OUTPUT ]]; then
   echo "FAILED: Could not find output file $OUTPUT"
   exit
fi

DIFF=$(diff $REFERENCE $OUTPUT)

if [[ $DIFF != "" ]]; then
   echo "FAILED"
   exit
else 
   echo "PASSED"
fi

