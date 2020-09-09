# Script to create a randomized go/no go experiment description

import random

NUMBER_OF_TEST_TRIALS   = 12;
NUMBER_OF_ACTUAL_TRIALS = 60;
OUTPUT_FILE_NAME        = "go_no_go.dat";

POSSIBLE_SLIDE_TYPES = ["X", "R->", "R<-", "G->", "G<-"];

f = open(OUTPUT_FILE_NAME,"w");

f.write("v1\n");

N = NUMBER_OF_TEST_TRIALS + NUMBER_OF_ACTUAL_TRIALS;

for i in range(0,N):
    
    line = str(i)
    while (len(line) < 3): 
        line = "0" + line;

    line = line + " " + random.choice(POSSIBLE_SLIDE_TYPES) + "\n";
    f.write(line);
       

f.close();
 
