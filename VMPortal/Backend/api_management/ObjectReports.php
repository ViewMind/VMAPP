<?php

include_once ("ObjectPortalUsers.php");
include_once (__DIR__ . "/../data_processing/ViewMindDataContainer.php");
include_once (__DIR__ . "/../data_processing/Processing.php");
include_once (__DIR__ . "/../data_processing/CSVGeneration/CSVGoNoGo.php");
include_once (__DIR__ . "/../data_processing/CSVGeneration/CSVReading.php");
include_once (__DIR__ . "/../data_processing/CSVGeneration/CSVBinding.php");
include_once (__DIR__ . "/../data_processing/CSVGeneration/CSVNBackRT.php");
include_once (__DIR__ . "/../data_processing/RProcessing.php");

include_once (__DIR__ . "/../db_management/TableEvaluations.php");
include_once (__DIR__ . "/../db_management/TableSubject.php");
include_once (__DIR__ . "/../db_management/TableInstitutionUsers.php");
include_once (__DIR__ . "/../db_management/TableInstitution.php");


class ObjectReports extends ObjectBaseClass
{

   public function __construct($service,$headers){
     parent::__construct($service,$headers);        
   }

   function generate($institution, $parameters){

      $this->returnable_error = "";

      ////////////// SET UP

      // Set the log for this opertion using the instituntion number and instance. 
      $instance = $_POST[POSTFields::INSTITUION_INSTANCE];
      $date = new DateTime();
      $base_proc_name =$institution ."_" . $instance . "_" . $date->format('Y_m_d_H_i_s'); 
      $log_file_name = $base_proc_name . ".log";
      $logger = new LogManager(CONFIG[GlobalConfigLogs::GROUP_NAME][GlobalConfigLogs::PROCESSING_LOG_LOCATION] . "/" . $log_file_name);
      $logger->logProgress("Processing request from $institution - $instance");

      if (!is_array($parameters)){
         $this->suggested_http_code = 401;
         $this->error = "Bad URL parameters";
         $logger->logError("URL Parameters are not an array");
         return false;         
      }

      // Creating the work directory.
      $workdir = CONFIG[GlobalConfigProcResources::GROUP_NAME][GlobalConfigProcResources::PROCESSING_WORK_DIRECTORY];
      shell_exec("mkdir -p $workdir/$base_proc_name");
      $workdir = "$workdir/$base_proc_name";
      if (!is_dir($workdir)){
         $this->suggested_http_code = 500;
         $this->error = "Processing directory error";
         $logger->logError("Failed in creating the work directory at $workdir");
         return false;
      }

      ////////////// URL verification.

      // First we verify that the parameter and identifier match the post fields. 
      if (!array_key_exists(URLParameterNames::INSTANCE,$parameters)){
         $this->suggested_http_code = 401;
         $this->error = "Missing URL parameters";
         $logger->logError("URL instance parameter is missing");
         return false;
      }
      
      $instance = $parameters[URLParameterNames::INSTANCE];
      if ($parameters[URLParameterNames::INSTANCE] != $instance){
         $this->suggested_http_code = 401;
         $this->error = "URL Instance mistmatch";
         $logger->logError("The URL instance for the accessing was $instance " . $parameters[URLParameterNames::INSTANCE] . " was used as a parameter");
         return false;
      }
      
      if ($_POST[POSTFields::INSTITUTION_ID] != $institution){
         $this->suggested_http_code = 401;
         $this->error = "URL Instance mistmatch";
         $logger->logError("The URL institution for the accessing was " . $_POST[POSTFields::INSTITUTION_ID] . " but $institution was used as an identifier");
         return false;         
      }


      ////////////// FILE UPLOAD AND UNZIPPING

      // Copy the received file to a new directory insider the work directory. 
      if (!array_key_exists(FileStructNames::FILE_TO_PROCESS,$_FILES)){
         $this->suggested_http_code = 401;
         $this->error = "Client error on file uplaod";
         $logger->logError("Could not find the FileToProcess key in FILES structure");
         return false;
      }

      // The current locaton of the uploaded file. 
      $source = $_FILES[FileStructNames::FILE_TO_PROCESS][FileStructNames::TMP_LOCATION];
      $file_name_parts = explode(".",$_FILES[FileStructNames::FILE_TO_PROCESS][FileStructNames::ACTUAL_FILE_NAME]);
      
      $json_filename = $file_name_parts[0] . ".json";
      $base_file_name = $file_name_parts[0];

      $logger->logProgress("Original filename: " . $_FILES[FileStructNames::FILE_TO_PROCESS][FileStructNames::ACTUAL_FILE_NAME]);
      
      $tarfile = "$workdir/" . $_FILES[FileStructNames::FILE_TO_PROCESS][FileStructNames::ACTUAL_FILE_NAME];
      shell_exec("mv $source $tarfile");
      if (!is_file($tarfile)){
         $this->suggested_http_code = 500;
         $this->error = "Failed unzipping file";
         $logger->logError("Could not copy the zip file from $source to $tarfile");
         return false;
      }

      $output = shell_exec("tar -xf $tarfile -C $workdir");
      //$output = shell_exec("tar -xf $tarfile");

      $json_filename = "$workdir/$json_filename";

      // We check that the json file exists. 
      if (!is_file($json_filename)){
         $this->suggested_http_code = 500;
         $this->error = "Failed unzipping file";
         $logger->logError("Could not unzip $tarfile to $json_filename in the same directory: $output");
         return false;
      }

      ////////////// LOADING AND VERIFICATION
      $vmdc = new ViewMindDataContainer();
      if (!$vmdc->loadFromFile($json_filename)){
         $this->suggested_http_code = 500;
         $this->error = "Failed processing file";
         $logger->logError("Failed loading $json_filename. Reason: " . $vmdc->getError());
         return false;
      }

      // Obtaining the medic information. 
      $medic = $vmdc->getAppUser(AppUserType::MEDIC);
      $uid = $medic[AppUserField::VIEWMIND_ID];

      $tiu = new TableInstitutionUsers($this->con_main);
      $institution_users = $tiu->getUsersForInstitution($institution);

      if ($institution_users === FALSE){
         $this->suggested_http_code = 500;
         $this->error = "Failed medic verification";
         $logger->logError("Failed getting the list of users for $institution_users. Reason: " . $vmdc->getError());
         return false;
      }

      $found = false;
      foreach ($institution_users as $row){
         if ($row[TableInstitutionUsers::COL_PORTAL_USER] == $uid){
            $found = true;
            break;
         }
      }

      if (!$found){
         $this->suggested_http_code = 401;
         $this->error = "Failed medic verification";
         $logger->logError("Medic with UID as $uid is not within the list of users for institution $institution");
         return false;         
      }

      ////////////// LOADING AND PROCESSING

      // Creating the processor. 
      $JSONProcessor = new Processing($vmdc->getProcessingParameters());

      // Checking if this a discard file or a process request. 
      $discard_reason = $vmdc->getMetaDataField(MetadataField::DISCARD_REASON,true);
      $should_process = ($discard_reason == "");

      // Processing all studies. 
      $all_studies = $vmdc->getAvailableStudies();
      if (empty($all_studies)){
         $this->suggested_http_code = 500;
         $this->error = "Failed processing file";
         $logger->logError("No studies were found in file " . $json_filename);
         return false;
      }

      $csv_file = array();

      foreach ($all_studies as $study){

         // If this file is stored for later analysis, then processing is moot. It's being discarded. 
         if (!$should_process) continue;
         
         // Setting the study. 
         if ($vmdc->setRawDataAccessPathForStudy($study) === false){
            $this->suggested_http_code = 500;
            $this->error = "Failed processing file";
            $logger->logError("Could not set access to study $study. Reason: " . $vmdc->getError());
            return false;            
         }

         $trial_list = $vmdc->getTrialList();

         if (!$JSONProcessor->processStudy($trial_list,$study)){
            $this->suggested_http_code = 500;
            $this->error = "Failed processing file";
            $logger->logError("Failed to process study $study. Reason: " . $JSONProcessor->getError());
            return false;                
         }

         $vmdc->setTrialList($trial_list);

         if (!$vmdc->setStudyComputedValues($JSONProcessor->getStudyComputedValues())){
            $this->suggested_http_code = 500;
            $this->error = "Failed processing file";
            $logger->logError("Failed setting the computed values for $study. Reason: " . $vmdc->getError());
            return false;                
         }

         // Saving the progress so far ...
         if (!$vmdc->save()){
            $this->suggested_http_code = 500;
            $this->error = "Failed processing file";
            $logger->logError("Failed saving after study $study, from $json_filename");
            return false;                
         }

         $csv_file[$study] = "$workdir/" . strtolower(str_replace(" ","_",$study)) . "_$base_file_name.csv";

      }

      if ($should_process) {

         // Generating the CSV
         $csv_generating_function = Processing::CSV_FUNCTION_MAP[$study];
     
         $csv_generating_error = call_user_func($csv_generating_function, $vmdc, $csv_file);

         if ($csv_generating_error != "") {
             $this->suggested_http_code = 500;
             $this->error = "Failed processing file";
             $logger->logError("Error generating CSV for study $study of file $json_filename. Reason: $csv_generating_error");
             return false;
         }

         // Pass the CSV through the RScript.
         $ans = RProcessing($vmdc, $csv_file, $workdir);
         if ($ans != "") {
             $this->suggested_http_code = 500;
             $this->error = "Failed processing file";
             $logger->logError("Error processing file through R Script. Reason: $ans");
             return false;
         }

         //
         if (!$vmdc->save()) {
             $this->suggested_http_code = 500;
             $this->error = "Failed saving processing file";
             $logger->logError("Failed saving new information into $json_filename");
             return false;
         }

      }
      else {
         $logger->logProgress("Discard reason for file was set to $discard_reason");
      } 

      ////////////// STORING IN DB. 

      // First we store the subject data. 
      $ts = new TableSubject($this->con_secure);

      //echoOut($vmdc->getAllSubjectData(),true);

      $subject = TableSubject::mapSubjectFromVMDCToColumns($vmdc->getAllSubjectData());
      $subject[TableSubject::COL_INSTITUTION_ID] = $institution;
      $subject[TableSubject::COL_INSTANCE_NUMBER] = $instance;

      //echoOut($subject,true);

      if ($ts->addOrUpdateSubject($subject) === FALSE){
         $this->suggested_http_code = 500;
         $this->error = "Failed database process";
         $logger->logError("Error Inserting/Updating subject. Reason: " . $ts->getError());
         return false;          
      }

      // Then we store the evaluation results. So first we figure out the study type.       

      $study_type = "";
      if (count($all_studies) > 1){
         // This is a Multi Part Study. 
         $test_study_name = $all_studies[0];
         foreach (MultiPartStudyBaseName::getConstList() as $multi_part_study){
            if (strpos($test_study_name,$multi_part_study) !== FALSE){
               $study_type = $multi_part_study;
               break;
            }
         }
      }
      else $study_type = $all_studies[0];

      // The RESULTS SHOULD NOT BE STORED IF MOUSE DATA. 
      if ($vmdc->getMetaDataField(MetadataField::MOUSE_USED) == "true"){
         $this->suggested_http_code = 200;
         $logger->logProgress("File was generated using mouse. Skipping DB insertion");
         return true;
      }

      // We get the evaluator data
      $evaluator = $vmdc->getAppUser(AppUserType::EVALUATOR);

      // Generate the file zipped file name. 
      $d = new DateTime();
      $compressed_filename = $d->format("Y_m_d_H_i_s_u") . ".zip";      

      // Set all corresponding fields. 
      $evaluation[TableEvaluations::COL_STUDY_TYPE]  = $study_type;
      $evaluation[TableEvaluations::COL_PORTAL_USER] = $uid;
      $evaluation[TableEvaluations::COL_EVALUATOR_LASTNAME] = $evaluator[AppUserField::LASTNAME];
      $evaluation[TableEvaluations::COL_EVALUATOR_NAME] = $evaluator[AppUserField::NAME];
      $evaluation[TableEvaluations::COL_EVALUATOR_EMAIL] = $evaluator[AppUserField::EMAIL];
      $evaluation[TableEvaluations::COL_INSTITUTION_ID] = $institution;
      $evaluation[TableEvaluations::COL_INSTITUTION_INSTANCE] = $instance;
      $evaluation[TableEvaluations::COL_STUDY_DATE] = $vmdc->getMetaDataField(MetadataField::DATE) . " ". $vmdc->getMetaDataField(MetadataField::HOUR);
      $evaluation[TableEvaluations::COL_SUBJECT_ID] = $subject[TableSubject::COL_UNIQUE_ID];
      $evaluation[TableEvaluations::COL_PROTOCOL] = $vmdc->getMetaDataField(MetadataField::PROTOCOL);
      $evaluation[TableEvaluations::COL_RESULTS] = $vmdc->getFinalizedResultString();
      $evaluation[TableEvaluations::COL_QC_PARAMS] = $vmdc->getQCParametersAsJSONString();
      $evaluation[TableEvaluations::COL_QC_GRAPHS] = $vmdc->getInsertableQualityControlGraphValuesString();
      $evaluation[TableEvaluations::COL_STUDY_CONFIG] = $vmdc->getStudyConfigAsJSONString();
      $evaluation[TableEvaluations::COL_DISCARD_REASON] = $discard_reason;
      $evaluation[TableEvaluations::COL_FILE_LINK] = $compressed_filename;

      $te = new TableEvaluations($this->con_main);
      $ans= $te->addEvaluation($evaluation);

      $evaluation_id = $te->getLastInserted();
      $evaluation_id = $evaluation_id[0];

      if ($ans === FALSE){
         $this->suggested_http_code = 500;
         $this->error = "Failed database process";
         $logger->logError("Error inserting new evaluation. Reason: " . $te->getError());
         return false;             
      }

      // Finally we do the S3 Insertion
      $aws_profile = CONFIG[GlobalConfigS3::GROUP_NAME][GlobalConfigS3::PROFILE];
      $aws_bucket  = CONFIG[GlobalConfigS3::GROUP_NAME][GlobalConfigS3::BUCKET];

      // It is necessary to generate the zip file again. 
      $compress_cmd = "cd $workdir; tar -c -z -f $compressed_filename $json_filename";
      $logger->logProgress("TAR CMD: $compress_cmd");
      $output = shell_exec($compress_cmd);
      
      $zipfile = $workdir . "/" .$compressed_filename;
      if (!is_file($zipfile)){
         $this->suggested_http_code = 500;
         $this->error = "Failed to generate compressed file";
         $logger->logError("Could not create zip file $zipfile from $json_filename.\nCMD Used: $compress_cmd\n OUTPUT:\n $output");
         return false;            
      }

      $aws_command = "aws s3 cp $zipfile s3://$aws_bucket/$compressed_filename 2>&1";
      $logger->logProgress("AWS CP CMD: $aws_command");

      //echoOut(CONFIG[GlobalConfigS3::GROUP_NAME],true);

      if (CONFIG[GlobalConfigS3::GROUP_NAME][GlobalConfigS3::ECHO_ONLY] == TRUE) {
         $logger->logProgress("Echo ONLY MODE. Not doing the S3 Thing");
         $this->suggested_http_code = 200;
         return true;
      }

      $output = shell_exec($aws_command);
      $logger->logProgress("CMD Output: " . $output);
      $this->suggested_http_code = 200;
      return true;

   }

