<?php

include_once (__DIR__ . "/../common/named_constants.php");
include_once (__DIR__ . "/../common/config.php");
include_once (__DIR__. "/ViewMindDataContainer.php");
//include_once (__DIR__. "/value_category.php");

const CURRENT_PDF_GEN_VERSION = 1;

abstract class RScriptNames {
   const BINDING_2_SCRIPT     = "binding2.R";
   const BINDING_3_SCRIPT     = "binding3.R";
   const GONOGO_SCRIPT        = "gonogo.R";
   const NBACKRT_SCRIPT       = "nbackrt.R";
   const READING_ES_SCRIPT    = "reading_es.R";   
}

abstract class CommonResultKeys {
   const MEDIC = "medic";
   const EVALUATOR = "evaluator";
   const DATE = "date";
   const HOUR = "hour";
   const AGE  = "age";
   const PATIENT = "patient";
   const REPORT_TYPE = "report_type";
   const RESOLUTION = "resolution";
   const PDF_GEN_VERSION = "pdf_gen_version";
   const FIXATIONS = "fixations";
}

abstract class ReportTypes {
   const BINDING_3 = "binding_3";
   const BINDING_2 = "binding_2";
   const GONOGO    = "gonogo";
   const NBACKRT   = "nbackrt";
}

abstract class ModelNames {
   const BINDING_3_MODEL      = "binding3_model.RDS";
   const BINDING_2_MODEL      = "binding3_model.RDS";   
}

abstract class TrialDisplayIndexes {
   const BINDING_3            = [19 => "10", 29 => "20",  41 => "32"];
   const BINDING_2            = [19 => "10", 29 => "20",  41 => "32"];
   const GONOGO               = [10 => "10", 25 => "25",  40 => "40", 55 => "55"];
   const NBACKRT              = [13 => "12", 51 => "50",  91 => "90"]; // WARNING They actually correspondes to trial 13, 52 sfn 92 due to dumb mistake. 
}

