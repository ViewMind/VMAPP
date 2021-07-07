<?php

include_once ("DataContainerNames.php");

class ViewMindDataContainer {
   
   private $error;
   private $data;
   private $available_studies;

   private $current_study;
   private $current_number_of_trials;
   private $available_data_sets;
   private $file_location;

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
         $this->error = "Could not decode $raw_data_file. JSON Error: " . json_last_error_msg();
         return false;
      }

      $skip = ["FINALIZED_RESULTS"];
      $main_fields = MainFields::getConstList($skip);

      foreach ($main_fields as $main_field){
         if (!array_key_exists($main_field,$this->data)){
            $this->error = "No $main_field field in data";
            return false;
         }   
      }

      $this->available_studies = array_keys($this->data[MainFields::STUDIES]);
      $this->file_location = $raw_data_file;

      return true;
   }

   function save(){
      // The data is hashed before saving. 
      $this->data[MainFields::CHECKSUM] = $this->computeHash();
      $fid = fopen($this->file_location,"w+");
      if ($fid === false) return false;
      fwrite($fid,json_encode($this->data,JSON_PRETTY_PRINT));
      fclose($fid);
      return true;
   }

   function getAvailableStudies(){
      return $this->available_studies;
   }

   function getSubjectDataValue($subject_field){
      if (!SubjectField::validate($subject_field)){
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

   function getAllSubjectData(){
      if (!array_key_exists(MainFields::SUBJECT,$this->data)){
         $this->error = "The main field subject does not exist";
         return array();
      }
      return $this->data[MainFields::SUBJECT];
   }

   function getMetaDataField($metada_field){
      if (!MetadataField::validate($metada_field)){
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

   function getStudyField($field){

      if (!StudyField::validate($field)){
         $this->error = "$field is an invalid study field field";
         return "";
      }

      if (!array_key_exists($field,$this->data[MainFields::STUDIES][$this->current_study])){
         $this->error = "$field does not exist for study " . $this->current_study;
         return "";
      }

      if ($this->current_study == ""){
         $this->error = "Current study has not been set";
         return false;
      }
      return $this->data[MainFields::STUDIES][$this->current_study][$field];
   }

   function getNumberOfTrials() {
      return $this->current_number_of_trials;
   }

   function verifyChecksum(){
      if (!array_key_exists(MainFields::CHECKSUM,$this->data)) {
         $this->error = "No checksum field";
         return false;
      }      
      
      // The current hash needs to be stored as the computation deletes it. 
      $hash = $this->data[MainFields::CHECKSUM];
      
      $computed_hash = $this->computeHash();
      
      // Previous hash is restored. 
      $this->data[MainFields::CHECKSUM] = $hash;
      
      if (!hash_equals($hash,$computed_hash)){
         $this->error = "Computed hash $computed_hash and verification $hash hash differ";
         return false;
      }
      return true;
   }

   private function computeHash(){
      $this->data[MainFields::CHECKSUM] = "*";
      return hash("sha3-512",json_encode($this->data));
   }

   function getProcessingParameters(){
      if (array_key_exists(MainFields::PROCESSING_PARAMETERS,$this->data)){
         return $this->data[MainFields::PROCESSING_PARAMETERS];
      }
      else return array();
   }

   function setRawDataAccessPathForStudy($study){

      $this->current_study = "";

      if (!in_array($study,$this->available_studies)){
         $this->error = "$study is not an available studies. Available studies are: " . implode(",",$this->available_studies);
         return false;
      }

      if (!array_key_exists(StudyField::TRIAL_LIST,$this->data[MainFields::STUDIES][$study])){
         $this->error = "Unable to find the trial list field for the selected study of $study";
         return false;
      }

      $this->current_number_of_trials = count($this->data[MainFields::STUDIES][$study][StudyField::TRIAL_LIST]);

      if ($this->current_number_of_trials > 0){
         if (!array_key_exists(TrialField::DATA,$this->data[MainFields::STUDIES][$study][StudyField::TRIAL_LIST][0])){
            $this->error = "Unable to find the data field of the first trial for the $study";
            return false;
         }             
      }

      $this->current_study = $study;

      // This does assume that ALL trials in the list contains the same DataSets (as the ones in the first one) which SHOULD always be the case. 
      $this->available_data_sets = array_keys($this->data[MainFields::STUDIES][$this->current_study][StudyField::TRIAL_LIST][0][TrialField::DATA]);
      //var_dump($this->available_data_sets);

      return true;
   }

   function getValidEyeForCurrentStudy(){

      if ($this->current_study == ""){
         $this->error = "Current study has not been set";
         return false;
      }
      
      if (!array_key_exists(StudyField::STUDY_CONFIGURATION,$this->data[MainFields::STUDIES][$this->current_study])){
         $this->error = "Study " . $this->current_study . " does not have a study configuration field";
         return "";
      }

      if (!array_key_exists(StudyParameter::VALID_EYE,$this->data[MainFields::STUDIES][$this->current_study][StudyField::STUDY_CONFIGURATION])){
         $this->error = "Configuration for study " . $this->current_study . " does not have a valid eye field";
         return "";
      }

      return $this->data[MainFields::STUDIES][$this->current_study][StudyField::STUDY_CONFIGURATION][StudyParameter::VALID_EYE];

   }

   function getAppUser($user){
      if (!AppUserType::validate($user)){
         $this->error = "App Type user is invalid: $user";
         return false;
      }
      if (!array_key_exists($user,$this->data[MainFields::APPLICATION_USER])){
         $this->error = "Missing application user main field in data";
         return false;
      }
      return $this->data[MainFields::APPLICATION_USER][$user];
   }

   function setFinalizedResults($results){
      $this->data[MainFields::FINALIZED_RESULTS] = $results;
   }

   function getFinalizedResultString(){
      if (!array_key_exists(MainFields::FINALIZED_RESULTS,$this->data)) return "";
      return json_encode($this->data[MainFields::FINALIZED_RESULTS]);
   }

   function getInsertableQualityControlParametersString(){
      if (array_key_exists(MainFields::QC_PARAMETERS,$this->data)){
         return json_encode($this->data[MainFields::QC_PARAMETERS]);
      }
      return "";
   }

   function getInsertableQualityControlGraphValuesString(){
      $ans = array();
      foreach ($this->available_studies as $study){
         $ans[$study] = array();
         if (!array_key_exists(StudyField::QUALITY_CONTROL,$this->data[MainFields::STUDIES][$study])) continue;
         $ans[$study] = $this->data[MainFields::STUDIES][$study][StudyField::QUALITY_CONTROL];
      }
      return json_encode($ans);
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

      return $this->data[MainFields::STUDIES][$this->current_study][StudyField::TRIAL_LIST][$trial_index][TrialField::DATA][$data_set_type][DataSetField::RAW_DATA];
   }

   function getTrialList(){
      if ($this->current_study == ""){
         $this->error = "Current study has not been set";
         return false;
      }
      return $this->data[MainFields::STUDIES][$this->current_study][StudyField::TRIAL_LIST];
   }

   function setTrialList($trial_list){
      if ($this->current_study == ""){
         $this->error = "Current study has not been set";
         return false;
      }      
      $this->data[MainFields::STUDIES][$this->current_study][StudyField::TRIAL_LIST] = $trial_list;
      return true;
   }

   function setStudyComputedValues($study_values){
      if ($this->current_study == ""){
         $this->error = "Current study has not been set";
         return false;
      }
      // No check is done as this function should only be called after processing is done.       

      $expected_study_fields = [StudyField::DURATION, StudyField::BINDING_SCORE];

      foreach ($study_values as $study_field_name => $value) {
         if (!in_array($study_field_name,$expected_study_fields)){
            $this->error = "Study Field $study_field_name is not allowed to be set";
            return false;
         }
         $this->data[MainFields::STUDIES][$this->current_study][$study_field_name] = $value;
      }
      return true;      
   }

   function getTrial($trial_index){

      if ($this->current_study == ""){
         $this->error = "Current study has not been set";
         return false;
      }

      if ($trial_index >= $this->current_number_of_trials){
         $this->error = "Out of bonds trial index $trial_index. The maximum number of trials is " . $this->current_number_of_trials;
         return false;
      }

      return $this->data[MainFields::STUDIES][$this->current_study][StudyField::TRIAL_LIST][$trial_index];
   }   

}

?> 
