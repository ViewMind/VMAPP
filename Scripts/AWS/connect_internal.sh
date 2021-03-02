#!/bin/bash

if [[ $1 == "proc" ]]; then
   ssh ec2-user@10.100.6.150
elif [[ $1 == "web" ]]; then
   ssh ec2-user@10.100.14.154
else
   echo "Do not know $1"
   exit;
fi
