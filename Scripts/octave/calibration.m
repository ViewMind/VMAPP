clc;
close all;
clear all;

# Loading calibration data
source("calibration_data.m")

#Inializing data sets
xrsets = cell(1,3);
yrsets = cell(1,3);
xlsets = cell(1,3);
ylsets = cell(1,3);

# Get the calibration points. and removing them from the calibration data. 
cp0 = cd0(1,:);
cd0(1,:) = [];
[M0 four] = size(cd0);
xrsets{1} = [cd0(:,1)];

cp1 = cd1(1,:);
cd1(1,:) = [];
[M1 four] = size(cd1);

cp2 = cd2(1,:);
cd2(1,:) = [];
[M2 four] = size(cd2);

cp3 = cd3(1,:);
cd3(1,:) = [];
[M3 four] = size(cd3);

cp4 = cd4(1,:);
cd4(1,:) = [];
[M4 four] = size(cd4);

disp(["The calibration point data sizes are: " num2str([M0 M1 M2 M3 M4])])

################################################## INITIALIZING X and Y SETS
allXValues = [cp0(1) cp1(1) cp2(1) cp3(1) cp4(1)];
allYValues = [cp0(2) cp1(2) cp2(2) cp3(2) cp4(2)];

diffXValues = unique(allXValues);
diffYValues = unique(allYValues);

xrsets = cell(1,length(diffXValues));
xlsets = cell(1,length(diffXValues));
yrsets = cell(1,length(diffYValues));
ylsets = cell(1,length(diffYValues));

for i=1:length(diffXValues)
  xrsets{i} = [];
  xlsets{i} = [];
endfor

for i=1:length(diffYValues)
  yrsets{i} = [];
  ylsets{i} = [];
endfor


