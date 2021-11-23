<?php

/// INCLUDING ALL OBJECTS.
foreach (glob("../api_management/*.php") as $filename)
{
   //echo "INCLUDING $filename\n";
   include_once $filename;
}


$service = DBCon::DB_SERVICE_PARTNERS;
$headers = array();
$identifier = "";
$parameters = array();

$object = new ObjectReports($service, $headers);

$ans = $object->admin_evaluation_list($identifier, $parameters);

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