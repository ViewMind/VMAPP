<?php

   /// INPUTS
   $QUERY_TO_GET = "SELECT DISTINCT puid FROM tEyeResults WHERE doctorid LIKE '1105849094%' AND study_id LIKE '%bc3%'";
   $OUTPUT_DIRECTORY = "osvaldo";


   /// THIS CODE NEEDS TO BE ADAPTED IN THE NEW INFRASTRUCTURE

   $config = array ( 

   "dbconfig" =>  array ('db_data_host' => 'viewminddb.cdqlb2rkfdvi.us-east-2.rds.amazonaws.com',
      'db_data_name' => 'viewmind_data',
      'db_data_user' => 'vmuser',
      'db_data_passwd' => 'viewmind_2018',
      'db_patid_host' => 'viewmind-id.cdqlb2rkfdvi.us-east-2.rds.amazonaws.com',
      'db_patid_name' => 'viewmind_id',
      'db_patid_user' => 'vmid',
      'db_patid_passwd' => 'ksjdn3228shs1852'),

   "run_local" => false
   ); 

   $config["run_local"] = is_file("local_flag_file");
   if ($config["run_local"]){
      echo "LOCAL RUN\n";
   }
   else {
      echo "SERVER RUN\n";
   }

   if ($config["run_local"]){
      $data_host      = "localhost";
      $data_name      = $config["dbconfig"]["db_data_name"];
      $data_user      = $config["dbconfig"]["db_data_user"];
      $data_passwd    = $config["dbconfig"]["db_data_passwd"];
      $patid_host     = "localhost";
      $patid_name     = $config["dbconfig"]["db_patid_name"];
      $patid_user     = $config["dbconfig"]["db_patid_user"];
      $patid_passwd   = $config["dbconfig"]["db_patid_passwd"];
   }
   else{
      $data_host      = $config["dbconfig"]["db_data_host"];
      $data_name      = $config["dbconfig"]["db_data_name"];
      $data_user      = $config["dbconfig"]["db_data_user"];
      $data_passwd    = $config["dbconfig"]["db_data_passwd"];
      $patid_host     = $config["dbconfig"]["db_patid_host"];
      $patid_name     = $config["dbconfig"]["db_patid_name"];
      $patid_user     = $config["dbconfig"]["db_patid_user"];
      $patid_passwd   = $config["dbconfig"]["db_patid_passwd"];
   }    

   // Resetting the output directory. 
   shell_exec("rm -rf $OUTPUT_DIRECTORY");
   shell_exec("mkdir $OUTPUT_DIRECTORY");
   if (!is_dir($OUTPUT_DIRECTORY)){
      echo "Could not create output directory $OUTPUT_DIRECTORY\n";
      return;
   }

   // Creating the DB connections. 
   $con_data  = mysqli_connect($data_host,$data_user,$data_passwd,$data_name);
   if (mysqli_connect_errno()){  
      $logger->logError("Cannot establish connection to data DB. Error code: " . mysqli_connect_errno());
      exit;
   }      
   
   $con_patid  = mysqli_connect($patid_host,$patid_user,$patid_passwd,$patid_name);
   if (mysqli_connect_errno()){  
      $logger->logError("Cannot establish connection to ID DB. Error code: " . mysqli_connect_errno());
      exit;
   }      
      
   // Ensuring the encoding we get back. 
   mysqli_query($con_data,"SET NAMES 'utf8'");
   mysqli_query($con_patid,"SET NAMES 'utf8'");

   // Making the actual query for the desired raw data.
   $res = mysqli_query($con_data,$QUERY_TO_GET);
   if (!$res){
      echo "DB Error: " . mysqli_error($con_data) . " on query " . $sql . "\n";
      return;
   }
   
   // Checking that there is anything to sync. 
   if (mysqli_num_rows($res) == 0) {
      echo "No patients in the resulting query, nothing to do\n";
      return;
   }   

   // If we got here we got a list of PUIds, we get the list of hashes. 
   $puids = array();
   while ($row = mysqli_fetch_array($res,MYSQLI_ASSOC)){   
      $puids[] = $row["puid"];
   }   

   $sql = "SELECT * FROM tPatientIDs WHERE keyid IN (" . implode(',',$puids) . ")";       
   $res = mysqli_query($con_patid,$sql);
   if (!$res){
      $logger->logError("DB Error: " . mysqli_error($con_patid) . " on query " . $sql . "\n");
      return;
   }
                     
   while ($row = mysqli_fetch_array($res,MYSQLI_ASSOC)){   
      $uid_data[$row["keyid"]] = $row["uid"];
   }   

   foreach ($uid_data as $puid => $hash){

      $aws_cmd = "aws s3 cp s3://viewmind-raw-eye-data/$hash $OUTPUT_DIRECTORY/$puid --recursive";

      echo "CMD: " . $aws_cmd . "\n";

      if (!$config["run_local"]){
         passthru($aws_cmd);
      }

   }

   echo "Finished\n";

?> 
