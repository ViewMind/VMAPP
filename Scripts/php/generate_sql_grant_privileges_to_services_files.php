<?php
  
  $DB_DATA     = "viewmind_data";
  $DB_PATID    = "viewmind_id";
  $DB_DASH     = "viewmind_dashboard";
  $DB_PATDATA  = "viewmind_patdata";
  
  $HOST        = "localhost";
  
  $SERVICE_EYE_DB_MANAGER     = "srvc_eye_db_manager";
  $PASS_SRV_EYE_DB_MANAGER    = "2!PL0iJ34YK@0c%!ls0d";
  
  $SERVICE_DASHBOARD          = "srvc_dashboard";
  $PASS_SRV_DASHBOARD         = "Fh&2Q@9jO!X668%7twtT";
  
  $SERVICE_RAW_DATA_SERVER    = "srvc_rawdata_server";
  $PASS_SRV_RAW_DATA_SERVER   = "_VBg55!WjIh=3700tdI?";
  
  $TABLE_COMPLAINTS           = "tComplaints";         
  $TABLE_EVALUATIONS          = "tEvaluations";        
  $TABLE_INSTITUTIONS         = "tInstitutions";       
  $TABLE_MONTH_COUNTERS       = "tMonthCounters";      
  $TABLE_PRODUCTION           = "tProduction";         
  $TABLE_PURCHASES            = "tPurchases";          
  $TABLE_STOCK_MP             = "tStockMP";            
  $TABLE_STRINGS              = "tStrings";            
  $TABLE_SUPPLIERS            = "tSuppliers";          
  $TABLE_TECHNICAL_ASSISTANCE = "tTechnicalAssistance";
  $TABLE_TRAINED_USERS        = "tTrainedUsers";       
  $TABLE_USERS                = "tUsers";            
  
  $TABLE_DOCTORS              = "tDoctors";
  $TABLE_EYE_RESULTS          = "tEyeResults";
  $TABLE_FDATA                = "tFData";          
  $TABLE_PERFORMANCE          = "tPerformance";    
  $TABLE_RESULTS_BINDING      = "tResultsBinding"; 
  $TABLE_RESULTS_GONOGO       = "tResultsGoNoGo";
  $TABLE_RESULTS_NBACKRT      = "tResultsNBackRT"; 
  $TABLE_RESULTS_READING      = "tResultsReading"; 
  $TABLE_STUDIES_IN_REPORT    = "tStudiesInReport";
  
  $TABLE_PATIENT_IDS          = "tPatientIDs";
  
  $TABLE_MEDICAL_RECORDS      = "tMedicalRecords";
  $TABLE_PATIENT_DATA         = "tPatientData";
  
  // Define the services and privileges
  $services = array();
  
  // PRIVILEGES 
  $PRIV_SELECT = "SELECT";
  $PRIV_INSERT = "INSERT";
  $PRIV_UPDATE = "UPDATE";
  
  $srv = array();
  $srv["mysql_user"] = $SERVICE_EYE_DB_MANAGER;
  $srv["mysql_pass"] = $PASS_SRV_EYE_DB_MANAGER;

  $srv["table_privileges"][$DB_DATA][$TABLE_DOCTORS]             = [$PRIV_INSERT,$PRIV_SELECT];
  $srv["table_privileges"][$DB_DATA][$TABLE_EYE_RESULTS]         = [$PRIV_INSERT,$PRIV_SELECT];
  $srv["table_privileges"][$DB_DATA][$TABLE_FDATA]               = [$PRIV_INSERT];
  $srv["table_privileges"][$DB_DATA][$TABLE_PERFORMANCE]         = [$PRIV_INSERT,$PRIV_SELECT];
  $srv["table_privileges"][$DB_DATA][$TABLE_RESULTS_BINDING]     = [$PRIV_INSERT,$PRIV_SELECT];
  $srv["table_privileges"][$DB_DATA][$TABLE_RESULTS_GONOGO]      = [$PRIV_INSERT,$PRIV_SELECT];
  $srv["table_privileges"][$DB_DATA][$TABLE_RESULTS_NBACKRT]     = [$PRIV_INSERT,$PRIV_SELECT];
  $srv["table_privileges"][$DB_DATA][$TABLE_RESULTS_READING]     = [$PRIV_INSERT,$PRIV_SELECT];
  $srv["table_privileges"][$DB_DATA][$TABLE_STUDIES_IN_REPORT]   = [$PRIV_INSERT];
                                                                 
  $srv["table_privileges"][$DB_DASH][$TABLE_EVALUATIONS]         = [$PRIV_SELECT];
  $srv["table_privileges"][$DB_DASH][$TABLE_INSTITUTIONS]        = [$PRIV_SELECT];
                                                                 
  $srv["table_privileges"][$DB_PATID][$TABLE_PATIENT_IDS]        = [$PRIV_INSERT];
                                                                 
  $srv["table_privileges"][$DB_PATDATA][$TABLE_MEDICAL_RECORDS]  = [$PRIV_INSERT];
  $srv["table_privileges"][$DB_PATDATA][$TABLE_PATIENT_DATA]     = [$PRIV_INSERT];
  
  $services[] = $srv;
  
  $srv = array();
  $srv["mysql_user"] = $SERVICE_DASHBOARD;
  $srv["mysql_pass"] = $PASS_SRV_DASHBOARD;
  
  $srv["table_privileges"][$DB_DASH][$TABLE_COMPLAINTS]             = [$PRIV_INSERT,$PRIV_SELECT];
  $srv["table_privileges"][$DB_DASH][$TABLE_MONTH_COUNTERS]         = [$PRIV_INSERT,$PRIV_SELECT,$PRIV_UPDATE];
  $srv["table_privileges"][$DB_DASH][$TABLE_PRODUCTION]             = [$PRIV_INSERT,$PRIV_SELECT];
  $srv["table_privileges"][$DB_DASH][$TABLE_PURCHASES]              = [$PRIV_INSERT,$PRIV_SELECT];
  $srv["table_privileges"][$DB_DASH][$TABLE_STRINGS]                = [$PRIV_INSERT,$PRIV_SELECT];
  $srv["table_privileges"][$DB_DASH][$TABLE_SUPPLIERS]              = [$PRIV_INSERT,$PRIV_SELECT];
  $srv["table_privileges"][$DB_DASH][$TABLE_TECHNICAL_ASSISTANCE]   = [$PRIV_INSERT,$PRIV_SELECT];  
  $srv["table_privileges"][$DB_DASH][$TABLE_TRAINED_USERS]          = [$PRIV_INSERT,$PRIV_SELECT];
  $srv["table_privileges"][$DB_DASH][$TABLE_USERS]                  = [$PRIV_INSERT,$PRIV_SELECT,$PRIV_UPDATE];  
  $srv["table_privileges"][$DB_DASH][$TABLE_EVALUATIONS]            = [$PRIV_SELECT,$PRIV_INSERT,$PRIV_UPDATE];
  $srv["table_privileges"][$DB_DASH][$TABLE_INSTITUTIONS]           = [$PRIV_SELECT,$PRIV_INSERT,$PRIV_UPDATE];
  
  $srv["table_privileges"][$DB_DATA][$TABLE_DOCTORS]                = [$PRIV_INSERT,$PRIV_SELECT];
  $srv["table_privileges"][$DB_DATA][$TABLE_EYE_RESULTS]            = [$PRIV_SELECT];
  $srv["table_privileges"][$DB_DATA][$TABLE_FDATA]                  = [$PRIV_SELECT];
  $srv["table_privileges"][$DB_DATA][$TABLE_PERFORMANCE]            = [$PRIV_SELECT];
  $srv["table_privileges"][$DB_DATA][$TABLE_RESULTS_BINDING]        = [$PRIV_SELECT];
  $srv["table_privileges"][$DB_DATA][$TABLE_RESULTS_GONOGO]         = [$PRIV_SELECT];
  $srv["table_privileges"][$DB_DATA][$TABLE_RESULTS_NBACKRT]        = [$PRIV_SELECT];
  $srv["table_privileges"][$DB_DATA][$TABLE_RESULTS_READING]        = [$PRIV_SELECT];
  $srv["table_privileges"][$DB_DATA][$TABLE_STUDIES_IN_REPORT]      = [$PRIV_SELECT];  

  $services[] = $srv;
    
  $srv = array();
  $srv["mysql_user"] = $SERVICE_RAW_DATA_SERVER;
  $srv["mysql_pass"] = $PASS_SRV_RAW_DATA_SERVER;
    
  $srv["table_privileges"][$DB_DATA][$TABLE_DOCTORS]                = [$PRIV_SELECT];
  $srv["table_privileges"][$DB_DATA][$TABLE_EYE_RESULTS]            = [$PRIV_SELECT];
  $srv["table_privileges"][$DB_DATA][$TABLE_FDATA]                  = [$PRIV_SELECT];
  $srv["table_privileges"][$DB_DATA][$TABLE_PERFORMANCE]            = [$PRIV_SELECT];
  $srv["table_privileges"][$DB_DATA][$TABLE_RESULTS_BINDING]        = [$PRIV_SELECT];
  $srv["table_privileges"][$DB_DATA][$TABLE_RESULTS_GONOGO]         = [$PRIV_SELECT];
  $srv["table_privileges"][$DB_DATA][$TABLE_RESULTS_NBACKRT]        = [$PRIV_SELECT];
  $srv["table_privileges"][$DB_DATA][$TABLE_RESULTS_READING]        = [$PRIV_SELECT];
  $srv["table_privileges"][$DB_DATA][$TABLE_STUDIES_IN_REPORT]      = [$PRIV_SELECT];  
  
  $srv["table_privileges"][$DB_PATID][$TABLE_PATIENT_IDS]           = [$PRIV_SELECT];
                                                                    
  $srv["table_privileges"][$DB_PATDATA][$TABLE_MEDICAL_RECORDS]     = [$PRIV_SELECT];
  $srv["table_privileges"][$DB_PATDATA][$TABLE_PATIENT_DATA]        = [$PRIV_SELECT];
  
  $services[] = $srv;  
  
  $flag_local = is_file("local_flag_file");
  
  // Generating the sql statementes for each database.
  $sql_statements[$DB_DASH]    = array();
  $sql_statements[$DB_DATA]    = array();
  $sql_statements[$DB_PATDATA] = array();
  $sql_statements[$DB_PATID]   = array();
  
  foreach ($services as $srv){
 
     $user["DROP"]   = "DROP USER IF EXISTS '" . $srv["mysql_user"] . "'@'$HOST';";
     $user["CREATE"] = "CREATE USER '" . $srv["mysql_user"] . "'@'$HOST' IDENTIFIED BY '" . $srv["mysql_pass"] . "';";
     
     foreach ($srv["table_privileges"] as $db_name => $tables){
        
        $user["GRANTS"] = array();
        
        //echo $srv["mysql_user"] . " ==> " . $db_name . "\n";
        foreach ($tables as $table_name => $privileges){
           //echo "TABLE: $table_name\n";
           $user["GRANTS"][] = "GRANT " . implode(",",$privileges) . " ON $db_name.$table_name TO '" . $srv["mysql_user"] . "'@'$HOST';"; 
        }
        
        $sql_statements[$db_name][$srv["mysql_user"]] = $user;           
     }
     
  }
  
  if ($flag_local){
    // Locally there are N Database in the same server all of them starting with iv_
    $fid = fopen("privileges.sql","w");
    
    // Adding only the drop and creation statements at first.
    foreach ($sql_statements as $db_name => $users){
      foreach($users as $user => $struct){
         fwrite($fid,$struct["DROP"] . "\n");
         fwrite($fid,"FLUSH PRIVILEGES;\n");
         fwrite($fid,$struct["CREATE"] . "\n");
      }
    }
    
    // Now adding all privileges. 
    foreach ($sql_statements as $db_name => $users){
      foreach($users as $user => $struct){
         fwrite($fid,implode("\n",$struct["GRANTS"]) . "\n");
      }
    }    
    
    fclose($fid);
  }
  else{
    // Remotely each Database can even be in their own separate server. We need a file for each server (Database).
    
    foreach ($sql_statements as $db_name => $users){
    
      $fid = fopen("privileges-$db_name.sql","w");
    
      foreach($users as $user => $struct){
         fwrite($fid,$struct["DROP"] . "\n");
         fwrite($fid,"FLUSH PRIVILEGES;\n");
         fwrite($fid,$struct["CREATE"] . "\n");
         fwrite($fid,implode("\n",$struct["GRANTS"]) . "\n");
      }
      
      fclose($fid);
    }
    
  
  }
  
  //var_dump($sql_statements);
  
  
  
  
?>
