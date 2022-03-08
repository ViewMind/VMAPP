<?php

/**
 * This file takes txt list of file IDs and downloads them locally. 
 */

//////////////////////////////////////////////////// CONSTANTS ///////////////////////////////////////////////////////////////

const WORK_DIRECTORY = "work";
const DL_DOWNLOAD_LIST = "filelist.cnf"; // CNF is used as it is added to gitignore. 
const S3_BUCKET    = "vm-eu-raw-data";
const S3_PROFILE   = "viewmind_eu";

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

$dl_list = file_get_contents(DL_DOWNLOAD_LIST);
$file_list = explode("\n",$dl_list);
$wdir = WORK_DIRECTORY;


foreach ($file_list as $flink) {

   $flink = trim($flink);
   if ($flink == "") continue;
   
   $wdir = WORK_DIRECTORY . "/$flink";
   shell_exec("mkdir -p $wdir");

   $flink = "$flink.zip";

   // Downloading each file from S3
   echo "Downloading from s3 $flink...\n";
   $cmd = "aws s3 cp s3://" . S3_BUCKET . "/$flink --profile=" . S3_PROFILE . " .";
   //echo "CMD: $cmd\n";
   shell_exec($cmd);
   echo "Untarring ...\n";
   $cmd = "tar -xvzf $flink";
   echo "CMD: $cmd\n";
   shell_exec($cmd);
   echo "Removing zip file ...\n";
   shell_exec("rm $flink");
   echo "Flattening the file path....";
   $json = "home";
   while (!is_file($json)) {
       $all = scandir($json); // scandir ALWAYS returns the . and the .. as the first two entries. 
       array_shift($all); // Removing the .
       array_shift($all); // Removing the .. 
       if (count($all) !=  1) {
           echo "Flattening failed. Current path of $json does not contain a single directory\n";
           exit();
       }
       $json = $json . "/" . $all[0];
   }
   echo "File path $json\n";
   $pathinfo = pathinfo($json);
   $filename = "$wdir/" . $pathinfo["filename"] . ".json";
   shell_exec("mv $json $filename");
   // Doign clean up
   shell_exec("rm -rf home");
}


echo "Finished\n";



?>