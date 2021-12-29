<?php

/// INCLUDING ALL OBJECTS.
foreach (glob("../api_management/*.php") as $filename)
{
   //echo "INCLUDING $filename\n";
   include_once $filename;
}


$service = DBCon::DB_SERVICE_PARTNERS;
//$service = DBCon::DB_SERVICE_ADMIN;
$headers = array();
$identifier = "";
$parameters = array();

//$object = new ObjectReports($service, $headers);
//$ans = $object->admin_evaluation_list($identifier, $parameters);

$headers[HeaderFields::AUTHORIZATION] = "1:algo";
$object = new ObjectInstitution($service,$headers);
$ans = $object->list(1,$parameters);

if ($ans === false){
   echo "Error: " . $object->getError() . "\n";
}
else{
   $fid = fopen("test_output.json","w");
   fwrite($fid,json_encode($ans,JSON_PRETTY_PRINT));
   fclose($fid);
   echo "Finished\n";
}

?>