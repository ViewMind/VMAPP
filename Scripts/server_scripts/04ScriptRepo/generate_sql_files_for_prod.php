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

  $config_prod_id["db_user"]   = "root";
  $config_prod_id["db_passwd"] = "givnar";
  $config_prod_id["db_name"]   = "viewmind_id";
  
  $START_DATE = "2018-11-07";
  $END_DATE = "2018-11-10";
  $PATIENT_REPO="/home/ariela/Workspace/Viewmind/PatientRepo/TOSEND";
  $GERA_ID = "AR25134823";
  $BUCKET_DEV = "develop-testing";
  $BUCKET_PROD = "viewmind-raw-eye-data";
  $AXIS = "1145868706";

  
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
  
  $con_prod_id   =mysqli_connect("localhost",$config_prod_id["db_user"],$config_prod_id["db_passwd"],$config_prod_id["db_name"]);
  // Se chequea la conexión.
  if (mysqli_connect_errno()){  
    $ret["error"] = "Cannot establish DB ID connection. El error: " . mysqli_connect_errno();
    echo $ret["error"];
    //echo json_encode($ret);
    return;
  }  
  
  echo "DB Connections established\n";
  
  //=============================== GENERATING THE PUID MAP =================================

  $oldpuid2new = array();
  if ($fh = fopen('uids_to_add', 'r')) {
     while (!feof($fh)) {
         $line = fgets($fh);
         $parts = explode(",",$line);
         if (count($parts) == 2){
            $sqlquery = "SELECT keyid FROM tPatientIDs WHERE uid ='" . $parts[1] . "'";
            $res = mysqli_query($con_prod_id,$sqlquery);
            if (!$res){
                $ret["error"] = "ErrorDB. Query: " . $sqlquery . ". El error SQL es: " . mysqli_error($con_prod_id);
                echo $ret["error"];
                return;    
            } 
            if (mysqli_num_rows($res) == 1) {
               $oldpuid2new[$parts[0]] = mysqli_fetch_array($res)["keyid"];
            }
            else{
               echo "No puid found for " . $parts[1] . "\n";
            }
         }         
     }
     fclose($fh);
  }
  
  //=============================== QUERIES FOR THE PATDATA TABLE =================================
  $patdata_queries = array();
  foreach ($oldpuid2new as $oldp => $newp){
     $sqlquery = "SELECT * FROM tPatientData WHERE puid = '" .  $oldp . "' ORDER BY keyid DESC LIMIT 1";   
     $res = mysqli_query($con_patdata,$sqlquery);
     if (!$res){
         $ret["error"] = "ErrorDB. Query: " . $sqlquery . ". El error SQL es: " . mysqli_error($con_patdata);
         echo $ret["error"];
         return;    
     }   
     if (mysqli_num_rows($res) == 1) {
        $row = mysqli_fetch_array($res,MYSQLI_ASSOC);
        $row["puid"] = $newp;
        unset($row["keyid"]);
        $columns = array();
        $values  = array();
        foreach ($row as $c => $v){
           $columns[] = $c;
           $values[]  = $v;
        }
        $patdata_queries[] = "INSERT INTO tPatientData (" . join($columns,',') . ") VALUES ('" . join($values,"', '") . "');\n";
     }
     else{
        echo "No patient data found for " . $oldp . "\n";
     }     
  }
  
  if ($fh = fopen("patdata_queries.sql",'w')){
     foreach ($patdata_queries as $q) {
        fwrite($fh,$q);
     }
     fclose($fh);
  }
  
  //=============================== QUERIES FOR TEYERESULTS =================================
  $eyeres_queries = array();
  foreach ($oldpuid2new as $oldp => $newp){
     $sqlquery = "SELECT * FROM tEyeResults WHERE puid = '" .  $oldp . "'";   
     $res = mysqli_query($con_data,$sqlquery);
     if (!$res){
         $ret["error"] = "ErrorDB. Query: " . $sqlquery . ". El error SQL es: " . mysqli_error($con_data);
         echo $ret["error"];
         return;    
     }   
     while($row = mysqli_fetch_array($res,MYSQLI_ASSOC)) {
        $row["puid"] = $newp;
        unset($row["keyid"]);
        $columns = array();
        $values  = array();
        foreach ($row as $c => $v){
           $columns[] = $c;
           $values[]  = $v;
        }
        $eyeres_queries[] = "INSERT INTO tEyeResults (" . join($columns,',') . ") VALUES ('" . join($values,"', '") . "');\n";
     } 
  }
  
  if ($fh = fopen("teyeresults_queries.sql",'w')){
     foreach ($eyeres_queries as $q) {
        fwrite($fh,$q);
     }
     fclose($fh);
  }
  
  
?>  
