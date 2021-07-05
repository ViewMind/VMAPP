<?php

   const FNAME = "dbconfig.cnf";

   $a["databases"] = array();
   $a["databases"]["vm_main"]["host"] = "localhost";
   $a["databases"]["vm_main"]["port"] = 3306;

   $a["databases"]["vm_secure"]["host"] = "localhost";
   $a["databases"]["vm_secure"]["port"] = 3306;

   
   $a["services"] = array();
   
   $a["services"]["srv_dp"]["username"] = "service_client_data_processing";
   $a["services"]["srv_dp"]["password"] = "1234";
   $a["services"]["srv_dp"]["databases"] = ["vm_main", "vm_secure"];

   $a["services"]["srv_admin"]["username"] = "service_administrative_management";
   $a["services"]["srv_admin"]["password"] = "1234";
   $a["services"]["srv_admin"]["databases"] = ["vm_main", "vm_secure"];

   echo json_encode ($a,JSON_PRETTY_PRINT) . "\n";
   $fid = fopen(FNAME,"w");
   fwrite($fid,json_encode ($a,JSON_PRETTY_PRINT));
   fclose($fid);

?>