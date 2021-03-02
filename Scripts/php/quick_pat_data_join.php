<?php
   
   ////////////////// Creating SQL DB Connection. 
   $con = mysqli_connect("localhost",
                         "root",
                         "givnar",
                         "viewmind_patdata");

   // Verifying the connection 
   if (mysqli_connect_errno()){  
      echo "Cannot establish connection to DB. Error code: " . mysqli_connect_errno($con) . "\n";
      return;
   }              
   // Setting the character set to avoid tilde problems. 
   mysqli_query($con,"SET NAMES 'utf8'");  

   $sql = "SELECT MAX(keyid) as keyid, puid FROM tPatientData GROUP BY puid";
   $res = mysqli_query($con,$sql);
   
   if (!$res){
      echo "ERROR in $sql\n";
      exit;
   }
   
   $keyids_to_search = [];
   while ($row = mysqli_fetch_array($res,MYSQLI_ASSOC)){   
      $keyids_to_search[] = $row["keyid"];
   }
   
   
   $sql = "SELECT sex, birthcountry, birthdate, formative_years, date_insertion, puid FROM tPatientData WHERE keyid IN (" . implode(",",$keyids_to_search) . ")";
   $res = mysqli_query($con,$sql);
   
   if (!$res){
      echo "ERROR in $sql\n";
      exit;
   }
   
   $pat_data = array();
   while ($row = mysqli_fetch_array($res,MYSQLI_ASSOC)){   
      $pat_data[$row["puid"]] = $row;
   }
   
   $sql = "SELECT MAX(keyid) as keyid, puid FROM tMedicalRecords GROUP BY puid";
   $res = mysqli_query($con,$sql);
   
   if (!$res){
      echo "ERROR in $sql\n";
      exit;
   }
   
   $keyids_to_search = [];
   while ($row = mysqli_fetch_array($res,MYSQLI_ASSOC)){   
      $keyids_to_search[] = $row["keyid"];
   }
   
   
   $sql = "SELECT date, presumptive_diagnosis, medication, RNM, evaluations, puid FROM tMedicalRecords WHERE keyid IN (" . implode(",",$keyids_to_search) . ")";
   $res = mysqli_query($con,$sql);
   
   if (!$res){
      echo "ERROR in $sql\n";
      exit;
   }

   // Joining the pat data info with the medical record info. 
   $mrecs = array();
   $puid_for_results = [];
   while ($row = mysqli_fetch_array($res,MYSQLI_ASSOC)){   
      $pd = $pat_data[$row["puid"]];
      $mrecs[] = array_merge($pd,$row);
      $puid_for_results[] = $row["puid"];
   }
   
   //var_dump($mrecs);
   //return;
   
   $fields_to_exclude = ["keyid"];
   $fid = fopen("medrecs.csv","w+");
   
   $to_include = ["puid", "sex", "birthcountry", "birthdate", "formative_years", "date_insertion", "date", "medication", "RNM", "evaluations", "presumptive_diagnosis"];
   fwrite($fid,"'" . implode("';'",$to_include) . "'\n");
   
   foreach($mrecs as $mr){
      $row = array();
      foreach ($to_include as $col){
         $row[] = "'" . $mr[$col] . "'";
      }
      fwrite($fid,implode(";",$row) . "\n");
   }
   
   fclose($fid);
   
   // Getting the results for tEyeDataTable, for that we need to open another connection. 
   mysqli_close($con);
   ////////////////// Creating SQL DB Connection. 
   $con = mysqli_connect("localhost",
                         "root",
                         "givnar",
                         "viewmind_data");

   // Verifying the connection 
   if (mysqli_connect_errno()){  
      echo "Cannot establish connection to DB. Error code: " . mysqli_connect_errno($con) . "\n";
      return;
   }              
   // Setting the character set to avoid tilde problems. 
   mysqli_query($con,"SET NAMES 'utf8'");  
   
   $sql = "SELECT * FROM tEyeResults WHERE puid IN (" . implode(",",$puid_for_results) .  ")";
   $res = mysqli_query($con,$sql);
   if (!$res){
      echo "ERROR in $sql\n";
      exit;
   }
   
   $keys = ['puid','study_id','study_date','protocol','rdTotalFixL','rdFStepFixL','rdMStepFixL','rdSStepFixL','rdTotalFixR','rdFStepFixR','rdMStepFixR','rdSStepFixR','bcAvgPupilR','ucAvgPupilR','bcAvgPupilL','ucAvgPupilL','bcGazeResR','ucGazeResR','bcGazeResL','ucGazeResL','bcCorrectAns','ucCorrentAns','bcWrongAns','ucWrongAns','bcTestCorrectAns','ucTestCorrectAns','bcTestWrongAns','ucTestWrongAns','fdHitR','fdHitL','fdHitTotal','ferror','binding_index_bc','binding_index_uc','cognitive_impairment_index','evaluation_id','client_study_date','nbrt_fix_enc','nbrt_fix_ret','nbrt_inhib_prob','nbrt_seq_complete','nbrt_target_hit','nbrt_mean_resp_time','nbrt_mean_sac_amp','gng_speed_processing','gng_dmt_interference','gng_dmt_facilitate','gng_pip_faciliatate','gng_pip_interference'];   
   $fid = fopen("results.csv","w+");
   fwrite($fid,"'" . implode("';'",$keys) . "'\n");
   while ($row = mysqli_fetch_array($res,MYSQLI_ASSOC)){   
      $csv_row = array();
      foreach ($keys as $k){
         $csv_row[] = "'" . $row[$k] . "'";
      }
      fwrite($fid,implode(";",$csv_row) . "\n");
   }
   fclose($fid);
   

?>
