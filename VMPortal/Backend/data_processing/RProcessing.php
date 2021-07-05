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

function RProcessing(ViewMindDataContainer &$vmdc, $csv_array, $workdir){

   $studies = $vmdc->getAvailableStudies();
   $study = $studies[0];

   // Setting up paths. 
   $output = "$workdir/routput";
   $rscript = "";
   $expected_output_fields = array();

   if ($study == Study::READING){
      $input  = $csv_array[Study::READING];
      $rscript = RScriptNames::READING_ES_SCRIPT;
      $expected_output_fields = ReadingResults::getConstList();
   }
   else if (($study == Study::BINDING_BC) || ($study == Study::BINDING_UC)){
      $input  = $csv_array[Study::BINDING_BC] . " " . $csv_array[Study::BINDING_UC];

      // No checks are done, because they were done previous to calling this function. 
      $vmdc->setRawDataAccessPathForStudy($study);
      $study_config = $vmdc->getStudyField(StudyField::STUDY_CONFIGURATION);

      if ($study_config[StudyParameter::NUMBER_TARGETS]  == BindingTargetCount::THREE) $rscript = RScriptNames::BINDING_3_SCRIPT;
      else if ($study_config[StudyParameter::NUMBER_TARGETS]  == BindingTargetCount::TWO) $rscript = RScriptNames::BINDING_2_SCRIPT;
      else return "Could not determine proper binding processing script from number of targets: " . $study_config[StudyParameter::NUMBER_TARGETS];

      $expected_output_fields = BindingResults::getConstList();

   }
   else if ($study == Study::NBACKRT){
      $input  = $csv_array[Study::NBACKRT];
      $rscript = RScriptNames::NBACKRT_SCRIPT;
      $expected_output_fields = NBackRTResults::getConstList();
   }
   else if ($study == Study::GONOGO){
      $input  = $csv_array[Study::GONOGO];
      $rscript = RScriptNames::GONOGO_SCRIPT;
      $expected_output_fields = GoNoGoResults::getConstList();
   }
   else{
      return "Unknown R Processing Study: $study";
   }

   $rdirectory = CONFIG[GlobalConfigProcResources::GROUP_NAME][GlobalConfigProcResources::R_SCRIPTS_REPO];
   $cmd = "cd $rdirectory; Rscript $rscript $input $output 2>&1";
   $temp = $cmd . "\n============================================================\n";   
   $routput = array();
   exec($cmd,$routput,$retval);
   $routput[] = "Exit Code: $retval";
   $temp = $temp . implode("\n",$routput);

   if (!is_file($output)) return "Failed to create output for R processing with command $temp";
   else {
      // Last step is to save the finalized results to the our JSON struture, first making sure there no more or no less values other than those expected. 

      $results = parse_ini_file($output);
      foreach ($results as $name => $value){
         if (!in_array($name,$expected_output_fields)){
            return "Unexpected result field $name for $study";
         }
         else {
            if (($key = array_search($name, $expected_output_fields)) !== false) {
               unset($expected_output_fields[$key]);
           }      
         }
      }

      if (!empty($expected_output_fields)){
         return "Missing the following result fields for study $study: " . implode(",",$expected_output_fields);
      }

      $vmdc->setFinalizedResults($results);

      return "";
   }

}
?>