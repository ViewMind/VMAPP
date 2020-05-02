clc;
close all;
clear all;

% DIRECTORY where csv reside for the asc. 
% DIR = "/home/ariela/Workspace/Viewmind/CSVReadingMaxDispSweep/asc/md_sweep";
% BASEFILENAME = "mw_fixation_list_md_";
% mdsweep = 3:1:200;
% TITLE = "MD Sweep ASC Paolini";
% SUMSQ = "Min Squares to Target ASC Paolini";

DIR = "/home/ariela/Workspace/Viewmind/CSVReadingMaxDispSweep/asc/md_sweep_norm_1000";
BASEFILENAME = "mw_fixation_list_norm_to_1000_md_";
TITLE = "MD Sweep ASC Paolini Res Norm 1000";
SUMSQ = "Min Squares to Target ASC Paolini";
mdsweep = 5:1:300;

% Reference file. 
config.FILE = [DIR "/fixlist_norm_to_1000.csv"];
config.FIXDURCOLUM = 4;
config.NUMBINS = 10;
config.MAXFIXDUR = 600;
reference = histDurationDataFromCSV(config);

% Maximum Dispersion Sweep File. 
L = length(mdsweep);
data = zeros(L,config.NUMBINS);
diff_all      = zeros(1,L);
diff_selected = zeros(1,L);

% For getting the best result for the reference. 
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
  
  legends{i} = reference.xnames{i};
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

##config.FILE = "/home/ariela/Workspace/Viewmind/CSVReadingMaxDispSweep/asc/mw_fixation_list_md_100.csv";
##config.FIXDURCOLUM = 4;
##
##ourparse = histDurationDataFromCSV(config);
##
##stem(reference.x,reference.histogram_data,'r');
##hold on
##stem(reference.x,ourparse.histogram_data,'k');
##set(gca,'xtick',reference.x,'xticklabel',reference.xnames);
##
##legend({"Referencia","Nuestro"})