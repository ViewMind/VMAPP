<?php
   $page = array();
   $ysubtitle = 0.55;
   $page["title"] = [
      ["text" => "#binding.page_titles.2.0#", "py" => 0],
      ["text" => "#binding.page_titles.2.1#", "py" => $ysubtitle]
   ];
   $page["items"] = array();


   $valid_vars = ["visual_search", "gaze_dur", "int_mem_perf", "assoc_mem_perf", "exec_func", "parahipocampal_region"];
   
   $item = [];
   $item["type"]  = "bulleted_list";
   $item["id"]    = "green_bullets";
   $item["x"]     = 0;
   $item["y"]     = 0.02;
   $item["title"] = "#binding.affected_areas.green#";
   $item["dynamic_bullets"] = [
      "basestring" => "binding.affected_areas", // This only represents the base from which the strings are generated. 
      "ref_color"  => "green",
      "variables" => $valid_vars
   ]; 
   $page["items"][] = $item;
   
   $item = [];
   $item["type"]  = "bulleted_list";
   $item["id"]    = "yellow_bullets";
   $item["x"]     = 0;
   $item["y"]     = ["green_bullets",0.02];
   $item["title"] = "#binding.affected_areas.yellow#";
   $item["dynamic_bullets"] = [
      "basestring" => "binding.affected_areas", // This only represents the base from which the strings are generated. 
      "ref_color"  => "yellow",
      "variables" => $valid_vars
   ]; 
   $page["items"][] = $item;
   
   $item = [];
   $item["type"] = "bulleted_list";
   $item["id"]   = "red_bullets";
   $item["x"]     = 0;
   $item["y"]     = ["yellow_bullets",0.02];
   $item["title"] = "#binding.affected_areas.red#";
   $item["dynamic_bullets"] = [
      "basestring" => "binding.affected_areas", // This only represents the base from which the strings are generated. 
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
   $item["y"] = $ysubtitle - $h*1.5; 
   $item["w"] = $w;
   $item["h"] = $h;
   $item["type"]        = "pdiag";
   $item["text_base"]   = "binding.presumptive_diagnosis.texts_3T";
   $item["text_index"]  = "&presumptive_diagnosis&";
   $item["value"]       = "&parahipocampal_region&";
   $item["left_title"]  = "#binding.presumptive_diagnosis.left_title#";
   $item["right_title"] = "#binding.presumptive_diagnosis.right_title#";
   $item["separator"]  = 0.66;   
   $page["items"][] = $item;

   // Overall performance. 
   $item = [];
   $item["type"] = "text";
   $item["text"] = "#binding.overall_performance.text#";
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
   $item["levels"] = ["+1","0","-1","-2"];
   $item["separator"] = 1;
   $item["values"] = [
    "&indexes.polygon.visual_search&",
    "&indexes.polygon.gaze_dur&",
    "&indexes.polygon.int_mem_perf&",
    "&indexes.polygon.assoc_mem_perf&",
    "&indexes.polygon.exec_func&"
   ];
   $item["vertex_labels"] = [
      "#binding.overall_performance.vertex.visual_search#",
      "#binding.overall_performance.vertex.gaze_dur#",
      "#binding.overall_performance.vertex.int_mem_perf#",
      "#binding.overall_performance.vertex.assoc_mem_perf#",
      "#binding.overall_performance.vertex.exec_functions#",
   ];
   $page["items"][] = $item;

   return $page;

?>
