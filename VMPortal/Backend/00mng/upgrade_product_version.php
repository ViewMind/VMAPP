<?php

/**
 * Inserts a version a new version of the application, leaving everythihing else as is
 * By using institution shorthand methodology. 
 */

include("../db_management/TableUpdates.php");
include("cli_interface.php");

$section_name = "upgrade_version";
$required = ["new_version","upgrade_list"];

DBCon::setPointerLocation("configs");  
$params = readAndVerifyData($required,$section_name);
// $params["upgrade_list"] = "x.x";
// $params["new_version"] = "x.x";

$dbcon = new DBCon();
$con_main = $dbcon->dbOpenConnection(DBCon::DB_MAIN,DBCon::DB_SERVICE_ADMIN);
if ($con_main == NULL){
   echo "Error creating db connection: " . $dbcon->getError() . "\n";
   exit();
}

$con_secure = $dbcon->dbOpenConnection(DBCon::DB_SECURE,DBCon::DB_SERVICE_ADMIN);
if ($con_secure == NULL){
   echo "Error creating db connection: " . $dbcon->getError() . "\n";
   exit();
}

$tup = new TableUpdates($con_main);
$ts  = new TableSecrets($con_secure);

if ($params["upgrade_list"] == "x.x"){

   echo "Updgrad all requested\n";
   $ans = $tup->updateAllValidToNewerVersion($params["new_version"]);
   if ($ans === false){
      echo "Error when updating all: " . $tup->getError() . "\n";
   }
   else{
      echo "Finished\n";
   }
   return;

}

$upgrade_list = expandShortInstanceListToConprenhensiveList($params["upgrade_list"],$ts);
if ($upgrade_list === FALSE) exit();

//var_dump($upgrade_list);

$ans = $tup->updateJustVersionOfList($upgrade_list,$params["new_version"]);
if ($ans === FALSE){
   echo "Error when updating list: " . $tup->getError() . "\n";
   exit();
}

echo "Finsihed\n";

?>