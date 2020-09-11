# Script to create a randomized go/no go experiment description

import random

TOTAL_NUM_SLIDES        = 60;
OUTPUT_FILE_NAME        = "go_no_go.dat";


POSSIBLE_SLIDE_TYPES = ["R->", "R<-", "G->", "G<-"];

DIST_TEST    = [3, 3, 2, 2];
DISTRIBUTION = [13, 13, 12, 12];

total = sum(DIST_TEST) + sum(DISTRIBUTION);
print("WILL GENERATE A TOTAL OF " + str(total) + " trials");

def distribute(distribution, types):
    
    non_randomize_vector = [];
    
    for i in range(0,len(distribution)):
        t = types[i];
        amount = distribution[i];
        for i in range(0,amount):
            non_randomize_vector.append(t);
            
    # Randomizing the vector.
    randomized_vector = [];
   
    while (len(non_randomize_vector) > 0):
        index = random.randint(0, len(non_randomize_vector)-1);
        randomized_vector.append(non_randomize_vector[index]);
        non_randomize_vector.pop(index);
        
    return randomized_vector;


test_trials = distribute(DIST_TEST,POSSIBLE_SLIDE_TYPES);
#print(test_trials);

actual_trials = distribute(DISTRIBUTION,POSSIBLE_SLIDE_TYPES);

alltrials = test_trials + actual_trials

f = open(OUTPUT_FILE_NAME,"w");
f.write("v1\n");

for i in range(0,len(alltrials)):
    
    line = str(i)
    while (len(line) < 3): 
        line = "0" + line;

    line = line + " " + alltrials[i] + "\n";
    f.write(line);
       

f.close();

print("Finished");
 