   function list($subject_id, $parameters){
      return $this->listReports($subject_id,true);
   }

   function list_all_own_institution($subject_id,$parameters){
      return $this->listReports($subject_id,false);
   }

   function get($report_id,$parameters){
      return $this->getReport($report_id,$parameters,true);
   }

   function admin_evaluation_list($identifier,$parameters){

      $tev = new TableEvaluations($this->con_main);
      $tpp = new TablePortalUsers($this->con_secure);
      $ti  = new TableInstitution($this->con_main);
      $ts  = new TableSubject($this->con_secure);

      $ans = $tev->listAllEvaluations();
      if ($ans === false){
         $this->suggested_http_code = 500;
         $this->error = "Failed getting the evaluation list " . $tev->getError();
         $this->returnable_error = "Internal database Error";
         return false;
      }      
      $ret["evaluations"]  = $ans;
      
      $ans = $tpp->getPPNameMap();
      if ($ans === false){
         $this->suggested_http_code = 500;
         $this->error = "Failed getting the portal user name map " . $tpp->getError();
         $this->returnable_error = "Internal database Error";
         return false;
      }
      $ret["portal_users"] = $ans;


      $ans = $ti->getInstitutionNameMap();
      if ($ans === false){
         $this->suggested_http_code = 500;
         $this->error = "Failed getting the institution name map " . $ti->getError();
         $this->returnable_error = "Internal database Error";
         return false;
      }
      $ret["institutions"] = $ans;

      $ans = $ts->getSubjectNameMap();
      if ($ans === false){
         $this->suggested_http_code = 500;
         $this->error = "Failed getting the subject name map " . $ts->getError();
         $this->returnable_error = "Internal database Error";
         return false;
      }
      $ret["subjects"] = $ans;
      
      return $ret;

   }

