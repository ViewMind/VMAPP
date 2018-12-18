#!/bin/bash

if [[ $# != 1 ]]; then
   echo "Only one parameter required"
   exit
fi

HOSTADDRESS=".cdqlb2rkfdvi.us-east-2.rds.amazonaws.com"
PORT=3306

if [[ $1 == "db" ]]; then
   HOST="viewminddb$HOSTADDRESS";
   USER="vmuser"
   PASSWD="viewmind_2018"   
elif [[ $1 == "patdata" ]]; then
   HOST="viewmind-patdata$HOSTADDRESS";
   USER="viewmind_patdata"
   PASSWD="2436dhfbvur82653"   
elif [[ $1 == "id" ]]; then
   HOST="viewmind-id$HOSTADDRESS";
   USER="vmid"
   PASSWD="ksjdn3228shs1852"   
else
   echo "Unrecognized RDS: $1"
   exit
fi

export MYSQL_PWD=$PASSWD
mysql -h $HOST -P $PORT -u $USER
