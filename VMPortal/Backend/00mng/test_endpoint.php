<?php

/// INCLUDING ALL OBJECTS.
foreach (glob("../api_management/*.php") as $filename)
{
   //echo "INCLUDING $filename\n";
   include_once $filename;
}


//$service = DBCon::DB_SERVICE_PARTNERS;
$service = DBCon::DB_SERVICE_ADMIN;
$headers = array();
$identifier = "0";
$parameters = array();


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

// $headers[HeaderFields::AUTHORIZATION] = "8:algo";
// $object = new ObjectReports($service,$headers);
// $info[ComplimentaryDataFields::ASSOCIATED_INSTITUTIONS] = [2];
// $object->setPortalUserInfo($info);
// $ans = $object->list_all_own_institution("2_2_20220131065457689",[]);

// //======================= Portal Users Operate
// $object = new ObjectPortalUsers($service,$headers);
// if ($object->getError() != ""){
//    echo "Error Creating Portal Users Object: " . $object->getError() . "\n";
//    exit();
// }

// // // GET
// // $body[ObjectPortalUsers::MODPERM_ID] = 3;
// // $body[ObjectPortalUsers::MODPERM_ACTION] = "get";

// // // CREATE
// // $body["action"] = "create";
// // $body["name"] = "Prueba";
// // $body["lastname"] = "Usuario";
// // $body["company"] = "Something";
// // $body["email"] = "prueba@user.com";
// // $body["passwd"] = "1234567";
// // $body["permissions"] = "clean";
// // $body["institution_id"] = 1;
// // $body["user_role"] = 1;
// // $body["force_enable"] = true;

// // SET
// $body["id"] = 16;
// $body["action"] = "set";
// $body["name"] = "Prueba";
// $body["lastname"] = "Gonzales";
// $body["company"] = "Pedritoz";

// $object->setJSONData($body);
// $ans = $object->operate($identifier,$parameters);

// //======================= Institution Operate
// $object = new ObjectInstitution($service,$headers);
// if ($object->getError() != ""){
//    echo "Error Creating Portal Users Object: " . $object->getError() . "\n";
//    exit();
// }

// // CREATE OR SET
// $body["id"] = 5;
// //$body["action"] = "create";
// $body["action"] = "set";
// $body["institution_name"] = "Prueba";
// $body["email"] = "Some@email.com";
// $body["country"] = "AR";
// $body["address"] = "123 Main St";

// // GET OR LIST
// $body["id"] = 5;
// //$body["action"] = "get";
// $body["action"] = "list";

//======================= Create new institution secret
$parameters["et-key"] = EyeTrackerCodes::HP_OMNICEPT;
$parameters["version"] = "1.0.0";
$identifier = 1;
$object = new ObjectInstances($service,$headers);
$ans = $object->create($identifier,$parameters);

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