   function get_own_institution($report_id,$parameters){
      return $this->getReport($report_id,$parameters,false);
   }
      
   private function listReports($subject_id, $own_inst_only = true){

      // The portal user that requests the reports for this subject. 
      $auth = $this->headers[HeaderFields::AUTHORIZATION];
      $auth = explode(":",$auth);
      $portal_user = $auth[0];

      //error_log("Inside evaluation list");
      $tev = new TableEvaluations($this->con_main);
      $tpu = new TablePortalUsers($this->con_secure);

      // Verification is ONLY needed when $own_inst_only is equal to false, as we need to ensure that the portal user's valid institutions belong to the report. 
      $portal_user_to_use = $portal_user;
      if (!$own_inst_only) $portal_user_to_use = "";

      $ans = $tev->getEvaluationListForSubjectAndPortalUser($subject_id,$portal_user_to_use);
      if ($ans === false){
         $this->suggested_http_code = 500;
         $this->error = "Failed getting the evaluation list for $subject_id and portal user $portal_user: " . $tev->getError();
         $this->returnable_error = "Internal database Error";
         return false;
      }

      $ret["evaluation_list"] = $ans;
      $ret["portal_users"] = [];

      if (!$own_inst_only) {

         $valid_institutions = $this->portal_user_info[ComplimentaryDataFields::ASSOCIATED_INSTITUTIONS];
          
         // We need to provide the name and last last for all portal users.
         $pp_ids = array();          
         foreach ($ans as $row) {
           // We need to verify that all evaluations are from a valid institution.             
           $inst = $row[TableEvaluations::COL_INSTITUTION_ID];
           if (!in_array($inst,$valid_institutions)){
              $this->suggested_http_code = 403;
              $this->error = "Failed getting evaluations for portal user $portal_user and subject $subject_id. There is an evaluation associated with institution '$inst' while valid institutions are  " . implode(",",$valid_institutions);
              $this->returnable_error = "Invalid institution request";
              return false;  
           }
           $pp_ids[] = $row[TableEvaluations::COL_PORTAL_USER];
         }
         $pp_ids = array_unique($pp_ids);
         
         // We need to get the name of the portal users.
         $ans = $tpu->getAllNamesForIDList($pp_ids, [], []);
         if ($ans === false) {
             $this->suggested_http_code = 500;
             $this->error = "Failed getting portal user names for evaluations " . $tpu->getError();
             $this->returnable_error = "Internal database error";
             return false;
         }

         foreach ($ans as $row) {
            $ret["portal_users"][$row[TableBaseClass::COL_KEYID]] = $row[TablePortalUsers::COL_LASTNAME] . ", " . $row[TablePortalUsers::COL_NAME];
         }
      }

      // If the the request is for a specific portal user and not administrative, no check is required as an invalid portal user will either not log in or return empty report list. 

      return $ret;
   }   

