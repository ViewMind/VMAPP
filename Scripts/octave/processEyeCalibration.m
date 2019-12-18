## Copyright (C) 2019 Ariel Arelovich
## 
## This program is free software: you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
## 
## This program is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see
## <https://www.gnu.org/licenses/>.

## -*- texinfo -*- 
## @deftypefn {} {@var{retval} =} processEyeCalibration (@var{input1}, @var{input2})
##
## @seealso{}
## @end deftypefn

## Author: Ariel Arelovich <ariela@pc01>
## Created: 2019-12-18

function coeffs = processEyeCalibration (targetData, eyeData, label, dataSet, distinctValues)
  
    xrt = targetData;
    xrd = eyeData;  
    xrsets = dataSet;  
    diffXValues = distinctValues;
  
    figure
    plot(xrd,xrt,'o')

    # Linear regression  Matrix
    N = length(xrd);
    Vm = [N         sum(xrd);
          sum(xrd)  sum(xrd.^2)];
    Tv = [sum(xrt); sum(xrt.*xrd)];
    coeffs = inv(Vm)*Tv;
    testx = min(xrd):0.00001:max(xrd);
    testy = coeffs(2)*testx + coeffs(1);
    hold on
    plot(testx,testy,'r')

    # Regression with polyfit
    hold on
    pfc = polyfit(xrd,xrt,10);
    testy2 = polyval(pfc,testx);
    plot(testx,testy,'g')

    # Doing the set fit.
    realData = [];
    for i = 1:length(xrsets)
      realData = [realData sum(xrsets{i})/length(xrsets{i})];
    endfor

    hold on
    pfc2 = polyfit(realData,diffXValues,1);
    testy3 = polyval(pfc2,testx);
    plot(testx,testy,'k')

    # Finally doing the fit after filtering. 
    filteredX = [];
    filteredY = [];
    maxminSet = zeros(length(xrsets),2);
    for i = 1:length(xrsets)
       dset = xrsets{i};       
       N = length(dset);
       mu = sum(dset)/N;
       sigma = sqrt(sum((dset - mu).^2)/N);
       
       #sigma = sqrt(var(dset))
       maxAllowed = mu+2*sigma;
       minAllowed = mu-2*sigma;
       
       tofilter = find(dset > maxAllowed);
       dset(tofilter) = [];
       tofilter = find(dset < minAllowed);
       dset(tofilter) = [];
       
       # Filtered min max for filtered values. 
       maxminSet(i,:) = [min(dset) max(dset)];
       
       filteredX = [filteredX dset];
       filteredY = [filteredY ones(1,length(dset))*diffXValues(i)];
    endfor
    
    hold on
    plot(filteredX,filteredY,'or')

    hold on
    pfc3 = polyfit(filteredX,filteredY,1);
    testy4 = polyval(pfc3,testx);
    plot(testx,testy,'m')

    xlabel(['EyeTracker ' label ' value']); ylabel('Target Value'); title([label ' eye'])
    
    disp(["Showing pixel variance for each set " label])
    disp(coeffs(2)*maxminSet + coeffs(1));
  

endfunction
