<?php

/***
 * The script uses php mostly to help with string and JSON manipulation. I actually simply executes a series of curl command calls. 
 */

//////////////////////////////////////// Setup credentials and constants ////////////////////////////////////////
//$id = "8effd75951b3b0162685441de7019f1174b47bb731df418c400d14937cc3613e2f8b5ac2bba9d3c2f311127922fcc25339b6157c9af18a7e1e4dfc10eac03086";
//$password = "eb740d37601423558dcd218d08e8b742550bad84b3fa7928baaa23acf5d8ac184c08584711e9fabbe6d749dfd1eab8d4d86270ff9e03e4ddcd9f8ae9edb073ffa8bcf93c5fb1f8792333d015ffc03e17b403684520a039417a48ff4a3d7c90da7c85b8ae5e360eb5c7b4790ae95fd3a74ec8a0f7edb02ee2371013458bde7807";
//$vm_api = "http://localhost/vmapi/";

$id = "14e87c0f36c0d18829b4d19f590930fdae6ae38f155dbce48854a5b212aa7662020016036206b8dd8ed3aff45d3ee88ba4a3ac8f080bca6f0ac8f34ff0fa148f";
$password = "a14561e47888285adf54ef0a3162bad1718351058c65e2d601e2a50d0482f70b4ed8b5673c8f2385321ff9c5faba7831049dfbb94ab3cbb84f8615f0f1e8c6b87a54cf30f91158514e2d90331e99b2d9edd23a78dd2f92aef1944d977078a7f100c6272c06986889491fb7237bf86c8463f0a0866ff8ecb60bc99a368bbce820";
$vm_api = "https://testdev.viewmind.ai/";

// The user name to search for. 
$uid_to_search_for = "OrbitLab1";

// ISO Standard Dates from and to. (They will be transformed to UNIX timestamps in milliseconds.)
// If left empty they won't be used as parameters. 
$from_date = "2021-05-07";
$to_date = "2021-07-09";

// If true the curl command is printed before it is executed. 
$enable_command_output = true;

$endpoint_reportlist = "reports/reportlist/";
$endpoint_pdf = "reports/institutionPDF/";

//////////////////////////////////////// Login process to get the authentication token.  //////////////////////////////////////// 

$headers = '-H "AuthType: Login"';  // Instructs the API to do a Login operation which will generate the bearer token.
$auth = "$id:$password"; // Login is done using standard Basic Auth. 

// All exchages with our API are POST requests. 
//$curl_cmd = "curl -s -X POST -u $auth $headers " . '"' . $vm_api . '"'; 
$curl_cmd = "curl -s -X POST -u $auth $headers $vm_api"; 

if ($enable_command_output) echo "Running curl command: $curl_cmd\n\n";

$output = shell_exec($curl_cmd);
$token_data = json_decode($output,true);

// All responses with our API will have a message and http_code field. If message is different than OK and http_code is different than 200, there was ane error. 
if ($token_data["http_code"] != 200){
   echo "Error getting login token: " . $token_data["message"] . "\n";
   exit;
}
else{
   echo "Token successfully obtained\n\n";
}

// Token is in the token subfield of the data subfield. 
$token = $token_data["data"]["token"];

//////////////////////////////////////// Creating the Headers that need to be sent with all future requests.  ////////////////////////////////////////
$headers = array();
$headers[] = '-H "AuthType: VMPartner"';
$headers[] = '-H "Authorization: ' . $id . ":" . $token . '"';
$header_str = implode(" ", $headers);


//////////////////////////////////////// Listing the resports for a specific patient via IUID ////////////////////////////////////////

$patient_to_search_for = $uid_to_search_for; // <-- This is the user ID provided by our partners. 

// Since no to and from are specified as URL parameters, this will search for all available reports. 
$final_url = $vm_api . $endpoint_reportlist . $patient_to_search_for . "?search_criteria=iuid"; // search-criteria=iuid indicates that the patient is refrence by the uid provided by the institution. 


// //https://testdev.viewmind.ai/reports/reportlist/:username?search_criteria=iuid&from=9999999999999&to=9999999999999
// $to_date = 9999999999999;
// $from_date = 9999999999999;
// $final_url = $final_url . "&from=9999999999999&to=9999999999999";

// Transforming date strings to UNIX timestamps. 
if ($from_date != ""){ 
   $from = strtotime($from_date)*1000;
   $final_url = $final_url . "&from=$from";
}

if ($to_date != ""){
   $to = strtotime($to_date)*1000;
   $final_url = $final_url . "&to=$to";
}


$curl_cmd = "curl -s -X POST $header_str " . '"' .  $final_url . '"';  // Quotation marks are necessary due to the & character in the URL. 

if ($enable_command_output) echo "Running curl command: $curl_cmd\n\n";

$output = shell_exec($curl_cmd);

// echo "\n===================== RAW OUTPUT ===================";
// echo "$output\n";
// echo "\n====================================================";
$report_list = json_decode($output,true);
echo json_encode($report_list,JSON_PRETTY_PRINT) . "\n\n";


//////////////////////////////////////// Downloading the very first PDF from the list  ////////////////////////////////////////

// Checking that everything was alright before moving on.
if ($report_list["http_code"] != 200) return;

$report_list = $report_list["data"]["reports"];

if (empty($report_list)){
   echo "No reports we found\n";
   exit;
}

echo "\nDownloading the very first report using the id\n\n";

// Obtaining the report whose ID we want. 
$report_id = $report_list[0]["id"];

// Creating the URL.
$final_url = $vm_api . $endpoint_pdf . $report_id . "?lang=en"; // <- Reports can be requested in different languages. Right now only english is implemented and the URL parameter is mandatory. 

// Composing the CURL command. 
$curl_cmd = "curl -s -X POST $header_str " . '"' .  $final_url . '"  >> test.pdf';  // Quotation marks are necessary due to the & character in the URL. 

if ($enable_command_output) echo "Running curl command: $curl_cmd\n\n";

// Run the command. 
$output = shell_exec($curl_cmd);


?>
