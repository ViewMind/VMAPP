<?php

include_once ("ObjectPortalUsers.php");
include_once (__DIR__ . "/../data_processing/ViewMindDataContainer.php");
include_once (__DIR__ . "/../data_processing/Processing.php");
include_once (__DIR__ . "/../data_processing/CSVGeneration/CSVGoNoGo.php");
include_once (__DIR__ . "/../data_processing/CSVGeneration/CSVReading.php");
include_once (__DIR__ . "/../data_processing/CSVGeneration/CSVBinding.php");
include_once (__DIR__ . "/../data_processing/CSVGeneration/CSVNBackRT.php");
include_once (__DIR__ . "/../data_processing/RProcessing.php");

class ObjectReports extends ObjectBaseClass
{

   public function __construct($service){
     parent::__construct($service);        
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
      }

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

      ////////////// LOADING AND PROCESSING
      $vmdc = new ViewMindDataContainer();
      if (!$vmdc->loadFromFile($json_filename)){
         $this->suggested_http_code = 500;
         $this->error = "Failed processing file";
         $logger->logError("Failed loading $json_filename. Reason: " . $vmdc->getError());
         return false;
      }

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
            $logger->logError("Could not save the computed processing information on $json_filename");
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
      
      // Load the received file and check for errors. 
      // Extract the subject data.
      // Do insert on update on that data.
      // Compute all necessary values on the JSON file. 
      // Generate the CSV 
      // Call the R Script
      // Insert generted values into the VM Data Container. 
      // Insert into DB the resulting values and the quality check values
      // Copy the resulting JSON from its current location to the S3 using the insertion id of the evaluation column. 

   }

}


?>
