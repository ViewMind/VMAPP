<?php

include_once ("DataContainerNames.php");

class RawDataInterpreter {
   
   private $error;
   private $data;
   private $available_studies;

   private $current_study;
   private $current_trial_list_type;
   private $current_number_of_trials;
   private $available_data_sets;

   function __construct(){
   }

   function getError(){
      return $this->error;
   }

   function loadFromFile($raw_data_file){
      $this->error = "";

      // Checking if the file exists. 
      if (!is_file($raw_data_file)){
         $this->error = "$raw_data_file is not a file";
         return false;
      }

      // Checking if it can be decoded into a valid JSON FILE
      $this->data = json_decode(file_get_contents($raw_data_file),true);

      if (json_last_error() != JSON_ERROR_NONE){
         $this->error = "Could not decode $raw_data_file. JSON Error: " + json_last_error_msg();
         return false;
      }

      // Figuring out studies the structure.
      if (!array_key_exists(MainFields::STUDIES,$this->data)){
         $this->error = "No studies field in data";
         return false;
      }

      $this->available_studies = array_keys($this->data[MainFields::STUDIES]);

      // Checking that metadata exists.
      if (!array_key_exists(MainFields::METADATA,$this->data)){
         $this->error = "No metada field was found in the data";
         return false;
      }

      return true;
   }

   function getAvailableStudies(){
      return $this->available_studies;
   }

   function getSubjectDataValue($subject_field){
      if (!SubjectFields::validate($subject_field)){
         $this->error = "$subject_field is an invalid subject field";
         return "";
      }

      if (!array_key_exists(MainFields::SUBJECT,$this->data)){
         $this->error = "The main field subject does not exist";
         return "";
      }

      if (!array_key_exists($subject_field,$this->data[MainFields::SUBJECT])){
         $this->error = "The field $subject_field does not exist as part of the subject";
         return "";
      }

      return $this->data[MainFields::SUBJECT][$subject_field];

   }

   function getMetaDataField($metada_field){
      if (!MetadaFields::validate($metada_field)){
         $this->error = "$metada_field is an invalid metadata field";
         return "";
      }

      if (!array_key_exists(MainFields::METADATA,$this->data)){
         $this->error = "The main field metadata does not exist";
         return "";
      }

      if (!array_key_exists($metada_field,$this->data[MainFields::METADATA])){
         $this->error = "The field $metada_field does not exist as part of the metadat";
         return "";
      }

      return $this->data[MainFields::METADATA][$metada_field];

   }   

   function getNumberOfTrials() {
      return $this->current_number_of_trials;
   }

   function setProcessingParameters($pp){
      $this->data[MainFields::PROCESSING_PARAMETERS] = $pp;
   }

   function getProcessingParameters(){
      if (array_key_exists(MainFields::PROCESSING_PARAMETERS,$this->data)){
         return $this->data[MainFields::PROCESSING_PARAMETERS];
      }
      else return array();
   }

   function setRawDataAccessPathForTrialList($study, $trial_list_type){

      $this->current_study = "";
      $this->current_trial_list_type = "";

      if (!in_array($study,$this->available_studies)){
         $this->error = "$study is not an available studies. Available studies are: " . implode(",",$this->available_studies);
         return false;
      }

      if (!array_key_exists(StudyFields::TRIAL_LIST,$this->data[MainFields::STUDIES][$study])){
         $this->error = "Unable to find the trial list field for the selected study of $study";
         return false;
      }

      if (!array_key_exists($trial_list_type,$this->data[MainFields::STUDIES][$study][StudyFields::TRIAL_LIST])){
         $this->error = "Unable to find the trial list type $trial_list_type for the selected study of $study";
         return false;
      }      

      $this->current_number_of_trials = count($this->data[MainFields::STUDIES][$study][StudyFields::TRIAL_LIST][$trial_list_type]);

      if ($this->current_number_of_trials > 0){
         if (!array_key_exists(TrialFields::DATA,$this->data[MainFields::STUDIES][$study][StudyFields::TRIAL_LIST][$trial_list_type][0])){
            $this->error = "Unable to find the data field of the first trial for the $study with trial list type of $trial_list_type";
            return false;
         }             
      }

      $this->current_study = $study;
      $this->current_trial_list_type = $trial_list_type;

      // This does assume that ALL trials in the list contains the same DataSets (as the ones in the first one) which SHOULD always be the case. 
      $this->available_data_sets = array_keys($this->data[MainFields::STUDIES][$this->current_study][StudyFields::TRIAL_LIST][$this->current_trial_list_type][0][TrialFields::DATA]);
      //var_dump($this->available_data_sets);

      return true;
   }

   function getValidEyeForCurrentStudy(){

      if ($this->current_study == ""){
         $this->error = "Current study has not been set";
         return false;
      }
      
      if (!array_key_exists(StudyFields::STUDY_CONFIGURATION,$this->data[MainFields::STUDIES][$this->current_study])){
         $this->error = "Study " . $this->current_study . " does not have a study configuration field";
         return "";
      }

      if (!array_key_exists(StudyConfigurationFields::VALID_EYE,$this->data[MainFields::STUDIES][$this->current_study][StudyFields::STUDY_CONFIGURATION])){
         $this->error = "Configuration for study " . $this->current_study . " does not have a valid eye field";
         return "";
      }

      return $this->data[MainFields::STUDIES][$this->current_study][StudyFields::STUDY_CONFIGURATION][StudyConfigurationFields::VALID_EYE];

   }

   function getAvailableDataSetsForSelectedStudy(){
      return $this->available_data_sets;
   }

   function getDataSetTypeFromTrial($data_set_type, $trial_index){

      if ($this->current_study == ""){
         $this->error = "Current study has not been set";
         return false;
      }

      if ($trial_index >= $this->current_number_of_trials){
         $this->error = "Out of bonds trial index $trial_index. The maximum number of trials is " . $this->current_number_of_trials;
         return array();
      }

      if (!in_array($data_set_type,$this->available_data_sets)){
         $this->error = "$data_set_type is not an available data set. Available data sets are: " . implode(",",$this->available_data_sets);
         return array();
      }

      return $this->data[MainFields::STUDIES][$this->current_study][StudyFields::TRIAL_LIST][$this->current_trial_list_type][$trial_index][TrialFields::DATA][$data_set_type][DataSetFields::RAW_DATA];
   }

   function getTrial($trial_index){

      if ($this->current_study == ""){
         $this->error = "Current study has not been set";
         return false;
      }

      if ($trial_index >= $this->current_number_of_trials){
         $this->error = "Out of bonds trial index $trial_index. The maximum number of trials is " . $this->current_number_of_trials;
         return array();
      }

      return $this->data[MainFields::STUDIES][$this->current_study][StudyFields::TRIAL_LIST][$this->current_trial_list_type][$trial_index];
   }   

}

?> 
