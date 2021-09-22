<?php
   $page = array();
   $ysubtitle = 0.55;
   $page["title"] = [
      ["text" => "#gonogo.page_titles.1.0#", "py" => 0],
      ["text" => "#gonogo.page_titles.1.1#", "py" => $ysubtitle]
   ];
   $page["items"] = array();

   $valid_vars = [
      "processing_speed",
      "processing_speed_interference",
      "processing_speed_facilitated",
      "reaction_time_facilitated",
      "reaction_time_interference",
      "executive_functions",
      "index_error_facilitated",
      "index_error_interference"
   ];
   
   $item = [];
   $item["type"]  = "bulleted_list";
   $item["id"]    = "green_bullets";
   $item["x"]     = 0;
   $item["y"]     = 0.02;
   $item["title"] = "#gonogo.affected_areas.green#";
   $item["dynamic_bullets"] = [
      "basestring" => "gonogo.affected_areas", // This only represents the base from which the strings are generated. 
      "ref_color"  => "green",
      "variables" => $valid_vars
   ]; 
   $page["items"][] = $item;
   
   $item = [];
   $item["type"]  = "bulleted_list";
   $item["id"]    = "yellow_bullets";
   $item["x"]     = 0;
   $item["y"]     = ["green_bullets",0.02];
   $item["title"] = "#gonogo.affected_areas.yellow#";
   $item["dynamic_bullets"] = [
      "basestring" => "gonogo.affected_areas", // This only represents the base from which the strings are generated. 
      "ref_color"  => "yellow",
      "variables" => $valid_vars
   ]; 
   $page["items"][] = $item;
   
   $item = [];
   $item["type"] = "bulleted_list";
   $item["id"]   = "red_bullets";
   $item["x"]     = 0;
   $item["y"]     = ["yellow_bullets",0.02];
   $item["title"] = "#gonogo.affected_areas.red#";
   $item["dynamic_bullets"] = [
      "basestring" => "gonogo.affected_areas", // This only represents the base from which the strings are generated. 
      "ref_color"  => "red",
      "variables" => $valid_vars
   ]; 
   $page["items"][] = $item;

   // Adding the presumptive diagnosis.
   $item = [];
   $w = 0.98;
   $h = 0.12;
   $item["id"]         = "pdiag";
   $item["x"] = (1 - $w)/2;
   $item["y"] = $ysubtitle - $h*1.35; 
   $item["w"] = $w;
   $item["h"] = $h;
   $item["type"]        = "pdiag";
   $item["text_base"]   = "gonogo.presumptive_diagnosis.texts";
   $item["text_index"]  = "&presumptive_diagnosis&";
   $item["value"]       = "0";
   $item["left_title"]  = "#gonogo.presumptive_diagnosis.left_title#";
   //$item["right_title"] = "#gonogo.presumptive_diagnosis.right_title#";
   $item["right_title"] = ""; // This will not render the right hand side. 
   $item["separator"]  = 0.66;   
   $page["items"][] = $item;

   // Overall performance. 
   $item = [];
   $item["type"] = "text";
   $item["text"] = "#gonogo.overall_performance.text#";
   $item["id"]   = "overall_performance_explained";
   $item["x"] = 0;   
   $item["y"] = $ysubtitle + 0.02;
   $page["items"][] = $item;

   $item = [];
   $poly_graph_w = 0.3;
   $item["type"] = "polygon_graph";
   $item["id"]   = "polygon_graph";
   $item["x"] = (1 - $poly_graph_w)/2;
   $item["y"] = $ysubtitle+0.125;
   $item["w"] = $poly_graph_w; // This is equivalent to the the l side of the graph.
   $item["levels"] = ["+1","0","-1","-2","-3"];
   $item["separator"] = 1;
   $item["values"] = [
    "&indexes.polygon.processing_speed&",
    "&indexes.polygon.processing_speed_interference&",
    "&indexes.polygon.processing_speed_facilitated&",
    "&indexes.polygon.reaction_time_facilitated&",
    "&indexes.polygon.reaction_time_interference&",
    "&indexes.polygon.executive_functions&",
    "&indexes.polygon.index_error_facilitated&",
    "&indexes.polygon.index_error_interference&"    
   ];
   
   $item["vertex_labels"] = [
      "#gonogo.overall_performance.vertex.processing_speed#",
      "#gonogo.overall_performance.vertex.processing_speed_interference#",
      "#gonogo.overall_performance.vertex.processing_speed_facilitated#",
      "#gonogo.overall_performance.vertex.reaction_time_interference#",
      "#gonogo.overall_performance.vertex.reaction_time_facilitated#",
      "#gonogo.overall_performance.vertex.executive_functions#",
      "#gonogo.overall_performance.vertex.index_error_facilitated#",
      "#gonogo.overall_performance.vertex.index_error_interference#"
   ];
   $page["items"][] = $item;

   return $page;

?>
