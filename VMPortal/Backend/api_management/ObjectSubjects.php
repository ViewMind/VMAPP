<?php

include_once ("ObjectBaseClass.php");
include_once (__DIR__ . "/../db_management/DBCon.php");
include_once (__DIR__ . "/../db_management/TableEvaluations.php");
include_once (__DIR__ . "/../db_management/TableSubject.php");

class ObjectSubjects extends ObjectBaseClass{


   function __construct($service,$headers){
      parent::__construct($service,$headers);
   }

   function list($identifier,$parameters){

      if ($identifier == ""){
         $this->suggested_http_code = 401;
         $this->error = "Empty institution identifier.";
         return false;
      }

      // The portal user that requests the reports for this subject. 
      $auth = $this->headers[HeaderFields::AUTHORIZATION];
      $auth = explode(":",$auth);
      $portal_user = $auth[0];

      // Finding the role of the user.
      $role = $this->portal_user_info[TablePortalUsers::COL_USER_ROLE];

      // Create all table objects. 
      $tev = new TableEvaluations($this->con_main);
      $tsu = new TableSubject($this->con_secure);
      //$tpu = new TablePortalUsers($this->con_secure);

      if ($role == TablePortalUsers::ROLE_INTITUTION_ADMIN){         
         $ans = $tev->getAllSubjectsWithEvaluationsForInstitutionIDAndPortalUser($identifier,"");
      }
      else{
         $ans = $tev->getAllSubjectsWithEvaluationsForInstitutionIDAndPortalUser($identifier,$portal_user);         
      }
      
      if ($ans === FALSE){
         $this->suggested_http_code = 500;
         $this->error = "Failed getting all subject with evaluations for $identifier and portal user $portal_user whose role is $role: " . $tev->getError();
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
         $this->error = "Got an empty array when attempting to get information for the patients of portal user $portal_user on institution $identifier";
         $this->returnable_error = "Internal database error";
         return false;
      }

      return $ans;

   }


}

?>