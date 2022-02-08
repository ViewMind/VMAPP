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
      const PDF_WORK_DIR                = "pdf_work_dir";
      const PDF_GEN_DIR                 = "pdf_generator";
   }

   abstract class GlobalConfigUpdateResources extends TypedString {
      const GROUP_NAME                 = "update_resources";
      const UPDATE_REPO                = "update_repo";
   }
 
   abstract class GlobalConfigS3 extends TypedString {
      const GROUP_NAME                = "S3";
      const PROFILE                   = "profile";
      const BUCKET                    = "bucket";
      const ECHO_ONLY                 = "echo_only";      
   }

   abstract class HeaderFields extends TypedString {
      const AUTH_TYPE                   = "authtype";
      const AUTHORIZATION               = "authorization";
      const AUTHENTICATION              = "authentication";
      const SIGNATURE                   = "signature";
      const LOGIN_TYPE                  = "logintype";
   }

   abstract class LoginMasks extends TypedString {
      const WEB_PORTAL                 = 1;
      const DISTRIBUTION_SITE          = 2;
      const ADMIN_SITE                 = 4;

      static function canLoginToPortal($role){
         return self::canLogin($role,self::WEB_PORTAL);
      }

      static function canLoginToAdminSite($role){
         return self::canLogin($role,self::ADMIN_SITE);
      }

      static function canLoginToDistribution($role){
         return self::canLogin($role,self::DISTRIBUTION_SITE);
      }

      private static function canLogin($role,$mask){
         return (($role & $mask) === $mask);
      }

   }
  
   abstract class AuthValues extends TypedString {
      const VMCLIENT                    = "VMClient";
      const VMPARTNER                   = "VMPartner";
      const VMLOGIN                     = "Login";
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
      const MEDICS              = "medics";
      const PROC_PARAMS         = "proc_params";
      const FREQ_PARAMS         = "freq_params";
      const UPDATE_ET_CHANGE    = "update_et_change";
      const UPDATE_VERSION      = "update_version";
      const RECOVERY_PASSWORD   = "recovery_password";
   }

   abstract class APIEndpoints extends TypedString {
      const PORTAL_USERS           = "portal_users";
      const REPORTS                = "reports";
      const RAWDATA                = "rawdata";
      const INSTITUTION            = "institution";
      const SUBJECTS               = "subjects";
      const MEDICAL_RECORDS        = "medical_record";
      const INSTANCES              = "instances";
   }

   abstract class PortalUserOperations extends TypedString {
      const GETALLMEDICAL           = "getallmedical";
      const OPERATING_INFO          = "operating_information";
      const MODIFY_OWN              = "modify_own";
      const ENABLE                  = "enable";
      const ADD_NO_LOG_PUSERS       = "addnologpusers";
      const LOGOUT                  = "logout";
   }

   abstract class FrontEndPermissions extends TypedString {
      const SEE_ALL_OWN_INSTITUTION = "see_all_own_institution";
      const LOAD_MEDICAL_RECORDS    = "load_medical_records";
      const ADMIN_DASHBOARD         = "admin_dashboard";
   }

   abstract class URLParameterNames extends TypedString {
      const PPKEY                   = "ppkey";
      const INSTANCE                = "instance";
      const ENABLE_TOKEN            = "enable_token";
      const PORTAL_USER             = "pp";
      const VERSION                 = "version";
      const REGION                  = "region";
      const LANGUAGE                = "lang";
      const SEARCH_CRITERIA         = "search_criteria";
      const FROM                    = "from";
      const TO                      = "to";
      const SEARCH                  = "search";
      const ROLE                    = "role";
      const DISCARDED               = "discarded";
      const INSTITUTION             = "institution";
      const EYETRACKER_KEY          = "et-key";
   }

   abstract class EndpointBodyActions extends TypedString {
      const CREATE                  = "create";
      const GET                     = "get";
      const SET                     = "set";
      const LIST                    = "list";
      const LINK                    = "link";
   }

   abstract class URLSearchCriteria extends TypedString {
      const VMID                    = "vmid";
      const IUID                    = "iuid";
   }

   abstract class InstitutionOperations extends TypedString {
      const LIST                   = "list";
      const OPR_INFO               = "operating_information";
      const GET_UPDATE             = "getupdate";
   }

   abstract class MedRecordsOperations extends TypedString {
      const MODIFY                 = "modify";
      const LIST                   = "list";
      const GET                    = "get";
   }

   abstract class ReportOperations extends TypedString {
      const GENERATE               = "generate";
      const LIST                   = "list";
      const LIST_ALL_OWN_INST      = "list_all_own_institution";
      const GET                    = "get";
      const GET_OWN_INSTITUTION    = "get_own_institution";
      const REPORTLIST             = "reportlist";
      const INST_PDF               = "institutionPDF";
      const ADMIN_LIST             = "admin_evaluation_list";
   }

   abstract class SubjectOperations extends TypedString {
      const LIST                   = "list";
      const LIST_ALL_OWN_INST      = "list_all_own_institution";
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

   abstract class EyeTrackerCodes extends TypedString {
      const HP_OMNICEPT      = "hpomnicept";
      const HTC_VIVE_EYE_PRO = "htcviveeyepro";
      const GAZEPOINT        = "gazepoint";
   }

   // Fields that need to compliment other defined structures. 
   abstract class ComplimentaryDataFields extends TypedString {
      const ASSOCIATED_INSTITUTIONS = "associated_institutions";
      const PERMISSIONS = "permissions";
   }

   abstract class FinalizedStudyNames extends TypedString {
      // This is the the ONLY one we need to refer by name in the backend. So the otheres are not added. 
      const BINDING_BEHAVIOURAL_RESPONSE = "behavioural_response";
   }
   

?>