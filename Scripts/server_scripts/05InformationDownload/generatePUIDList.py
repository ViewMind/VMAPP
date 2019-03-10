#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Mar 10 15:36:01 2019

@author: ariela
"""
import sys;

try:
   f = open("result_query_res",'r+');
except:
   exit();

# Must have only one argument (first one is script name)
if (len(sys.argv) != 2):
    exit();

# Opening the file to write.
try:
   w = open(sys.argv[1],'w+');
except:
   print("Could nto open file for writing: " + sys.argv[1]);
   f.close();
   exit();
    
content = f.readlines();
f.close();

# Finiding the puid index
title_line = content.pop(0);
columns = title_line.split('\t');
puid_index = -1;

for i in range(len(columns)):
    col = columns[i].strip();
    if (col == "puid"):
        puid_index = i;
        break;

if (puid_index == -1):
    print("Could not locate the puid column in the tEyeResultTable");
    exit();

puids = [];

for l in content:
    parts = l.split('\t');
    puid = parts[puid_index].strip();
    if not (puid in puids):
        puids.append(puid);


w.write(",".join(puids));
w.close();

