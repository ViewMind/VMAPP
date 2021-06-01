<?php

include_once ("../db_management/DBCon.php");
include_once ("../db_management/TableInstitution.php");
include_once ("../db_management/TableInstitutionUsers.php");
include_once ("../db_management/TablePlacedProducts.php");
include_once ("../db_management/TablePortalUsers.php");
include_once ("../db_management/TableSecrets.php");

include("cli_interface.php");

$section_name = "create_user";
$required = [TablePortalUsers::COL_EMAIL, 
TablePortalUsers::COL_NAME, 
TablePortalUsers::COL_LASTNAME, 
TableInstitutionUsers::COL_INSTITUTION_ID,
TablePortalUsers::COL_USER_ROLE];

$params = readAndVerifyData($required,$section_name);

$dbcon = new DBCon();
$con_secure = $dbcon->dbOpenConnection(DBCon::DB_SECURE,DBCon::DB_SERVICE_ADMIN);
if ($con_secure == NULL){
   echo "Error creating db connection: " . $dbcon->getError() . "\n";
   exit();
}

$con_main = $dbcon->dbOpenConnection(DBCon::DB_MAIN,DBCon::DB_SERVICE_ADMIN);
if ($con_main == NULL){
   echo "Error creating db connection: " . $dbcon->getError() . "\n";
   exit();
}


echo "Verifying institution existance\n";

$inst_id = $params[TableInstitutionUsers::COL_INSTITUTION_ID];

$table_inst = new TableInstitution($con_main);
$ans = $table_inst->exists($inst_id);
if ($ans === false){
   echo "Error verifying on institution's existance: " . $table_inst->getError();
   exit();
}
else{
   if ($ans[0] == 1){
      echo "Institution Exists. Moving on\n";
   }
   else{
      echo "Institution $inst_id doesn't exist\n";
      exit();
   }
}

$portal_user = new TablePortalUsers($con_secure);
$role = $params[TablePortalUsers::COL_USER_ROLE];
unset($params[TableInstitutionUsers::COL_INSTITUTION_ID]);

if ($role == TablePortalUsers::ROLE_INTITUTION_ADMIN) {
    $ans = $portal_user->addInstitutionAdminRole($params);
}
else if ($role == TablePortalUsers::ROLE_MEDICAL){
   $ans = $portal_user->addMedicalRoleUser($params);
}
else{
   echo "Unknown role $role\n";
   exit();
}

if ($ans === false){
   echo "Could not create portal user: " . $portal_user->getError() . "\n";
   exit();
}
else{
   $user_id = $portal_user->getLastInserted()[0];
   echo "Created user: " . $user_id  . "\n";
}

$table_inst_users = new TableInstitutionUsers($con_main);
$ans = $table_inst_users->linkUserToInstitution($user_id,$inst_id);
if ($ans === FALSE){
   echo "Failed linking $user_id and $inst_id: " . $table_inst_users->getError() . "\n";   
}
else echo "Finished linking user $user_id and institution $inst_id\n";





?>
