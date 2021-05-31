<?php

include_once ("TableBaseClass.php");
include_once ("DBCon.php");

class TableSecrets extends TableBaseClass {

   const TABLE_NAME                = "secrets";
   const IN_DB                     = DBCon::DB_SECURE;
   
   const COL_SECRET_KEY            = "secret_key";
   const COL_INSTITUTION_ID        = "institution_id";
   const COL_INSTITUTION_INSTANCE  = "institution_instance";
   const COL_SECRET                = "secret";
   const COL_ENABLED               = "enabled";

   function __construct($con){
      parent::__construct($con);
   }

}
?>