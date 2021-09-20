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
TablePortalUsers::COL_USER_ROLE,
TablePortalUsers::COL_PASSWD
];

DBCon::setPointerLocation("configs");  
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

// If role API then an ID and password is generated. 
$role = $params[TablePortalUsers::COL_USER_ROLE];
if ($role == TablePortalUsers::ROLE_API_USER_ONLY){
   $email = generateToken(128);   
   $password = generateToken(256);
   echo "Generated ID: $email\n";
   echo "Generated Password: $password\n";
   $params[TablePortalUsers::COL_PASSWD] = $password;
   $params[TablePortalUsers::COL_EMAIL] = $email;
}

// Hashing the password. 
$password = password_hash($params[TablePortalUsers::COL_PASSWD], PASSWORD_BCRYPT, ["cost" => 10]);
$params[TablePortalUsers::COL_PASSWD] = $password;

$portal_user = new TablePortalUsers($con_secure);
unset($params[TableInstitutionUsers::COL_INSTITUTION_ID]);

if ($role == TablePortalUsers::ROLE_INTITUTION_ADMIN) {
    $ans = $portal_user->addInstitutionAdminRole($params);
}
else if ($role == TablePortalUsers::ROLE_MEDICAL){
   $ans = $portal_user->addMedicalRoleUser($params);
}
else if ($role == TablePortalUsers::ROLE_API_USER_ONLY){
   $ans = $portal_user->addAPIOnlyUser($params);
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

   $creation_token = $ans[0][TablePortalUsers::COL_CREATION_TOKEN];

   echo "Enable Endpoint:\n";
   $email = $params[TablePortalUsers::COL_EMAIL];
   echo "portal_users/enable/$email?enable_token=$creation_token\n";

}

$table_inst_users = new TableInstitutionUsers($con_main);
$ans = $table_inst_users->linkUserToInstitution($user_id,$inst_id);
if ($ans === FALSE){
   echo "Failed linking $user_id and $inst_id: " . $table_inst_users->getError() . "\n";   
}
else echo "Finished linking user $user_id and institution $inst_id\n";





?>