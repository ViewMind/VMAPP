#!/bin/bash 

base_path=$(cd ..; pwd)

if [ $# != 1 ]; then
   echo "One and only one argument required. Possibilities: "
   echo "Base Path: $base_path"
   echo "1 -> Reading Spanish"
   echo "2 -> Binding 2"
   echo "3 -> Binding 3"
   echo "4 -> NBack RT"
   echo "5 -> GoNoGo"
   exit
fi

if [[ $1 == 1 ]]; then
   echo "RUNNING READING ES TEST ...."
   RSCRIPT="reading_es.R"
   OUTPUT="test_scripts/comparison_reading_output" 
   INPUT="test_scripts/reference_data/reference_reading_es.csv"   
elif [[ $1 == 2 ]]; then 
   echo "RUNNING BINDING 2 TEST ...."
   RSCRIPT="$base_path/binding2.R"
   OUTPUT="$base_path/test_scripts/comparison_binding2_output"
   MODEL="$base_path/binding2_model.RDS"
   INPUT="$base_path/test_scripts/reference_data/reference_binding_bc_2.csv $base_path/test_scripts/reference_data/reference_binding_uc_2.csv"
elif [[ $1 == 3 ]]; then 
   echo "RUNNING BINDING 3 TEST ...."
   RSCRIPT="$base_path/binding3.R"
   OUTPUT="$base_path/test_scripts/comparison_binding3_output"
   MODEL="$base_path/binding3_model.RDS"
   INPUT="$base_path/test_scripts/reference_data/reference_binding_bc_3.csv $base_path/test_scripts/reference_data/reference_binding_uc_3.csv"
elif [[ $1 == 4 ]]; then 
   echo "RUNNING NBACK RT TEST ...."
   RSCRIPT="nback_rt.R"
   OUTPUT="test_scripts/comparison_nback_rt_output" 
   INPUT="test_scripts/reference_data/reference_nbackrt.csv"   
elif [[ $1 == 5 ]]; then
   RSCRIPT="$base_path/gonogo.R"
   MODEL=""
   OUTPUT="$base_path/test_scripts/comparison_gonogo_output" 
   INPUT="$base_path/test_scripts/reference_data/reference_gonogo.csv"             
else
   echo "Unrecognized parameter $1. Exiting"
   exit
fi

# Making sure the output does not exist
rm $OUTPUT 2> /dev/null

# Running the test. 
Rscript $RSCRIPT $INPUT $MODEL $OUTPUT
