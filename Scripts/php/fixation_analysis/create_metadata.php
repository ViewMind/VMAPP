<?php

/**
 * This file is only a helper for creating a JSON File which contains the Metadata required to process each individual file. 
 * 
 */

include_once("named_constants.php");
 
$output_file = "metadata.json";
$metadata = array();

// Binding Directory
$dir = "binding";

$et_dir = "gp_files";
$key = "$dir/$et_dir/binding_bc_3_l_2_2019_04_02_09_54.dat";
$temp["code"] = ETCode::GP;
$temp["f"] = SamplingFrequency::GP;
$temp["resolution"] = Resolutions::GP_HD;
$temp["converter"] = InputFileParser::LEGACY_BINDING;
$temp["md_to_fixation_method"] = FixationComputationMethod::MD;
$metadata[$key] = $temp;

$et_dir = "htc_files";
$key = "$dir/$et_dir/binding_bc_2_l_2_2020_11_25_20_58.dat";
$temp["code"] = ETCode::HTC;
$temp["f"] = SamplingFrequency::HTC;
$temp["resolution"] = Resolutions::HTC;
$temp["converter"] = InputFileParser::LEGACY_BINDING;
$temp["md_to_fixation_method"] = FixationComputationMethod::MD;
$metadata[$key] = $temp;

$key = "$dir/$et_dir/binding_bc_3_l_2_2020_07_14_19_38.dat";
$temp["code"] = ETCode::HTC;
$temp["f"] = SamplingFrequency::HTC;
$temp["resolution"] = Resolutions::HTC;
$temp["converter"] = InputFileParser::LEGACY_BINDING;
$temp["md_to_fixation_method"] = FixationComputationMethod::MD;
$metadata[$key] = $temp;

$et_dir = "hp_files";
$key = "$dir/$et_dir/binding_2021_08_05_16_47.json";
$temp["code"] = ETCode::HP;
$temp["f"] = SamplingFrequency::HP;
$temp["resolution"] = Resolutions::HP_HALF;
$temp["converter"] = InputFileParser::JSON_BINDING;
$temp["md_to_fixation_method"] = FixationComputationMethod::MD;
$metadata[$key] = $temp;

// Reading
$dir = "reading";

$et_dir = "eyelink_files";
$key = "$dir/$et_dir/Paolini.asc";
$temp["code"] = ETCode::EYELINK;
$temp["f"] = SamplingFrequency::EYELINK;
$temp["resolution"] = Resolutions::EYELINK;
$temp["converter"] = InputFileParser::ASC_PARSER;
$temp["md_to_fixation_method"] = FixationComputationMethod::EYELINK;
$metadata[$key] = $temp;

$et_dir = "gp_files";
$key = "$dir/$et_dir/reading_2021_08_26_07_42.json";
$temp["code"] = ETCode::GP;
$temp["f"] = SamplingFrequency::GP;
$temp["resolution"] = Resolutions::GP_FULL_HD;
$temp["converter"] = InputFileParser::JSON_READING;
$temp["md_to_fixation_method"] = FixationComputationMethod::MD;
$metadata[$key] = $temp;

$et_dir = "hp_files";
$key = "$dir/$et_dir/reading_2021_08_26_07_29.json";
$temp["code"] = ETCode::HP;
$temp["f"] = SamplingFrequency::HP;
$temp["resolution"] = Resolutions::HP_FULL;
$temp["converter"] = InputFileParser::JSON_READING;
$temp["md_to_fixation_method"] = FixationComputationMethod::MD;
$metadata[$key] = $temp;

$et_dir = "smi_files";
$key = "$dir/$et_dir/jonatan_everti_reading_2018_09_18.dat";
$temp["code"] = ETCode::SMI;
$temp["f"] = SamplingFrequency::SMI;
$temp["resolution"] = Resolutions::SMI_HI_DEF;
$temp["converter"] = InputFileParser::LEGACY_READING;
$temp["md_to_fixation_method"] = FixationComputationMethod::MD;
$metadata[$key] = $temp;

$fid = fopen($output_file,"w");
fwrite($fid,json_encode($metadata,JSON_PRETTY_PRINT));
fclose($fid);


?> 
