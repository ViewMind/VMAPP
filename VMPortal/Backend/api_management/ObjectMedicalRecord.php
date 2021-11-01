<?php


include_once ("ObjectBaseClass.php");
include_once (__DIR__ . "/../db_management/DBCon.php");
include_once (__DIR__ . "/../db_management/TableMedicalRecords.php");

class ObjectMedicalRecord extends ObjectBaseClass {

   function __construct($service,$headers){
      parent::__construct($service,$headers);
   }

   function modify($identifier,$parameters){

      // The portal user that requests the reports for this subject. 
      $auth = $this->headers[HeaderFields::AUTHORIZATION];
      $auth = explode(":",$auth);
      $portal_user = $auth[0];         

      $record = $this->json_data;
      $institution_id = $record[TableMedicalRecords::COL_INSTITUTION_ID];

      // Verifying that the requested modification is for an institution to which the user is enabled. 
      if (!in_array($institution_id,$this->portal_user_info[ComplimentaryDataFields::ASSOCIATED_INSTITUTIONS])){
         $this->suggested_http_code = 403;
         $this->error = "Portal user $portal_user requested modification for medical record $identifier which belongs to institution $institution_id but it's only enabled for " . implode(",",$this->portal_user_info[ComplimentaryDataFields::ASSOCIATED_INSTITUTIONS]);
         $this->returnable_error = "Institution authentication error";
         return false;
      } 
      
      // Making sure the portal user is saved as the last modification
      $record[TableMedicalRecords::COL_LAST_MOD_BY] = $portal_user;

      $tmr = new TableMedicalRecords($this->con_secure);

      // We need to check if the record is new or not
      if ($identifier == 0){
         // This a new record.
         $ans = $tmr->newMedicalRecord($record);
         if ($ans === false){
            $this->suggested_http_code = 500;
            $this->error = "Portal user $portal_user requested creation for medical record for institution $institution_id but it failed. Reason: " . $tmr->getError();
            $this->returnable_error = "Institution authentication error";
            return false;            
         }
      }
      else{

         // The identifier should coincide with the viewmind id, so it's forced.
         $record[TableMedicalRecords::COL_VIEWMIND_ID] = $identifier;
         $ans = $tmr->updateMedicalRecord($record);
         if ($ans === false){
            $this->suggested_http_code = 500;
            $this->error = "Portal user $portal_user requested modification for medical record $identifier which belongs to institution $institution_id but it failed. Reason: " . $tmr->getError();
            $this->returnable_error = "Institution authentication error";
            return false;            
         }
      }

      // No issues, so we return an empty array. 
      return array();


   }

   function list($identifier,$parameters){

      // The portal user that requests the reports for this subject. 
      $auth = $this->headers[HeaderFields::AUTHORIZATION];
      $auth = explode(":",$auth);
      $portal_user = $auth[0];      

      // Verifying that the requested modification is for an institution to which the user is enabled. 
      if (!in_array($identifier,$this->portal_user_info[ComplimentaryDataFields::ASSOCIATED_INSTITUTIONS])){
         $this->suggested_http_code = 403;
         $this->error = "Portal user $portal_user requested medical record list for institution $identifier but it's only enabled for " . implode(",",$this->portal_user_info[ComplimentaryDataFields::ASSOCIATED_INSTITUTIONS]);
         $this->returnable_error = "Institution authentication error";
         return false;
      }     

      $tmr = new TableMedicalRecords($this->con_secure);
      $list = $tmr->getMedicalRecordsForInstitution($identifier);
      if ($list === false){
         $this->suggested_http_code = 500;
         $this->error = "Portal user $portal_user requested medical record list for institution $identifier but it failed. Reason: " . $tmr->getError();
         $this->returnable_error = "Institution authentication error";
         return false;            
      }
      
      return $list;

   }

   function get($identifier,$parameters){

      // The portal user that requests the reports for this subject. 
      $auth = $this->headers[HeaderFields::AUTHORIZATION];
      $auth = explode(":",$auth);
      $portal_user = $auth[0];      

      $institution_id = "";
      $record = array();

      if ($identifier == 0){
         // This is getting the information for a new medical records. Which means that the institution should be a paraemter. 
         if (!array_key_exists(URLParameterNames::INSTITUTION,$parameters)){
            $this->suggested_http_code = 401;
            $this->error = "Portal user $portal_user requested a new medical record with no institution parameter";
            $this->returnable_error = "Missing endpoint information";
            return false;   
         }
         $institution_id = $parameters[URLParameterNames::INSTITUTION];
      }

      if ($identifier != 0){

         $tmr = new TableMedicalRecords($this->con_secure);
         $record = $tmr->getSubjectMedicalRecord($identifier);
         if ($record === false){
            $this->suggested_http_code = 500;
            $this->error = "Portal user $portal_user requested the medical record for $identifier but it failed. Reason: " + $tmr->getError();
            $this->returnable_error = "Internal database error";
            return false;   
         }

         if (count($record) != 1){
            $this->suggested_http_code = 500;
            $this->error = "Portal user $portal_user requested the medical record for $identifier but found " . count($record) . " records instead of 1";
            $this->returnable_error = "Internal database error";
            return false;   
         }

         $record = $record[0];

         // The institution is set to the medical record institution. 
         $institution_id = $record[TableMedicalRecords::COL_INSTITUTION_ID];

      }

      // Verifying that the requested record is from a valid institution.
      if (!in_array($institution_id,$this->portal_user_info[ComplimentaryDataFields::ASSOCIATED_INSTITUTIONS])){
         $this->suggested_http_code = 403;
         $this->error = "Portal user $portal_user requested medical record $identifier which belongs to institution $institution_id but it's only enabled for " . implode(",",$this->portal_user_info[ComplimentaryDataFields::ASSOCIATED_INSTITUTIONS]);
         $this->returnable_error = "Institution authentication error";
         return false;
      }

      // Finally we get all the patients for a given institution.
      $tsu = new TableSubject($this->con_secure);
      $ans = $tsu->getAllSubjectsInInstitution($identifier);
      if ($ans === false){
         $this->suggested_http_code = 500;
         $this->error = "Failed in getting all subjects for institution $identifier";
         $this->returnable_error = "Internal database error";
         return false;         
      }

      $ret["record"] = $record;
      $ret["subject_list"] = $ans;

      return $ret;

   }


}

?>