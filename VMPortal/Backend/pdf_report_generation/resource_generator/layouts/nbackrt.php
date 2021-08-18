<?php

/**
 * Generates the layout JSON for the GoNoGo Report.
 * PDF Gen Version: 1
 */

$ret = array();
$ret["uuid"] = "nbackrt";
$ret["categories"] = include("nbackrt/categories.php");
//$ret["pdiag"] = include("binding/pdiag_logic_3T.php");

$pages = [];
$pages[] = include("nbackrt/page1.php");
$pages[] = include("nbackrt/page2.php");
$pages[] = include("nbackrt/page3.php");

$ret["pages"] = $pages;

return $ret;

?> 
