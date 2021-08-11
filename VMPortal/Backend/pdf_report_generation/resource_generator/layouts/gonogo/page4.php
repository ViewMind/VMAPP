<?php
   $page = array();
   $ysubtitle = 0.5;
   $page["title"] = [
      ["text" => "#gonogo.page_titles.3.0#", "py" => 0],
      ["text" => "#gonogo.page_titles.3.1#", "py" => $ysubtitle]
   ];
   $page["items"] = array();


   //----------------------- Saccadic Behaviour 4 dot graph. 
   $item = [];
   $w = 0.4;
   $x = (1-$w)/2; // This would be the x value to center the graph
   $item["type"] = "single_4_pt_graph";
   $item["id"] = "saccadic_behaviour_graph";
   $item["x"] = $x;
   $item["y"] = 0.02;
   $item["w"] = $w;
   $item["h"] = 0.3;
   $item["left_column_text"]  = "#gonogo.saccadic_behaviour.xlabel.0#";
   $item["right_column_text"] = "#gonogo.saccadic_behaviour.xlabel.1#";
   $item["ylabel"] = "#gonogo.saccadic_behaviour.ylabel#";
   $item["yunits"] = "";
   $item["legend_data_set_0"] = "#gonogo.saccadic_behaviour.legend.0#";
   $item["legend_data_set_1"] = "#gonogo.saccadic_behaviour.legend.1#";
   // The control data. Faciliated (G)/ Inhibited (R)
   $item["data_set_left"]  = [ 2.916667, 0.1311396,  3.761308, 0.1486889];
   // The patient data. Facilitated (G) / Inhibited (R)
   $item["data_set_right"] = [ "&saccade.facilitated_task.0&", "&saccade.facilitated_task.1&", 
                               "&saccade.inhibite_task.0&", "&saccade.inhibite_task.1&" ];

   $page["items"][] = $item;

   // Text below the graph. 
   $item = [];
   $item["type"] = "text";
   $item["text"] = "#gonogo.saccadic_behaviour.text#";
   $item["id"]   = "saccadic_behaviour_text";
   $item["x"] = 0;   
   $item["y"] = ["saccadic_behaviour_graph", 0.05];
   $page["items"][] = $item;     

   //----------------------- Fixation Duration 4 dot graph. 
   $item = [];
   $w = 0.4;
   $x = (1-$w)/2; // This would be the x value to center the graph
   $item["type"] = "single_4_pt_graph";
   $item["id"] = "fixation_duration_graph";
   $item["x"] = $x;
   $item["y"] = $ysubtitle + 0.02;;
   $item["w"] = $w;
   $item["h"] = 0.3;
   $item["left_column_text"]  = "#gonogo.fixation_duration.xlabel.0#";
   $item["right_column_text"] = "#gonogo.fixation_duration.xlabel.1#";
   $item["ylabel"] = "#gonogo.fixation_duration.ylabel#";
   $item["yunits"] = "";
   $item["legend_data_set_0"] = "#gonogo.fixation_duration.legend.0#";
   $item["legend_data_set_1"] = "#gonogo.fixation_duration.legend.1#";
   // The control data. Faciliated (G)/ Inhibited (R)
   $item["data_set_left"]  = [ 121.5165, 3.146649,  120.7572, 2.65294];
   // The patient data. Facilitated (G) / Inhibited (R)
   $item["data_set_right"] = [ "&duration.facilitated_task.0&", "&duration.facilitated_task.1&", 
                               "&duration.inhibite_task.0&", "&duration.inhibite_task.1&" ];

   $page["items"][] = $item;

   // Text below the graph. 
   $item = [];
   $item["type"] = "text";
   $item["text"] = "#gonogo.fixation_duration.text#";
   $item["id"]   = "fixation_duration_text";
   $item["x"] = 0;   
   $item["y"] = ["fixation_duration_graph", 0.05];
   $page["items"][] = $item;     


   return $page;
?>
