function fixation_duration_analisis(CSVDIR,FILE_BASE_NAME,MAXDISPSWEEP)

##% Where are the CSVs generated by the maximum dispersion sweep
##CSVDIR = "/home/ariela/Workspace/Viewmind/CSVReadingMaxDispSweep/set_150_1";
##CSVDIR = "/home/ariela/Workspace/Viewmind/CSVReadingMaxDispSweep/set_120_1";
##
##% All files in the CSVDIR should be named like this followed by -MAXDISPVALUE.csv
##FILE_BASE_NAME = "reading_es_2_2019_06_13_19_21";
##FILE_BASE_NAME = "reading_es_2_2020_02_13_11_08"

% Which csv column is the fixation duration.
% READING FILES  = 9;
FIXDURCOLUM = 9;

% Number of bins for histogram
NUMBINS = 10;

% Maximum expected fixation duration (in ms).
MAXFIXDUR = 600;

% The percentage of percentiles
percentiles_percent = 0:10:100;

% Where the fixation durations will be stored
percentiles = zeros(length(MAXDISPSWEEP),length(percentiles_percent));

% Bins for histogram
bins = 0:(MAXFIXDUR/NUMBINS):MAXFIXDUR;

% Where the histogram data will be stored. 
histogram_data = zeros(length(MAXDISPSWEEP),NUMBINS);

% For plotting
lineparameters = {'bo','b+','b>','ro','r+','r>','ko','k+','k>','go','g+','g>','mo','m+','m>','yo','y+','y>','co','c+','c>'};
NLineP = length(lineparameters);
lpInd = 1;

for MD=MAXDISPSWEEP
   csvfile = [CSVDIR "/" FILE_BASE_NAME "-" num2str(MD) ".csv"];
   csvdata = csvread(csvfile);  
   index = MD-MAXDISPSWEEP(1)+1;
   % fixdur{MD-MAXDISPSWEEP(1)+1} =  csvdata(:,FIXDURCOLUM);
   
   % Getting the fixation duration. Need to avoid one as text is transformed into a 0. 
   fixdur = csvdata(2:end,FIXDURCOLUM);
   
   % Sorting from highest to lowest. 
   fixdur = sort(fixdur);
   
   % Finding the indexes of the percentiles
   indexes = round(percentiles_percent*length(fixdur)/100);
   
   % Adding one to the first, so that it's not 0
   indexes(1) = 1;
   
   % Finding the actual percentiles and storing them
   percentiles(index,:) = fixdur(indexes);   
   
   % Filling the histogram data
   for j=2:(NUMBINS+1)
     % The each index corresponds to a bin for how many fixation last less than or equal to bins(j).
     histogram_data(index,j-1) = length(find( fixdur <= bins(j) & fixdur > bins(j-1) ));
   endfor
   
endfor


% Actually plotting


##figure()
##for i=1:length(percentiles_percent)
##  plot(MAXDISPSWEEP,percentiles(:,i));
##  hold on;
##endfor

figure()
legends = cell(1,NUMBINS);
for i=1:NUMBINS
  
  plot(MAXDISPSWEEP,histogram_data(:,i),lineparameters{lpInd});
  hold on;   
  lpInd = lpInd + 1;
  if (lpInd == NLineP) 
     lpInd = 0;  
  endif  
  
  legends{i} = [num2str(bins(i)) " <= " num2str(bins(i+1))];
endfor
legend(legends);

##figure()
##for i=1:length(MAXDISPSWEEP)
##  plot(percentiles_percent,percentiles(i,:));
##  hold on;
##endfor

endfunction

###########################################################################