function RProcessing(ViewMindDataContainer &$vmdc, $csv_array, $workdir){

   $studies = $vmdc->getAvailableStudies();
   $study = $studies[0];

   // Setting up paths. 
   $output = "$workdir/routput.json";
   $rscript = "";
   $model = "";
   $report_type = "";
   $valid_eye = "";
   $fixation_trial_list = [];

   if ($study == Study::READING){
      return "Unknown R Processing Study: $study";
      $input  = $csv_array[Study::READING];
      $rscript = RScriptNames::READING_ES_SCRIPT;
      //$expected_output_fields = ReadingResults::getConstList();
   }
   else if (($study == Study::BINDING_BC) || ($study == Study::BINDING_UC)){
      $input  = $csv_array[Study::BINDING_BC] . " " . $csv_array[Study::BINDING_UC];

      // No checks are done, because they were done previous to calling this function. 
      $study = Study::BINDING_BC;
      $vmdc->setRawDataAccessPathForStudy(Study::BINDING_BC); // This is the one we need for the binding score, later on. 
      $study_config = $vmdc->getStudyField(StudyField::STUDY_CONFIGURATION);
      $valid_eye = $study_config[StudyParameter::VALID_EYE];

      if ($study_config[StudyParameter::NUMBER_TARGETS]  == BindingTargetCount::THREE) {
         $rscript = RScriptNames::BINDING_3_SCRIPT;
         $model = ModelNames::BINDING_3_MODEL;
         $report_type = ReportTypes::BINDING_3;   
         $fixation_trial_list = TrialDisplayIndexes::BINDING_3;      
      }
      else if ($study_config[StudyParameter::NUMBER_TARGETS]  == BindingTargetCount::TWO){
          $rscript = RScriptNames::BINDING_2_SCRIPT;
          $model = ModelNames::BINDING_2_MODEL;
          $report_type = ReportTypes::BINDING_2;
          $fixation_trial_list = TrialDisplayIndexes::BINDING_2;
      }
      else return "Could not determine proper binding processing script from number of targets: " . $study_config[StudyParameter::NUMBER_TARGETS];

      //$expected_output_fields = BindingResults::getConstList();

   }
   else if ($study == Study::NBACKRT){
      $input  = $csv_array[Study::NBACKRT];
      $rscript = RScriptNames::NBACKRT_SCRIPT;
      $model = "";
      $report_type = ReportTypes::NBACKRT;
      $fixation_trial_list = TrialDisplayIndexes::NBACKRT;
      //$expected_output_fields = NBackRTResults::getConstList();
   }
   else if ($study == Study::GONOGO){
      $input  = $csv_array[Study::GONOGO];
      $rscript = RScriptNames::GONOGO_SCRIPT;
      $model = "";
      $report_type = ReportTypes::GONOGO;
      $fixation_trial_list = TrialDisplayIndexes::GONOGO;
      //$expected_output_fields = GoNoGoResults::getConstList();
   }
   else{
      return "Unknown R Processing Study: $study";
   }

   $rdirectory = CONFIG[GlobalConfigProcResources::GROUP_NAME][GlobalConfigProcResources::R_SCRIPTS_REPO];
   
   // Computing the input parameters for the script
   $rscript = $rdirectory . "/$rscript";
   if ($model != "") $model   = $rdirectory . "/$model";
      
   $cmd = "Rscript $rscript $input $model $output 2>&1";
   $temp = $cmd . "\n============================================================\n";   
   $routput = array();
   exec($cmd,$routput,$retval);
   $routput[] = "Exit Code: $retval";
   $temp = $temp . implode("\n",$routput);

   if (!is_file($output)) return "Failed to create output for R processing with command $temp";
   else {
      
      // Last step is to save the finalized results to the our JSON struture, first making sure there no more or no less values other than those expected. 

      $results = json_decode(file_get_contents($output),true);
      if (json_last_error() != JSON_ERROR_NONE){
         return "Failed to decode JSON string of output file";
      }

      // Some studies require extra steps after R Processing.
      $ans = ExtraProcessing($vmdc,$results,$study,$valid_eye,$fixation_trial_list);
      if ($ans != "") return $ans;

      // Adding the necessary metadata in order to generate the report.
      $results[CommonResultKeys::AGE] = $vmdc->getSubjectDataValue(SubjectField::AGE);
      $results[CommonResultKeys::PATIENT] = $vmdc->getSubjectDataValue(SubjectField::LASTNAME) . ", " . $vmdc->getSubjectDataValue(SubjectField::NAME);
      $results[CommonResultKeys::DATE] = $vmdc->getMetaDataField(MetadataField::DATE);
      $results[CommonResultKeys::HOUR] = $vmdc->getMetaDataField(MetadataField::HOUR);
      $evaluator = $vmdc->getAppUser(AppUserType::EVALUATOR);
      $medic = $vmdc->getAppUser(AppUserType::MEDIC);
      $results[CommonResultKeys::EVALUATOR] =  $evaluator[AppUserField::LASTNAME] .  ", " . $evaluator[AppUserField::NAME];
      $results[CommonResultKeys::MEDIC] =  $medic[AppUserField::LASTNAME] .  ", " . $medic[AppUserField::NAME];
      $results[CommonResultKeys::PDF_GEN_VERSION] = CURRENT_PDF_GEN_VERSION;
      $results[CommonResultKeys::REPORT_TYPE] = $report_type;

      // Saving the final ourput version to disk. For reference. 
      $fid = fopen($output,"w");
      fwrite($fid,json_encode($results,JSON_PRETTY_PRINT));
      fclose($fid);      

      // Then we finally set the results. 
      $vmdc->setFinalizedResults($results);

      return "";
   }

}

