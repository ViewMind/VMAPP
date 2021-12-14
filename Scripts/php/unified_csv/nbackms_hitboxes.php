<?php

/**
 * Computes the nbackMSHitBoxes EXACTLY like the App 19.0.0 does, using constants and the resolution. 
 * Required for NBackMS studies which lack these. 
 */

const K_RECT_WIDTH =                               0.085;
const K_RECT_HEIGHT =                              0.143;

const K_HORIZONAL_MARGIN =                         0.06;
const K_SPACE_BETWEEN_BOXES =                      0.09;
const K_VERTICAL_MARGIN =                          0.06;

const TARGET_BOX_EX_W =                            0.25;  //1.5/6.0;
const TARGET_BOX_EX_H =                            0.268; //1.5/5.6;

const TARGET_BOX_0 =                               0;
const TARGET_BOX_1 =                               1;
const TARGET_BOX_2 =                               2;
const TARGET_BOX_3 =                               3;
const TARGET_BOX_4 =                               4;
const TARGET_BOX_5 =                               5;


class DrawingConstantsCalculator {

   private $targetW;
   private $targetH;
   private $K;
   private $sqrtK;

   private const REF_WIDTH = 1920;
   private const REF_HEIGHT = 1080;

   function __construct(){
       $this->targetH = 1;
       $this->targetW = 1;
       $this->computeK();
   }
   
   function setTargetResolution($w, $h){
       $this->targetH = $h;
       $this->targetW = $w;
       $this->computeK();
   }
   
   function getVerticalRatio($ref_ratio){
       return $ref_ratio*$this->sqrtK;
   }
   
   function getHorizontalRatio($ref_ratio){
       return $ref_ratio/$this->sqrtK;
   }
   
   private function computeK(){
       $this->K = (self::REF_HEIGHT*$this->targetW)/(self::REF_WIDTH*$this->targetH);
       $this->sqrtK = sqrt($this->K);
   }   

}


function nbackMSHitBoxes ($resolutionWidth, $resolutionHeight){

   // Computing the drawn target box locations and the hit target boxes
   $dcc = new DrawingConstantsCalculator();
   $dcc->setTargetResolution($resolutionWidth,$resolutionHeight);

   $targetBoxWidth = ($resolutionWidth*$dcc->getHorizontalRatio(K_RECT_WIDTH));
   $targetBoxHeight = ($resolutionHeight*$dcc->getVerticalRatio(K_RECT_HEIGHT));

   // Computing the boxes's coordinates.
   $x5 = $resolutionWidth*$dcc->getHorizontalRatio(K_HORIZONAL_MARGIN);
   $x2 = $resolutionWidth*(1- $dcc->getHorizontalRatio(K_HORIZONAL_MARGIN)) - $targetBoxWidth;
   $x0 = $x5 + $targetBoxWidth + $resolutionWidth*$dcc->getHorizontalRatio(K_SPACE_BETWEEN_BOXES);
   $x4 = $x0;
   $x1 = $x2 - $targetBoxWidth - $resolutionWidth*$dcc->getHorizontalRatio(K_SPACE_BETWEEN_BOXES);
   $x3 = $x1;
   $y2 = $resolutionHeight/2.0 - $targetBoxWidth/2.0;
   $y5 = $y2;
   $y0 = $resolutionHeight*$dcc->getVerticalRatio(K_VERTICAL_MARGIN);
   $y1 = $y0;
   $y3 = $resolutionHeight*(1-$dcc->getVerticalRatio(K_VERTICAL_MARGIN)) - $targetBoxHeight;
   $y4 = $y3;

   // Rectangle origins in order, in order
   $rectangleLocations = [];
   $rectangleLocations[] = [$x0,$y0];
   $rectangleLocations[] = [$x1,$y1];
   $rectangleLocations[] = [$x2,$y2];
   $rectangleLocations[] = [$x3,$y3];
   $rectangleLocations[] = [$x4,$y4];
   $rectangleLocations[] = [$x5,$y5];


   // Adding the rectangles to the scene and computing the target boxes, including the error margin.
   $targetBoxIncreasedMarginWidth = $targetBoxWidth*TARGET_BOX_EX_W;
   $targetBoxIncreasedMarginHeight = $targetBoxHeight*TARGET_BOX_EX_H;
   $targetBoxWidthEX = $targetBoxWidth + 2*$targetBoxIncreasedMarginWidth;
   $targetBoxHeightEX = $targetBoxHeight + 2*$targetBoxIncreasedMarginHeight;

   $hitTargetBoxes = array();

   for ($i = 0; $i < count($rectangleLocations); $i++){
       $x = $rectangleLocations[$i][0]-$targetBoxIncreasedMarginWidth;
       $y = $rectangleLocations[$i][1]-$targetBoxIncreasedMarginHeight;
       $w = $targetBoxWidthEX;
       $h = $targetBoxHeightEX;

       /// PATCH: All boxes except 2 and 5 were to have the target box increased by half it's size.
       if (($i == TARGET_BOX_5) || ($i == TARGET_BOX_2)){
           $hitTargetBoxes[] = [$x,$y,$w,$h];
       }
       else{
           $leeway = $h/2;
           $h = 2*$h;
           $y = $y - $leeway;
           $hitTargetBoxes[] = [$x,$y,$w,$h];
       }
   }

   return $hitTargetBoxes;

}

// $res = [1920, 1080];
// $w = $res [0];
// $h = $res[1];
// echo "Testing with $w x $h\n";
// $ans = nbackMSHitBoxes($w,$h);
// var_dump($ans);

?>