#!/bin/bash

if [[ $1 == "rawdataserver1" ]]; then
   ssh ec2-user@10.100.6.150
elif [[ $1 == "web" ]]; then
   ssh ec2-user@10.100.7.77
elif [[ $1 == "proc1" ]]; then   
   ssh ec2-user@10.100.15.115
else
   echo "Do not know $1"
   exit;
fi
