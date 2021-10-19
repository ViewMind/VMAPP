 
<?php

/**
 * Generates the layout JSON for Pages that are common to all reports..
 * PDF Gen Version: 1
 */

$ret = array();
$ret["uuid"] = "qc";
$ret["categories"] = [];   // To avoid creation warning. Unused. 

$pages = [];
$pages[] = include("common/qc.php");

$ret["pages"] = $pages;

return $ret;

?> 