   private function getReport($report_id,$parameters,$own_report){

      // The portal user that requests the reports for this subject. 
      $auth = $this->headers[HeaderFields::AUTHORIZATION];
      $auth = explode(":",$auth);
      $portal_user = $auth[0];

      if (!array_key_exists(URLParameterNames::LANGUAGE,$parameters)){
         $this->suggested_http_code = 401;
         $this->error = "Missing language parameter on PDF Report request";
         $this->returnable_error = "Missing URL parameter for language";
         return false;         
      }

      // Discarded needs to be checked here. Even though we check for this below, when the discareded flag is set,
      // the wepage expects a JSON and NOT a Blob. Hece a JSON needs to be forced, even if later the discarded reason turns to be empty. 
      // The other way around is also true. If a blob is expected, a blob must be sent. 
      $discarded = false;
      error_log(json_encode($parameters));
      if (array_key_exists(URLParameterNames::DISCARDED,$parameters)){
         if ($parameters[URLParameterNames::DISCARDED] == true){
            $discarded = true;
         }
      }

      $tev = new TableEvaluations($this->con_main);

      if (!$own_report){
         // And admin might solicit a report that is not his own. So we empyt the variable to forgo that check. 
         $portal_user_to_use = "";
      }
      else{
         $portal_user_to_use = $portal_user;
      }

      // If someone NOT authorized to get the information for this report they will either not authenticate (because the token and the user ide will be a mistmach)
      // Or this will return empty because the specified report is NOT associated this this portal user in ANY evaluations.
      // And administrative user will empty the portal user  so as to void this check. 
      $ans = $tev->getEvaluation($report_id,$portal_user_to_use);
      
      if ($ans === FALSE){
         $this->suggested_http_code = 401;
         $this->error = "Failed for getting the evaluation $report_id for user $portal_user. Reason: " . $tev->getError();
         $this->returnable_error = "Internal Server Error";
         return false;
      }

      if (empty($ans)){
         $this->suggested_http_code = 401;
         $this->error = "User $portal_user attempted to access evaluation $report_id which is not linked to him";
         $this->returnable_error = "You do not have permission to look at this report";
         return false;
      }
     
      $lang = $parameters[URLParameterNames::LANGUAGE];

      // Creating the json report structure.
      $report_row = $ans[0]; // Getting the report.

      // Make sure the report is actually from the institution of the adminstrative user.
      if (!$own_report){
         if (!in_array($report_row[TableEvaluations::COL_INSTITUTION_ID],$this->portal_user_info[ComplimentaryDataFields::ASSOCIATED_INSTITUTIONS])){
            $list = implode(",",$this->portal_user_info[ComplimentaryDataFields::ASSOCIATED_INSTITUTIONS]);
            $this->suggested_http_code = 403;
            $this->error = "Failed getting evaluation for portal user $portal_user report $report_id. Reason: Evaluation is associated with institution " . $report_row[TableEvaluations::COL_INSTITUTION_ID] . ", while valid institutions are $list";
            $this->returnable_error = "Invalid institution request";
            return false;  
         }
      }

      // Checking if this is a discarded report.
      $discard_reason = $report_row[TableEvaluations::COL_DISCARD_REASON];

      if ($discard_reason != null){
         if ($discard_reason != ""){

            if ($discarded === false){
               $this->suggested_http_code = 500;
               $this->error = "User $portal_user attempted to access evaluation $report_id which is discarded however parameter indicates it is not";
               $this->returnable_error = "Discard status mismatch for report";
               return false;      
            }

            // This is a discarded report. So the report results need to be parsed to obtain the comment field, if there is one. 
            $report_result_data = json_decode($report_row[TableEvaluations::COL_RESULTS],true);
            //error_log("KEYS in report row are : " . implode(",",array_keys($report_result_data)) );
            $comment = $report_result_data[FinalizedResultFields::COMMENT];
            $ret["discard_reason"] = $discard_reason;
            $ret["comment"] = $comment;
            $this->suggested_http_code = 201; // There is no error. But will nto be returning a blob. This is used to signal that. 
            return $ret;
         }
      }

      // The report is not discared. We check this is what was expected
      if ($discarded === true){
         $this->suggested_http_code = 500;
         $this->error = "User $portal_user attempted to access evaluation $report_id which is not discarded however parameter indicates it is";
         $this->returnable_error = "Discard status mismatch for report";
         return false;      
      }

      return $this->generatePDFReport($lang,$report_row);

   }

