# -*- coding: utf-8 -*-
"""
This calculates the times between each of the binding slides
"""
HEADER = "#IMAGE"
#bindingfile = "C:\\Users\\Viewmind\\Documents\\ExperimenterOutputs\\patients\\alvarez\\binding_bc_2018_06_18.dat";
#bindingfile="C:\\Users\\Viewmind\\Documents\\ExperimenterOutputs\\patients\\otherBindings\\binding_uc_0000_0.dat"
bindingfile="C:\\Users\\Viewmind\\Documents\\ExperimenterOutputs\\patients\\otherBindings\\gera_nuevo.dat"

f = open(bindingfile,'r');
lines = f.readlines();
imagecount = 0;
resolution = "";
divider = 1;
#divider = 1000;

# States
NAME0 = 0;
START1 = 1;
NAME1 = 2;
START2 = 3;
ANS = 4;

state = NAME0;
lastline = "";
currentname = 0;
times = {};

for line in lines:
    # Finding the second header
    line = line.strip()
    if (imagecount < 2):
        if (line == HEADER):
            imagecount = imagecount + 1;
        continue;
    
    # This is the resolution
    if (resolution == ""):
        resolution = line;
        print("The resolution is: " + resolution);
        continue

    # If it got here, we are processing the file.
    tokens = line.split(' ');
    #print(line)    
    if (state == NAME0):
        if (len(tokens) == 2):
            #print("NAME0")
            times[tokens[0]] = [0,0];
            currentname = tokens[0];
            state = START1;
    elif (state == START1):        
        if (len(tokens) > 2):
            #print("START1")
            times[currentname][0] = int(tokens[0]);
            state = NAME1;
    elif (state == NAME1):
        if (len(tokens) == 2):
            #print("NAME1")
            tokens = lastline.split(' ');
            end = int(tokens[0]);
            times[currentname][0] = (end - times[currentname][0])/divider;
            state = START2;
    elif (state == START2):
        if (len(tokens) > 2):
            #print("START2")
            times[currentname][1] = int(tokens[0]);
            state = ANS;        
    elif (state == ANS):
        if (len(tokens) == 1):
            #print("ANS")
            tokens = lastline.split(' ');
            end = int(tokens[0]);
            times[currentname][1] = (end - times[currentname][1])/divider;
            state = NAME0;
        
    lastline = line;
        
for name in times:
    print(name + " 0: " + str(times[name][0]) + " 1: " + str(times[name][1]));
    
print("DONE")

