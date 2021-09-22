<?php
   $page = array();
   $ysubtitle = 0.7;
   $page["title"] = [
      ["text" => "#binding.page_titles.5.0#", "py" => 0],
      ["text" => "#binding.page_titles.5.1#", "py" => $ysubtitle]
   ];
   $page["items"] = array();

   // Text before the fixations
   $item = [];
   $item["type"] = "text";
   $item["text"] = "#binding.fixation_plotting.text#";
   $item["id"]   = "fixation_plotting_text";
   $item["x"] = 0;   
   $item["y"] = 0.02;
   $page["items"][] = $item;     

   // Some geometry math. 
   $w = 0.3;
   $h = (1/6);
   $space_between_columns = $w*0.2;
   $space_between_rows    = 0.02;
   $x_col_left = (1 - ($w + $space_between_columns + $w))/2;
   $x_col_right = $x_col_left + $w + $space_between_columns;
   $row_y = 0.11;

   /// First Row.
   $item = [];
   $item["type"] = "fixation_plot";
   $item["id"]   = "fix11";
   $item["x"] = $x_col_left;
   $item["y"] = $row_y;
   $item["w"] = $w;
   $item["h"] = $h;
   $item["label"] = "#binding.fixation_plotting.row_1#";
   $item["title"] = "#binding.fixation_plotting.title_left#";
   $item["resolution"] = "&resolution&";
   $item["binding"] =[ [0,0], [1,1] ];
   $item["fixations"] = "&fixations.10.encoding_1&";
   $page["items"][] = $item;

   $item = [];
   $item["type"] = "fixation_plot";
   $item["id"]   = "fix12";
   $item["x"] = $x_col_right;
   $item["y"] = $row_y;
   $item["w"] = $w;
   $item["h"] = $h;
   $item["title"] = "#binding.fixation_plotting.title_right#";
   $item["resolution"] = "&resolution&";
   $item["binding"] =  [ [0,2], [1,2] ];
   $item["fixations"] = "&fixations.10.retrieval_1&";
   $page["items"][] = $item;

   $row_y = $row_y + $h + $space_between_rows;

   /// Second Row.
   $item = [];
   $item["type"] = "fixation_plot";
   $item["id"]   = "fix21";
   $item["x"] = $x_col_left;
   $item["y"] = $row_y;
   $item["w"] = $w;
   $item["h"] = $h;
   $item["label"] = "#binding.fixation_plotting.row_2#";
   $item["resolution"] = "&resolution&";
   $item["binding"] = [ [0,2], [1,1] ];
   $item["fixations"] = "&fixations.20.encoding_1&";
   $page["items"][] = $item;
   
   $item = [];
   $item["type"] = "fixation_plot";
   $item["id"]   = "fix22";
   $item["x"] = $x_col_right;
   $item["y"] = $row_y;
   $item["w"] = $w;
   $item["h"] = $h;
   $item["resolution"] = "&resolution&";
   $item["binding"] = [ [0,0], [1,2] ];
   $item["fixations"] = "&fixations.20.retrieval_1&";
   $page["items"][] = $item;

   $row_y = $row_y + $h + $space_between_rows;

   /// Third Row.
   $item = [];
   $item["type"] = "fixation_plot";
   $item["id"]   = "fix31";
   $item["x"] = $x_col_left;
   $item["y"] = $row_y;
   $item["w"] = $w;
   $item["h"] = $h;
   $item["label"] = "#binding.fixation_plotting.row_3#";
   $item["resolution"] = "&resolution&";
   $item["binding"] = [ [0,1], [1,0] ];
   $item["fixations"] = "&fixations.32.encoding_1&";
   $page["items"][] = $item;
   
   $item = [];
   $item["type"] = "fixation_plot";
   $item["id"]   = "fix32";
   $item["x"] = $x_col_right;
   $item["y"] = $row_y;
   $item["w"] = $w;
   $item["h"] = $h;
   $item["resolution"] = "&resolution&";
   $item["binding"] = [ [0,2], [1,2] ];
   $item["fixations"] = "&fixations.32.retrieval_1&";
   $page["items"][] = $item;   

   ///---------------- The Control Fixation Graph. 
   $item = [];
   $item["type"] = "text";
   $item["text"] = "#binding.fixation_plotting.control.text#";
   $item["id"]   = "fixation_plotting_control_text";
   $item["x"] = 0;   
   $item["y"] = $ysubtitle + 0.02;
   $page["items"][] = $item;      

   $row_y = $ysubtitle + 0.11;

   $item = [];
   $item["type"] = "fixation_plot";
   $item["id"]   = "control_left";
   $item["x"] = $x_col_left;
   $item["y"] = $row_y;
   $item["w"] = $w;
   $item["h"] = $h;
   $item["label"] = "#binding.fixation_plotting.control.label#";
   $item["title"] = "#binding.fixation_plotting.title_left#";
   $item["legacy"] = true;
   $item["resolution"] = [1920, 1080];
   $item["binding"] = [ [0,2], [1,0] ];
   $item["fixations"] = [
      [740 ,410],
      [778 ,425],
      [689 ,622],
      [717 ,623],
      [669 ,665],
      [716 ,694],
      [1181,298],
      [1233,277],
      [1261,259],
      [1295,277],
      [1294,300],
      [1224,296],
      [1208,319],
      [1253,316],
      [1253,328]
   ];
   $page["items"][] = $item;      

   $item = [];
   $item["type"] = "fixation_plot";
   $item["id"]   = "control_rigt";
   $item["x"] = $x_col_right;
   $item["y"] = $row_y;
   $item["w"] = $w;
   $item["h"] = $h;
   $item["legacy"] = true;
   $item["title"] = "#binding.fixation_plotting.title_right#";
   $item["resolution"] = [1920, 1080];
   $item["binding"] = [ [0,1], [1,0] ];
   $item["fixations"] = [
      [875 ,418],
      [869 ,436],
      [929 ,430],
      [1221,287],
      [1195,314]
   ];   
   $page["items"][] = $item;   
   return $page;
?>
