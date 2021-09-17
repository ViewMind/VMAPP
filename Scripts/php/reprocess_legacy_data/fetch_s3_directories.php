<?php

   /**
    * This script will fetch all the s3 data relevant to a given date range and from a given instituion based on the local data which is a copy of legacy system's database. 
    */

   /////////////////////////////////////////////////////////// CONSTANTS
   
   // Names and addresses
   const S3_BUCKET   = "viewmind-raw-eye-data";
   const S3_PROFILE  = "viewmind_eu";
   const WORK_DIRECTORY = "work";
   const CREDENTIALS = "credentials.cnf";

   // DB NAMES
   const VMID = "viewmind_id";
   const VMDATA = "viewmind_data";

   // Institutions in the legacy system (their ID)
   const INST_AXIS        = 1145868706;
   const INST_VIEWMIND    = 1369462188;
   const INST_ANTIOQUIA   = 1105849094;
   const INST_NOVARTIS    = 989302458;
   const INST_STRATHCLYDE = 1242673082;

   /////////////////////////////////////////////////////////// INPUTS
   const START_DATE = "2017-06-03";  // Before Time
   const END_DATE   = "2022-08-01";  // After Time. This way we get it all. 
   $institution = INST_VIEWMIND;

   /////////////////////////////////////////////////////////// SCRIPT START.

   // Loading the credentials.
   $connection_data = parse_ini_file(CREDENTIALS);
   $host = $connection_data["host"];
   $dbpasswd = $connection_data["password"];
   $dbuser = $connection_data["user"];
   
   $con_id   = mysqli_connect($host,$dbuser,$dbpasswd,VMID);
   if ($con_id === false){
      echo "Failed opening the connection to ID DB: " . mysqli_connect_error() . "\n";
      exit();
   }
   $con_data = mysqli_connect($host,$dbuser,$dbpasswd,VMDATA);
   if ($con_data === false){
      echo "Failed opening the connection to Result DB: " . mysqli_connect_error() . "\n";
      exit();
   }

   echo "Established connection to DB. Will search for Raw Data Between " . START_DATE . " and " . END_DATE . " for instituion $institution\n";

   // Query to get all results for the institution from the data table, we are only interested in the PUIDs. 
   $sql = "SELECT DISTINCT puid FROM tEyeResults WHERE doctorid LIKE '$institution%'";
   $res = mysqli_query($con_data,$sql);
   if (!$res){
      echo "Query Failed: " . mysqli_error($con_data) . ". SQL: $sql\n";
      exit();
   }

   $puid_list = array();
   while ($row = mysqli_fetch_array($res,MYSQLI_ASSOC)){
      $puid_list[] = $row["puid"];
   }

   echo "Found " . count($puid_list) . " unique patients. Forming a hash equivalence\n";

   $sql = "SELECT * FROM tPatientIDs WHERE keyid IN (" . implode(",",$puid_list) . ")";
   $res = mysqli_query($con_id,$sql);
   if (!$res){
      echo "Query Failed: " . mysqli_error($con_id) . ". SQL: $sql\n";
      exit();
   }

   $hash_map = array();
   while ($row = mysqli_fetch_array($res,MYSQLI_ASSOC)){
      $hash_map[$row["keyid"]] = $row["uid"];
   }

   echo "Hash map built. Staring S3 Download ...\n";

   $total = count($hash_map);
   $downloaded = 0;

   foreach ($hash_map as $uid => $hash){

      $output = WORK_DIRECTORY . "/$institution/$uid";
      shell_exec("mkdir -p $output");      
      $cmd = "aws s3 cp s3://" . S3_BUCKET . "/$hash " . $output . " --recursive  --profile=" . S3_PROFILE;
      echo "CMD: $cmd\n";
      shell_exec($cmd);
      $downloaded++;
      echo "PROGRESS: $downloaded out of $total\n";

   }


?>