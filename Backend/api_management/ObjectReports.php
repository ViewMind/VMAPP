<?php

include_once ("ObjectPortalUsers.php");
//include_once (__DIR__ . "/../db_management/TableProcessingParameters.php");
include_once (__DIR__ . "/../common/named_constants.php");
include_once (__DIR__ . "/../common/log_manager.php");
include_once (__DIR__ ."./../common/config.php");

class ObjectReports extends ObjectBaseClass
{

   public function __construct($service){
     parent::__construct($service);        
   }

   function generate($institution, $parameters){

      // Set the log for this opertion using the instituntion number and instance. 
      $instance = $parameters[URLParameterNames::INSTANCE];
      $date = new DateTime();
      $log_file_name = $institution ."_" . $instance . "_" . $date->format('Y_m_d_H_i_s') . ".log";
      $logger = new LogManager(CONFIG[ParameterGeneral::NAME][ValueGeneral::PROCESSING_LOG_LOCATION] . "/" . $log_file_name);
      $logger->logProgress("Processing request from $institution - $instance");

      // Copy the received file to a new directory insider the work directory. 
      //$source = $_FILES[self::FILE_STRUCT_TMP_NAME];
      echoOut($_FILES,true);

      // Uncompress the received file.
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
