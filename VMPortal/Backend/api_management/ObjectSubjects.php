<?php

include_once ("ObjectBaseClass.php");
include_once (__DIR__ . "/../db_management/DBCon.php");
include_once (__DIR__ . "/../db_management/TableEvaluations.php");
include_once (__DIR__ . "/../db_management/TableSubject.php");

class ObjectSubjects extends ObjectBaseClass{


   function __construct($service,$headers){
      parent::__construct($service,$headers);
   }

   /**
    * Lists all the subjects for a portal user (which has logged on)
    */

   function list($identifier,$parameters){
      return $this->getSubjectList($identifier,false);
   }

   /**
    * Lists all the subjects for the institution of a portal user (which has logged on)
    */

   function list_all_own_institution($identifier,$parameter){
      return $this->getSubjectList($identifier,true);
   }

   private function getSubjectList($institution,$all){
      if ($institution == ""){
         $this->suggested_http_code = 401;
         $this->error = "Empty institution identifier.";
         return false;
      }

      // The portal user that requests the reports for this subject. 
      $auth = $this->headers[HeaderFields::AUTHORIZATION];
      $auth = explode(":",$auth);
      $portal_user = $auth[0];

      // Create all table objects. 
      $tev = new TableEvaluations($this->con_main);
      $tsu = new TableSubject($this->con_secure);

      // Verifying that the requested institution is a valid one
      if (!in_array($institution,$this->portal_user_info[ComplimentaryDataFields::ASSOCIATED_INSTITUTIONS])){
         $this->suggested_http_code = 403;
         $this->error = "Portal user $portal_user attempted to get subject list for institution $institution but it's only enabled for " . implode(",",$this->portal_user_info[ComplimentaryDataFields::ASSOCIATED_INSTITUTIONS]);
         $this->returnable_error = "Instituion authentication error";
         return false;
      }

      if ($all)  $ans = $tev->getAllSubjectsWithEvaluationsForInstitutionIDAndPortalUser($institution,"");
      else $ans = $tev->getAllSubjectsWithEvaluationsForInstitutionIDAndPortalUser($institution,$portal_user);
      
      if ($ans === FALSE){
         $this->suggested_http_code = 500;
         $this->error = "Failed getting all subject with evaluations for $institution and portal user $portal_user: " . $tev->getError();
         $this->returnable_error = "Internal database error";
         return false;
      }

      // There are no subjects for this portal user in this institution. 
      if (empty($ans)){
         return array();
      }

      // The subjcts are added to a list. 
      $subjects  = array();
      foreach ($ans as $sub){
         $subjects[] = $sub[TableEvaluations::COL_SUBJECT_ID];
      }

      // And now we get the information for them.
      $ans = $tsu->getAllSubjectsFromList($subjects);
      if ($ans === FALSE){
         $this->suggested_http_code = 500;
         $this->error = "Failed to get all subjects from a list: " . $tsu->getError();
         $this->returnable_error = "Internal database error";
         return false;
      }
      
      if (empty($ans)){
         $this->suggested_http_code = 500;
         $this->error = "Got an empty array when attempting to get information for the patients of portal user $portal_user on institution $institution";
         $this->returnable_error = "Internal database error";
         return false;
      }

      return $ans;            
   }


}

?>