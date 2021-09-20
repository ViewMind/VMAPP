<?php

/**
 * Generates the layout JSON for the GoNoGo Report.
 * PDF Gen Version: 1
 */

$ret = array();
$ret["uuid"] = "gonogo";
$ret["categories"] = include("gonogo/categories.php");
//$ret["pdiag"] = include("binding/pdiag_logic_3T.php");

$pages = [];
$pages[] = include("gonogo/page1.php");
$pages[] = include("gonogo/page2.php");
$pages[] = include("gonogo/page3.php");
$pages[] = include("gonogo/page4.php");
$pages[] = include("gonogo/page5.php");

$ret["pages"] = $pages;

return $ret;

?> 
