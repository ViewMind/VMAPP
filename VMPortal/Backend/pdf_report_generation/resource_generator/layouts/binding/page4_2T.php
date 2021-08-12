<?php
   $page = array();
   $ysubtitle = 0.5;
   $page["title"] = [
      ["text" => "#binding.page_titles.4.0#", "py" => 0],
      ["text" => "#binding.page_titles.4.1#", "py" => $ysubtitle]
   ];
   $page["items"] = array();

   //----------------------- Fixation Duration 4 dot graph. There two side by side. 
   $item = [];
   $item["type"] = "side_by_side_4_pt_graph";
   $item["id"] = "fixation_duration_graph";
   $item["left_graph"] = array();
   $item["left_column_text"] = "#binding.fixation_duration.xlabel.0#";
   $item["right_column_text"] = "#binding.fixation_duration.xlabel.1#";
   $item["ylabel"] = "#binding.fixation_duration.ylabel#";
   $item["yunits"] = "ms";
   $item["legend_data_set_0"] = "#binding.fixation_duration.legend.0#";
   $item["legend_data_set_1"] = "#binding.fixation_duration.legend.1#";

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
   $item["left_graph"]["left_set"] = [ "&bc.duration.encoding.0&", "&bc.duration.encoding.1&", "&uc.duration.encoding.0&", "&uc.duration.encoding.1&" ];
   // BC Retrieval / UC Retrieval.  
   $item["left_graph"]["right_set"] = [ "&bc.duration.retrieval.0&", "&bc.duration.retrieval.1&", "&uc.duration.retrieval.0&", "&uc.duration.retrieval.1&" ];
   $item["left_graph"]["title"] = "#binding.fixation_duration.graph_title.0#";

   // Conntrol Graph.
   $item["right_graph"]["x"] = $x + $w*$separation_multiplier;
   $item["right_graph"]["y"] = $yvalue;
   $item["right_graph"]["w"] = $base_w;
   $item["right_graph"]["h"] = $height;
   // BC Econding/ UC Encondiing.  
   $item["right_graph"]["left_set"]  = [227.2522,2.004669,213.3064,1.5859984];
   // BC Retrieval / UC Retrieval.  
   $item["right_graph"]["right_set"] = [219.5501,2.0547333,195.709,1.5606745];
   $item["right_graph"]["title"] = "#binding.fixation_duration.graph_title.1#";

   $page["items"][] = $item;

   // Text below the graph. 
   $item = [];
   $item["type"] = "text";
   $item["text"] = "#binding.fixation_duration.text#";
   $item["id"]   = "fixation_duration_text";
   $item["x"] = 0;   
   $item["y"] = ["fixation_duration_graph", 0.05];
   $page["items"][] = $item;   


   //----------------------- Num Fixations 4 dot graph. There two side by side. 
   $item = [];
   $item["type"] = "side_by_side_4_pt_graph";
   $item["id"] = "num_fixations_graph";
   $item["left_graph"] = array();
   $item["left_column_text"] = "#binding.number_of_fixations.xlabel.0#";
   $item["right_column_text"] = "#binding.number_of_fixations.xlabel.1#";
   $item["ylabel"] = "#binding.number_of_fixations.ylabel#";
   $item["yunits"] = "";
   $item["legend_data_set_0"] = "#binding.number_of_fixations.legend.0#";
   $item["legend_data_set_1"] = "#binding.number_of_fixations.legend.1#";
   //$item["ndecimals"] = 2;

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
   $item["left_graph"]["left_set"] = [ "&bc.num_fixations.encoding.0&", "&bc.num_fixations.encoding.1&", "&uc.num_fixations.encoding.0&", "&uc.num_fixations.encoding.1&" ];
   // BC Retrieval / UC Retrieval.  
   $item["left_graph"]["right_set"] = [ "&bc.num_fixations.retrieval.0&", "&bc.num_fixations.retrieval.1&", "&uc.num_fixations.retrieval.0&", "&uc.num_fixations.retrieval.1&" ];
   $item["left_graph"]["title"] = "#binding.number_of_fixations.graph_title.0#";

   // Conntrol Graph.
   $item["right_graph"]["x"] = $x + $w*$separation_multiplier;
   $item["right_graph"]["y"] = $yvalue;
   $item["right_graph"]["w"] = $base_w;
   $item["right_graph"]["h"] = $height;
   // BC Econding/ UC Encondiing.  
   $item["right_graph"]["left_set"]  = [8.443308,0.03341333,9.083555,0.03164692];
   // BC Retrieval / UC Retrieval.  
   $item["right_graph"]["right_set"] = [7.263889,0.06063463,9.711452,0.11451316];
   $item["right_graph"]["title"] = "#binding.number_of_fixations.graph_title.1#";

   $page["items"][] = $item;

   // Text below the graph. 
   $item = [];
   $item["type"] = "text";
   $item["text"] = "#binding.number_of_fixations.text#";
   $item["id"]   = "num_fixations_graph_text";
   $item["x"] = 0;   
   $item["y"] = ["num_fixations_graph", 0.05];
   $page["items"][] = $item;     
   
   return $page;
   
?>
