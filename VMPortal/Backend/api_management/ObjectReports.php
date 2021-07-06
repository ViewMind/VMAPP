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


class ObjectReports extends ObjectBaseClass
{

   public function __construct($service,$headers){
     parent::__construct($service,$headers);        
   }

   function generate($institution, $parameters){

      $this->returnable_error = "";

      ////////////// SET UP

      // Set the log for this opertion using the instituntion number and instance. 
      $instance = $parameters[URLParameterNames::INSTANCE];
      $date = new DateTime();
      $base_proc_name =$institution ."_" . $instance . "_" . $date->format('Y_m_d_H_i_s'); 
      $log_file_name = $base_proc_name . ".log";
      $logger = new LogManager(CONFIG[GlobalConfigLogs::GROUP_NAME][GlobalConfigLogs::PROCESSING_LOG_LOCATION] . "/" . $log_file_name);
      $logger->logProgress("Processing request from $institution - $instance");

      // Creating the work directory.
      $workdir = CONFIG[GlobalConfigProcResources::GROUP_NAME][GlobalConfigProcResources::PROCESSING_WORK_DIRECTORY];
      shell_exec("mkdir -p $workdir/$base_proc_name");
      $workdir = "$workdir/$base_proc_name";
      if (!is_dir($workdir)){
         $this->http_response_code = 500;
         $this->error = "Processing directory error";
         $logger->logError("Failed in creating the work directory at $workdir");
         return false;
      }

      ////////////// URL verification.

      // First we verify that the parameter and identifier match the post fields. 
      if (!array_key_exists(URLParameterNames::INSTANCE,$parameters)){
         $this->http_response_code = 401;
         $this->error = "Missing URL parameters";
         $logger->logError("URL instance parameter is missing");
         return false;
      }
      
      $instance = $parameters[URLParameterNames::INSTANCE];
      if ($_POST[POSTFields::INSTITUION_INSTANCE] != $instance){
         $this->http_response_code = 401;
         $this->error = "URL Instance mistmatch";
         $logger->logError("The URL instance for the accessing was " . $_POST[POSTFields::INSTITUION_INSTANCE] . " but $instance was used as a parameter");
         return false;
      }
      
      if ($_POST[POSTFields::INSTITUTION_ID] != $institution){
         $this->http_response_code = 401;
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

      // Generating the CSV
      $csv_generating_function = Processing::CSV_FUNCTION_MAP[$study];
     
      $csv_generating_error = call_user_func($csv_generating_function, $vmdc, $csv_file);

      if ($csv_generating_error != ""){
         $this->suggested_http_code = 500;
         $this->error = "Failed processing file";
         $logger->logError("Error generating CSV for study $study of file $json_filename. Reason: $csv_generating_error");
         return false;             
      }

      // Pass the CSV through the RScript. 
      $ans = RProcessing($vmdc,$csv_file,$workdir);
      if ($ans != ""){
         $this->suggested_http_code = 500;
         $this->error = "Failed processing file";
         $logger->logError("Error processing file through R Script. Reason: $ans");
         return false;             
      }

      // 
      if (!$vmdc->save()){
         $this->suggested_http_code = 500;
         $this->error = "Failed saving processing file";
         $logger->logError("Failed saving new information into $json_filename");
         return false;                
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
      $evaluation[TableEvaluations::COL_QC_GRAPHS] = $vmdc->getInsertableQualityControlGraphValuesString();
      $evaluation[TableEvaluations::COL_QC_PARAMS] = $vmdc->getInsertableQualityControlParametersString();

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
      $compress_cmd = "cd $workdir; tar -c -z -f $evaluation_id.zip $json_filename";
      $output = shell_exec($compress_cmd);
      
      $zipfile = $workdir . "/" . $evaluation_id . ".zip";
      if (!is_file($zipfile)){
         $this->suggested_http_code = 500;
         $this->error = "Failed to generate compressed file";
         $logger->logError("Could not create zip file $zipfile from $json_filename.\nCMD Used: $compress_cmd\n OUTPUT:\n $output");
         return false;            
      }
      
      //$aws_command = "aws s3 cp $zipfile s3://$aws_bucket/$base_file_name.zip --profile $aws_profile 2>&1";
      $aws_command = "aws s3 cp $zipfile s3://$aws_bucket/$base_file_name.zip 2>&1";
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

      // The portal user that requests the reports for this subject. 
      $auth = $this->headers[HeaderFields::AUTHORIZATION];
      $auth = explode(":",$auth);
      $portal_user = $auth[0];

      $tev = new TableEvaluations($this->con_main);

      // If someone NOT authorized to get the information for this subject they will either not authenticate (because the token and the user ide will be a mistmach)
      // Or this will return empty because the specified subject id is NOT associated this this portal user in ANY evaluations. 
      $ans = $tev->getEvaluationListForSubjectAndPortalUser($subject_id,$portal_user);
      
      if ($ans == FALSE){
         $this->suggested_http_code = 500;
         $this->error = $tev->getError();
         $this->returnable_error = "Internal Server Error";
         return false;
      }

      return $ans;
   }

   function get($report_id,$parameters){

      // The portal user that requests the reports for this subject. 
      $auth = $this->headers[HeaderFields::AUTHORIZATION];
      $auth = explode(":",$auth);
      $portal_user = $auth[0];

      $tev = new TableEvaluations($this->con_main);

      // If someone NOT authorized to get the information for this report they will either not authenticate (because the token and the user ide will be a mistmach)
      // Or this will return empty because the specified report is NOT associated this this portal user in ANY evaluations. 
      $ans = $tev->getEvaluation($report_id,$portal_user);
      
      if ($ans === FALSE){
         $this->suggested_http_code = 500;
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

      return $ans;


   }


}


?>
