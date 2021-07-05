<?php

/***
 * Used in order to avoid logs and pass the AWS healthchecks.
 * Anything that is a private subnet and NOT one of the exceptions is a healthcheck.
  */

 function isHealthCheck($ip){
    //$ip = $_SERVER['REMOTE_ADDR'];
    $ip_parts = explode(".",$ip);

    $VALID_IP_LIST = explode("\n",file_get_contents("valid_ips"));

    if (in_array($ip,$VALID_IP_LIST)){
       return false;
    }

    if (count($ip_parts) != 4) return false;
    $is24BitBlock = ($ip_parts[0] == 10);
    $is20BitBlock = (($ip_parts[0] == 172) && ($ip_parts[1] >= 16) && ($ip_parts[1] <= 31));
    $is16BitBlock = (($ip_parts[0] == 192) && ($ip_parts[1] == 168));

    // Any private address that is not in the valid list is a healthcheck 
    if ($is16BitBlock || $is20BitBlock || $is24BitBlock) return true;

    return false;
 }

//  if (isHealthCheck("192.169.3.231") === FALSE){
//     echo "Valid\n";
//  }
//  else{
//     echo "HealthCheck\n";
//  }

?>