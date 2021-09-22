<?php
   $page = array();
   $ysubtitle = 0.7;
   $page["title"] = [
      ["text" => "#nbackrt.page_titles.2.0#", "py" => 0],
      ["text" => "#nbackrt.page_titles.2.1#", "py" => $ysubtitle]
   ];
   $page["items"] = array();

   // Text before the fixations
   $item = [];
   $item["type"] = "text";
   $item["text"] = "#nbackrt.fixation_plotting.text#";
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
   $item["label"] = "#nbackrt.fixation_plotting.row_1#";
   $item["title"] = "#nbackrt.fixation_plotting.title_left#";
   $item["resolution"] = "&resolution&";
   $item["nbackrt"] = [];
   $item["fixations"] = "&fixations.12.encoding_1&";
   $page["items"][] = $item;

   $item = [];
   $item["type"] = "fixation_plot";
   $item["id"]   = "fix12";
   $item["x"] = $x_col_right;
   $item["y"] = $row_y;
   $item["w"] = $w;
   $item["h"] = $h;
   $item["title"] = "#nbackrt.fixation_plotting.title_right#";
   $item["resolution"] = "&resolution&";
   $item["nbackrt"] = [3,4,2];
   $item["fixations"] = "&fixations.12.retrieval_1&";
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
   $item["label"] = "#nbackrt.fixation_plotting.row_2#";
   $item["resolution"] = "&resolution&";
   $item["nbackrt"] = [];
   $item["fixations"] = "&fixations.50.encoding_1&";
   $page["items"][] = $item;
   
   $item = [];
   $item["type"] = "fixation_plot";
   $item["id"]   = "fix22";
   $item["x"] = $x_col_right;
   $item["y"] = $row_y;
   $item["w"] = $w;
   $item["h"] = $h;
   $item["resolution"] = "&resolution&";
   $item["nbackrt"] = [1,3,0];
   $item["fixations"] = "&fixations.50.retrieval_1&";
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
   $item["label"] = "#nbackrt.fixation_plotting.row_3#";
   $item["resolution"] = "&resolution&";
   $item["nbackrt"] = [ ];
   $item["fixations"] = "&fixations.90.encoding_1&";
   $page["items"][] = $item;
   
   $item = [];
   $item["type"] = "fixation_plot";
   $item["id"]   = "fix32";
   $item["x"] = $x_col_right;
   $item["y"] = $row_y;
   $item["w"] = $w;
   $item["h"] = $h;
   $item["resolution"] = "&resolution&";
   $item["nbackrt"] = [ 1,4,0 ];
   $item["fixations"] = "&fixations.90.retrieval_1&";
   $page["items"][] = $item;   

   // ///---------------- The Control Fixation Graph. 
   // $item = [];
   // $item["type"] = "text";
   // $item["text"] = "#nbackrt.fixation_plotting.control.text#";
   // $item["id"]   = "fixation_plotting_control_text";
   // $item["x"] = 0;   
   // $item["y"] = $ysubtitle + 0.02;
   // $page["items"][] = $item;      

   // $row_y = $ysubtitle + 0.11;

   // $item = [];
   // $item["type"] = "fixation_plot";
   // $item["id"]   = "control_left";
   // $item["x"] = $x_col_left;
   // $item["y"] = $row_y;
   // $item["w"] = $w;
   // $item["h"] = $h;
   // $item["label"] = "#nbackrt.fixation_plotting.control.label#";
   // $item["title"] = "#nbackrt.fixation_plotting.title_left#";
   // $item["legacy"] = true;
   // $item["resolution"] = [1366, 768];
   // $item["nbackrt"] = [ [1,2], [2,1], [0,0] ];
   // $item["fixations"] = [
   //    [409,136],
   //    [407,150],
   //    [406,160],
   //    [444,129],
   //    [442,144],
   //    [605,312],
   //    [688,311],
   //    [694,342],
   //    [669,554],
   //    [696,543],
   //    [909,322],
   //    [931,279],
   //    [943,300],
   //    [948,318],
   //    [972,301]
   // ];
   // $page["items"][] = $item;      

   // $item = [];
   // $item["type"] = "fixation_plot";
   // $item["id"]   = "control_rigt";
   // $item["x"] = $x_col_right;
   // $item["y"] = $row_y;
   // $item["w"] = $w;
   // $item["h"] = $h;
   // $item["legacy"] = true;
   // $item["title"] = "#nbackrt.fixation_plotting.title_right#";
   // $item["resolution"] = [1366, 768];
   // $item["nbackrt"] = [ [2,2], [0,2], [2,0] ];
   // $item["fixations"] = [
   //    [391,520],
   //    [437,506],
   //    [442,479],
   //    [950,465],
   //    [961,491],
   //    [976,458],
   //    [873,237],
   //    [893,255],
   //    [939,184],
   //    [961,163],
   //    [971,127]
   // ];   
   // $page["items"][] = $item;   
   
   
   return $page;
?>
