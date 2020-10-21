# -*- coding: utf-8 -*-
"""
Created on Wed Oct 21 04:35:05 2020

@author: Viewmind
"""

import random;

input_path = "../../VMSoftwareSuite/EyeExperimenter/src/experiment_data/fielding_only3.dat"
output_path = "../../VMSoftwareSuite/EyeExperimenter/src/experiment_data/fielding.dat"
EXPAND_TO = 20;

h = open(input_path,'r');

new_trials = [];

# Skipping the first line. 
line = h.readline();
line = h.readline();

new_trials.append("v1");

while line:
    #print(line);

    trial_description = line.split();
    #print(trial_description);
    trial_id = trial_description.pop(0);
    #print(trial_id);
    #print("=========================================");
    #print(trial_description);
    
    rem_targets = [0,1,2,3,4,5];
    
    for i in trial_description:
        rem_targets.pop(rem_targets.index(int(i)));
        
    #print(rem_targets);
    
    # Compute how many targets to add. 
    to_add = EXPAND_TO - len(trial_description);
    
    while (to_add > 0):
                
        if (len(rem_targets) > 0):
            index = random.randint(0,len(rem_targets)-1);
            trial_description.append(str(rem_targets[index]));
            rem_targets.pop(index);
        else:
            box_id = random.randint(0,5);
            trial_description.append(str(box_id));
        
        to_add = to_add - 1;
    
    trial_description.insert(0,trial_id);
    new_trials.append(' '.join(trial_description));
    #print(trial_description);
    
    
    line = h.readline();
    
h.close();

# Output.
h = open(output_path,'w');
h.write("\n".join(new_trials));
h.close();

print("Finished");