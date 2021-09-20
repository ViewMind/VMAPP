<?php

/**
 * BINDING 2 TARGETS
 * PDF_GEN_VERSION: 1
 * REVISAR 
 * Gaze Duration (mean_gazing +/- se_gazing)
 *    Integrative Memory (BC)
 *       control
 *          encoding      : 1768.788    2.35715 
 *          recognition   : 1471.864    12.494839	
 *       subject
 *          encoding      : ?
 *          recognition   : ?
 *    Associative Memory (UC)
 *       control
 *          enconding:    1788.454	   1.829591	
 *          recognition:  1717.571	   14.808148	
 *       subject
 *          encoding      : ?
 *          recognition   : ? 
 * 
 * Visual Scanning (mean_saccade +/- SEsaccade)
 *    Integrative Memory (BC)
 *       control
 *          encoding      : 2.193599	0.03913513	
 *          recognition   : 2.325449	0.05862766	
 *       subject
 *          encoding      : ?
 *          recognition   : ?
 *    Associative Memory (UC)
 *       control
 *          enconding:    2.442039	0.03711389	
 *          recognition:  2.346819	0.04632124	
 *       subject
 *          encoding      : ?
 *          recognition   : ? * 
 * 
 * Fixation Duration (mean_duration +/- SEduration)
 *    Integrative Memory (BC)
 *       control
 *          encoding      : 227.2522	2.004669	
 *          recognition   : 219.5501	2.0547333	
 *       subject
 *          encoding      : ?
 *          recognition   : ?
 *    Associative Memory (UC)
 *       control
 *          enconding:    213.3064	1.5859984	
 *          recognition:  195.709	1.5606745
 *       subject
 *          encoding      : ?
 *          recognition   : ?  
 * 
 * 
 * Number of Fixations (mnf +/- SEnf)
 *    Integrative Memory (BC)
 *       control
 *          encoding      : 8.443308	0.03341333
 *          recognition   : 7.263889	0.06063463
 *       subject
 *          encoding      : ?
 *          recognition   : ?
 *    Associative Memory (UC)
 *       control
 *          enconding:    9.083555	0.03164692
 *          recognition:  9.711452	0.11451316
 *       subject
 *          encoding      : ?
 *          recognition   : ? * 
 * 
 * 
 */
 
  $ret = array();
  $ret["uuid"] = "binding_2";
  $ret["categories"] = include("binding/categories_2T.php");
  $ret["pdiag"] = include("binding/pdiag_logic_2T.php");
  
  $pages = [];
  $pages[] = include("binding/page1_2T.php");
  $pages[] = include("binding/page2_2T.php");
  $pages[] = include("binding/page3_2T.php");
  $pages[] = include("binding/page4_2T.php");
  $pages[] = include("binding/page5_2T.php");

  $ret["pages"] = $pages;
  return $ret;

?>