   /**
    * The difference between this function and the get is that this endpoint checks that the requested report belongs to the institution the user belogns to. 
    */
   function institutionPDF($report_id,$parameters){

      $auth = $this->headers[HeaderFields::AUTHORIZATION];
      $auth = explode(":",$auth);
      $portal_user = $auth[0];      

      // The portal user that requests the reports for this subject. 
      $portal_user_id = $this->portal_user_unique_id;

      if (!array_key_exists(URLParameterNames::LANGUAGE,$parameters)){
         $this->suggested_http_code = 401;
         $this->error = "Missing language parameter on PDF Report request";
         $this->returnable_error = "Missing URL parameter for language";
         return false;         
      }

      // Getting the list of institutions to which this user belongs. 
      $tiu = new TableInstitutionUsers($this->con_main);
      $ans = $tiu->getInstitutionsForUser($portal_user_id);
      if ($ans === FALSE){
         $this->suggested_http_code = 500;
         $this->error = "While attetmpting to the get institutiosn for this user: " . $tiu->getError();
         $this->returnable_error = "Failed report ownership verfication";
         return false;
      }

      // Generating the list of institutions.
      $institutions = array();
      foreach ($ans as $row){
         $institutions[] = $row[TableInstitutionUsers::COL_INSTITUTION_ID];
      }

      $tev = new TableEvaluations($this->con_main);

      // If someone NOT authorized to get the information for this report they will either not authenticate (because the token and the user ide will be a mistmach)
      // Or this will return empty because the specified report is NOT associated this institution and this evaluation.
      $ans = $tev->getInstitutionEvaluation($report_id,$institutions);
      
      if ($ans === FALSE){
         $this->suggested_http_code = 401;
         $this->error = "Failed for getting the evaluation $report_id for user $portal_user for institutios: " . implode(",",$institutions) . " . Reason: " . $tev->getError();
         $this->returnable_error = "Internal Server Error";
         return false;
      }

      if (empty($ans)){
         $this->suggested_http_code = 401;
         $this->error = "User $portal_user attempted to access evaluation $report_id which is not linked to any of the instituions " . implode(",",$institutions);
         $this->returnable_error = "You do not have permission to look at this report";
         return false;
      }
     
      $lang = $parameters[URLParameterNames::LANGUAGE];

      // Creating the json report structure.
      $report_row = $ans[0]; // Getting the report.

      return $this->generatePDFReport($lang,$report_row);

   }

