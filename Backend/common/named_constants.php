<?php

   include_once ("TypedString.php");

   /**
    * All parameter classes (start name with Parameter) require a NAME constant and a 
    * REQUIRED constant array, even if it's empty.     
    */

   abstract class ParameterGeneral extends TypedString{

      const NAME              = "general_config";

      // The list of required parameters. 
      const REQUIRED          = [];
   }

   abstract class ValueGeneral extends TypedString {
   }

   abstract class HeaderFields extends TypedString {
      const AUTH_TYPE            = "AuthType";
      const AUTHORIZATION        = "Authorization";
      const AUTHENTICATION       = "Authentication";
      const SIGNATURE            = "Signature";
   }
  
   abstract class AuthValues extends TypedString {
      const VMCLIENT             = "VMClient";
      const VMPARTNER            = "VMPartner";
      const BASIC                = "Basic";
   }  

   abstract class POSTFields extends TypedString{
      const INSTITUTION_ID       = "institution_id";
      const INSTITUION_INSTANCE  = "institution_instance";
   }

   abstract class ResponseFields extends TypedString {
      const MESSAGE       = "message";
      const CODE          = "code";
      const HTTP_CODE     = "http_code";    
      const DATA          = "data";
   }

   abstract class ResponseDataSubFields extends TypedString {
      const MEDICS        = "medics";
      const PROC_PARAMS   = "proc_params";
      const FREQ_PARAMS   = "freq_params";
   }

   abstract class APIEndpoints extends TypedString {
      const PORTAL_USERS           = "portal_users";
      const REPORTS                = "reports";
      const RAWDATA                = "rawdata";
      const INSTITUTION            = "institution";
   }

   abstract class PortalUserOperations extends TypedString {
      const GETALLMEDICAL           = "getallmedical";
      const OPERATING_INFO          = "operating_information";
   }

   abstract class URLParameterNames extends TypedString {
      const PPKEY                   = "ppkey";
   }

   abstract class InstitutionOperations extends TypedString {
      const LIST                   = "list";
      const OPR_INFO               = "operating_information";
   }

   abstract class ReportOperations extends TypedString {
      const GENERATE               = "generate";
      const LIST                   = "list";
      const GET                    = "get";
   }

   

?>