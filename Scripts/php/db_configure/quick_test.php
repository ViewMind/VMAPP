<?php

   // Creating connection. 
   $credentials = parse_ini_file("credentials.cnf");
   $con = mysqli_connect($credentials["host"],$credentials["user"],$credentials["password"],$credentials["database"]);   
   if (!$con){
      echo "Could not create DB connection: " . mysqli_connect_error() . "\n";
      echo "Error number: " . mysqli_connect_errno() . "\n";
      return;
   }
   
   // Making sure tEyeResultsOld exists before running the script. 
   $sql = "SELECT results FROM tEyeResults WHERE keyid = 1027";
   $eye_results = mysqli_query($con,$sql);
   if (!$eye_results){
      echo "DBERROR: " . mysqli_error($con) . " FROM $sql\n";
      return;
   }
   
   $res = mysqli_fetch_array($eye_results,MYSQLI_ASSOC)["results"];
   $res = json_decode($res,true);
   var_dump($res);
   
?>