   function reportlist($identifer,$parameters){

      // The $identifier is what we need to search form
      // The parameter must tell if the search is via internal institution id (iuid) or vm id (vmid). 
      // The other parameter is date range represented by to and from parameters 

      // Getting the portal user and it's respective ID. 
      $auth = $this->headers[HeaderFields::AUTHORIZATION];
      $auth = explode(":",$auth);
      $portal_user = $auth[0];    
      $portal_user_id = $this->portal_user_unique_id;     

      // Checking for mandatory parameters. 
      $mandatory = [URLParameterNames::SEARCH_CRITERIA];
      foreach ($mandatory as $parameter){
         // First we verify that the parameter and identifier match the post fields. 
         if (!array_key_exists($parameter,$parameters)){
            $this->suggested_http_code = 401;
            $this->error = "Missing mandatory URL parameter $parameter from user $portal_user";
            $this->returnable_error = "Missing mandatory URL paramter $parameter";
            return false;
         }               
      }

      // Getting the user's institutions. 
      $tiu = new TableInstitutionUsers($this->con_main);
      $ans = $tiu->getInstitutionsForUser($portal_user_id);
      if ($ans === FALSE){
         $this->suggested_http_code = 500;
         $this->error = "Failed to the get institutiosn for $portal_user with id $portal_user_id. Reason: " . $tiu->getError();
         $this->returnable_error = "Failed report ownership verfication";
         return false;
      }

      // Generating the list of institutions.
      $institutions = array();
      foreach ($ans as $row){
         $institutions[] = $row[TableInstitutionUsers::COL_INSTITUTION_ID];
      }

      if (empty($institutions)){
         $this->suggested_http_code = 500;
         $this->error = "Got no institutions for $portal_user with id $portal_user_id";
         $this->returnable_error = "Failed report ownership verfication";
         return false;         
      }

      // Next we check for a valid search criteria.
      $search_criteria = $parameters[URLParameterNames::SEARCH_CRITERIA];

      $vmids = array();

      if ($search_criteria == URLSearchCriteria::IUID){

         // We need to search for all user vmid's (that is subject's keyids) that match this institution and this UID. 
         $search_subj = array();

         $s["comparator"] = SelectColumnComparison::EQUAL;
         $s["value"] = $identifer;
         $search_subj[TableSubject::COL_INTERNAL_ID] = [$s];

         $s["comparator"] = SelectColumnComparison::IN;
         $s["value"] = $institutions;
         $search_subj[TableSubject::COL_INSTITUTION_ID] = [$s]; 

         $ts = new TableSubject($this->con_secure);
         $ans = $ts->search($search_subj);

         if ($ans === FALSE){
            $this->suggested_http_code = 500;
            $this->returnable_error = "Failed to ascertain identity of $identifer";
            $this->error = "IUID matching failed when attempted by $portal_user ($portal_user_id) from institutions: " . implode(",",$institutions) . ". Reason: " . $ts->getError();
            return false;            
         }

         foreach ($ans as $row){
            $vmids[] = $row[TableSubject::COL_UNIQUE_ID];
         }


      }
      else if ($search_criteria != URLSearchCriteria::VMID){
         $this->suggested_http_code = 401;
         $this->returnable_error = "Unknown search criteria $search_criteria";
         $this->error = "Unknown search criteria $search_criteria used by user $portal_user from institutions: " . implode(",",$institutions);
         return false;         
      }
      else{
         // When VMID is used the identifier is straight up.   
         $vmids[] = $identifer; 
      }

      // At this point, we have the vmid's we need to search for. No we just need the date range. 
      $minimum_date = new DateTime(date('c',"0"));  // The minimum date is initialized to the beginning of time (computer time, that is). 
      $minimum_date = $minimum_date->format("Y-m-d H:i:s");

      $maximum_date = new DateTime();
      $maximum_date->add( new DateInterval('P1D') ); //P1D = Plus 1 Day.
      $maximum_date = $maximum_date->format("Y-m-d H:i:s"); // Maximum date is tomorrow. 

      //error_log(json_encode($parameters,JSON_PRETTY_PRINT));

      if (array_key_exists(URLParameterNames::FROM,$parameters)){
         // Start date was set and it is used instead of beginning of time. 
         $secs = $parameters[URLParameterNames::FROM]/1000; // PHP requires seconds since the epoch, not ms. 
         $minimum_date = new DateTime(date('c',$secs));
         $minimum_date = $minimum_date->format("Y-m-d H:i:s");         
      }

      if (array_key_exists(URLParameterNames::TO,$parameters)){
         // End date was set and it is used instead of tomorrow. 
         $secs = $parameters[URLParameterNames::TO]/1000; // PHP requires seconds since the epoch, not ms. 
         $maximum_date = new DateTime(date('c',$secs));
         $maximum_date = $maximum_date->format("Y-m-d H:i:s");                  
      }

      $search = array();
      $search[TableEvaluations::COL_SUBJECT_ID] = [
         ["comparator" => SelectColumnComparison::IN, "value" => $vmids]
      ];
      $search[TableEvaluations::COL_STUDY_DATE] = [
         ["comparator" => SelectColumnComparison::GTE, "value" => $minimum_date],
         ["comparator" => SelectColumnComparison::LTE, "value" => $maximum_date]
      ];

      //error_log(json_encode($search,JSON_PRETTY_PRINT));

      $te = new TableEvaluations($this->con_main);
      $ans = $te->search($search);
      if ($ans === false){
         $this->suggested_http_code = 500;
         $this->returnable_error = "Failure executing the search ";
         $this->error = "Failure while searching for evalutions for $portal_user from institutions: " . implode(",",$institutions) . " for ids: "  . implode(",",$vmids) . " in dates between $minimum_date and $maximum_date. Reason: " . $te->getError();
         return false;         
      }

      $ret["start_date"] = $minimum_date;
      $ret["end_date"] = $maximum_date;
      $ret["result_count"] = count($ans);
      $ret["searched_for"] = $identifer;
      $ret["reports"] = [];
      foreach ($ans as $row){
         $report["id"] = $row[TableEvaluations::COL_KEYID];
         $report["study_type"] = $row[TableEvaluations::COL_STUDY_TYPE];
         $report["study_date"] = $row[TableEvaluations::COL_STUDY_DATE];
         $report["processing_date"] = $row[TableEvaluations::COL_STUDY_DATE];
         $report["evaluator"] = $row[TableEvaluations::COL_EVALUATOR_LASTNAME] . ", " . $row[TableEvaluations::COL_EVALUATOR_NAME];
         $ret["reports"][] = $report;
      }

      return $ret;

   }

