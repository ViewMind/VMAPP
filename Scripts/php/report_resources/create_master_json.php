<?php

  // Loading the Spanish Text and The English Text. 
  
  $master_json = array();
  
  $master_json["en"]["motto"]        =  "Cognitive Performance|Evaluations through|Eye-Tracking";
  $master_json["en"]["color_into"]   =  "The result of each function analyzed, is stated in the following code";
  $master_json["en"]["index_blue"]   =  "Function above|average";
  $master_json["en"]["index_green"]  =  "No relevant functional|problems|were detected";
  $master_json["en"]["index_yellow"] =  "Midly deficient|functioning, not|necessarily pathological";
  $master_json["en"]["index_red"]    =  "Completely deficient|and/or|pathological functioning";
  $master_json["en"]["patient"]      =  "NAME";
  $master_json["en"]["age"]          =  "AGE";
  $master_json["en"]["date"]         =  "DATE";
  $master_json["en"]["reqdr"]        =  "REQUESTING DOCTOR";
  
  $master_json["es"]["motto"]        =  "Medición de Desempeño|Cognitivo mediante|Eye-Tracking";
  $master_json["es"]["color_into"]   =  "Para cada función analizada, el resultado se indica según el siguiente código:";
  $master_json["es"]["index_blue"]   =  "Funcionamiento|superior a|la media";
  $master_json["es"]["index_green"]  =  "No se han detectado|problemas|de funcionamiento relevante";
  $master_json["es"]["index_yellow"] =  "Funcionamiento|levemente deficiente,|no necesariamente patológico";
  $master_json["es"]["index_red"]    =  "Funcionamiento|completamente|deficiente y/o patológico";
  $master_json["es"]["patient"]      =  "NOMBRE";
  $master_json["es"]["age"]          =  "EDAD";
  $master_json["es"]["date"]         =  "FECHA";
  $master_json["es"]["reqdr"]        =  "MÉDICO SOLICITANTE";
  
  $dirlist = scandir(".");
  
  $study_name_map = array(); 
  $study_name_map["Binding"]["es"]  = "COLORES";
  $study_name_map["Go No-Go"]["es"] = "GO\\NO GO";
  $study_name_map["NBackRT"]["es"]  = "NBACK RT";
  $study_name_map["Reading"]["es"]  = "LECTURA";

  $study_name_map["Binding"]["en"]  = "COLORS";
  $study_name_map["Go No-Go"]["en"] = "GO\\NO GO";
  $study_name_map["NBackRT"]["en"]  = "NBACK RT";
  $study_name_map["Reading"]["en"]  = "READING";

  
  foreach ($dirlist as $dir){
     if ($dir == ".") continue;
     if ($dir == "..") continue;     
     if (!is_dir($dir)) continue;
     
     echo "$dir\n";
     
     $file_list = scandir($dir);
     $master_json[$dir] = array();
     
     foreach ($file_list as $file){
        if ($file == ".") continue;
        if ($file == "..") continue;     
        
        //var_dump(parse_ini_file("$dir/$file",true));
            
        $master_json[$dir]["study_title_en"] = $study_name_map[$dir]["en"];
        $master_json[$dir]["study_title_es"] = $study_name_map[$dir]["es"];
        $master_json[$dir][$file] = parse_ini_file("$dir/$file",true,INI_SCANNER_RAW);
     
     }
     
  }
  
  $fid = fopen("report_reference.js","w");
  $temp = json_encode($master_json,JSON_UNESCAPED_UNICODE|JSON_PRETTY_PRINT);
  
  //var_dump($temp);
  fwrite($fid,"var REPORT_REFERENCE = \n" . $temp);
  fclose($fid);

?>
