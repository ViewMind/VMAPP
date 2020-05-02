clc;
close all;
clear all;

% DIRECTORY where csv reside for the GazePoint MD Sweep. 
% DIR = "/home/ariela/Workspace/Viewmind/CSVReadingMaxDispSweep/htc_vr";
% BASEFILENAME = "reading_es_2_2020_02_13_11_08-";
% mdsweep = 3:1:400;

DIR = "/home/ariela/Workspace/Viewmind/CSVReadingMaxDispSweep/htc_vr/norm_1000";
BASEFILENAME = "reading_es_2_2020_02_13_11_08-";
TITLE = "MD Sweep HTC VR Res Norm 1000";
SUMSQ = "Min Squares to Target HTC VR";
mdsweep = 1:1:300;

% Reference file. 
config.FILE = [DIR "/fixlist.csv"];
config.FIXDURCOLUM = 4;
config.NUMBINS = 10;
config.MAXFIXDUR = 600;
reference = histDurationDataFromCSV(config);

% Configuration for these files
config.FIXDURCOLUM = 9;
config.NUMBINS = 10;
config.MAXFIXDUR = 600;

% Maximum Dispersion Sweep File. 
L = length(mdsweep);
data = zeros(L,config.NUMBINS);

% For getting the best result for the reference. 
diff_all      = zeros(1,L);
diff_selected = zeros(1,L);
bestdiff_all = 0;
bestOption = 0;
bestdiff_selected = 0;
bestOption_selected = 0;

for md = mdsweep
  config.FILE = [DIR "/" BASEFILENAME num2str(md) ".csv"];
  ans = histDurationDataFromCSV(config);
  index = md - mdsweep(1) + 1;
  data(index,:) = ans.histogram_data;  
  
  diff_all(index)       = sqrt(sum((ans.histogram_data - reference.histogram_data).^2));  
  if ((bestOption == 0) || (diff_all(index) < bestdiff_all))   
     bestOption = md;
     bestdiff_all = diff_all(index);
  end
  
  diff_selected(index)  = sqrt(sum((ans.histogram_data(2:8) - reference.histogram_data(2:8)).^2));
  if ((bestOption_selected == 0) || (diff_selected(index) < bestdiff_selected))   
     bestOption_selected = md;
     bestdiff_selected = diff_selected(index);
  end  
  
endfor

lineparameters = {'bo','b+','b>','ro','r+','r>','ko','k+','k>','go','g+','g>','mo','m+','m>','yo','y+','y>','co','c+','c>'};
NLineP = length(lineparameters);
lpInd = 1;

figure()
legends = cell(1,config.NUMBINS);
for i=1:config.NUMBINS
  
  plot(mdsweep,data(:,i),lineparameters{lpInd});
  hold on;   
  lpInd = lpInd + 1;
  if (lpInd == NLineP) 
     lpInd = 0;  
  endif  
  
  legends{i} = ans.xnames{i};
endfor
legend(legends);
title(TITLE)

% Plotting the minimum squares. 
figure();
plot(mdsweep,diff_all);
hold on
plot(mdsweep,diff_selected);
legend({"ALL","SELECTED"});
title(SUMSQ)

disp("Done");
