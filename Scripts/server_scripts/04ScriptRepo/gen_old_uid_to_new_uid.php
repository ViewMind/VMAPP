<?php
  

  //SELECT study_id, study_date, puid FROM `tEyeResults` WHERE study_date BETWEEN '2018-11-07' AND '2018-11-10'
  
  //=============================== INPUT PARAMETERS =================================
  
  $config_data["db_host"]   = "viewminddb.cdqlb2rkfdvi.us-east-2.rds.amazonaws.com";
  $config_data["db_user"]   = "root";
  $config_data["db_passwd"] = "vimiroot";
  $config_data["db_name"]   = "viewmind_data";

  $config_patdata["db_host"]   = "viewmind-patdata.cdqlb2rkfdvi.us-east-2.rds.amazonaws.com";
  $config_patdata["db_user"]   = "root";
  $config_patdata["db_passwd"] = "vimiroot";
  $config_patdata["db_name"]   = "viewmind_patdata";

  $config_id["db_host"]   = "viewmind-id.cdqlb2rkfdvi.us-east-2.rds.amazonaws.com";
  $config_id["db_user"]   = "root";
  $config_id["db_passwd"] = "vimiroot";
  $config_id["db_name"]   = "viewmind_id";

  $START_DATE = "2018-11-07";
  $END_DATE = "2018-11-10";
  $PATIENT_REPO="/home/ariela/Workspace/Viewmind/PatientRepo/TOSEND";
  $GERA_ID = "AR25134823";
  $BUCKET_DEV = "develop-testing";
  $BUCKET_PROD = "viewmind-raw-eye-data";
  $AXIS = "1145868706";

  
  //=============================== ESTABLISHING CONNECTIONS  =================================
  
  $con_data     =mysqli_connect($config_data["db_host"],$config_data["db_user"],$config_data["db_passwd"],$config_data["db_name"]);
  // Se chequea la conexión.
  if (mysqli_connect_errno()){  
    $ret["error"] = "Cannot establish DB Data connection. El error: " . mysqli_connect_errno();
    echo $ret["error"];
    //echo json_encode($ret);
    return;
  }
  
  $con_patdata =mysqli_connect($config_patdata["db_host"],$config_patdata["db_user"],$config_patdata["db_passwd"],$config_patdata["db_name"]);
  // Se chequea la conexión.
  if (mysqli_connect_errno()){  
    $ret["error"] = "Cannot establish DB PatData connection. El error: " . mysqli_connect_errno();
    echo $ret["error"];
    //echo json_encode($ret);
    return;
  }
  
  $con_id       =mysqli_connect($config_id["db_host"],$config_id["db_user"],$config_id["db_passwd"],$config_id["db_name"]);
  // Se chequea la conexión.
  if (mysqli_connect_errno()){  
    $ret["error"] = "Cannot establish DB ID connection. El error: " . mysqli_connect_errno();
    echo $ret["error"];
    //echo json_encode($ret);
    return;
  }
  
  echo "DB Connections established\n";
  
  $uidmap = array();
  
  if ($fh = fopen('uids_to_add', 'r')) {
     while (!feof($fh)) {
         $line = fgets($fh);
         $parts = explode(",",$line);
         if ($count($parts[1]) == 2) {
            $res = mysqli_query($con_patid,"INSERT INTO tPaientIDs (uid) VALUES ('" . $parts[1] . "')");
            if (!res){
               echo "ErrorDB. Query: " . $sqlquery . ". El error SQL es: " . mysqli_error($con_patdata);
               mysqli_close($con_data); mysqli_close($con_id); mysqli_close($con_patdata);     
               return;   
            }
         }
     }
     fclose($fh);
  }  
  
//   //=============================== LISTING AND HASHING =================================
//   $res = shell_exec("./AWSLs.sh $BUCKET_DEV/$GERA_ID list");     
//   echo $res;
//   
//   $patient_ids = array();
//   if ($fh = fopen('list', 'r')) {
//      while (!feof($fh)) {
//          $line = fgets($fh);
//          $parts = explode("/",$line);
//          if (count($parts) == 4){
//             if (!in_array($parts[1],$patient_ids)){
//                $patient_ids[] = $parts[1];
//             }
//          }         
//      }
//      fclose($fh);
//   }
//   
//   // Doing the hashmap and saving it to a file.
//   $pat_hashmap = array();
//   if ($fh = fopen('hash_map', 'w')) {
//      foreach ($patient_ids as $p){
//         $sha = hash("sha3-512",$p);           
//         $pat_hashmap[$sha] = $p;
//         fwrite($fh,"$p --> $sha\n");
//      }
//      fclose($fh);
//   }
//   
//   //=============================== GETTING THE PATIENTS CREATED IN THE DEFINED PERIOD =================================
//     
//   $sqlquery = "SELECT DISTINCT * FROM tPatientData WHERE date_insertion BETWEEN '$START_DATE' AND '$END_DATE'";
//   $res = mysqli_query($con_patdata,$sqlquery);
//   if (!$res){
//       $ret["error"] = "ErrorDB. Query: " . $sqlquery . ". El error SQL es: " . mysqli_error($con_patdata);
//       mysqli_close($con_data); mysqli_close($con_id); mysqli_close($con_patdata);     
//       //echo json_encode($ret);
//       echo $ret["error"];
//       return;    
//   }    
//  
//   $puid = array();
//   $table_pat_data = array();
//   while ($row = mysqli_fetch_array($res)){  
//       $table_pat_data[] = $row;
//       $puid[] = $row["puid"];
//   }
// 
//   $sqlquery = "SELECT DISTINCT uid, keyid FROM tPatientIDs WHERE keyid IN ('" . join($puid,"', '") . "')";
//   $res = mysqli_query($con_id,$sqlquery);
//  
//   if (!$res){
//       $ret["error"] = "ErrorDB. Query: " . $sqlquery . ". El error SQL es: " . mysqli_error($con_id);
//       mysqli_close($con_data); mysqli_close($con_id); mysqli_close($con_patdata);     
//       //echo json_encode($ret);
//       echo $ret["error"];
//       return;    
//   }    
//   
//   $hashes = array();
//   while ($row = mysqli_fetch_array($res)){       
//      // Checking that the $hash is in the $hasmap
//      if (!in_array($row["uid"],array_keys($pat_hashmap))){
//         echo "WARNING no ID for HASH: " . $row["uid"] . "\n";
//      }
//      else{
//         $hashes[$row["uid"]] = $row["keyid"];
//      }
//   }
//   
//   // Array of strings that will do the queries that are required.
//   if ($fh = fopen('uids_to_add', 'w')) {
//      foreach ($hashes as $hash => $puid){        
//         fwrite($fh,"$puid,$hash\n");
//      }
//      fclose($fh);
//   }
  
  // Returning statements
  mysqli_close($con_data);
  mysqli_close($con_id);      
  mysqli_close($con_patdata);      
  //echo json_encode($ret);
?>  
