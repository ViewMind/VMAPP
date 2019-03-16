#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Mar 10 15:36:01 2019

@author: ariela
"""
import sys;

# Must have only two argument (first one is script name)
if (len(sys.argv) != 3):
    print("Two arguments are requiered")
    exit();

fileInput = sys.argv[1];
fileOutput = sys.argv[2];

try:
   f = open(fileInput,'r+');
except:
   exit();


# Opening the file to write.
try:
   w = open(fileOutput,'w+');
except:
   print("Could nto open file for writing: " + sys.argv[1]);
   f.close();
   exit();
    
content = f.readlines();
f.close();

# Finiding the puid index
title_line = content[0];


for l in content:
    parts = l.split('\t');
    joined = '","'.join(parts)
    joined = '"' + joined + '"\n';
    w.write(joined);
        
w.close();
