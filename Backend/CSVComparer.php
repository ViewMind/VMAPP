<?php

class CSVComparer {


   private const CMP_NAME       = "name";
   private const CMP_IGNORE     = "ignore";
   private const CMP_TOLERANCE  = "tolerance";

   private $columns;
   private $difference_list;

   function __construct(){
      $this->reset();
   }

   function reset(){
      $this->columns = array();
      $this->difference_list = array();
   }

   function getDifferenceList(){
      return $this->difference_list;
   }

   // By default is $n_tolerance is zero then values are compared as STRINGS.
   // The must be exact. Otherwise it's a numeric comparison. 
   function addComparisonColumn($name, $ignore, $n_tolerance = 0){
      $column[self::CMP_NAME] = $name;
      $column[self::CMP_IGNORE] = $ignore;
      $column[self::CMP_TOLERANCE] = $n_tolerance;
      $this->columns[] = $column;
   }

   function compareCSVs($a, $name_for_a, $b, $name_for_b){
      $a = file_get_contents($a);
      $b = file_get_contents($b);

      $a = explode("\n",$a);
      $this->eliminateEmptyRows($a);
      $b = explode("\n",$b);
      $this->eliminateEmptyRows($b);

      if (count($a) != count($b)){
         $this->difference_list[] = "$name_for_a has " . count($a) . " rows and file $name_for_b has " . count($b) . " rows";
         return false;
      }

      $cc = count($this->columns);

      $n = count($a);
      // We skip the comparison of the header. 
      for ($i = 1; $i < $n; $i++){

         $row_a = explode(",",$a[$i]);
         $row_b = explode(",",$b[$i]);
         
         $nca = count($row_a);
         $ncb = count($row_b);

         if ($ncb != $nca){
            $this->difference_list[] = "On row $i: $name_for_a has $nca columns and $name_for_b has $ncb columns";
            continue;
         }

         if ($nca != $cc){
            $this->difference_list[] = "On row $i: files have $nca columns but $cc were configured";
            continue;
         }

         $row_diffs = array();

         for ($j = 0; $j < $nca; $j++){
            $value_a = trim($row_a[$j]);
            $value_b = trim($row_b[$j]);
            $diff = $this->compareColumnValues($j,$value_a, $value_b,$name_for_a,$name_for_b);
            if ($diff != ""){
              $row_diffs[] = "Col $j: $diff";
            }
         }

         if (count($row_diffs) > 0) {
            $row_num = $i+1;
            $this->difference_list[] = "ROW $row_num: " . implode(" | ", $row_diffs);
         }
      }

      return (empty($this->difference_list));
   }

   private function compareColumnValues($col_index, $value_a, $value_b, $name_for_a, $name_for_b){

      if ($col_index >= count($this->columns)){
         return "Out of bounds column index: $col_index. N Columns: " .  count($this->columns);
      }

      if ($this->columns[$col_index][self::CMP_IGNORE]) return "";
      if ($this->columns[$col_index][self::CMP_TOLERANCE] == 0){
         if (strval($value_a) !== strval($value_b)){
            return "COL " . $this->columns[$col_index][self::CMP_NAME] . " $value_a ($name_for_a) != $value_b ($name_for_b)";
         }
      }
      else{
         if (abs(floatval($value_a) - floatval($value_b)) > $this->columns[$col_index][self::CMP_TOLERANCE]){
            return "COL " . $this->columns[$col_index][self::CMP_NAME] . "  |$value_a ($name_for_a) - $value_b ($name_for_b)| > TOL";
         }
      }

      return "";
   }

   private function eliminateEmptyRows(&$rowlist){
      $last = count($rowlist)-1;
      while ($rowlist[$last] == ""){
         array_pop($rowlist);
         $last = count($rowlist)-1;
      }
   }

}

//////////// The comparer class. 
$csv_comparer = new CSVComparer();
$tol = 0.01;


