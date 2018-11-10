#!/bin/bash

# Parameters
DBHOST="viewminddb.cdqlb2rkfdvi.us-east-2.rds.amazonaws.com"
DBPORT=3306
DBUSER=root
DBPASS=vimiroot


# Need exactly two arguments: Where AND Patch Number

if [[ $# -ne 1 ]]; then
   echo "Need exactly 1 argument: which DB [dev|prod]";
   exit
fi

if [[ $1 == "dev" ]]; then
   DBNAME="viewmind_develop"
elif [[ $1 == "prod" ]]; then
   DBNAME="viewmind_data"
else
   echo "Invalid target: $1. Possibilities are: dev or prod";
   exit
fi


mysql -h $DBHOST  -P $DBPORT -u $DBUSER -p"$DBPASS" $DBNAME < test_check.sql
