<?php

include_once ("../db_management/DBCon.php");
include_once ("../db_management/TableInstitution.php");
include_once ("../db_management/TableInstitutionUsers.php");
include_once ("../db_management/TablePlacedProducts.php");
include_once ("../db_management/TablePortalUsers.php");
include_once ("../db_management/TableSecrets.php");

include("cli_interface.php");

$section_name = "set_password";
$required = [TablePortalUsers::COL_EMAIL,TablePortalUsers::COL_PASSWD];

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

echo "Verifying user existance\n";

$email = $params[TablePortalUsers::COL_EMAIL];

$tpu = new TablePortalUsers($con_secure);
$ans = $tpu->getInfoForUser($email,false);
if ($ans === false){
   echo "Error user's existance: " . $tpu->getError();
   exit();
}
else{
   //var_dump($ans);
   if (count($ans) == 1){
      echo "User Exists. Moving on\n";
   }
   else{
      echo "User $email doesn't exist\n";
      exit();
   }
}

// Hashing the password. 
$password = password_hash($params[TablePortalUsers::COL_PASSWD], PASSWORD_BCRYPT, ["cost" => 10]);
$ans = $tpu->setUserPassword($email,$password);
if ($ans === FALSE){
   echo "Failed setting the password for $email. Reason: " . $tpu->getError();
   exit();
}

?>
