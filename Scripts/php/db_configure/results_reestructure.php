<?php

   // Creating connection. 
   $credentials = parse_ini_file("credentials.cnf");
   $con = mysqli_connect($credentials["host"],$credentials["user"],$credentials["password"],$credentials["database"]);   
   if (!$con){
      echo "Could not create DB connection: " . mysqli_connect_error() . "\n";
      echo "Error number: " . mysqli_connect_errno() . "\n";
      return;
   }
   
   // Making sure tEyeResultsOld exists before running the script. 
   $sql = "SELECT * FROM tEyeResults";
   $eye_results = mysqli_query($con,$sql);
   if (!$eye_results){
      echo "DBERROR: " . mysqli_error($con) . " FROM $sql\n";
      return;
   }
   
   echo "CREATING NEW COLUMN.....\n";
   
//    // Adding a new column to the tEyeResults table. 
//    $sql = "ALTER TABLE tEyeResults DROP IF EXISTS results";
//    if (!mysqli_query($con,$sql)){
//       echo "DBERROR: " . mysqli_error($con) . " FROM $sql\n";
//       return;
//    }
   
   
   // Adding a new column to the tEyeResults table. 
   $sql = "ALTER TABLE tEyeResults ADD results JSON";
   if (!mysqli_query($con,$sql)){
      echo "DBERROR: " . mysqli_error($con) . " FROM $sql\n";
      return;
   }
   
   ////////////////////////////////////////////////////////////
   echo "PROCESSING OLD RESULTS.....\n";
   
   
   $rd_cols    = ["rdTotalFixL","rdFStepFixL","rdMStepFixL","rdSStepFixL","rdTotalFixR","rdFStepFixR","rdMStepFixR","rdSStepFixR","cognitive_impairment_index"];
   $bd_cols    = ["bcAvgPupilR","ucAvgPupilR","bcAvgPupilL","ucAvgPupilL","bcGazeResR","ucGazeResR","bcGazeResL","ucGazeResL","bcCorrectAns","ucCorrentAns","bcWrongAns","ucWrongAns","bcTestCorrectAns","ucTestCorrectAns","bcTestWrongAns","ucTestWrongAns","binding_index_bc","binding_index_uc"];
   $nbrt_cols  = ["nbrt_fix_enc","nbrt_fix_ret","nbrt_inhib_prob","nbrt_seq_complete","nbrt_target_hit","nbrt_mean_resp_time","nbrt_mean_sac_amp"];
   $gng_cols   = ["gng_speed_processing","gng_dmt_interference","gng_dmt_facilitate","gng_pip_faciliatate","gng_pip_interference"];
   $study_cols = ["doctorid","study_id","study_date","puid","protocol","ferror","evaluation_id","client_study_date"];
   
   $NAME_FIELDING = "fielding";
   $NAME_NBACKRT = "nbackrt";
   $NAME_NBACKVS = "nbackvs";
   $NAME_BIND_UC = "binding_uc";
   $NAME_BIND_BC = "binding_bc";
   $NAME_BINDING  = "binding";
   $NAME_READING = "reading";
   $NAME_PARKINSON = "parkinson";
   $NAME_GONOGO = "gonogo";
   $NAME_PERCEPTION = "perception";
      
   while ($row = mysqli_fetch_array($eye_results,MYSQLI_ASSOC)){
      $study_id = $row["study_id"];
      $keyid    = $row["keyid"];

      $result = [];
      
      if ($study_id == "") continue;
      
      if (strpos($study_id,"rd") !== FALSE){      
         // Reading experiment is present
         $result[$NAME_READING] = array();
         foreach ($rd_cols as $col){
            $result[$NAME_READING][$col] = $row[$col];   
         }
      }
      if (strpos($study_id,"bc") !== FALSE){      
         // Bindin is present
         $result[$NAME_BINDING] = array();
         foreach ($bd_cols as $col){
            $result[$NAME_BINDING][$col] = $row[$col];   
         }
      }
      if (strpos($study_id,"nbrt") !== FALSE){      
         $result[$NAME_NBACKRT] = array();
         foreach ($nbrt_cols as $col){
            $result[$NAME_NBACKRT][$col] = $row[$col];   
         }
      }
      if (strpos($study_id,"gng") !== FALSE){      
         $result[$NAME_GONOGO] = array();
         foreach ($gng_cols as $col){
            $result[$NAME_GONOGO][$col] = $row[$col];   
         }
      } 

      $data = json_encode($result);
      $sql = "UPDATE tEyeResults SET results = '$data' WHERE keyid = $keyid";
      if (!mysqli_query($con,$sql)){
         echo "UPDATE error: " . mysqli_error($con). " FROM SQL: $sql\n";
         exit;
      }
      
   }
   
   if (count($argv) > 1){
      if ($argv[1] == "do_the_drop"){
         echo "Dropping all columns\n";
         $sql = "ALTER TABLE tEyeResults DROP ";
         
         $all_cols = array_merge($rd_cols,$bd_cols);
         $all_cols = array_merge($all_cols,$gng_cols);
         $all_cols = array_merge($all_cols,$nbrt_cols);
         $all_cols[] = "fdHitR";
         $all_cols[] = "fdHitL";
         $all_cols[] = "fdHitTotal";
         
         $sql = $sql . implode(", DROP ", $all_cols);
         if (!mysqli_query($con,$sql)){
            echo "DROP COL error: " . mysqli_error($con). " FROM SQL: $sql\n";
            exit;
         }      
         
         return;
      }
   }
   
   else{
      echo "Parameter do_the_drop was not passed. Finishing\n";
   }
   
      
   
?>
