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
   $sql = "SELECT * FROM tEyeResultsOld";
   $res = mysqli_query($con,$sql);
   if (!$res){
      echo "DBERROR: " . mysqli_error($con) . " FROM $sql\n";
      return;
   }
   
   echo "CREATING NEW TABLES.....\n";
   
   // Dropping new tables if they exist. 
   $sql = "DROP TABLE IF EXISTS tEyeResults";
   if (!mysqli_query($con,$sql)){
      echo "DBERROR: " . mysqli_error($con) . " FROM $sql\n";
      return;
   }
   
   $sql = "DROP TABLE IF EXISTS tResultsReading";
   if (!mysqli_query($con,$sql)){
      echo "DBERROR: " . mysqli_error($con) . " FROM $sql\n";
      return;
   }
   
   $sql = "DROP TABLE IF EXISTS tResultsBinding";
   if (!mysqli_query($con,$sql)){
      echo "DBERROR: " . mysqli_error($con) . " FROM $sql\n";
      return;
   }
   
   $sql = "DROP TABLE IF EXISTS tResultsNBackRT";
   if (!mysqli_query($con,$sql)){
      echo "DBERROR: " . mysqli_error($con) . " FROM $sql\n";
      return;
   }
   
   $sql = "DROP TABLE IF EXISTS tResultsGoNoGo";
   if (!mysqli_query($con,$sql)){
      echo "DBERROR: " . mysqli_error($con) . " FROM $sql\n";
      return;
   }
   
   $sql = "DROP TABLE IF EXISTS tStudiesInReport";
   if (!mysqli_query($con,$sql)){
      echo "DBERROR: " . mysqli_error($con) . " FROM $sql\n";
      return;
   }   

   // Creating all new tables. 
   $sql = "CREATE TABLE tEyeResults (
           keyid                INT AUTO_INCREMENT PRIMARY KEY,
           doctorid             VARCHAR(255),
           study_id             VARCHAR(255),
           study_date           DATETIME DEFAULT CURRENT_TIMESTAMP,
           puid                 INT,
           protocol             VARCHAR(255),
           ferror               TINYINT DEFAULT 0,
           evaluation_id        VARCHAR(50),
           client_study_date    DATETIME)";      
           
   if (!mysqli_query($con,$sql)){
      echo "DBERROR: " . mysqli_error($con) . " FROM $sql\n";
      return;
   }           
   
   $sql = "CREATE TABLE tResultsReading (
           keyid                      INT AUTO_INCREMENT PRIMARY KEY,
           rdTotalFixL                INT,
           rdFStepFixL                INT,
           rdMStepFixL                INT,
           rdSStepFixL                INT,
           rdTotalFixR                INT,
           rdFStepFixR                INT,   
           rdMStepFixR                INT,   
           rdSStepFixR                INT,     
           cognitive_impairment_index FLOAT
   )";      
           
   if (!mysqli_query($con,$sql)){
      echo "DBERROR: " . mysqli_error($con) . " FROM $sql\n";
      return;
   }     
   
   $sql = "CREATE TABLE tResultsBinding (
           keyid                      INT AUTO_INCREMENT PRIMARY KEY,
           bcAvgPupilR                FLOAT,      
           ucAvgPupilR                FLOAT,      
           bcAvgPupilL                FLOAT,      
           ucAvgPupilL                FLOAT,      
           bcGazeResR                 FLOAT,      
           ucGazeResR                 FLOAT,      
           bcGazeResL                 FLOAT,      
           ucGazeResL                 FLOAT,      
           bcCorrectAns               TINYINT, 
           ucCorrentAns               TINYINT, 
           bcWrongAns                 TINYINT, 
           ucWrongAns                 TINYINT, 
           bcTestCorrectAns           TINYINT, 
           ucTestCorrectAns           TINYINT, 
           bcTestWrongAns             TINYINT, 
           ucTestWrongAns             TINYINT, 
           binding_index_bc           FLOAT,      
           binding_index_uc           FLOAT                 
   )";      
           
   if (!mysqli_query($con,$sql)){
      echo "DBERROR: " . mysqli_error($con) . " FROM $sql\n";
      return;
   }   

   $sql = "CREATE TABLE tResultsNBackRT (
           keyid                      INT AUTO_INCREMENT PRIMARY KEY,
           nbrt_fix_enc               INT,
           nbrt_fix_ret               INT,
           nbrt_inhib_prob            FLOAT,  
           nbrt_seq_complete          FLOAT,  
           nbrt_target_hit            FLOAT,  
           nbrt_mean_resp_time        FLOAT,  
           nbrt_mean_sac_amp          FLOAT                 
   )";      
           
   if (!mysqli_query($con,$sql)){
      echo "DBERROR: " . mysqli_error($con) . " FROM $sql\n";
      return;
   }
   
   $sql = "CREATE TABLE tResultsGoNoGo (
           keyid                      INT AUTO_INCREMENT PRIMARY KEY,
           gng_speed_processing       FLOAT,
           gng_dmt_interference       FLOAT,
           gng_dmt_facilitate         FLOAT,
           gng_pip_faciliatate        FLOAT,
           gng_pip_interference       FLOAT
   )";  
   if (!mysqli_query($con,$sql)){
      echo "DBERROR: " . mysqli_error($con) . " FROM $sql\n";
      return;
   }   
   
   $sql = "CREATE TABLE tStudiesInReport (
           keyid                      INT AUTO_INCREMENT PRIMARY KEY,
           evaluation_id              INT,
           study_type                 INT,
           study_keyid                INT
   )";
           
   if (!mysqli_query($con,$sql)){
      echo "DBERROR: " . mysqli_error($con) . " FROM $sql\n";
      return;
   } 
   
   ////////////////////////////////////////////////////////////
   function insertIntoTableAndGetID($con,$table,$data,$cols){
      $values = array();
      $cols_to_insert = array();
      foreach($cols as $c){
         if (empty($data[$c])) continue;
         $values[] = "'" .  $data[$c]  . "'";
         $cols_to_insert[] = $c;
      }
      $sql = "INSERT INTO $table (" . implode(",", $cols_to_insert) . ") VALUES (" . implode(",", $values) . ")";
      if (!mysqli_query($con,$sql)){
         echo "INSERTION DBERROR: " . mysqli_error($con) . " FROM $sql\n";
         return -1;
      }       
      return mysqli_insert_id($con);
   }
   
   
   ////////////////////////////////////////////////////////////
   echo "PROCESSING OLD RESULTS.....\n";
   
   $sql = "SELECT * FROM tEyeResultsOld";
   $res = mysqli_query($con,$sql);
   if (!$res){
      echo "DBERROR: " . mysqli_error($con) . " FROM $sql\n";
      return;
   }
   
   $rd_cols   = ["rdTotalFixL","rdFStepFixL","rdMStepFixL","rdSStepFixL","rdTotalFixR","rdFStepFixR","rdMStepFixR","rdSStepFixR","cognitive_impairment_index"];
   $bd_cols   = ["bcAvgPupilR","ucAvgPupilR","bcAvgPupilL","ucAvgPupilL","bcGazeResR","ucGazeResR","bcGazeResL","ucGazeResL","bcCorrectAns","ucCorrentAns","bcWrongAns","ucWrongAns","bcTestCorrectAns","ucTestCorrectAns","bcTestWrongAns","ucTestWrongAns","binding_index_bc","binding_index_uc"];
   $nbrt_cols = ["nbrt_fix_enc","nbrt_fix_ret","nbrt_inhib_prob","nbrt_seq_complete","nbrt_target_hit","nbrt_mean_resp_time","nbrt_mean_sac_amp"];
   $gng_cols  = ["gng_speed_processing","gng_dmt_interference","gng_dmt_facilitate","gng_pip_faciliatate","gng_pip_interference"];
   $study_cols = ["doctorid","study_id","study_date","puid","protocol","ferror","evaluation_id","client_study_date"];
      
   $ID_RD   = 0;
   $ID_BD   = 1;
   $ID_NBRT = 4;
   $ID_GNG  = 5;
   
   while ($row = mysqli_fetch_array($res,MYSQLI_ASSOC)){
      $study_id = $row["study_id"];

      $restuls_id[$ID_RD]    = -1;
      $restuls_id[$ID_BD]    = -1;
      $restuls_id[$ID_NBRT]    = -1;
      $restuls_id[$ID_GNG]    = -1;
      $insert_study = false;
      
      if ($study_id == "") continue;
      
      if (strpos($study_id,"rd") !== FALSE){      
         $restuls_id[$ID_RD] = insertIntoTableAndGetID($con,"tResultsReading",$row, $rd_cols);  
         if ($restuls_id[$ID_RD] == -1) return;
         $insert_study = true;
      }
      if (strpos($study_id,"bc") !== FALSE){      
         $restuls_id[$ID_BD] = insertIntoTableAndGetID($con,"tResultsBinding",$row, $bd_cols);  
         if ($restuls_id[$ID_BD] == -1) return;
         $insert_study = true;
      }
      if (strpos($study_id,"nbrt") !== FALSE){      
         $restuls_id[$ID_NBRT] = insertIntoTableAndGetID($con,"tResultsNBackRT",$row, $nbrt_cols);  
         if ($restuls_id[$ID_NBRT] == -1) return;
         $insert_study = true;
      }
      if (strpos($study_id,"gng") !== FALSE){      
         $restuls_id[$ID_GNG] = insertIntoTableAndGetID($con,"tResultsGoNoGo",$row, $gng_cols);  
         if ($restuls_id[$ID_GNG] == -1) return;
         $insert_study = true;
      } 
      
      if ($insert_study){
         $study_keyid = insertIntoTableAndGetID($con,"tEyeResults",$row,$study_cols);
         if ($study_keyid == -1) return;
         
         foreach ($restuls_id as $study => $keyid){
            if ($keyid == -1) continue;
            $sql = "INSERT INTO tStudiesInReport (evaluation_id,study_type,study_keyid) VALUES('$study_keyid','$study','$keyid')";
            if (!mysqli_query($con,$sql)){
               echo "DBERROR: " . mysqli_error($con) . " FROM $sql\n";
               return -1;
            }                   
         }
      }
      
   }
      
   
?>

