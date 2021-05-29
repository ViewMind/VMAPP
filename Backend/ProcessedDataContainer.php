<?php

include_once ("DataContainerNames.php");

class ProcessedDataContainer {

   private $data;
   private $available_studies;
   private $error;
   private $current_study;
   private $current_trial_list_type;
   private $number_of_trials;

   function __construct(){
      $this->error = "";      
      $this->data[MainFields::METADATA] = array();
      $this->data[MainFields::METADATA][MetadaFields::SUBJECT_IDENTIFIER] = "";
      $this->data[MainFields::METADATA][MetadaFields::SUBJECT_AGE] = "";      
      $this->data[MainFields::STUDIES] = array();
      $this->available_studies = array();
      $this->current_study = "";
      $this->current_trial_list_type = "";
   }

   function getError(){
      return $this->error;
   }

   function setSubjectData($sub_id,$subject_age){
      $this->data[MainFields::METADATA][MetadaFields::SUBJECT_IDENTIFIER] = $sub_id;
      $this->data[MainFields::METADATA][MetadaFields::SUBJECT_AGE] = $subject_age;
   }

   function createNewStudy($study,$trial_list_type, $valid_eye){

      if (!EyeType::validate($valid_eye)){
         $this->error = "Invalid eye parameter $valid_eye";
         return false;
      }      

      if (!StudyTypes::validate($study)){
         $this->error = "$study is not a valid study type. Valid types are : " + StudyTypes::getConstValueList();
         return false;
      }

      if (!TrialListTypes::validate($trial_list_type)){
         $this->error = "$study is not a valid trial list type. Valid types are : " + TrialListTypes::getConstValueList();
         return false;
      }

      $date = new DateTime();
      $pdate = $date->format('Y_m_d');
      $time = $date->format('_H_i'); 

      $this->data[MainFields::METADATA][MetadaFields::DATE] = $pdate;
      $this->data[MainFields::METADATA][MetadaFields::HOUR] = $time;

      // If the study or the trial_list_type exist. 
      if (!array_key_exists($study,$this->data[MainFields::STUDIES])){
         $this->data[MainFields::STUDIES][$study] = array();
         $this->data[MainFields::STUDIES][$study][StudyFields::FINALIZED_RESULTS] = array();
         $this->data[MainFields::STUDIES][$study][StudyFields::TRIAL_LIST] = array();
         $this->data[MainFields::STUDIES][$study][StudyFields::VALID_EYE] = $valid_eye;
      }      
      
      if (!array_key_exists($trial_list_type,$this->data[MainFields::STUDIES][$study][StudyFields::TRIAL_LIST])){
         $this->data[MainFields::STUDIES][$study][StudyFields::TRIAL_LIST][$trial_list_type] = array();
         $this->data[MainFields::STUDIES][$study][StudyFields::TRIAL_LIST][$trial_list_type] = array();
         $this->data[MainFields::STUDIES][$study][StudyFields::TRIAL_LIST][$trial_list_type][TrialListTypeFields::DATA] = array();
         $this->data[MainFields::STUDIES][$study][StudyFields::TRIAL_LIST][$trial_list_type][TrialListTypeFields::LIST] = array();
      }      

      $this->available_studies = array_keys($this->data[MainFields::STUDIES]);

      return true;
   }


   function getNumberOfTrials(){
      return $this->number_of_trials;
   }


   function setTrialListData($trial_list_data){
      if ($this->current_study == ""){
         $this->error = "Current study has not been set";
         return false;
      }
            
      $this->data[MainFields::STUDIES][$this->current_study][StudyFields::TRIAL_LIST][$this->current_trial_list_type][TrialListTypeFields::DATA] = $trial_list_data;
      return true;
   }

   function getTrialListData($trial_list_data_field){
      if ($this->current_study == ""){
         $this->error = "Current study has not been set";
         return NULL;
      }

      if (!array_key_exists($trial_list_data_field,$this->data[MainFields::STUDIES][$this->current_study][StudyFields::TRIAL_LIST][$this->current_trial_list_type][TrialListTypeFields::DATA])){
         $this->error = "No $trial_list_data_field found in the TrialListType Data";
         return NULL;         
      }

      return $this->data[MainFields::STUDIES][$this->current_study][StudyFields::TRIAL_LIST][$this->current_trial_list_type][TrialListTypeFields::DATA][$trial_list_data_field];

   }

   function getSubjectID(){
      return $this->data[MainFields::METADATA][MetadaFields::SUBJECT_IDENTIFIER];
   }   

   function getSubjectAge(){
      return $this->data[MainFields::METADATA][MetadaFields::SUBJECT_AGE];
   }   


