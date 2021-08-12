<?php
   $page = array();
   $ysubtitle = 0.5;
   $page["title"] = [
      ["text" => "#binding.page_titles.3.0#", "py" => 0],
      ["text" => "#binding.page_titles.3.1#", "py" => $ysubtitle]
   ];
   $page["items"] = array();

   //----------------------- Gaze Duration 4 dot graph. There two side by side. 
   $item = [];
   $item["type"] = "side_by_side_4_pt_graph";
   $item["id"] = "gaze_duration_graph";
   $item["left_graph"] = array();
   $item["left_column_text"] = "#binding.gaze_duration.xlabel.0#";
   $item["right_column_text"] = "#binding.gaze_duration.xlabel.1#";
   $item["ylabel"] = "#binding.gaze_duration.ylabel#";
   $item["yunits"] = "ms";
   $item["legend_data_set_0"] = "#binding.gaze_duration.legend.0#";
   $item["legend_data_set_1"] = "#binding.gaze_duration.legend.1#";

   // Patient Graph.
   $base_w = 0.4;
   $yvalue = 0.02;
   $w = $base_w*1.08;
   $height = 0.3;
   $separation_multiplier = 1.02;
   // Computing width for centering.
   $total_w = $w*$separation_multiplier + $base_w;
   $x = (1 - $total_w)/2;
   $item["left_graph"]["x"] = $x;
   $item["left_graph"]["y"] = $yvalue;
   $item["left_graph"]["w"] = $w;
   $item["left_graph"]["h"] = $height;
   // BC Econding/ UC Encondiing.  
   $item["left_graph"]["left_set"] = [ "&bc.gazing.encoding.0&", "&bc.gazing.encoding.1&", "&uc.gazing.encoding.0&", "&uc.gazing.encoding.1&" ];
   // BC Retrieval / UC Retrieval.  
   $item["left_graph"]["right_set"] = [ "&bc.gazing.retrieval.0&", "&bc.gazing.retrieval.1&", "&uc.gazing.retrieval.0&", "&uc.gazing.retrieval.1&" ];
   $item["left_graph"]["title"] = "#binding.gaze_duration.graph_title.0#";

   // Conntrol Graph.
   $item["right_graph"]["x"] = $x + $w*$separation_multiplier;
   $item["right_graph"]["y"] = $yvalue;
   $item["right_graph"]["w"] = $base_w;
   $item["right_graph"]["h"] = $height;
   // BC Econding/ UC Encondiing.  
   $item["right_graph"]["left_set"]  = [1811.3527, 0.6886,  1786.9819, 0.835];
   // BC Retrieval / UC Retrieval.  
   $item["right_graph"]["right_set"] = [2030.2978, 4.8334,  1860.3862, 5.001];
   $item["right_graph"]["title"] = "#binding.gaze_duration.graph_title.1#";

   $page["items"][] = $item;

   // Text below the graph. 
   $item = [];
   $item["type"] = "text";
   $item["text"] = "#binding.gaze_duration.text#";
   $item["id"]   = "gazing_graph_text";
   $item["x"] = 0;   
   $item["y"] = ["gaze_duration_graph", 0.05];
   $page["items"][] = $item;   


   //----------------------- Visual Scanning 4 dot graph. There two side by side. 
   $item = [];
   $item["type"] = "side_by_side_4_pt_graph";
   $item["id"] = "visual_scanning_graph";
   $item["left_graph"] = array();
   $item["left_column_text"] = "#binding.visual_scanning.xlabel.0#";
   $item["right_column_text"] = "#binding.visual_scanning.xlabel.1#";
   $item["ylabel"] = "#binding.visual_scanning.ylabel#";
   $item["yunits"] = "";
   $item["legend_data_set_0"] = "#binding.visual_scanning.legend.0#";
   $item["legend_data_set_1"] = "#binding.visual_scanning.legend.1#";
   $item["ndecimals"] = 2;

   // Patient Graph.
   $base_w = 0.4;
   $yvalue = $ysubtitle + 0.02;
   $w = $base_w*1.08;
   $height = 0.3;
   $separation_multiplier = 1.02;
   // Computing width for centering.
   $total_w = $w*$separation_multiplier + $base_w;
   $x = (1 - $total_w)/2;
   $item["left_graph"]["x"] = $x;
   $item["left_graph"]["y"] = $yvalue;
   $item["left_graph"]["w"] = $w;
   $item["left_graph"]["h"] = $height;
   // BC Econding/ UC Encondiing.  
   $item["left_graph"]["left_set"] = [ "&bc.saccade.encoding.0&", "&bc.saccade.encoding.1&", "&uc.saccade.encoding.0&", "&uc.saccade.encoding.1&" ];
   // BC Retrieval / UC Retrieval.  
   $item["left_graph"]["right_set"] = [ "&bc.saccade.retrieval.0&", "&bc.saccade.retrieval.1&", "&uc.saccade.retrieval.0&", "&uc.saccade.retrieval.1&" ];
   $item["left_graph"]["title"] = "#binding.visual_scanning.graph_title.0#";

   // Conntrol Graph.
   $item["right_graph"]["x"] = $x + $w*$separation_multiplier;
   $item["right_graph"]["y"] = $yvalue;
   $item["right_graph"]["w"] = $base_w;
   $item["right_graph"]["h"] = $height;
   // BC Econding/ UC Encondiing.  
   $item["right_graph"]["left_set"]  = [2.2052, 0.0113, 2.3637, 0.0125];
   // BC Retrieval / UC Retrieval.  
   $item["right_graph"]["right_set"] = [2.7807, 0.0177, 2.7588, 0.0181];
   $item["right_graph"]["title"] = "#binding.visual_scanning.graph_title.1#";

   $page["items"][] = $item;

   // Text below the graph. 
   $item = [];
   $item["type"] = "text";
   $item["text"] = "#binding.visual_scanning.text#";
   $item["id"]   = "visual_scanning_graph_text";
   $item["x"] = 0;   
   $item["y"] = ["visual_scanning_graph", 0.05];
   $page["items"][] = $item;     
   
   return $page;
?>
