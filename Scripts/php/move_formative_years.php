<?php
  

  //=============================== INPUT PARAMETERS =================================
  
  $LOCAL_PROD = 1;

  $config_patdata["db_user"]   = "root";
  $config_patdata["db_name"]   = "viewmind_patdata";
 
  if ($LOCAL_PROD == 1){
     $config_patdata["db_passwd"] = "givnar";     
     $config_patdata["db_host"]   = "localhost";
  }
  else{
     $config_patdata["db_passwd"] = "vimiroot";
     $config_patdata["db_host"]   = "viewmind-patdata.cdqlb2rkfdvi.us-east-2.rds.amazonaws.com";     
  }

  
  //=============================== ESTABLISHING CONNECTIONS  =================================
    
  $con_patdata =mysqli_connect($config_patdata["db_host"],$config_patdata["db_user"],$config_patdata["db_passwd"],$config_patdata["db_name"]);
  // Se chequea la conexión.
  if (mysqli_connect_errno()){  
    $ret["error"] = "Cannot establish DB PatData connection. El error: " . mysqli_connect_errno();
    echo $ret["error"];
    //echo json_encode($ret);
    return;
  }
  
  echo "DB Connections established\n";
  
  //=============================== Selecting the different data =================================
  $sqlquery = "SELECT keyid,record_index,puid,formative_years FROM tMedicalRecords ORDER BY keyid";
  $res = mysqli_query($con_patdata,$sqlquery);
  if (!$res){
      $ret["error"] = "ErrorDB. Query: " . $sqlquery . ". El error SQL es: " . mysqli_error($con_patdata);
      mysqli_close($con_patdata); 
      //echo json_encode($ret);
      echo $ret["error"];
      return;    
  }    
  
  while ($row = mysqli_fetch_array($res)){  
      echo $row["keyid"] . ", " . $row["record_index"] . ", " . $row["puid"] . ", " . $row["formative_years"] . "\n";
  }
  
  echo "FINISHED";

  // Returning statements
  mysqli_close($con_patdata);      
?>  
