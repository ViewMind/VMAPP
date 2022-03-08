<?php
   $page = array();
   $page["title"] = [
      ["text" => "#nbackrt.page_titles.0.0#", "py" => 0]
   ];
   $page["items"] = array();
   $y = 0.01;
   $h = 0.098;
   
   $item = [];
   $item["type"]            = "result_segment";
   $item["id"]              = "visual_exploration_efficiency";
   $item["title"]           = "#nbackrt.visual_exploration_efficiency.title#";
   $item["range_text"]      = "#nbackrt.visual_exploration_efficiency.range_text#";
   $item["x"]               = 0;
   $item["y"]               = $y;
   $item["w"]               = 1;
   $item["h"]               = $h;
   $item["value"]           = "&visual_exploration_efficiency&";
   $item["color_code"]      = "&indexes.color.visual_exploration_efficiency&";
   $item["nsegments"]       = 4;
   $item["ndecimals"]       = 2;
   $page["items"][] = $item;

   $item = [];
   $y = $y + $h;
   $item["type"]            = "result_segment";
   $item["id"]              = "visual_working_memory";
   $item["title"]           = "#nbackrt.visual_working_memory.title#";
   $item["range_text"]      = "#nbackrt.visual_working_memory.range_text#";
   $item["x"]               = 0;
   $item["y"]               = $y;
   $item["w"]               = 1;
   $item["h"]               = $h;
   $item["value"]           = "&visual_working_memory&";
   $item["color_code"]      = "&indexes.color.visual_working_memory&";
   $item["nsegments"]       = 4;
   $item["ndecimals"]       = 0;
   $page["items"][] = $item;
   
   $item = [];
   $y = $y + $h;
   $item["type"]            = "result_segment";
   $item["id"]              = "correct_responses";
   $item["title"]           = "#nbackrt.correct_responses.title#";
   $item["range_text"]      = "#nbackrt.correct_responses.range_text#";
   $item["x"]               = 0;
   $item["y"]               = $y;
   $item["w"]               = 1;
   $item["h"]               = $h;
   $item["value"]           = "&correct_responses&";
   $item["color_code"]      = "&indexes.color.correct_responses&";
   $item["nsegments"]       = 4;
   $item["ndecimals"]       = 2;
   $page["items"][] = $item;

   $item = [];
   $y = $y + $h;
   $item["type"]            = "result_segment";
   $item["title"]           = "#nbackrt.working_memory.title#";
   $item["id"]              = "working_memory";
   $item["range_text"]      = "#nbackrt.working_memory.range_text#";
   $item["x"]               = 0;
   $item["y"]               = $y;
   $item["w"]               = 1;
   $item["h"]               = $h;
   $item["value"]           = "&working_memory&";
   $item["color_code"]      = "&indexes.color.working_memory&";
   $item["nsegments"]       = 4;
   $item["ndecimals"]       = 2;
   $page["items"][] = $item;
   
   $item = [];
   $y = $y + $h;
   $item["type"]            = "result_segment";
   $item["title"]           = "#nbackrt.inhibitory_processes.title#";
   $item["range_text"]      = "#nbackrt.inhibitory_processes.range_text#";
   $item["id"]              = "inhibitory_processes";
   $item["x"]               = 0;
   $item["y"]               = $y;
   $item["w"]               = 1;
   $item["h"]               = $h;
   $item["value"]           = "&inhibitory_processes&";
   $item["color_code"]      = "&indexes.color.inhibitory_processes&";
   $item["nsegments"]       = 4;
   $item["ndecimals"]       = 2;
   $page["items"][] = $item;
   
   $item = [];
   $y = $y + $h;
   $item["type"]            = "result_segment";
   $item["title"]           = "#nbackrt.decoding_efficiency.title#";
   $item["range_text"]      = "#nbackrt.decoding_efficiency.range_text#";
   $item["id"]              = "decoding_efficiency";
   $item["x"]               = 0;
   $item["y"]               = $y;
   $item["w"]               = 1;
   $item["h"]               = $h;
   $item["value"]           = "&decoding_efficiency&";
   $item["color_code"]      = "&indexes.color.decoding_efficiency&";
   $item["nsegments"]       = 4;
   $item["ndecimals"]       = 0;
   $page["items"][] = $item;

   $item = [];
   $y = $y + $h;
   $item["type"]            = "result_segment";
   $item["title"]           = "#nbackrt.enconding_efficiency.title#";
   $item["range_text"]      = "#nbackrt.enconding_efficiency.range_text#";
   $item["id"]              = "enconding_efficiency";
   $item["x"]               = 0;
   $item["y"]               = $y;
   $item["w"]               = 1;
   $item["h"]               = $h;
   $item["value"]           = "&enconding_efficiency&";
   $item["color_code"]      = "&indexes.color.enconding_efficiency&";
   $item["nsegments"]       = 4;
   $item["ndecimals"]       = 0;
   $page["items"][] = $item;
   
   $item = [];
   $item["type"] = "color_code";
   $item["id"]   = "res_seg_color_code";
   $h = 0.13;
   $y = 0.78;
   $item["x"]               = 0;
   $item["y"]               = $y;
   $item["w"]               = 1;
   $item["h"]               = $h;
   $item["color_texts"]     = "#nbackrt.color_code.explanations#";
   $item["title"]           = "#nbackrt.color_code.title#";
   $page["items"][] = $item;
   
   return $page;
?>
