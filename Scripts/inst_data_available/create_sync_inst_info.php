<?php
   include("config.php");
   include("log_manager.php");
   include("fast_config_interpreter.php");

   //////////////////////////////////// CONSTANTS

   const TABLE_EYERESULTS       = "tEyeResults";
   const COL_EYERES_PUID        = "puid";
   const COL_EYERES_PROTOCOL    = "protocol";
   const COL_EYERES_STUDY_DATE  = "study_date";
   const COL_EYERES_DOCTORID    = "doctorid";
   
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
   
   
   /////////////////////////////////// Database and log setup 
   $date = new DateTime();
   $logfile = $date->format('Y_m_d_H_i_s') . "_routputs.log";         
   $logger = new LogManager("");
   $logROutputs = new LogManager($logfile);
   $logROutputs->suppressOutput();
   
   $previous_date_file = "previous_dates.json";
   
   // Starting from scratch.
   $logger->logProgress("Deleting output dir");
   shell_exec("rm -rf OUTPUTS");

   // Setting the variables for MYSQL Connection.
   $data_user      = $config["dbconfig"]["db_user"];
   $data_passwd    = $config["dbconfig"]["db_passwd"];
   $data_name      = $config["dbconfig"]["db_data_name"];
   $data_host      = $config["dbconfig"]["db_data_host"];

   $patid_user     = $config["dbconfig"]["db_user"];
   $patid_passwd   = $config["dbconfig"]["db_passwd"];
   $patid_name     = $config["dbconfig"]["db_patid_name"];
   $patid_host     = $config["dbconfig"]["db_patid_host"];
   
   if ($config["run_local"]){
      $logger->logProgress("LOCAL RUN ...");
      $local_key      = TRUE;
   }
   else{
      $logger->logProgress("SERVER RUN ...");
      $local_key      = FALSE;
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

   /////////////////////////////////// Getting the current path.
   $CURRENT_PATH = trim(shell_exec("pwd ."));

   /////////////////////////////////// Checking the existance and path of the EyeReportGenerator. 
   $eye_rep_gen_path = $config["eye_rep_gen_path"];
   $eye_rep_gen_bin  = "$eye_rep_gen_path/EyeReportGen";
   $eye_rep_gen_res  = "$eye_rep_gen_path/res";
   
   if (is_dir($eye_rep_gen_path)){
      if (!is_dir($eye_rep_gen_res)){
         $logger->logError("EyeReportGenerator res path is not a directory: $eye_rep_gen_res. Aborting");
         return;   
      }

      if (!is_file($eye_rep_gen_bin)){
         $logger->logError("EyeReportGenerator binary not found: $eye_rep_gen_bin. Aborting");
         return;   
      }
   }
   else{
      $logger->logError("EyeReportGenerator path is not a directory: $eye_rep_gen_path. Aborting");
      return;
   }
   
   /////////////////////////////////// Previous Date Loading
   $previous_sync_dates = array();
   if (is_file($previous_date_file)){
      $json = file_get_contents($previous_date_file);
      $previous_sync_dates = json_decode($json,true);
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
          $previous_date = $previous_sync_dates[$inst_uid];          
       }
       else{
          $logger->logProgress("   First time sync");
       }
       $logger->logProgress("   Getting data from $previous_date");
       
       // Saving current date. 
       $date = new DateTime();
       $previous_sync_dates[$inst_uid] = $date->format('Y-m-d');
       
       // Actually listing the studies. 
       $sql = "SELECT DISTINCT " . COL_EYERES_PUID . "," . COL_EYERES_PROTOCOL . " from " . TABLE_EYERESULTS 
              . " WHERE " . COL_EYERES_STUDY_DATE . " > '$previous_date' AND " . COL_EYERES_DOCTORID . " LIKE '$inst_uid%'";           
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
          if (($row[COL_EYERES_PROTOCOL] != "") && ($row[COL_EYERES_PROTOCOL] != null)) $protocol = $row[COL_EYERES_PROTOCOL];
          $uid[COL_EYERES_PROTOCOL] = $protocol;
          $uid_data[$row[COL_EYERES_PUID]] = $uid;
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

       foreach ($uid_data as $puid => $uid){          
          $hashuid  = $uid["hash"];
          $protocol = $uid[COL_EYERES_PROTOCOL];
          
          $protocol = str_replace(" ","_",$protocol);
          $inst_protocol_repo = "$inst_dir/$protocol";
          
          if (!is_dir($inst_protocol_repo)){
             mkdir($inst_protocol_repo,0755,true);
          }
                    
          //$logger->logProgress("   OUPUT REP: $inst_dir/$protocol");
          
          // Download the information. 
          $cmd = "aws s3 cp s3://viewmind-raw-eye-data/$hashuid $inst_protocol_repo/$hashuid --recursive";
          $logger->logProgress("   DL CMD: $cmd");
          if ($local_key){
             $logger->logProgress("LOCALLY: Just outputting command\n   -> $cmd");
          }
          else{
             $shell_output = shell_exec($cmd);          
             $logger->logProgress("   STD CMD OUTPUT:\n" . $shell_output);  
          }
          
          // Listing work directories directories. 
          $work_dirs = scandir("$inst_protocol_repo/$hashuid");
          //var_dump($work_dirs);
          if (empty($work_dirs)){
             $logger->logProgress("WARNING: SKIPPING due to empty directory: $inst_protocol_repo/$hashuid");
             continue;
          }
          
          // Required for full path creation
          for ($d = 2; $d < count($work_dirs); $d++){
             $work_dir = "$CURRENT_PATH/$inst_protocol_repo/$hashuid/" .  $work_dirs[$d];
             $eye_rep_config = $work_dir . "/eye_rep_gen_conf";
             if (!is_file($eye_rep_config)) {
                // PATCH DUE to old bug that some times generated eye_rep_gen_conf. instead of eye_rep_gen_conf. 
                $eye_rep_config = $eye_rep_config . ".";
                if (!is_file($eye_rep_config)) {
                   $logger->logError("   SKIPPING work dir due to not finding eye rep gen conf: $eye_rep_config");
                   continue;
                   //return;
                }
             }
             
             $conf = new FastConfigInterpreter($eye_rep_config);
             
             $conf->removeField("data_saved_to_db_flag");
             $conf->saveToHDD();
             
             // Running the EyeReportGenerator. 
             $cmd = "cd $eye_rep_gen_path; ./EyeReportGen  $eye_rep_config > " . $logROutputs->getLogFile() . " 2>&1 ";
             $logger->logProgress("   PROC COMMAND: $cmd");
             $logROutputs->logProgress("EXECUTED COMMAND: $cmd\n\n=====================>");
             $shell_output = shell_exec($cmd);             
             $logROutputs->logProgress("=========================================================================================");
             
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
       }
       
       $logger->logProgress("   Finshed Data Processing. Starting Information Transfer");
       $cmd = "scp -r $inst_dir root@$IP:/home/$inst_user; ssh root@$IP \"chown -R $inst_user:$inst_user /home/$inst_user\"";
       $logger->logProgress("   TRANSFER CMD: $cmd");
       if (!$local_key){
         $shell_output = shell_exec($cmd);  
       }       
       $logger->logProgress("   STD CMD OUTPUT:\n" . $shell_output);
           
   }
   
   // Saving last checkpoint. 
   $logger->logProgress("Saving last checkpoint");
   $fid = fopen($previous_date_file,"w");
   fwrite($fid,json_encode($previous_sync_dates));
   fclose($fid);
   
   // The actual synching. 
   
   $logger->logProgress("Finished");
      
?>
