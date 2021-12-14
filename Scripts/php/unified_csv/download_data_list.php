<?php

const DL_DIR = "download_dir";
const RAW_DATA_DIR = "raw_data";
const S3_BUCKET    = "vm-eu-raw-data";
const S3_PROFILE   = "viewmind_eu";
const INPUT_FILE_LIST = "data_list";

shell_exec("rm -rf "  . DL_DIR);
shell_exec("mkdir -p " . DL_DIR);
shell_exec("mkdir -p " . RAW_DATA_DIR);


echo "Loading Data File\n";
$file_list = explode("\n",file_get_contents(INPUT_FILE_LIST));

foreach ($file_list as $raw_file_link) {
   
   if ($raw_file_link === "") continue;
   $filelink = $raw_file_link . ".zip";

   echo "   Downloading from s3 ...\n";
   $cmd = "aws s3 cp s3://" . S3_BUCKET . "/$filelink --profile=" . S3_PROFILE . " .";
   echo "   CMD: $cmd\n";
   shell_exec($cmd);

   echo "   Untarring ...\n";
   $cmd = "tar -xvzf $filelink";
   echo "   CMD: $cmd\n";
   shell_exec($cmd);

   echo "   Removing zip file ...\n";
   shell_exec("rm $filelink");

   echo "   Flattening the file path....";

   $json = "home";
   while (!is_file($json)) {
      $all = scandir($json);
      array_shift($all);
      array_shift($all);
      if (count($all) !=  1) {
         echo "Flattening failed. Current path of $json does not contain a single directory\n";
         exit();
      }
      $json = $json . "/" . $all[0];
   }

   echo "   File path $json\n";
   $pathinfo = pathinfo($json);
   $filename = RAW_DATA_DIR . "/" . $pathinfo["filename"] . ".json";
   shell_exec("mv $json $filename");

   // Doign clean up
   shell_exec("rm -rf home");

}




?>