   private function generatePDFReport($lang, $report_row){

      $report_id = $report_row[TableEvaluations::COL_KEYID];
      $report = json_decode($report_row[TableEvaluations::COL_RESULTS],true);

      if (json_last_error() != JSON_ERROR_NONE){
         $this->suggested_http_code = 500;
         $this->error = "Failed to decode report string: " . $report_row[TableEvaluations::COL_RESULTS] . " from $report_id with error: " . json_last_error_msg();
         $this->returnable_error = "Internal Server Error";
         return false;         
      }

      // Getting the report version. 
      if (!array_key_exists(CommonResultKeys::PDF_GEN_VERSION,$report)){
         $this->suggested_http_code = 401;
         $this->error = "Requested report $report_id does not have a PDF Generator Version";
         $this->returnable_error = "Invalid report format";
         return false;
      }

      $version = $report[CommonResultKeys::PDF_GEN_VERSION];

      // Now we create the work directory
      $file_link = $report_row[TableEvaluations::COL_FILE_LINK];
      $basename = str_replace(".zip","",$file_link); // Getting rid of the .zip file. 
      $work_directory = CONFIG[GlobalConfigProcResources::GROUP_NAME][GlobalConfigProcResources::PDF_WORK_DIR];
      $work_directory = $work_directory . "/" . $basename;
      shell_exec("mkdir -p $work_directory");
      if (!is_dir($work_directory)){
         $this->suggested_http_code = 500;
         $this->error = "Failed to create work directory for PDF generation: $work_directory";
         $this->returnable_error = "Internal Server Error";
         return false;         
      }

      // We get the path for the PDF generation script and generate the strign for both the output error log and pdf output. 
      $pdf_output = $work_directory . "/" . $basename . ".pdf";
      $json_input = $work_directory . "/" . $basename . ".json";
      $error_log  = $work_directory . "/error.log";
      $pdf_js = CONFIG[GlobalConfigProcResources::GROUP_NAME][GlobalConfigProcResources::PDF_GEN_DIR] . "/v$version/pdf_report_generator.js";

      // Actually saving the JS Input file into the work directory. 
      $fid = fopen($json_input,"w");
      fwrite($fid,json_encode($report,JSON_PRETTY_PRINT));
      fclose($fid);

      // Running the report generating command.
      $cmd = "node $pdf_js $lang $json_input $pdf_output > $error_log 2>&1";
      $output = array();
      $retval = 0;
      exec($cmd, $output,$retval);
      
      
      if ($retval != 0){
         $this->suggested_http_code = 500;
         $this->error = $this->error .  "Error in report generation\n";
         $this->error = $this->error .  "CMD: $cmd\n";
         $this->error = $this->error .  "Error Log at: $error_log";
         $this->returnable_error = "Failed to generate report. ";
         return false;
      }

      // The PDF was generated successfully and it is now returned. 
      $this->file_path_to_return = $pdf_output;
   
      return array();

   }


}


?>