//////////// Compare Two Reading Files

// $file_a = "/home/ariel/repos/viewmind_projects/VMSoftwareSuite/EyeServer/bin/OUTPUTS/test/reading_es_2_2020_05_09_10_12.csv";
// $name_for_a = "REF";
// $file_b = "/home/ariel/repos/viewmind_projects/Backend/test.csv";
// $name_for_b = "NEW";

// $csv_comparer->addComparisonColumn("placeholder_id",true,0);
// $csv_comparer->addComparisonColumn("age",false,0);
// $csv_comparer->addComparisonColumn("suj",true,0);
// $csv_comparer->addComparisonColumn("sujnum",false,0);
// $csv_comparer->addComparisonColumn("oracion",false,0);
// $csv_comparer->addComparisonColumn("trial",false,0);
// $csv_comparer->addComparisonColumn("fixn",false,0);
// $csv_comparer->addComparisonColumn("screenpos",false,$tol);
// $csv_comparer->addComparisonColumn("wn",false,$tol);
// $csv_comparer->addComparisonColumn("pos",false,$tol);
// $csv_comparer->addComparisonColumn("dur",false,$tol);
// $csv_comparer->addComparisonColumn("longoracion",false,0);
// $csv_comparer->addComparisonColumn("ojoDI",false,0);
// $csv_comparer->addComparisonColumn("pupila",false,$tol);
// $csv_comparer->addComparisonColumn("blink",false,0);
// $csv_comparer->addComparisonColumn("amp_sac",false,$tol);
// $csv_comparer->addComparisonColumn("gaze",false,0);
// $csv_comparer->addComparisonColumn("nf",false,0);
// $csv_comparer->addComparisonColumn("fixY" ,false,$tol);

// //////////// Compare Two NBackRT Files

// $file_a = "/home/ariel/repos/viewmind_projects/VMSoftwareSuite/EyeReportGenerator/bin/res/test_scripts/reference_data/reference_nbackrt.csv";
// $name_for_a = "REF";
// $file_b = "/home/ariel/repos/viewmind_projects/Backend/test_nbackrt.csv";
// $name_for_b = "NEW";

// $csv_comparer->addComparisonColumn("suj"        , true,$tol); 
// $csv_comparer->addComparisonColumn("idtrial"    , false,0); 
// $csv_comparer->addComparisonColumn("imgnum"     , false,0); 
// $csv_comparer->addComparisonColumn("target"     , false,0); 
// $csv_comparer->addComparisonColumn("target_hit" , false,0); 
// $csv_comparer->addComparisonColumn("nback"      , false,0); 
// $csv_comparer->addComparisonColumn("trial_seq"  , false,0); 
// $csv_comparer->addComparisonColumn("seq_comp"   , false,0); 
// $csv_comparer->addComparisonColumn("dur"        , false,$tol); 
// $csv_comparer->addComparisonColumn("ojoDI"      , false,0); 
// $csv_comparer->addComparisonColumn("latSac"     , false,$tol); 
// $csv_comparer->addComparisonColumn("amp_sacada" , false,$tol); 
// $csv_comparer->addComparisonColumn("resp_time" ,  false,$tol); 


// //////////// Compare Two Binding Files

// //$file_a = "/home/ariel/repos/viewmind_projects/VMSoftwareSuite/EyeReportGenerator/bin/res/test_scripts/reference_data/reference_binding_bc_2.csv";
// $file_a = "/home/ariel/repos/viewmind_projects/VMSoftwareSuite/EyeReportGenerator/bin/res/test_scripts/reference_data/reference_binding_uc_2.csv";
// $name_for_a = "REF";
// //$file_b = "/home/ariel/repos/viewmind_projects/Backend/test_bindingbc.csv";
// $file_b = "/home/ariel/repos/viewmind_projects/Backend/test_bindinguc.csv";
// $name_for_b = "NEW";

