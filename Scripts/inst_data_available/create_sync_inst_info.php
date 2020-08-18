<?php
   include("config.php");
   include("log_manager.php");
   include("fast_config_interpreter.php");
   
   /////////////////////////////////// Directory cleanup function
   function directoryCleanUp($dir,$to_leave){
      $file_list = scandir($dir);
      for ($f = 0; $f < count($file_list); $f++){
         $spath = $file_list[$f];
         if (($spath == ".") || ($spath == "..")) continue;
         $path = $dir . "/" . $spath;
         
         $parts = explode(".",$spath);
         $delete = false;
         if (count($parts) != 2){
            $delete = true;
         }
         else{
            $ext = $parts[1];
            //echo "Extension $ext\n";
            if (!in_array($ext,$to_leave)) $delete = true;
         }
         
         if ($delete){
            //echo "DELETING $path\n";
            unlink($path);
         }
                  
      }
   }
   
   
//    $work_dir = "OUTPUTS/989302458/MTHLGY0002/8782bdba8c18a53673f4826c333be16adf52b30f68ed300d69c97d71c32ae7d132c077391c5718a425406cc3933d30e91fa4e059faec5fcb634bf4c3168275e7/2020_05_27_20_25_23/";   
//    directoryCleanUp($work_dir,$config["ext_leave_on_cleanup"]);   
//    return;
   
   /////////////////////////////////// Database and log setup 
   $date = new DateTime();
   $logfile = $date->format('Y_m_d_H_i_s') . ".log";         
   $logger = new LogManager($logfile);
   
   $previous_date_file = "previous_dates.json";
   
   // Starting from scratch.
   $logger->logProgress("Deleting output dir");
   shell_exec("rm -rf OUTPUTS");
   
   if ($config["run_local"]){
      $logger->logProgress("LOCAL RUN ...");
      $local_key      = 1;
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
      $local_key      = 0;
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
   if (is_file($previous_date_file)){
      $json = file_get_contents($previous_date_file);
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
       $previous_date = '2020-04-01';
       if (array_key_exists($inst_uid,$previous_sync_dates)){
          $previous_date = $previous_dates[$inst_uid];          
       }
       else{
          $logger->logProgress("   First time sync");
       }
       $logger->logProgress("   Getting data from $previous_date");
       
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
       
       // Copying and processing each directory.
       $counter = 0;
       foreach ($uid_data as $puid => $uid){          
          $hashuid  = $uid["hash"];
          $protocol = $uid["protocol"];
          
          $protocol = str_replace(" ","_",$protocol);
          $inst_protocol_repo = "$inst_dir/$protocol";
          
          if (!is_dir($inst_protocol_repo)){
             mkdir($inst_protocol_repo,0755,true);
          }
                    
          //$logger->logProgress("   OUPUT REP: $inst_dir/$protocol");
          
          // Download the information. 
          $cmd =  "./dl_s3_data.sh $local_key $inst_protocol_repo $hashuid";
          $logger->logProgress("   DL CMD: $cmd");
          shell_exec($cmd);          
          
          // Listing work directories directories. 
          $work_dirs = scandir("$inst_protocol_repo/$hashuid");
          //var_dump($work_dirs);
          
          for ($d = 2; $d < count($work_dirs); $d++){
             $work_dir = "$inst_protocol_repo/$hashuid/" .  $work_dirs[$d];
             $eye_rep_config = $work_dir . "/eye_rep_gen_conf";
             if (!is_file($eye_rep_config)) {
                // PATCH DUE to old bug that some times generated eye_rep_gen_conf. instead of eye_rep_gen_conf. 
                $eye_rep_config = $eye_rep_config . ".";
                if (!is_file($eye_rep_config)) {
                   $logger->logError("   SKIPPING work dir due to not finding eye rep gen conf: $eye_rep_config");
                   continue;
                }
             }
             
             $conf = new FastConfigInterpreter($eye_rep_config);
             
             $conf->removeField("data_saved_to_db_flag");
             $conf->saveToHDD();
             
             // Running the EyeReportGenerator. 
             $cmd = "cd EyeReportGen; ./EyeReportGen  ../$eye_rep_config";
             $logger->logProgress("   PROC COMMAND: $cmd");
             shell_exec($cmd);
             
             // Directory cleanup
             directoryCleanUp($work_dir,$config["ext_leave_on_cleanup"]);
             
             // Seeing if the report was generated. 
             $repfilename = $work_dir . "/" . $conf->getValueForKey("report_filename");
             
             if (!is_file($repfilename)) {
                $logger->logError("   Could not create report file $repfilename");
                continue;
             }
             
             // Leaving only report keys.
             $keys_to_leave = array_keys($config["report_variable_names"]);
             $report = new FastConfigInterpreter($repfilename);
             $report->leaveOnlyKeys($keys_to_leave);
             
             // Replacing the keys with the final names.
             $report->replaceKeys($config["report_variable_names"]);
             
             // Finally saving the resport to HDD.
             $report->saveToHDD();
             
          }
          
          // DEBUG CODE:
          $counter++;
          if ($counter == 2){
             break;
          }
                    
       }
       
       $logger->logProgress("   Finshed Data Processing. Starting Information Transfer");
       $cmd = "./transfer.sh $inst_dir $IP $inst_user";
       $logger->logProgress("   TRANSFER CMD: $cmd");
       shell_exec($cmd);          
           
   }
   
   // Saving last checkpoint. 
   $logger->logProgress("Saving last checkpoint");
   $fid = fopen($previous_date_file,"w");
   fwrite($fid,json_encode($previous_sync_dates));
   fclose($fid);
   
   // The actual synching. 
   
   $logger->logProgress("Finished");
      
?>
