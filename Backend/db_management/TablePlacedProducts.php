<?php

include_once ("DBCon.php");
include_once ("TableBaseClass.php");

class TablePlacedProducts extends TableBaseClass {
   
   const TABLE_NAME                = "placed_products";
   const IN_DB                     = DBCon::DB_SECURE;

   const COL_PRODUCT_ID            = "product_id";
   const COL_PRODUCT_DESCRIPTION   = "product_description";
   const COL_INSTITUTION_ID        = "institution_id";
   const COL_INSTITUTION_INSTANCE  = "institution_instance";
   const COL_ENABLED               = "enabled";
   const COL_STATUS                = "status";
   const COL_ENABLE_TOKEN          = "enable_token";

   function __construct($con){
      parent::__construct($con);
   }

}
?>