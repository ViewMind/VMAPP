<?php
  

  //SELECT study_id, study_date, puid FROM `tEyeResults` WHERE study_date BETWEEN '2018-11-07' AND '2018-11-10'
  
  //=============================== INPUT PARAMETERS =================================
  
  $config_data["db_user"]   = "root";
  $config_data["db_passwd"] = "givnar";
  $config_data["db_name"]   = "viewmind_develop";

  $config_patdata["db_user"]   = "root";
  $config_patdata["db_passwd"] = "givnar";
  $config_patdata["db_name"]   = "viewmind_patdata_dev";

  $config_id["db_user"]   = "root";
  $config_id["db_passwd"] = "givnar";
  $config_id["db_name"]   = "viewmind_id_dev";

  $START_DATE = "2018-11-07";
  $END_DATE = "2018-11-10";
  $PATIENT_REPO="/home/ariela/Workspace/Viewmind/PatientRepo/TOSEND";

  
  //=============================== ESTABLISHING CONNECTIONS  =================================
  
  $con_data     =mysqli_connect("localhost",$config_data["db_user"],$config_data["db_passwd"],$config_data["db_name"]);
  // Se chequea la conexión.
  if (mysqli_connect_errno()){  
    $ret["error"] = "Cannot establish DB Data connection. El error: " . mysqli_connect_errno();
    echo $ret["error"];
    //echo json_encode($ret);
    return;
  }
  
  $con_patdata =mysqli_connect("localhost",$config_patdata["db_user"],$config_patdata["db_passwd"],$config_patdata["db_name"]);
  // Se chequea la conexión.
  if (mysqli_connect_errno()){  
    $ret["error"] = "Cannot establish DB PatData connection. El error: " . mysqli_connect_errno();
    echo $ret["error"];
    //echo json_encode($ret);
    return;
  }
  
  $con_id       =mysqli_connect("localhost",$config_id["db_user"],$config_id["db_passwd"],$config_id["db_name"]);
  // Se chequea la conexión.
  if (mysqli_connect_errno()){  
    $ret["error"] = "Cannot establish DB ID connection. El error: " . mysqli_connect_errno();
    echo $ret["error"];
    //echo json_encode($ret);
    return;
  }
  
  echo "DB Connections established\n";
  
  
  //=============================== GETTING THE PATIENTS CREATED IN THE DEFINED PERIOD =================================
  
  
  $sqlquery = "SELECT DISTINCT puid FROM tPatientData WHERE date_insertion BETWEEN '$START_DATE' AND '$END_DATE'";
  $res = mysqli_query($con_patdata,$sqlquery);
  if (!$res){
      $ret["error"] = "ErrorDB. Query: " . $sqlquery . ". El error SQL es: " . mysqli_error($con_patdata);
      mysqli_close($con_data); mysqli_close($con_id); mysqli_close($con_patdata);     
      //echo json_encode($ret);
      echo $ret["error"];
      return;    
  }    
 
  $puid = array();
  while ($row = mysqli_fetch_array($res)){  
      $puid[] = $row["puid"];
  }

  $sqlquery = "SELECT DISTINCT uid, keyid FROM tPatientIDs WHERE keyid IN ('" . join($puid,"', '") . "')";
  $res = mysqli_query($con_id,$sqlquery);
  $puidmap = array();
  if (!$res){
      $ret["error"] = "ErrorDB. Query: " . $sqlquery . ". El error SQL es: " . mysqli_error($con_id);
      mysqli_close($con_data); mysqli_close($con_id); mysqli_close($con_patdata);     
      //echo json_encode($ret);
      echo $ret["error"];
      return;    
  }    

  $hashes = array();
  while ($row = mysqli_fetch_array($res)){  
     $hashes[] = $row["uid"];
     $puidmap[$row["uid"]] = $row["keyid"];
  }
  
  //=============================== DOWNLOADING THE DIRECTORIES =================================
  
  foreach ($hashes as $hash) {
     echo "DOWNLOADNG $hash\n";
     //$dname = $puidmap[$hash] .  "_" . $hash;
     $dname = $hash;
     $res = shell_exec("./copyS3PatDirDev.sh $hash $PATIENT_REPO");     
     echo "   " . $res;
//      //$res = shell_exec("mv $PATIENT_REPO/$hash $PATIENT_REPO/$dname");
  }
    
  
  echo "FINISHED";

  // Returning statements
  mysqli_close($con_data);
  mysqli_close($con_id);      
  mysqli_close($con_patdata);      
  //echo json_encode($ret);
?>  
