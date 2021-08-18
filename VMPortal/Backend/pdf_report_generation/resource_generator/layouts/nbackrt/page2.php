<?php
   $page = array();
   $ysubtitle = 0.55;
   $page["title"] = [
      ["text" => "#nbackrt.page_titles.1.0#", "py" => 0],
      ["text" => "#nbackrt.page_titles.1.1#", "py" => $ysubtitle]
   ];
   $page["items"] = array();

   $valid_vars = [
      "visual_exploration_efficiency",
      "visual_working_memory",
      "correct_responses",
      "working_memory",
      "inhibitory_processes",
      "decoding_efficiency",
      "enconding_efficiency"
   ];
   
   // $item = [];
   // $item["type"]  = "bulleted_list";
   // $item["id"]    = "green_bullets";
   // $item["x"]     = 0;
   // $item["y"]     = 0.02;
   // $item["title"] = "#nbackrt.affected_areas.green#";
   // $item["dynamic_bullets"] = [
   //    "basestring" => "nbackrt.affected_areas", // This only represents the base from which the strings are generated. 
   //    "ref_color"  => "green",
   //    "variables" => $valid_vars
   // ]; 
   // $page["items"][] = $item;
   
   // $item = [];
   // $item["type"]  = "bulleted_list";
   // $item["id"]    = "yellow_bullets";
   // $item["x"]     = 0;
   // $item["y"]     = ["green_bullets",0.02];
   // $item["title"] = "#nbackrt.affected_areas.yellow#";
   // $item["dynamic_bullets"] = [
   //    "basestring" => "nbackrt.affected_areas", // This only represents the base from which the strings are generated. 
   //    "ref_color"  => "yellow",
   //    "variables" => $valid_vars
   // ]; 
   // $page["items"][] = $item;
   
   // $item = [];
   // $item["type"] = "bulleted_list";
   // $item["id"]   = "red_bullets";
   // $item["x"]     = 0;
   // $item["y"]     = ["yellow_bullets",0.02];
   // $item["title"] = "#nbackrt.affected_areas.red#";
   // $item["dynamic_bullets"] = [
   //    "basestring" => "nbackrt.affected_areas", // This only represents the base from which the strings are generated. 
   //    "ref_color"  => "red",
   //    "variables" => $valid_vars
   // ]; 
   // $page["items"][] = $item;

   // // Adding the presumptive diagnosis.
   // $item = [];
   // $w = 0.98;
   // $h = 0.12;
   // $item["id"]         = "pdiag";
   // $item["x"] = (1 - $w)/2;
   // $item["y"] = $ysubtitle - $h*1.5; 
   // $item["w"] = $w;
   // $item["h"] = $h;
   // $item["type"]        = "pdiag";
   // $item["text_base"]   = "binding.presumptive_diagnosis.texts";
   // $item["text_index"]  = "&presumptive_diagnosis&";
   // $item["value"]       = "&parahipocampal_region&";
   // $item["left_title"]  = "#nbackrt.presumptive_diagnosis.left_title#";
   // $item["right_title"] = "#nbackrt.presumptive_diagnosis.right_title#";
   // $item["separator"]  = 0.66;   
   // $page["items"][] = $item;

   // Overall performance. 
   $item = [];
   $item["type"] = "text";
   $item["text"] = "#nbackrt.overall_performance.text#";
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
    "&indexes.polygon.visual_exploration_efficiency&",
    "&indexes.polygon.visual_working_memory&",
    "&indexes.polygon.correct_responses&",
    "&indexes.polygon.working_memory&",
    "&indexes.polygon.inhibitory_processes&",
    "&indexes.polygon.decoding_efficiency&",
    "&indexes.polygon.enconding_efficiency&"    
   ];
   
   $item["vertex_labels"] = [
      "#nbackrt.overall_performance.vertex.visual_exploration_efficiency#",
      "#nbackrt.overall_performance.vertex.visual_working_memory#",
      "#nbackrt.overall_performance.vertex.correct_responses#",
      "#nbackrt.overall_performance.vertex.working_memory#",
      "#nbackrt.overall_performance.vertex.inhibitory_processes#",
      "#nbackrt.overall_performance.vertex.decoding_efficiency#",
      "#nbackrt.overall_performance.vertex.enconding_efficiency#"
   ];
   $page["items"][] = $item;

   return $page;

?>
