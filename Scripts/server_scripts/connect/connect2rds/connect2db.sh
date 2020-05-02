#!/bin/bash

if [[ $# != 1 ]]; then
   echo "Only one parameter required"
   exit
fi

HOSTADDRESS=".cdqlb2rkfdvi.us-east-2.rds.amazonaws.com"
PORT=3306

# if [[ $1 == "db" ]]; then
#    HOST="viewminddb$HOSTADDRESS";
#    USER="vmuser"
#    PASSWD="viewmind_2018"   
# elif [[ $1 == "patdata" ]]; then
#    HOST="viewmind-patdata$HOSTADDRESS";
#    USER="vmpatdata"
#    PASSWD="2436dhfbvur82653"   
# elif [[ $1 == "id" ]]; then
#    HOST="viewmind-id$HOSTADDRESS";
#    USER="vmid"
#    PASSWD="ksjdn3228shs1852"   
# elif [[ $1 == "rootdb" ]]; then
if [[ $1 == "rootdb" ]]; then
   HOST="viewminddb$HOSTADDRESS";
   USER="root"
   PASSWD="vimiroot"   
elif [[ $1 == "rootid" ]]; then
   HOST="viewmind-id$HOSTADDRESS";
   USER="root"
   PASSWD="vimiroot"   
elif [[ $1 == "rootpatdata" ]]; then
   HOST="viewmind-patdata$HOSTADDRESS";
   USER="root"
   PASSWD="vimiroot"   
elif [[ $1 == "rootdash" ]]; then
   HOST="viewmind-dashboard$HOSTADDRESS"
   USER="root"
   PASSWD="65fjnd621nf35"
else
   echo "Unrecognized RDS: $1"
   exit
fi



export MYSQL_PWD=$PASSWD
mysql -h $HOST -P $PORT -u $USER
