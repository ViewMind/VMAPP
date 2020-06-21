#!/bin/bash 

if [ $# != 1 ]; then
   echo "One and only one argument required. Possibilities: "
   echo "1 -> Reading Spanish"
   echo "2 -> Binding 2"
   echo "3 -> Binding 3"
   echo "4 -> NBack RT"
   exit
fi

if [[ $1 == 1 ]]; then
   echo "RUNNING READING ES TEST ...."
   RSCRIPT="res/reading_es.R"
   OUTPUT="test_scripts/comparison_reading_output" 
   INPUT="test_scripts/reference_data/reference_reading_es.csv"   
elif [[ $1 == 2 ]]; then 
   echo "RUNNING BINDING 2 TEST ...."
   RSCRIPT="res/binding2.R"
   OUTPUT="test_scripts/comparison_binding2_output"
   INPUT="test_scripts/reference_data/reference_binding_bc_2.csv test_scripts/reference_data/reference_binding_uc_2.csv"
elif [[ $1 == 3 ]]; then 
   echo "RUNNING BINDING 3 TEST ...."
   RSCRIPT="res/binding3.R"
   OUTPUT="test_scripts/comparison_binding3_output"
   INPUT="test_scripts/reference_data/reference_binding_bc_3.csv test_scripts/reference_data/reference_binding_uc_3.csv"
elif [[ $1 == 4 ]]; then 
   echo "RUNNING NBACK RT TEST ...."
   RSCRIPT="res/nback_rt.R"
   OUTPUT="test_scripts/comparison_nback_rt_output" 
   INPUT="test_scripts/reference_data/reference_nbackrt.csv"   
else
   echo "Unrecognized parameter $1. Exiting"
   exit
fi

# Switchting to bin directory.
cd ../../

# Making sure the output does not exist
rm $OUTPUT 2> /dev/null

# Running the test. 
Rscript $RSCRIPT $INPUT $OUTPUT
