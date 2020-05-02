function ans = histDurationDataFromCSV(config)

% Bins for histogram
bins = 0:(config.MAXFIXDUR/config.NUMBINS):config.MAXFIXDUR;

% Load the data
fulldata = csvread(config.FILE);
fixdur = fulldata(2:end,config.FIXDURCOLUM);

histogram_data = zeros(1,config.NUMBINS);
xnames = cell(1,config.NUMBINS);

for j=2:(config.NUMBINS+1)
   % The each index corresponds to a bin for how many fixation last less than or equal to bins(j).
   histogram_data(j-1) = length(find( fixdur <= bins(j) & fixdur > bins(j-1) ));
   xnames{j-1} = ["(" num2str(bins(j-1)) "," num2str(bins(j)) "]"];
endfor

% Adding all points that are greater than the maximum
histogram_data(config.NUMBINS) = histogram_data(j-1) + length(find( fixdur > bins(end)));
xnames{config.NUMBINS} = ["(" num2str(bins(end)) ", +\infty)"];

% Normalizing the histogram
if (sum(histogram_data) == 0)
  disp(["No fixations for file: " config.FILE])
endif
histogram_data = histogram_data*100/sum(histogram_data);

ans.histogram_data = histogram_data;
ans.xnames = xnames;
ans.x = 1:config.NUMBINS;
  
endfunction
