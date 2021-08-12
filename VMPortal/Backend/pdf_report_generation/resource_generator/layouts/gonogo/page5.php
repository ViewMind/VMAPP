<?php
   $page = array();
   $page["title"] = [
      ["text" => "#gonogo.page_titles.4.0#", "py" => 0],
   ];
   $page["items"] = array();

   // Text before the fixations
   $item = [];
   $item["type"] = "text";
   $item["text"] = "#gonogo.fixation_plotting.text#";
   $item["id"]   = "fixation_plotting_text";
   $item["x"] = 0;   
   $item["y"] = 0.02;
   $page["items"][] = $item;     

   // Some geometry math. 
   $w = 0.3;    // Width of each fixation diagram.
   $h = (1/6);  // Height of each fixation diagram. 
   $space_between_columns = $w*0.2;
   $space_between_rows    = 0.02;
   $x_col_left = (1 - ($w + $space_between_columns + $w))/2;
   $x_col_right = $x_col_left + $w + $space_between_columns;
   $row_y = 0.15;

   /// First Row.
   $item = [];
   $item["type"] = "fixation_plot";
   $item["id"]   = "fix11";
   $item["x"] = $x_col_left;
   $item["y"] = $row_y;
   $item["w"] = $w;
   $item["h"] = $h;
   $item["label"] = "#gonogo.fixation_plotting.row_1#";
   $item["title"] = "#gonogo.fixation_plotting.title_left#";
   $item["resolution"] = "&resolution&";
   $item["gonogo"] = "G->";
   $item["fixations"] = "&fixations.10&";
   $page["items"][] = $item;

   $item = [];
   $item["type"] = "fixation_plot";
   $item["id"]   = "fix12";
   $item["x"] = $x_col_right;
   $item["y"] = $row_y;
   $item["w"] = $w;
   $item["h"] = $h;
   $item["title"] = "#gonogo.fixation_plotting.title_right#";
   $item["resolution"] = [1366, 768];
   $item["gonogo"] = "G->";
   $item["fixations"] = [
      [592,439],
      [606,429],
      [649,412],
      [1043,412]
   ];
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
   $item["label"] = "#gonogo.fixation_plotting.row_2#";
   $item["resolution"] = "&resolution&";
   $item["gonogo"] = "R<-";
   $item["fixations"] = "&fixations.25&";
   $page["items"][] = $item;

   $item = [];
   $item["type"] = "fixation_plot";
   $item["id"]   = "fix22";
   $item["x"] = $x_col_right;
   $item["y"] = $row_y;
   $item["w"] = $w;
   $item["h"] = $h;
   $item["resolution"] = [1366, 768];
   $item["gonogo"] = "R<-";
   $item["fixations"] = [
      [696,331],
      [706,371],
      [674,374],
      [675,387],
      [688,404],
      [1168,396]
   ];
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
   $item["label"] = "#gonogo.fixation_plotting.row_3#";
   $item["resolution"] = "&resolution&";
   $item["gonogo"] = "R->";
   $item["fixations"] = "&fixations.40&";
   $page["items"][] = $item;

   $item = [];
   $item["type"] = "fixation_plot";
   $item["id"]   = "fix32";
   $item["x"] = $x_col_right;
   $item["y"] = $row_y;
   $item["w"] = $w;
   $item["h"] = $h;
   $item["resolution"] = [1366, 768];
   $item["gonogo"] = "R->";
   $item["fixations"] = [
      [637,373],
      [657,395],
      [631,441],
      [653,443],
      [145,417]
   ];
   $page["items"][] = $item;

   $row_y = $row_y + $h + $space_between_rows;   


   /// Third Row.
   $item = [];
   $item["type"] = "fixation_plot";
   $item["id"]   = "fix41";
   $item["x"] = $x_col_left;
   $item["y"] = $row_y;
   $item["w"] = $w;
   $item["h"] = $h;
   $item["label"] = "#gonogo.fixation_plotting.row_4#";
   $item["resolution"] = "&resolution&";
   $item["gonogo"] = "G<-";
   $item["fixations"] = "&fixations.55&";
   $page["items"][] = $item;

   $item = [];
   $item["type"] = "fixation_plot";
   $item["id"]   = "fix42";
   $item["x"] = $x_col_right;
   $item["y"] = $row_y;
   $item["w"] = $w;
   $item["h"] = $h;
   $item["resolution"] = [1366, 768];
   $item["gonogo"] = "G<-";
   $item["fixations"] = [
      [663,376],
      [676,400],
      [676,427],
      [625,392],
      [617,407],
      [619,419],
      [93,398],
   ];
   $page["items"][] = $item;

   $row_y = $row_y + $h + $space_between_rows; 
      

   return $page;
?>
