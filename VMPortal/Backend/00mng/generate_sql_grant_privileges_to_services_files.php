<?php
  
  include ("../db_management/DBCon.php");
  include ("../db_management/TableInstitution.php");
  include ("../db_management/TableInstitutionUsers.php");
  include ("../db_management/TablePlacedProducts.php");
  include ("../db_management/TablePortalUsers.php");
  include ("../db_management/TableSecrets.php");
  include ("../db_management/TableProcessingParameters.php");
  include ("../db_management/TableEvaluations.php");
  include ("../db_management/TableSubject.php");
  include ("../db_management/TableUpdates.php");
  include ("../db_management/TableAppPasswordRecovery.php");
  include ("../db_management/TableMedicalRecords.php");

  // Setting the pointer location for db conf
  DBCon::setPointerLocation("configs");  

  // Loading the database configuration to create. 
  $dbcon = new DBCon();
  if ($dbcon->getError() != ""){
     echo "Error loading DB Data: " . $dbcon->getError() . "\n";
     exit();
  }
  $crendentials = $dbcon->getCredentials();
  
  // Define the services and privileges
  $services = array();
  
  // PRIVILEGES 
  $PRIV_SELECT = "SELECT";
  $PRIV_INSERT = "INSERT";
  $PRIV_UPDATE = "UPDATE";
 
  // Data Processing Server
  $srv = array();
  $srv["mysql_user"] = $crendentials[DBcon::DBCNF_MAIN_SRV][DBCon::DB_SERVICE_DP][DBCon::DBCNF_SRV_USERNAME];
  $srv["mysql_pass"] = $crendentials[DBcon::DBCNF_MAIN_SRV][DBCon::DB_SERVICE_DP][DBCon::DBCNF_SRV_PASSWORD];

  $srv["table_privileges"][TableInstitution::IN_DB][TableInstitution::TABLE_NAME]                         = [$PRIV_SELECT];
  $srv["table_privileges"][TableInstitutionUsers::IN_DB][TableInstitutionUsers::TABLE_NAME]               = [$PRIV_SELECT,$PRIV_INSERT];
  $srv["table_privileges"][TablePlacedProducts::IN_DB][TablePlacedProducts::TABLE_NAME]                   = [$PRIV_SELECT];
  $srv["table_privileges"][TablePortalUsers::IN_DB][TablePortalUsers::TABLE_NAME]                         = [$PRIV_SELECT,$PRIV_INSERT];
  $srv["table_privileges"][TableSecrets::IN_DB][TableSecrets::TABLE_NAME]                                 = [$PRIV_SELECT];
  $srv["table_privileges"][TableProcessingParameters::IN_DB][TableProcessingParameters::TABLE_NAME]       = [$PRIV_SELECT];
  $srv["table_privileges"][TableEvaluations::IN_DB][TableEvaluations::TABLE_NAME]                         = [$PRIV_INSERT];
  $srv["table_privileges"][TableSubject::IN_DB][TableSubject::TABLE_NAME]                                 = [$PRIV_INSERT, $PRIV_SELECT, $PRIV_UPDATE];
  $srv["table_privileges"][TableUpdates::IN_DB][TableUpdates::TABLE_NAME]                                 = [$PRIV_SELECT];
  $srv["table_privileges"][TableAppPasswordRecovery::IN_DB][TableAppPasswordRecovery::TABLE_NAME]         = [$PRIV_SELECT];
  
  $services[] = $srv;

  // Administrator scripts. 
  $srv["mysql_user"] = $crendentials[DBcon::DBCNF_MAIN_SRV][DBCon::DB_SERVICE_ADMIN][DBCon::DBCNF_SRV_USERNAME];
  $srv["mysql_pass"] = $crendentials[DBcon::DBCNF_MAIN_SRV][DBCon::DB_SERVICE_ADMIN][DBCon::DBCNF_SRV_PASSWORD];

  $srv["table_privileges"][TableInstitution::IN_DB][TableInstitution::TABLE_NAME]                         = [$PRIV_SELECT, $PRIV_INSERT, $PRIV_UPDATE];
  $srv["table_privileges"][TableInstitutionUsers::IN_DB][TableInstitutionUsers::TABLE_NAME]               = [$PRIV_SELECT, $PRIV_INSERT, $PRIV_UPDATE];
  $srv["table_privileges"][TablePlacedProducts::IN_DB][TablePlacedProducts::TABLE_NAME]                   = [$PRIV_SELECT, $PRIV_INSERT, $PRIV_UPDATE];
  $srv["table_privileges"][TablePortalUsers::IN_DB][TablePortalUsers::TABLE_NAME]                         = [$PRIV_SELECT, $PRIV_INSERT, $PRIV_UPDATE];
  $srv["table_privileges"][TableProcessingParameters::IN_DB][TableProcessingParameters::TABLE_NAME]       = [$PRIV_SELECT, $PRIV_INSERT, $PRIV_UPDATE];
  $srv["table_privileges"][TableSecrets::IN_DB][TableSecrets::TABLE_NAME]                                 = [$PRIV_SELECT, $PRIV_INSERT, $PRIV_UPDATE];
  $srv["table_privileges"][TableUpdates::IN_DB][TableUpdates::TABLE_NAME]                                 = [$PRIV_SELECT, $PRIV_INSERT, $PRIV_UPDATE];
  $srv["table_privileges"][TableAppPasswordRecovery::IN_DB][TableAppPasswordRecovery::TABLE_NAME]         = [$PRIV_SELECT, $PRIV_INSERT, $PRIV_UPDATE];
  $services[] = $srv;
  
  // Partner privileges. 
  $srv["mysql_user"] = $crendentials[DBcon::DBCNF_MAIN_SRV][DBCon::DB_SERVICE_PARTNERS][DBCon::DBCNF_SRV_USERNAME];
  $srv["mysql_pass"] = $crendentials[DBcon::DBCNF_MAIN_SRV][DBCon::DB_SERVICE_PARTNERS][DBCon::DBCNF_SRV_PASSWORD];

  $srv["table_privileges"][TableInstitution::IN_DB][TableInstitution::TABLE_NAME]                         = [$PRIV_SELECT ];
  $srv["table_privileges"][TableInstitutionUsers::IN_DB][TableInstitutionUsers::TABLE_NAME]               = [$PRIV_SELECT ];
  //$srv["table_privileges"][TablePlacedProducts::IN_DB][TablePlacedProducts::TABLE_NAME]                   = [$PRIV_SELECT, $PRIV_INSERT, $PRIV_UPDATE];
  $srv["table_privileges"][TablePortalUsers::IN_DB][TablePortalUsers::TABLE_NAME]                         = [$PRIV_SELECT, $PRIV_UPDATE];
  $srv["table_privileges"][TableEvaluations::IN_DB][TableEvaluations::TABLE_NAME]                         = [$PRIV_SELECT];
  //$srv["table_privileges"][TableProcessingParameters::IN_DB][TableProcessingParameters::TABLE_NAME]       = [$PRIV_SELECT, $PRIV_INSERT, $PRIV_UPDATE];
  //$srv["table_privileges"][TableSecrets::IN_DB][TableSecrets::TABLE_NAME]                                 = [$PRIV_SELECT, $PRIV_INSERT, $PRIV_UPDATE];
  $srv["table_privileges"][TableMedicalRecords::IN_DB][TableMedicalRecords::TABLE_NAME]                   = [$PRIV_SELECT, $PRIV_INSERT, $PRIV_UPDATE];
  $services[] = $srv;

  
  $flag_local = is_file("local_flag_file");
  
  // Generating the sql statementes for each database.
  $sql_statements[DBCon::DB_MAIN]    = array();
  $sql_statements[DBCon::DB_SECURE]  = array();

  // We don't filter user via IP. It's to complicated to configure. 
  $HOST = '%';
  
  foreach ($services as $srv){
 
     $user["DROP"]   = "DROP USER IF EXISTS '" . $srv["mysql_user"] . "'@'$HOST';\n" . "DROP USER IF EXISTS '" . $srv["mysql_user"] . "'@'localhost';\n";
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
  
     echo "Universal Host\n";  
  
     echo "Running Locally\n";
     // Locally there are N Database in the same server all of them starting with iv_
     $priv_file = "privileges.sql";
     $fid = fopen($priv_file,"w");
     
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
     $cnf = "unified.cnf";
     $cmd = "mysql --defaults-extra-file=$cnf < $priv_file";
     echo "Running MySQL cmd: $cmd\n";
     shell_exec($cmd);
  }  
  else{
     
     echo "Separate Host\n";  
     
     // Remotely each Database can even be in their own separate server. We need a file for each server (Database).    
     foreach ($sql_statements as $db_name => $users){
     
       $priv_file = "privileges-$db_name.sql";       
       $fid = fopen($priv_file,"w");  
       foreach($users as $user => $struct){
          fwrite($fid,$struct["DROP"] . "\n");
          fwrite($fid,"FLUSH PRIVILEGES;\n");
          fwrite($fid,$struct["CREATE"] . "\n");
          fwrite($fid,implode("\n",$struct["GRANTS"]) . "\n");
       }    
       fclose($fid);
       
       $cnf = $db_name . ".cnf";
       $cmd = "mysql --defaults-extra-file=$cnf < $priv_file";
       echo "Running MySQL cmd: $cmd\n";
       shell_exec($cmd);  
     }
  }

  
?>