function ExtraProcessing(ViewMindDataContainer &$vmdc, &$results, $study,  $valid_eye, $fixation_trial_list){

   // We always get the Right fixations, as long as the selected eye is not the left one. 
   $fix_to_get = DataSetField::FIXATIONS_R;
   if ($valid_eye == Eye::LEFT) $fix_to_get = DataSetField::FIXATIONS_L;

   // Getting the resolution which is required for fixation mapping on the reports.   
   $pp = $vmdc->getProcessingParameters();
   $resolution = [0,0];
   $resolution[0] = $pp[ProcessingParameter::RESOLUTION_WIDTH];
   $resolution[1] = $pp[ProcessingParameter::RESOLUTION_HEIGHT];
   
   $results[CommonResultKeys::RESOLUTION] = $resolution;

   $results[CommonResultKeys::FIXATIONS] = array();


   if (($study == Study::BINDING_BC)){
      
      // At this instance on the Binding Requires an extra processing step
      // which basically boils down to the BC Correct answer score to the report. 
      $binding_score = $vmdc->getStudyField(StudyField::BINDING_SCORE);
      if ($binding_score === FALSE){
         return "Could not get the BC Binding Score. Reason: " . $vmdc->getError();
      }
      
      $results[FinalizedStudyNames::BINDING_BEHAVIOURAL_RESPONSE] = $binding_score[BindingScore::CORRECT];

      // So the fixation trial list is an associative array where the key is the actual index in the list of trials
      // Where we need to get the fixations while the value is the string representaion of the trial number to display in the report. 
      foreach ($fixation_trial_list as $trial_index => $display_trial_number){
         $fixations = array();
         $trial = $vmdc->getTrial($trial_index);

         $set_to_get = [DataSetType::ENCODING_1, DataSetType::RETRIEVAL_1];
         foreach ($set_to_get as $data_set_type_to_get){
            
            $allfix = $trial[TrialField::DATA][$data_set_type_to_get][$fix_to_get];
            $fixations[$data_set_type_to_get] = array();

            foreach ($allfix as $fix) {
               $fixations[$data_set_type_to_get][] = [$fix[FixationVectorField::X], $fix[FixationVectorField::Y]];
            }
   
         }

         $results[CommonResultKeys::FIXATIONS][$display_trial_number] = $fixations;
         
      }

   }
   else if ($study == Study::GONOGO){


      // So the fixation trial list is an associative array where the key is the actual index in the list of trials
      // Where we need to get the fixations while the value is the string representaion of the trial number to display in the report. 
      foreach ($fixation_trial_list as $trial_index => $display_trial_number) {
          $fixations = array();
          $trial = $vmdc->getTrial($trial_index);

          $set_to_get = DataSetType::UNIQUE;
 
          $allfix = $trial[TrialField::DATA][$set_to_get][$fix_to_get];

          foreach ($allfix as $fix) {
              $fixations[] = [$fix[FixationVectorField::X], $fix[FixationVectorField::Y]];
          }

          $results[CommonResultKeys::FIXATIONS][$display_trial_number] = $fixations;
      }

   }
   else if ($study == Study::NBACKRT){
      // So the fixation trial list is an associative array where the key is the actual index in the list of trials
      // Where we need to get the fixations while the value is the string representaion of the trial number to display in the report. 
      // For NBack RT the fixation for encoding_1 are really for econding_1, encoding_2, and encoding_3


      $sets_to_get = [
         DataSetType::ENCODING_1 =>  [DataSetType::ENCODING_1, DataSetType::ENCODING_2, DataSetType::ENCODING_3], // Al econding are saved on the encondign 1 category. 
         DataSetType::RETRIEVAL_1 => [ DataSetType::RETRIEVAL_1 ]
      ];

      foreach ($fixation_trial_list as $trial_index => $display_trial_number) {
         $fixations = array();
         $trial = $vmdc->getTrial($trial_index);

         $results[CommonResultKeys::FIXATIONS][$display_trial_number] = array();

         foreach ($sets_to_get as $name_to_save => $list_of_data_sets_to_add){

            $fixations = array();
            
            // Adding all fixations for all data set types for this category, to the list. 
            foreach ($list_of_data_sets_to_add as $data_set_type){
               $allfix = $trial[TrialField::DATA][$data_set_type][$fix_to_get];
               foreach ($allfix as $fix) {
                  $fixations[] = [$fix[FixationVectorField::X], $fix[FixationVectorField::Y]];
               }   

            }
            
            // Saved as the fixations for this category. 
            $results[CommonResultKeys::FIXATIONS][$display_trial_number][$name_to_save] = $fixations;

         }
     }      

   }

   return "";
}

?>