// $csv_comparer->addComparisonColumn("placeholder_id",false,0);
// $csv_comparer->addComparisonColumn("age",false,0);
// $csv_comparer->addComparisonColumn("suj",true,0);
// $csv_comparer->addComparisonColumn("trial_id",false,0);
// $csv_comparer->addComparisonColumn("is_trial",false,0);
// $csv_comparer->addComparisonColumn("trial_name",false,0);
// $csv_comparer->addComparisonColumn("trial_type",false,0);
// $csv_comparer->addComparisonColumn("response",false,0);
// $csv_comparer->addComparisonColumn("dur",false,$tol);
// $csv_comparer->addComparisonColumn("ojoDI",false,0);
// $csv_comparer->addComparisonColumn("blink",false,$tol);
// $csv_comparer->addComparisonColumn("amp_sacada",false,$tol);
// $csv_comparer->addComparisonColumn("pupila",false,$tol);
// $csv_comparer->addComparisonColumn("gaze",false,$tol);
// $csv_comparer->addComparisonColumn("nf",false,0);
// $csv_comparer->addComparisonColumn("fixX",false,$tol);
// $csv_comparer->addComparisonColumn("fixY",false,$tol);
// $csv_comparer->addComparisonColumn("targetSide",true,0);
// $csv_comparer->addComparisonColumn("tX1",true,0);
// $csv_comparer->addComparisonColumn("tY1",true,0);
// $csv_comparer->addComparisonColumn("tX2",true,0);
// $csv_comparer->addComparisonColumn("tY2",true,0);
// $csv_comparer->addComparisonColumn("tX3",true,0);
// $csv_comparer->addComparisonColumn("tY3",true,0);
// $csv_comparer->addComparisonColumn("fixStart",false,$tol);
// $csv_comparer->addComparisonColumn("fixEnd",false,$tol);
// $csv_comparer->addComparisonColumn("fixMid",false,$tol);
// $csv_comparer->addComparisonColumn("timeline",false,0);
// $csv_comparer->addComparisonColumn("score",false,0);


// //////////// Compare Two Go No Go


//$file_a = "/home/ariel/repos/viewmind_projects/VMSoftwareSuite/EyeReportGenerator/bin/res/test_scripts/reference_data/reference_gonogo.csv";
$file_a = "/home/ariel/Workspace/Viewmind/vm_data/gonogo/ref_test/gonogo_2_2020_11_09_15_05.csv";
$name_for_a = "REF";
$file_b = "/home/ariel/repos/viewmind_projects/Backend/test_gonogo.csv";
$name_for_b = "NEW";

$csv_comparer->addComparisonColumn("suj",true,0);
$csv_comparer->addComparisonColumn("idtrial",false,0);
$csv_comparer->addComparisonColumn("target_hit",false,0);
$csv_comparer->addComparisonColumn("dur",false,$tol);
$csv_comparer->addComparisonColumn("ojoDI",false,0);
$csv_comparer->addComparisonColumn("latSac",false,$tol);
$csv_comparer->addComparisonColumn("amp_sacada",false,$tol);
$csv_comparer->addComparisonColumn("resp_time",false,0);
$csv_comparer->addComparisonColumn("pupil_size",false,$tol);
$csv_comparer->addComparisonColumn("num_trial_fix",false,0);
$csv_comparer->addComparisonColumn("gazing",false,0);
$csv_comparer->addComparisonColumn("num_center_fix",false,0);
$csv_comparer->addComparisonColumn("total_study_time",false,0);
$csv_comparer->addComparisonColumn("arrow_type",false,0);
$csv_comparer->addComparisonColumn("trial_type",false,0);

if (!$csv_comparer->compareCSVs($file_a,$name_for_a,$file_b,$name_for_b)){
   echo "FAILED\n";
   var_dump($csv_comparer->getDifferenceList());
}
else{
   echo "EQUAL\n";
}

?>