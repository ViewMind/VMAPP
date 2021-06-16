<?php

   include_once ("TypedString.php");

   /**
    * All parameter classes (start name with Parameter) require a NAME constant and a 
    * REQUIRED constant array, even if it's empty.     
    */

   abstract class GlobalConfigGeneral extends TypedString{
      const GROUP_NAME                  = "general_config";
      const API_PARTS_TO_REMOVE         = "api_route_parts_to_remove";
   }

   abstract class GlobalConfigLogs extends TypedString {
      const GROUP_NAME                  = "logs";
      const PROCESSING_LOG_LOCATION     = "processing_log_location";
      const AUTH_LOG_LOCATION           = "auth_log_location";
      const API_GENERAL_LOG_LOCATION    = "api_logs";
      const ROUTING_LOG_LOCATION        = "routing_log";            
      const BASE_STD_LOG                = "base_std_log";
   }

   abstract class GlobalConfigProcResources extends TypedString {
      const GROUP_NAME                  = "processing_resources";  
      const PROCESSING_WORK_DIRECTORY   = "processing_work_dir";
      const R_SCRIPTS_REPO              = "r_repo";
   }
 
   abstract class HeaderFields extends TypedString {
      const AUTH_TYPE                   = "AuthType";
      const AUTHORIZATION               = "Authorization";
      const AUTHENTICATION              = "Authentication";
      const SIGNATURE                   = "Signature";
   }
  
   abstract class AuthValues extends TypedString {
      const VMCLIENT                    = "VMClient";
      const VMPARTNER                   = "VMPartner";
      const BASIC                       = "Basic";
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
      const INSTANCE                = "instance";
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

   abstract class ReturnableError extends TypedString {
      const DATABASE_QUERY   = "Database query error";      
      const DATABASE_CON     = "Database connection error";
   }

   abstract class FileStructNames extends TypedString {
      const FILE_TO_PROCESS  = "FileToProcess";
      const TMP_LOCATION     = "tmp_name";
      const ACTUAL_FILE_NAME = "name";
      const FILE_SIZE        = "size";
      const ERROR            = "error";
   }

   

?>