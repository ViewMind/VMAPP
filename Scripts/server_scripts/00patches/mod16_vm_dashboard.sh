#!/bin/bash

############## ACTUAL PATCH LOGIC #################

if [[ $1 == "prod" ]]; then
   BASE_HOST_NAME=".cdqlb2rkfdvi.us-east-2.rds.amazonaws.com"
   DB_RES_HOST="viewminddb$BASE_HOST_NAME"
   DB_ID_HOST="viewmind-id$BASE_HOST_NAME"
   DB_PATDATA_HOST="viewmind-patdata$BASE_HOST_NAME"
   PASSWD="vimiroot"
else
   BASE_HOST_NAME=""
   DB_RES_HOST="localhost"
   DB_ID_HOST="localhost"
   DB_PATDATA_HOST="localhost"
   PASSWD="givnar"
fi

DB_RES_NAME="viewmind_data";
DB_ID_NAME="viewmind_id";
DB_PATDATA_NAME="viewmind_patdata";  
DB_DASHBOARD_NAME="viewmind_dashboard";

USER="root"
PORT=3306

# Setting up the password access
export MYSQL_PWD=$PASSWD

echo ">> APPLYING PATCH 16"

mysql -h $DB_PATDATA_HOST -u $USER -P $PORT --database=$DB_DASHBOARD_NAME < sql_scripts/mod16_vm_dashboard.sql

echo "Finished"


