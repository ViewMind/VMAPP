<?php
  

  //=============================== INPUT PARAMETERS =================================
  
  $config_data["db_user"]   = "root";
  $config_data["db_passwd"] = "givnar";
  $config_data["db_name"]   = "viewmind_data";

  $config_patdata["db_user"]   = "root";
  $config_patdata["db_passwd"] = "givnar";
  $config_patdata["db_name"]   = "viewmind_patdata";

  $config_id["db_user"]   = "root";
  $config_id["db_passwd"] = "givnar";
  $config_id["db_name"]   = "viewmind_id";

  $INST_ID="1145868706";
  $PATIENT_REPO="/home/ariela/Workspace/Viewmind/PatientRepo/TOSEND";
  $TO_MERGE="/home/ariela/Workspace/Viewmind/PatientRepo/OLD";


  
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
  
  //=============================== GETTING DOCTOR PATIENT INFORMATION FROM MERGE REPO AND STARTING THE SYNC OF OLD DATA =================================
 
  
  $dirlist=scandir($TO_MERGE);
  $hashmap = array();
  //print_r(hash_algos());
  
  foreach ($dirlist as $pat_uid){
     if (($pat_uid != ".") and ($pat_uid != "..")){
        $sha = hash("sha3-512",$pat_uid);           
        $hashmap[$sha] = $pat_uid;
     }
  }
    
  echo "================= Synching OLD FILES\n"; 
  foreach ($hashmap as $sha => $p){
     
     $dname = "";
     
     // Getting the PUID
     $sqlquery = "SELECT keyid FROM tPatientIDs WHERE uid ='" . $sha . "'";
     $res = mysqli_query($con_id,$sqlquery);
     if (!$res){
         $ret["error"] = "ErrorDB. Query: " . $sqlquery . ". El error SQL es: " . mysqli_error($con_id);
         mysqli_close($con_data); mysqli_close($con_id); mysqli_close($con_patdata);     
         //echo json_encode($ret);
         echo $ret["error"];
         return;    
     }    
     
     if (mysqli_num_rows($res) == 1) {
        $puid = mysqli_fetch_array($res)["keyid"];
        $sqlquery = "SELECT doctorid, firstname, lastname FROM tPatientData WHERE puid = '" . $puid .  "'";
        $res = mysqli_query($con_patdata,$sqlquery);
        if (!$res){
            $ret["error"] = "ErrorDB. Query: " . $sqlquery . ". El error SQL es: " . mysqli_error($con_patdata);
            mysqli_close($con_data); mysqli_close($con_id); mysqli_close($con_patdata);     
            //echo json_encode($ret);
            echo $ret["error"];
            return;    
        } 
        if (mysqli_num_rows($res) == 1){
           $row = mysqli_fetch_array($res);
           $dname = $row["doctorid"] . "_" . $p . "_" . $puid . "_" . utf8_encode($row["lastname"]) . "_" .utf8_encode( $row["firstname"]);
           $dname = str_replace(" ","_",$dname);
           $dname = str_replace("ñ","n",$dname);
           $dname = str_replace("á","a",$dname);
           $dname = str_replace("é","e",$dname);
           $dname = str_replace("í","i",$dname);
           $dname = str_replace("ó","o",$dname);
           $dname = str_replace("ú","u",$dname);
        }
        else{
            echo "WARNING $sha from $p, with PUID: $puid, has not data entry in DB\n";
            $dname = $p . "_" . $puid;
        }
        
     }
     else{
        echo "WARNING $sha from $p, NOT IN DB\n";
        $dname = $p;
     }
     
     // Create the directory.
     shell_exec("mkdir $PATIENT_REPO/$dname");
     
     // Move all the stuff from the originl to this one.
     shell_exec("cp $TO_MERGE/$p/* $PATIENT_REPO/$dname");
     
  }
  
  //=============================== QUERYING ALL PATIENTS FOR THE INSTITUTION =================================
  $sqlquery = "SELECT DISTINCT r.study_id AS sid, r.study_date AS sd, r.puid AS puid, r.keyid AS keyid
               FROM tEyeResults AS r, tDoctors AS d
               WHERE d.medicalinstitution = '" . $INST_ID . "' AND d.uid = r.doctorid ORDER BY r.puid";
  
  
  $res = mysqli_query($con_data,$sqlquery);
  if (!$res){
      $ret["error"] = "ErrorDB. Query: " . $sqlquery . ". El error SQL es: " . mysqli_error($con);
      mysqli_close($con_data); mysqli_close($con_id); mysqli_close($con_patdata);     
      //echo json_encode($ret);
      echo $ret["error"];
      return;    
  }    
  
  
  echo "============== RESULTS FOUND ================\n";
  $puid = array();
  $counter = 0;
  while ($row = mysqli_fetch_array($res)){  
     echo $counter . " - Patient: " . $row["puid"] . " has study " . $row["sid"] . " on " . $row["sd"] . "  KEYID: " . $row["keyid"] . "\n";
     $counter++;
     if (!in_array($row["puid"],$puid)) $puid[]=$row["puid"];
  }
  
  //=============================== GETTING ALL THE HASH NAMES =================================
  $sqlquery = "SELECT uid, keyid FROM tPatientIDs WHERE keyid IN ('" . join($puid,"', '")  .  "')";
  $res = mysqli_query($con_id,$sqlquery);
  
  if (!$res){
      $ret["error"] = "ErrorDB. Query: " . $sqlquery . ". El error SQL es: " . mysqli_error($con);
      mysqli_close($con_data); mysqli_close($con_id); mysqli_close($con_patdata);     
      //echo json_encode($ret);
      echo $ret["error"];
      return;    
  }
  
  //echo "============== HASHES FOR THE PATIENTS ================\n";
  $hashes = array();  ;
  while ($row = mysqli_fetch_array($res)){  
     $hashes[$row["keyid"]] = $row["uid"];
     //echo $row["keyid"] . " has UID: " . $hashes[$row["keyid"]] .  "\n";
  }

  //=============================== GETTING THE PATIENT NAME AND DR ID =================================  
  $sqlquery = "SELECT doctorid, firstname, lastname, puid FROM tPatientData WHERE puid IN ('" . join($puid,"', '")  .  "')";
  $res = mysqli_query($con_patdata,$sqlquery);
  if (!$res){
      $ret["error"] = "ErrorDB. Query: " . $sqlquery . ". El error SQL es: " . mysqli_error($con_patdata);
      mysqli_close($con_data); mysqli_close($con_id); mysqli_close($con_patdata);     
      //echo json_encode($ret);
      echo $ret["error"];
      return;    
  }    
  
  echo "============== PATIENT INFORMATION ================\n";
  $counter = 0;
  $dirnames = array();
  while ($row = mysqli_fetch_array($res)){  
     $p = (int)$row["puid"];
     echo $counter . " - PATIENT " . $p . ": " . $row["lastname"] . ", " . $row["firstname"] . ", tested by DR:  " . $row["doctorid"]  . ". UID: " . $hashes[$p] .  "\n";
     $counter++;
     
     $pat_uid = "UNKNOWN";
     if (array_key_exists($hashes[$p],$hashmap)) $pat_uid = $hashmap[$hashes[$p]];
     
     $dname = $row["doctorid"] . "_" . $pat_uid . "_" . $p . "_" . utf8_encode($row["lastname"]) . "_" .utf8_encode( $row["firstname"]);
     $dname = str_replace(" ","_",$dname);
     $dname = str_replace("ñ","n",$dname);
     $dname = str_replace("á","a",$dname);
     $dname = str_replace("é","e",$dname);
     $dname = str_replace("í","i",$dname);
     $dname = str_replace("ó","o",$dname);
     $dname = str_replace("ú","u",$dname);
     $dirnames[$hashes[$p]] = $dname;
  }
  
  //=============================== DOWNLOADING S3 ================================= 
  echo "============== DOWNLOADING THE INFORMATION ================\n";
  foreach ($dirnames as $hash => $dirname) {
     echo "DOWNLOADING $hash to $dirname\n";
     //shell_exec("mkdir $PATIENT_REPO/$dirname");
     $res = shell_exec("./copyS3PatDir.sh $hash $PATIENT_REPO/$dirname");
     echo "   " . $res;
  }
  
  
  echo "FINISHED";

  // Returning statements
  mysqli_close($con_data);
  mysqli_close($con_id);      
  mysqli_close($con_patdata);      
  //echo json_encode($ret);
?>  