idx = find(diffXValues == cp0(1));
xrsets{idx} = [xrsets{idx} cd0(:,1)';];
idx = find(diffXValues == cp1(1));
xrsets{idx} = [xrsets{idx} cd1(:,1)';];
idx = find(diffXValues == cp2(1));
xrsets{idx} = [xrsets{idx} cd2(:,1)';];
idx = find(diffXValues == cp3(1));
xrsets{idx} = [xrsets{idx} cd3(:,1)';];
idx = find(diffXValues == cp4(1));
xrsets{idx} = [xrsets{idx} cd4(:,1)';];

idx = find(diffXValues == cp0(1));
xlsets{idx} = [xlsets{idx} cd0(:,3)';];
idx = find(diffXValues == cp1(1));
xlsets{idx} = [xlsets{idx} cd1(:,3)';];
idx = find(diffXValues == cp2(1));
xlsets{idx} = [xlsets{idx} cd2(:,3)';];
idx = find(diffXValues == cp3(1));
xlsets{idx} = [xlsets{idx} cd3(:,3)';];
idx = find(diffXValues == cp4(1));
xlsets{idx} = [xlsets{idx} cd4(:,3)';];

idx = find(diffYValues == cp0(2));
yrsets{idx} = [yrsets{idx} cd0(:,2)';];
idx = find(diffYValues == cp1(2));
yrsets{idx} = [yrsets{idx} cd1(:,2)';];
idx = find(diffYValues == cp2(2));
yrsets{idx} = [yrsets{idx} cd2(:,2)';];
idx = find(diffYValues == cp3(2));
yrsets{idx} = [yrsets{idx} cd3(:,2)';];
idx = find(diffYValues == cp4(2));
yrsets{idx} = [yrsets{idx} cd4(:,2)';];

idx = find(diffYValues == cp0(2));
ylsets{idx} = [ylsets{idx} cd0(:,4)';];
idx = find(diffYValues == cp1(2));
ylsets{idx} = [ylsets{idx} cd1(:,4)';];
idx = find(diffYValues == cp2(2));
ylsets{idx} = [ylsets{idx} cd2(:,4)';];
idx = find(diffYValues == cp3(2));
ylsets{idx} = [ylsets{idx} cd3(:,4)';];
idx = find(diffYValues == cp4(2));
ylsets{idx} = [ylsets{idx} cd4(:,4)';];

################################################## DOING IT AS A SINGLE VECTOR
# Target value vectors. Column order is xr yr xl yl
xrt = [ cp0(1)*ones(1,M0)  cp1(1)*ones(1,M1) cp2(1)*ones(1,M2) cp3(1)*ones(1,M3) cp4(1)*ones(1,M4) ];
yrt = [ cp0(2)*ones(1,M0)  cp1(2)*ones(1,M1) cp2(2)*ones(1,M2) cp3(2)*ones(1,M3) cp4(2)*ones(1,M4) ];
xlt = [ cp0(3)*ones(1,M0)  cp1(3)*ones(1,M1) cp2(3)*ones(1,M2) cp3(3)*ones(1,M3) cp4(3)*ones(1,M4) ];
ylt = [ cp0(4)*ones(1,M0)  cp1(4)*ones(1,M1) cp2(4)*ones(1,M2) cp3(4)*ones(1,M3) cp4(4)*ones(1,M4) ];

# Data value vectors. Same column order
xrd  = [ cd0(:,1); cd1(:,1); cd2(:,1); cd3(:,1); cd4(:,1);]';
yrd  = [ cd0(:,2); cd1(:,2); cd2(:,2); cd3(:,2); cd4(:,2);]';
xld  = [ cd0(:,3); cd1(:,3); cd2(:,3); cd3(:,3); cd4(:,3);]';
yld  = [ cd0(:,4); cd1(:,4); cd2(:,4); cd3(:,4); cd4(:,4);]';

processEyeCalibration(xrt,xrd,"XR",xrsets,diffXValues)
processEyeCalibration(yrt,yrd,"YR",yrsets,diffYValues)
processEyeCalibration(xlt,xld,"XL",xlsets,diffXValues)
processEyeCalibration(ylt,yld,"YL",ylsets,diffYValues)

# Plot the input vs output.
##plot(xrd,xrt,'o')
##
### Linear regression  Matrix
##N = length(xrd);
##Vm = [N         sum(xrd);
##      sum(xrd)  sum(xrd.^2)];
##Tv = [sum(xrt); sum(xrt.*xrd)];
##coeffs = inv(Vm)*Tv;
##testx = min(xrd):0.00001:max(xrd);
##testy = coeffs(2)*testx + coeffs(1);
##hold on
##plot(testx,testy,'r')
##
### Regression with polyfit
##hold on
##pfc = polyfit(xrd,xrt,10);
##testy2 = polyval(pfc,testx);
##plot(testx,testy,'g')
##
### Doing the set fit.
##realData = [];
##for i = 1:length(xrsets)
##  realData = [realData sum(xrsets{i})/length(xrsets{i})];
##endfor
##
##hold on
##pfc2 = polyfit(realData,diffXValues,1);
##testy3 = polyval(pfc2,testx);
##plot(testx,testy,'k')
##xlabel('EyeTracker XR value'); ylabel('Target X'); title('XR eye')
##
### Finally doing the fit after filtering. 
##filteredX = [];
##filteredY = [];
##for i = 1:length(xrsets)
##   dset = xrsets{i};
##   N = length(dset);
##   mu = sum(dset)/N
##   sigma = sqrt(sum((dset - mu).^2)/N)  
##   sigma = sqrt(var(dset))
##   maxAllowed = mu+2*sigma;
##   minAllowed = mu-2*sigma;
##   
##   tofilter = find(dset > maxAllowed);
##   dset(tofilter) = [];
##   tofilter = find(dset < minAllowed);
##   dset(tofilter) = [];
##   
##   filteredX = [filteredX dset];
##   filteredY = [filteredY ones(1,length(dset))*diffXValues(i)];
##endfor
##hold on
##plot(filteredX,filteredY,'or')
##
##hold on
##pfc3 = polyfit(filteredX,filteredY,1);
##testy4 = polyval(pfc3,testx);
##plot(testx,testy,'m')
##
##xlabel('EyeTracker XR value'); ylabel('Target X'); title('XR eye')


##absMin = min(xrd);
##absMax = max(xrd);
##
##target = [cp0(1); cp1(1); cp2(1)];
##varTarget = [2 -2; 2 -2; 2 -2];
##N = length(xrd);
##for i = 1:N
##  idx = find(target == xrt(i));
##  if (xrd(i) < varTarget(idx,1)) 
##    varTarget(idx,1) = xrd(i);
##  elseif (xrd(i) > varTarget(idx,2)) 
##    varTarget(idx,2) = xrd(i);
##  endif  
##endfor

##N = length(xrd);
##mu = sum(xrd)/N;
##sigma = sqrt(sum(xrd - mu).^2/N);

##figure
##plot(xld,xlt,'o')
##xlabel('EyeTracker XL value'); ylabel('Target X'); title('XL eye')
##
##figure
##plot(yrd,yrt,'o')
##xlabel('EyeTracker YR value'); ylabel('Target Y'); title('YR eye')
##
##figure
##plot(yld,ylt,'o')
##xlabel('EyeTracker YL value'); ylabel('Target Y'); title('YL eye')



