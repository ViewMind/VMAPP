 
<?php

$page = array();

$ysubtitle = 0.55;

$page["title"] = [
   ["text" => "#qc.title#", "py" => 0],
   ["text" => "#qc.comments#", "py" => $ysubtitle],
];



$page["items"] = array();
$y = 0.01;
$h = 0.10;

$item = [];
$item["type"]            = "result_segment";
$item["id"]              = "data_points";
$item["title"]           = "#qc.data_points.title#";
$item["range_text"]      = "#qc.data_points.subtitle#";
$item["x"]               = 0;
$item["y"]               = $y;
$item["w"]               = 1;
$item["h"]               = $h;
$item["value"]           = "&qc_graphs.STUDY_NAME.qc_data_point_index&";
$item["color_code"]      = "";
$item["nsegments"]       = 2;
$item["ndecimals"]       = 0;
$page["items"][] = $item;

$y = $y + $h;

$item = [];
$item["type"]            = "result_segment";
$item["id"]              = "fixations";
$item["title"]           = "#qc.fixations.title#";
$item["range_text"]      = "#qc.fixations.subtitle#";
$item["x"]               = 0;
$item["y"]               = $y;
$item["w"]               = 1;
$item["h"]               = $h;
$item["value"]           = "&qc_graphs.STUDY_NAME.qc_fix_index&";
$item["color_code"]      = "";
$item["nsegments"]       = 2;
$item["ndecimals"]       = 0;
$page["items"][] = $item;


$y = $y + $h;

$item = [];
$item["type"]            = "result_segment";
$item["id"]              = "sample_rate";
$item["title"]           = "#qc.sample_rate.title#";
$item["range_text"]      = "#qc.sample_rate.subtitle#";
$item["x"]               = 0;
$item["y"]               = $y;
$item["w"]               = 1;
$item["h"]               = $h;
$item["value"]           = "&qc_graphs.STUDY_NAME.qc_freq_index&";
$item["color_code"]      = "";
$item["nsegments"]       = 2;
$item["ndecimals"]       = 0;
$page["items"][] = $item;


// Data quality conclusion.
$item = [];
$w = 0.96;
$h = 0.12;
$item["id"] = "qc_conclusion";
$item["x"]  = (1 - $w)/2;
$item["y"]  = $y + $h*1.2; 
$item["w"]  = $w;
$item["h"]  = $h;
$item["type"]        = "pdiag";
$item["text_base"]   = "qc.results";
$item["text_index"]  = "";
$item["value"]       = "";
$item["left_title"]  = "#qc.results.title#";
$item["right_title"] = ""; // This will not render the right hand side. 
$item["separator"]  = 0.66;   
$page["items"][] = $item;


// Comment box
$item = [];
$w = 0.96;
$item["id"] = "comment_box";
$item["x"]  = (1 - $w)/2;
$item["w"]  = $w;
$item["y"]  = $ysubtitle + 0.03; 
$item["h"]  = 0.37;
$item["text"] = "&comment&";
$item["type"]        = "text_box";
$page["items"][] = $item;

return $page;

?>