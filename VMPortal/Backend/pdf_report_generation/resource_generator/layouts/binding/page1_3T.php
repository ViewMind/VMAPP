<?php
   $page = array();
   $page["title"] = [
      ["text" => "#binding.page_titles.1.0#", "py" => 0]
   ];
   $page["items"] = array();
   $y = 0.01;
   $h = 0.11;
   
   $item = [];
   $item["type"]            = "result_segment";
   $item["id"]              = "visual_search";
   $item["title"]           = "#binding.visual_search.title#";
   $item["range_text"]      = "#binding.visual_search.range_text#";
   $item["x"]               = 0;
   $item["y"]               = $y;
   $item["w"]               = 1;
   $item["h"]               = $h;
   $item["value"]           = "&visual_search&";
   $item["color_code"]      = "&indexes.color.visual_search&";
   $item["nsegments"]       = 3;
   $item["ndecimals"]       = 1;
   $page["items"][] = $item;

   $item = [];
   $y = $y + $h;
   $item["type"]            = "result_segment";
   $item["id"]              = "gaze_dur";
   $item["title"]           = "#binding.gaze_dur.title#";
   $item["range_text"]      = "#binding.gaze_dur.range_text#";
   $item["x"]               = 0;
   $item["y"]               = $y;
   $item["w"]               = 1;
   $item["h"]               = $h;
   $item["value"]           = "&gaze_dur&";
   $item["color_code"]      = "&indexes.color.gaze_dur&";
   $item["nsegments"]       = 3;
   $item["ndecimals"]       = 0;
   $page["items"][] = $item;
   
   $item = [];
   $y = $y + $h;
   $item["type"]            = "result_segment";
   $item["id"]              = "int_mem_perf";
   $item["title"]           = "#binding.int_mem_perf.title#";
   $item["range_text"]      = "#binding.int_mem_perf.range_text#";
   $item["x"]               = 0;
   $item["y"]               = $y;
   $item["w"]               = 1;
   $item["h"]               = $h;
   $item["value"]           = "&int_mem_perf&";
   $item["color_code"]      = "&indexes.color.int_mem_perf&";
   $item["nsegments"]       = 3;
   $item["ndecimals"]       = 0;
   $page["items"][] = $item;

   $item = [];
   $y = $y + $h;
   $item["type"]            = "result_segment";
   $item["title"]           = "#binding.assoc_mem_perf.title#";
   $item["id"]              = "assoc_mem_perf";
   $item["range_text"]      = "#binding.assoc_mem_perf.range_text#";
   $item["x"]               = 0;
   $item["y"]               = $y;
   $item["w"]               = 1;
   $item["h"]               = $h;
   $item["value"]           = "&assoc_mem_perf&";
   $item["color_code"]      = "&indexes.color.assoc_mem_perf&";
   $item["nsegments"]       = 3;
   $item["ndecimals"]       = 0;
   $page["items"][] = $item;
   
   $item = [];
   $y = $y + $h;
   $item["type"]            = "result_segment";
   $item["title"]           = "#binding.exec_functions.title#";
   $item["range_text"]      = "#binding.exec_functions.range_text_3T#";
   $item["id"]              = "exec_func";
   $item["x"]               = 0;
   $item["y"]               = $y;
   $item["w"]               = 1;
   $item["h"]               = $h;
   $item["value"]           = "&exec_func&";
   $item["color_code"]      = "&indexes.color.exec_func&";
   $item["nsegments"]       = 3;
   $item["ndecimals"]       = 0;
   $page["items"][] = $item;
   
   $item = [];
   $y = $y + $h;
   $item["type"]            = "result_segment";
   $item["title"]           = "#binding.parahipocampal_region.title#";
   $item["range_text"]      = "#binding.parahipocampal_region.range_text_3T#";
   $item["id"]              = "parahipocampal_region";
   $item["x"]               = 0;
   $item["y"]               = $y;
   $item["w"]               = 1;
   $item["h"]               = $h;
   $item["value"]           = "&parahipocampal_region&";
   $item["color_code"]      = "&indexes.color.parahipocampal_region&";
   $item["nsegments"]       = 3;
   $item["ndecimals"]       = 0;
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
   $item["color_texts"]     = "#binding.color_code.explanations#";
   $item["title"]           = "#binding.color_code.title#";
   $page["items"][] = $item;
   
   return $page;
?>
