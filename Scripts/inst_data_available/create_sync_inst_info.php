<?php
   include("config.php");
   include("log_manager.php");
   
   /////////////////////////////////// Database and log setup 
   $date = new DateTime();
   $logfile = $date->format('Y_m_d_H_i_s') . ".log";         
   $logger = new LogManager($logfile);
   
   if ($config["run_local"]){
      $logger->logProgress("LOCAL RUN ...");
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
      $logger->logProgress("SERVER RUN ...");
      $data_host      = $config["dbconfig"]["db_data_host"];
      $data_name      = $config["dbconfig"]["db_data_name"];
      $data_user      = $config["dbconfig"]["db_data_user"];
      $data_passwd    = $config["dbconfig"]["db_data_passwd"];
      $patid_host     = $config["dbconfig"]["db_patid_host"];
      $patid_name     = $config["dbconfig"]["db_patid_name"];
      $patid_user     = $config["dbconfig"]["db_patid_user"];
      $patid_passwd   = $config["dbconfig"]["db_patid_passwd"];
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
   
   /////////////////////////////////// Previous Date Loading
   $previous_sync_dates = array();
   if (is_file("previous_dates.json")){
      $json = file_get_contents("previous_dates.json");
      $previous_sync_dates = json_decode($json);
   }
   
   /////////////////////////////////// Iterating over all institutions to get.
   $sync_institutions = $config["sync_info"];
   
   for ($i = 0; $i < count($sync_institutions); $i++){
       $inst_uid  = $sync_institutions[$i]["institution_uid"];
       $inst_user = $sync_institutions[$i]["institution_user"];
       $IP        = $sync_institutions[$i]["IP"];
       
       $logger->logProgress("INSTITUTION $inst_uid ($inst_user)");
       
       // Creating the base path for the instiution. 
       $inst_dir = "OUTPUTS/" . $inst_uid;
       if (!is_dir($inst_dir)){
          mkdir($inst_dir,0755,true);
       }
       
       // Getting the last update date. 
       $previous_date = '0000-00-00';
       if (array_key_exists($inst_uid,$previous_sync_dates)){
          $previous_date = $previous_dates[$inst_uid];
          $logger->logProgress("   Getting data from $previous_date");
       }
       else{
          $logger->logProgress("   First time sync");
       }
       
       // Saving current date. 
       $date = new DateTime();
       $previous_sync_dates[$inst_uid] = $date->format('Y-m-d');
       
       // Actually listing the studies. 
       $sql = "SELECT DISTINCT puid, protocol from tEyeResults where study_date > '$previous_date' AND doctorid LIKE '$inst_uid%'";           
       $res = mysqli_query($con_data,$sql);
       if (!$res){
          $logger->logError("DB Error: " . mysqli_error($con_data) . " on query " . $sql . "\n");
          return;
       }
       
       // Checking that there is anything to sync. 
       if (mysqli_num_rows($res) == 0) {
          $logger->logProgress("   No new patient data found since previous date. Not doing anything");
          continue;
       }
       
       // Iterating over all puids. 
       $uid_data = array();
       while ($row = mysqli_fetch_array($res,MYSQLI_ASSOC)){   
          $uid["hash"] = "";
          $protocol = "no_protocol";
          if (($row["protocol"] != "") && ($row["protocol"] != null)) $protocol = $row["protocol"];
          $uid["protocol"] = $protocol;
          $uid_data[$row["puid"]] = $uid;
       }
       
       // Getting the info for the puid huid map
       $puids = array_keys($uid_data);
       $sql = "SELECT * FROM tPatientIDs WHERE keyid IN (" . implode(',',$puids) . ")";       
       $res = mysqli_query($con_patid,$sql);
       if (!$res){
          $logger->logError("DB Error: " . mysqli_error($con_patid) . " on query " . $sql . "\n");
          return;
       }
                         
       while ($row = mysqli_fetch_array($res,MYSQLI_ASSOC)){   
          $uid_data[$row["keyid"]]["hash"] = $row["uid"];
       }
       
       
   }
   
   $logger->logProgress("Finished");
   
?>