   function setActiveStudyAndTrialListType($study, $trial_list_type){


      $this->current_study = "";
      $this->current_trial_list_type = "";

      
      if (!in_array($study,$this->available_studies)){
         $this->error = "$study is not an available study. Available studies are: " . implode(",",$this->available_studies);
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
      
      if (!array_key_exists(TrialListTypeFields::LIST, $this->data[MainFields::STUDIES][$study][StudyFields::TRIAL_LIST][$trial_list_type])){
         $this->error = "Unable to find the list field for the $trial_list_type for the selected study of $study";
         return false;         
      }

      $this->current_study = $study;
      $this->current_trial_list_type = $trial_list_type;
      $this->number_of_trials = count($this->data[MainFields::STUDIES][$study][StudyFields::TRIAL_LIST][$trial_list_type][TrialListTypeFields::LIST]);

      return true;      
   }

   function getValidEyeForStudy(){      
      if ($this->current_study == ""){
         $this->error = "Current study has not been set";
         return "";
      }      
      return $this->data[MainFields::STUDIES][$this->current_study][StudyFields::VALID_EYE];
   }

   
   function addTrial($trial,$trial_values){

      if ($this->current_study == ""){
         $this->error = "Current study has not been set";
         return -1;
      }
      
      // Check that at least ID, response and data are fields. 
      $mandatory_trial_fields = [TrialFields::ID, TrialFields::RESPONSE, TrialFields::DATA ];
      foreach ($mandatory_trial_fields as $key){
         if (!array_key_exists($key, $trial)) {
            $this->error = "Trial requires $key field";
            return -1;
         }
      }

      $all_data_set_types = array_keys($trial[TrialFields::DATA]);

      foreach ($all_data_set_types as $data_set_type){
         // Clearing the current data set type subfield
         $trial[TrialFields::DATA][$data_set_type] = array(); 
      }

      // Setting the trial based values.
      $trial[TrialFields::TRIAL_VALUES] = $trial_values;

      // Adding the trial to the global data structure. 
      $ret_index = count($this->data[MainFields::STUDIES][$this->current_study][StudyFields::TRIAL_LIST][$this->current_trial_list_type][TrialListTypeFields::LIST]);
      $this->data[MainFields::STUDIES][$this->current_study][StudyFields::TRIAL_LIST][$this->current_trial_list_type][TrialListTypeFields::LIST][] = $trial;      
      return $ret_index;
   }

   function addDataSetDataToTrial($trial_id, $data_set_computed_data, $fix_l, $fix_r, $data_set_type){

      if ($this->current_study == ""){
         $this->error = "Current study has not been set";
         return false;
      }      

      $n = count($this->data[MainFields::STUDIES][$this->current_study][StudyFields::TRIAL_LIST][$this->current_trial_list_type][TrialListTypeFields::LIST]);
      if ($trial_id >= $n){
         $this->error = "Out of bounds trial index $trial_id. Current trial count is $n";
         return false;
      }

      // The trial is valid so we get it. 
      $trial = $this->data[MainFields::STUDIES][$this->current_study][StudyFields::TRIAL_LIST][$this->current_trial_list_type][TrialListTypeFields::LIST][$trial_id];
      

      if (!array_key_exists($data_set_type,$trial[TrialFields::DATA])){
         $this->error = "Trial does not have a $data_set_type data set type";
         return false;
      }

      // Clearing the current data set type subfield, Data set exists so we set it's values. 
      $trial[TrialFields::DATA][$data_set_type][DataSetFields::FIXATIONS_L] = $fix_l;
      $trial[TrialFields::DATA][$data_set_type][DataSetFields::FIXATIONS_R] = $fix_r;
      $trial[TrialFields::DATA][$data_set_type][DataSetFields::DATA_SET_VALUES] = $data_set_computed_data;

      $this->data[MainFields::STUDIES][$this->current_study][StudyFields::TRIAL_LIST][$this->current_trial_list_type][TrialListTypeFields::LIST][$trial_id] = $trial; 

      return true;
   }


   function getTrial($trial_index){

      if ($this->current_study == ""){
         $this->error = "Current study has not been set";
         return array();
      }      

      if ($trial_index >= $this->number_of_trials){
         $this->error = "Out of bounds trial index of $trial_index as there are only " . $this->number_of_trials;
         return array();
      }

      // If the current study is set all other checks have been done. 
      return  $this->data[MainFields::STUDIES][$this->current_study][StudyFields::TRIAL_LIST][$this->current_trial_list_type][TrialListTypeFields::LIST][$trial_index];

   }

   function getFixationListsFromTrial(&$trial,$data_set_type,$eye){
      if ($eye == EyeType::LEFT){
         $fix_field = DataSetFields::FIXATIONS_L;
      }
      else if ($eye == EyeType::RIGHT){
         $fix_field = DataSetFields::FIXATIONS_R;
      }
      else{
         $this->error = "Invalid eye selection: $eye";
         return array();
      }      
      if (!array_key_exists($data_set_type,$trial[TrialFields::DATA])){
         $this->error = "Trial data does not have a data set type $data_set_type";
         //var_dump($trial);
         return array();
      }
      return $trial[TrialFields::DATA][$data_set_type][$fix_field];
   }


   function getValuesForDataSetFromTrial(&$trial,$data_set_type){     
      if (!array_key_exists($data_set_type,$trial[TrialFields::DATA])){
         $this->error = "Trial does not have a data set type $data_set_type";
         return array();
      }
      return $trial[TrialFields::DATA][$data_set_type][DataSetFields::DATA_SET_VALUES];
   }

}

?>