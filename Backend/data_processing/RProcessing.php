<?php

include_once (__DIR__ . "/../common/named_constants.php");
include_once (__DIR__ . "/../common/config.php");
include_once ("ViewMindDataContainer.php");

abstract class RScriptNames {
   const BINDING_2_SCRIPT     = "binding2.R";
   const BINDING_3_SCRIPT     = "binding3.R";
   const GONOGO_SCRIPT        = "gonogo.R";
   const NBACKRT_SCRIPT       = "nback_rt.R";
   const READING_ES_SCRIPT    = "reading_es.R";   
}

// if [[ $1 == 1 ]]; then
   // echo "RUNNING READING ES TEST ...."
   // RSCRIPT="res/reading_es.R"
   // OUTPUT="test_scripts/comparison_reading_output" 
   // INPUT="test_scripts/reference_data/reference_reading_es.csv"   
// elif [[ $1 == 2 ]]; then 
   // echo "RUNNING BINDING 2 TEST ...."
   // RSCRIPT="res/binding2.R"
   // OUTPUT="test_scripts/comparison_binding2_output"
   // INPUT="test_scripts/reference_data/reference_binding_bc_2.csv test_scripts/reference_data/reference_binding_uc_2.csv"
// elif [[ $1 == 3 ]]; then 
   // echo "RUNNING BINDING 3 TEST ...."
   // RSCRIPT="res/binding3.R"
   // OUTPUT="test_scripts/comparison_binding3_output"
   // INPUT="test_scripts/reference_data/reference_binding_bc_3.csv test_scripts/reference_data/reference_binding_uc_3.csv"
// elif [[ $1 == 4 ]]; then 
   // echo "RUNNING NBACK RT TEST ...."
   // RSCRIPT="res/nback_rt.R"
   // OUTPUT="test_scripts/comparison_nback_rt_output" 
   // INPUT="test_scripts/reference_data/reference_nbackrt.csv"   
// elif [[ $1 == 5 ]]; then
   // RSCRIPT="res/gonogo.R"
   // OUTPUT="test_scripts/comparison_gonogo_output" 
   // INPUT="test_scripts/reference_data/reference_gonogo.csv"             
// else
   // echo "Unrecognized parameter $1. Exiting"
   // exit
// fi
// 
// # Switchting to bin directory.
// cd ../../
// 
// # Making sure the output does not exist
// rm $OUTPUT 2> /dev/null
// 
// # Running the test. 
// Rscript $RSCRIPT $INPUT $OUTPUT

function RProcessing(ViewMindDataContainer &$vmdc, $csv_array, $workdir){

   $studies = $vmdc->getAvailableStudies();
   $study = $studies[0];

   // Setting up paths. 
   $output = "$workdir/routput";
   $rscript = "";

   if ($study == Study::READING){
      $input  = $csv_array[Study::READING];
      $rscript = RScriptNames::READING_ES_SCRIPT;
   }
   else if (($study == Study::BINDING_BC) || ($study == Study::BINDING_UC)){
      $input  = $csv_array[Study::BINDING_BC] . " " . $csv_array[Study::BINDING_UC];

      // No checks are done, because they were done previous to calling this function. 
      $vmdc->setRawDataAccessPathForStudy($study);
      $study_config = $vmdc->getStudyField(StudyField::STUDY_CONFIGURATION);

      if ($study_config[StudyParameter::NUMBER_TARGETS]  == BindingTargetCount::THREE) $rscript = RScriptNames::BINDING_3_SCRIPT;
      else if ($study_config[StudyParameter::NUMBER_TARGETS]  == BindingTargetCount::TWO) $rscript = RScriptNames::BINDING_2_SCRIPT;
      else return "Could not determine proper binding processing script from number of targets: " . $study_config[StudyParameter::NUMBER_TARGETS];
   }
   else if ($study == Study::NBACKRT){
      $input  = $csv_array[Study::NBACKRT];
      $rscript = RScriptNames::NBACKRT_SCRIPT;
   }
   else if ($study == Study::GONOGO){
      $input  = $csv_array[Study::GONOGO];
      $rscript = RScriptNames::GONOGO_SCRIPT;
   }
   else{
      return "Unknown R Processing Study: $study";
   }

   $rdirectory = CONFIG[GlobalConfigProcResources::GROUP_NAME][GlobalConfigProcResources::R_SCRIPTS_REPO];
   $cmd = "cd $rdirectory; Rscript $rscript $input $output";
   $temp = $cmd . "\n============================================================\n";   
   $routput = array();
   exec($cmd,$routput,$retval);
   $routput[] = "Exit Code: $retval";
   $temp = $temp . implode("\n",$routput);

   if (!is_file($output)) return "Failed to create output for R processing with command $temp";
   else return "";

}

?>