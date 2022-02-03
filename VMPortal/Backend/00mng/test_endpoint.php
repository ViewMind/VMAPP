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
$identifier = "0";
$parameters = array();

// JSON DATA


//$object = new ObjectReports($service, $headers);
//$ans = $object->admin_evaluation_list($identifier, $parameters);

// The code below get users permission, sets the role to 7 (login in all sites and adds the permission to modify any user permission)
//$object = new ObjectPortalUsers($service,$headers);


// $data[ObjectPortalUsers::MODPERM_ID] = "ariel.arelovich@viewmind.ai";
// $data[ObjectPortalUsers::MODPERM_ACTION] = EndpointBodyActions::GET;
// $object->setJSONData($data);
// $ans = $object->modify_permissions($identifier,$parameters);

// $ans[ObjectPortalUsers::MODPERM_ID] = $data[ObjectPortalUsers::MODPERM_ID];
// $ans[ObjectPortalUsers::MODPERM_ACTION] = EndpointBodyActions::SET;
// $ans[ObjectPortalUsers::MODPERM_ROLE] = 7;
// $ans[ObjectPortalUsers::MODPERM_PERMISSIONS][APIEndpoints::PORTAL_USERS][] = "modify_permissions";

// //var_dump($ans);

// $object->setJSONData($ans);

// $ans = $object->modify_permissions($identifier,$parameters);

//$headers[HeaderFields::AUTHORIZATION] = "1:algo";
//$object = new ObjectInstitution($service,$headers);
//$ans = $object->list(1,$parameters);

$headers[HeaderFields::AUTHORIZATION] = "8:algo";
$object = new ObjectReports($service,$headers);
$info[ComplimentaryDataFields::ASSOCIATED_INSTITUTIONS] = [2];
$object->setPortalUserInfo($info);
$ans = $object->list_all_own_institution("2_2_20220131065457689",[]);

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