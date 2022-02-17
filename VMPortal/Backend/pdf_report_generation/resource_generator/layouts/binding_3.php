<?php

/**
 * PDF_GEN_VERSION: 1
 * BINDING 3 TARGETS
 *  
 * Gaze Duration (mean_gazing +/- se_gazing)
 *    Integrative Memory (BC)
 *       control
 *          encoding      : 1811.3527    0.6886
 *          recognition   : 2030.2978    4.8334
 *       subject
 *          encoding      : ?
 *          recognition   : ?
 *    Associative Memory (UC)
 *       control
 *          encoding:     1786.9819 0.835
 *          recognition:  1860.3862 5.001
 *       subject
 *          encoding      : ?
 *          recognition   : ? 
 * 
 * Visual Scanning (mean_saccade +/- SEsaccade)
 *    Integrative Memory (BC)
 *       control
 *          encoding      : 2.2052	0.0113	
 *          recognition   : 2.7807  0.0177
 *       subject
 *          encoding      : ?
 *          recognition   : ?
 *    Associative Memory (UC)
 *       control
 *          encoding:    2.3637 0.0125
 *          recognition: 2.7588 0.0181 
 *       subject
 *          encoding      : ?
 *          recognition   : ? * 
 * 
 * 
 * Fixation Duration (mean_duration +/- SEduration)
 *    Integrative Memory (BC)
 *       control
 *          encoding      : 227.2301	0.7214	
 *          recognition   : 208.8938    0.6297
 *       subject
 *          encoding      : ?
 *          recognition   : ?
 *    Associative Memory (UC)
 *       control
 *          encoding:    216.8528 0.6504
 *          recognition: 201.3191 0.6452
 *       subject
 *          encoding      : ?
 *          recognition   : ?  
 * 
 * 
 * Number of Fixations (mnf +/- SEnf)
 *    Integrative Memory (BC)
 *       control
 *          encoding      : 9.0752	0.012
 *          recognition   : 10.7987  0.0296
 *       subject
 *          encoding      : ?
 *          recognition   : ?
 *    Associative Memory (UC)
 *       control
 *          encoding:    9.2888 0.012
 *          recognition: 10.5517 0.0347 
 *       subject
 *          encoding      : ?
 *          recognition   : ? * 
 * 
 * 
 * Fixation are from trials
 * 10 -> Index 19
 *    E: 20     11     02     
 *    R: 10     22     01     
 * 20 -> Index 29
 *    E: 02     11     00     
 *    R: 12     01     10     
 * 32 -> Index 41
 *    E: 21     02     00     
 *    R: 01     21     12    
 * 
 * Rerference from Index 25-trial-different-34 from BC. 
 * 
 *    E: [1,2], [2,1], [0,0]
 *    R: [2,2], [0,2], [2,0]
 * 
 * 
 * 
 */

  $ret = array();
  $ret["uuid"] = "binding_3";
  $ret["categories"] = include("binding/categories_3T.php");
  $ret["pdiag"] = include("binding/pdiag_logic_3T.php");
  
  $pages = [];
  $pages[] = include("binding/page1_3T.php");
  $pages[] = include("binding/page2_3T.php");
  //$pages[] = include("binding/page3_3T.php");
  //$pages[] = include("binding/page4_3T.php");
  $pages[] = include("binding/page5_3T.php");

  $ret["pages"] = $pages;
  //echo "Returning";
  //echo json_encode($ret,JSON_PRETTY_PRINT);
  return $ret;

?>
