<?php
   $page = array();
   $page["title"] = [
      ["text" => "#gonogo.page_titles.0.0#", "py" => 0]
   ];
   $page["items"] = array();
   $y = 0.01;
   $h = 0.098;
   
   $item = [];
   $item["type"]            = "result_segment";
   $item["id"]              = "processing_speed";
   $item["title"]           = "#gonogo.processing_speed.title#";
   $item["range_text"]      = "#gonogo.processing_speed.range_text#";
   $item["x"]               = 0;
   $item["y"]               = $y;
   $item["w"]               = 1;
   $item["h"]               = $h;
   $item["value"]           = "&processing_speed&";
   $item["color_code"]      = "&indexes.color.processing_speed&";
   $item["nsegments"]       = 3;
   $item["ndecimals"]       = 2;
   $page["items"][] = $item;

   $item = [];
   $y = $y + $h;
   $item["type"]            = "result_segment";
   $item["id"]              = "processing_speed_facilitated";
   $item["title"]           = "#gonogo.processing_speed_facilitated.title#";
   $item["range_text"]      = "#gonogo.processing_speed_facilitated.range_text#";
   $item["x"]               = 0;
   $item["y"]               = $y;
   $item["w"]               = 1;
   $item["h"]               = $h;
   $item["value"]           = "&processing_speed_facilitated&";
   $item["color_code"]      = "&indexes.color.processing_speed_facilitated&";
   $item["nsegments"]       = 3;
   $item["ndecimals"]       = 2;
   $page["items"][] = $item;

   $item = [];
   $y = $y + $h;
   $item["type"]            = "result_segment";
   $item["id"]              = "processing_speed_interference";
   $item["title"]           = "#gonogo.processing_speed_interference.title#";
   $item["range_text"]      = "#gonogo.processing_speed_interference.range_text#";
   $item["x"]               = 0;
   $item["y"]               = $y;
   $item["w"]               = 1;
   $item["h"]               = $h;
   $item["value"]           = "&processing_speed_interference&";
   $item["color_code"]      = "&indexes.color.processing_speed_interference&";
   $item["nsegments"]       = 3;
   $item["ndecimals"]       = 2;
   $page["items"][] = $item;
   

   $item = [];
   $y = $y + $h;
   $item["type"]            = "result_segment";
   $item["title"]           = "#gonogo.reaction_time_facilitated.title#";
   $item["id"]              = "reaction_time_facilitated";
   $item["range_text"]      = "#gonogo.reaction_time_facilitated.range_text#";
   $item["x"]               = 0;
   $item["y"]               = $y;
   $item["w"]               = 1;
   $item["h"]               = $h;
   $item["value"]           = "&reaction_time_facilitated&";
   $item["color_code"]      = "&indexes.color.reaction_time_facilitated&";
   $item["nsegments"]       = 3;
   $item["ndecimals"]       = 0;
   $page["items"][] = $item;
   
   $item = [];
   $y = $y + $h;
   $item["type"]            = "result_segment";
   $item["title"]           = "#gonogo.reaction_time_interference.title#";
   $item["range_text"]      = "#gonogo.reaction_time_interference.range_text#";
   $item["id"]              = "reaction_time_interference";
   $item["x"]               = 0;
   $item["y"]               = $y;
   $item["w"]               = 1;
   $item["h"]               = $h;
   $item["value"]           = "&reaction_time_interference&";
   $item["color_code"]      = "&indexes.color.reaction_time_interference&";
   $item["nsegments"]       = 3;
   $item["ndecimals"]       = 0;
   $page["items"][] = $item;
   
   $item = [];
   $y = $y + $h;
   $item["type"]            = "result_segment";
   $item["title"]           = "#gonogo.executive_functions.title#";
   $item["range_text"]      = "#gonogo.executive_functions.range_text#";
   $item["id"]              = "executive_functions";
   $item["x"]               = 0;
   $item["y"]               = $y;
   $item["w"]               = 1;
   $item["h"]               = $h;
   $item["value"]           = "&executive_functions&";
   $item["color_code"]      = "&indexes.color.executive_functions&";
   $item["nsegments"]       = 3;
   $item["ndecimals"]       = 0;
   $page["items"][] = $item;

   $item = [];
   $y = $y + $h;
   $item["type"]            = "result_segment";
   $item["title"]           = "#gonogo.index_error_facilitated.title#";
   $item["range_text"]      = "#gonogo.index_error_facilitated.range_text#";
   $item["id"]              = "index_error_facilitated";
   $item["x"]               = 0;
   $item["y"]               = $y;
   $item["w"]               = 1;
   $item["h"]               = $h;
   $item["value"]           = "&index_error_facilitated&";
   $item["color_code"]      = "&indexes.color.index_error_facilitated&";
   $item["nsegments"]       = 3;
   $item["ndecimals"]       = 2;
   $page["items"][] = $item;

   $item = [];
   $y = $y + $h;
   $item["type"]            = "result_segment";
   $item["title"]           = "#gonogo.index_error_interference.title#";
   $item["range_text"]      = "#gonogo.index_error_interference.range_text#";
   $item["id"]              = "index_error_interference";
   $item["x"]               = 0;
   $item["y"]               = $y;
   $item["w"]               = 1;
   $item["h"]               = $h;
   $item["value"]           = "&index_error_interference&";
   $item["color_code"]      = "&indexes.color.index_error_interference&";
   $item["nsegments"]       = 3;
   $item["ndecimals"]       = 2;
   $page["items"][] = $item;
   
   $item = [];
   $item["type"] = "color_code";
   $item["id"]   = "res_seg_color_code";
   $h = 0.15;
   $y = 1 - $h;
   $item["x"]               = 0;
   $item["y"]               = $y;
   $item["w"]               = 1;
   $item["h"]               = $h;
   $item["color_texts"]     = "#gonogo.color_code.explanations#";
   $item["title"]           = "#gonogo.color_code.title#";
   $page["items"][] = $item;
   
   return $page;
?>
