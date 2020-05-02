clc;
close all;
clear all;

data = load('sweep_resultsreading_es_2_2019_06_13_19_21_10-150_50-150.out');
selected_dispersions = [70 80 90 100];

% Separating into matrixes by sample frequency

[N,three] = size(data);

[C Ind ic] = unique(data(:,1));

L = length(Ind);
start = 1;
legends = cell(1,L);

%lineparameters = {'b','r','k','g','m','y','c'};
lineparameters = {'bo','b+','b>','ro','r+','r>','ko','k+','k>','go','g+','g>','mo','m+','m>','yo','y+','y>','co','c+','c>'};
NLineP = length(lineparameters);
lpInd = 1;
optimumvalues = zeros(L,2);

% Selected dispersions struct
SL = length(selected_dispersions);
bestDisp = cell(1,SL);
for i=1:SL
  bestDisp{i} = zeros(L,2);
endfor

for i=1:L
  x = data(start:Ind(i),2);
  y = data(start:Ind(i),3);
  
  % Finding the optimum values.
  optimumvalues(i,1) = data(start,1);
  [maxfix, maxfixInd] = max(y);
  optimumvalues(i,2) = x(maxfixInd);
  legends{i} = num2str(data(start,1));
  
  % Gathering data for selected dispersion.
  for j=1:SL
    bestDisp{j}(i,1) = data(start,1);
    ind = find(x == selected_dispersions(j));
    bestDisp{j}(i,2) = y(ind);
  endfor  
  
  plot(x,y,lineparameters{lpInd})
  hold on
  start = Ind(i)+1;    
  lpInd = lpInd + 1;
  if (lpInd == NLineP) 
     lpInd = 0;  
  endif
endfor  
legend(legends);
xlabel("Maximum Dispersion Used");
ylabel("Total Number of Fixations Found");

%Plotting the optimum value
figure();
bar(optimumvalues(:,1),optimumvalues(:,2))
ylabel('Optimum Maximum Dispersion');
xlabel('Frequency')

% Plotting the best dispersions.
figure()
legends = cell(1,SL);
for i=1:SL
  plot(bestDisp{i}(:,1),bestDisp{i}(:,2));
  hold on
  legends{i} = num2str(selected_dispersions(i));
endfor
legend(legends)
ylabel('Fixations Found')
xlabel('Sample Frequency (Hz)')
