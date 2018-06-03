#!/usr/bin/env python2
# -*- coding: utf-8 -*-
"""
Created on Thu May 31 18:23:27 2018

@author: ariela
"""

# Linux path
#defines = "/home/ariela/Dropbox/viewmind_projects/CommonClasses/common.h"
#output  = "/home/ariela/Dropbox/viewmind_projects/EyeExperimenter/src/qml/VMDefines.qml"

# Windows path
defines = "C:/Users/Viewmind/Documents/QtProjects/CommonClasses/common.h"
output  = "C:/Users/Viewmind/Documents/QtProjects/EyeExperimenter/src/qml/VMDefines.qml"

# Constants
startswith = "#define   CONFIG";
preamble = "    readonly property string vm";

# Opening the input file
f = open(defines,"r");
content = f.readlines();
f.close();

# Opening the output file
f = open(output,'w');
f.write("import QtQuick 2.0\n\nItem{\n\n");


for l in content:
    if l.startswith(startswith):
        tokens = l.split();
        #print(tokens[1] + ": " + tokens[2]);        
        f.write(preamble + tokens[1] + ": " + tokens[2] + "\n");
        
f.write("}");
f.close();
print("FINISHED")