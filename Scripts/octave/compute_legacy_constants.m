

##      drawValues.sideh = dcc.getHorizontalRatio(0.0942708);
##      drawValues.sidev = dcc.getVerticalRatio(0.167593);
##      drawValues.hl    = dcc.getHorizontalRatio(0.0244792);
##      drawValues.hs    = dcc.getHorizontalRatio(0.00208333);
##      drawValues.vl    = dcc.getVerticalRatio(0.0592593);
##      drawValues.vs    = dcc.getVerticalRatio(0.0222222);
##
##      if (useTwo){
##          drawValues.gx = dcc.getHorizontalRatio(0.266667);
##          drawValues.gy = dcc.getVerticalRatio(0.177778);
##      }
##      else{
##          // Assuming 3 targets
##          drawValues.gx = dcc.getHorizontalRatio(0.133333);
##          drawValues.gy = dcc.getVerticalRatio(0.177778);
##      }

##function res = vcalc(value,K,target_value);
##  value = value*K
##end

close all;
clear all;
clc;

targetW = 1366;
targetH = 768;
refW = 1920;
refH = 1080;

K = refH*targetW/(refW*targetH);
K = sqrt(K);

sideh = 0.0942708;
sidev = 0.167593;
hl    = 0.0244792;
hs    = 0.00208333;
vl    = 0.0592593;
vs    = 0.0222222;


a = refW*sideh/K
b = targetW*sideh/K