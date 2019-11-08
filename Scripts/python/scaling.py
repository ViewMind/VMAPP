#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Nov  7 06:55:27 2019

@author: ariela
"""

import os

inputPath = "../../VMSoftwareSuite/EyeExperimenter/src/qml/";
outputPath = "../../VMSoftwareSuite/EyeExperimenter/src/qml/output/";

for entry in os.listdir(inputPath):
     if os.path.isfile(os.path.join(inputPath, entry)):
        print("PROCESSING: " + entry)
        h = open(inputPath + entry,"r");        
        ho = open(outputPath + entry,"w")
        line = h.readline();
        while line:    
            if (("width" in line) or ("rightMargin:" in line) or ("leftMargin" in line)):    
                #print(line)
                parts = line.split(":");
                if (len(parts) == 2):
                    value = parts[1].strip()                 
                    if (value.isdigit()):
                        k = round(float(value)/1280.0,3)
                        line = line.replace(value,"mainWindow.width*"+str(k))
                        #print("R:" + line)  
                    
            elif (("height" in line) or ("topMargin:" in line) or ("bottomMargin" in line) or ("vmHeight" in line)):
                #print(line)
                parts = line.split(":");
                if (len(parts) == 2):                
                    value = parts[1].strip()                 
                    if (value.isdigit()):
                        k = round(float(value)/690.0,3)
                        line = line.replace(value,"mainWindow.height*"+str(k))
                        #print("R:" + line)                
            ho.write(line)
            line = h.readline();
        h.close();    
        ho.close();
        
print("FINISHED")