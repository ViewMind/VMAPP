<?php
   $page = array();
   $ysubtitle = 0.5;
   $page["title"] = [
      ["text" => "#gonogo.page_titles.2.0#", "py" => 0],
      ["text" => "#gonogo.page_titles.2.1#", "py" => $ysubtitle]
   ];
   $page["items"] = array();


   //----------------------- Gaze Duration 4 dot graph. 
   $item = [];
   $w = 0.4;
   $x = (1-$w)/2; // This would be the x value to center the graph
   $item["type"] = "single_4_pt_graph";
   $item["id"] = "gaze_duration_graph";
   $item["x"] = $x;
   $item["y"] = 0.02;
   $item["w"] = $w;
   $item["h"] = 0.3;
   $item["left_column_text"]  = "#gonogo.gaze_duration.xlabel.0#";
   $item["right_column_text"] = "#gonogo.gaze_duration.xlabel.1#";
   $item["ylabel"] = "#gonogo.gaze_duration.ylabel#";
   $item["yunits"] = "";
   $item["legend_data_set_0"] = "#gonogo.gaze_duration.legend.0#";
   $item["legend_data_set_1"] = "#gonogo.gaze_duration.legend.1#";
   // The control data. Faciliated (G)/ Inhibited (R)
   $item["data_set_left"]  = [ 525.2137, 6.434115,  563.5136, 6.109360];
   // The patient data. Facilitated (G) / Inhibited (R)
   $item["data_set_right"] = [ "&gaze_duration.facilitated_task.0&", "&gaze_duration.facilitated_task.1&", 
                               "&gaze_duration.inhibite_task.0&", "&gaze_duration.inhibite_task.1&" ];

   $page["items"][] = $item;

   // Text below the graph. 
   $item = [];
   $item["type"] = "text";
   $item["text"] = "#gonogo.gaze_duration.text#";
   $item["id"]   = "gaze_duration_text";
   $item["x"] = 0;   
   $item["y"] = ["gaze_duration_graph", 0.05];
   $page["items"][] = $item;     

   return $page;
?>
