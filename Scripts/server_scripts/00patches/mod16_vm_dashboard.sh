#!/bin/bash

############## ACTUAL PATCH LOGIC #################

if [[ $1 == "prod" ]]; then
   BASE_HOST_NAME=".cdqlb2rkfdvi.us-east-2.rds.amazonaws.com"
   DB_RES_HOST="viewminddb$BASE_HOST_NAME"
   DB_ID_HOST="viewmind-id$BASE_HOST_NAME"
   DB_PATDATA_HOST="viewmind-patdata$BASE_HOST_NAME"
   DB_DASHBOARD_HOST="viewmind-dashboard$BASE_HOST_NAME"
   PASSWD="vimiroot"
else
   BASE_HOST_NAME=""
   DB_RES_HOST="localhost"
   DB_ID_HOST="localhost"
   DB_PATDATA_HOST="localhost"
   DB_DASHBOARD_HOST="localhost"
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
# Applying both patches. 
echo "Dashboard patch"
mysql -h $DB_DASHBOARD_HOST -u $USER -P $PORT --database=$DB_DASHBOARD_NAME < sql_scripts/mod16_vm_dashboard.sql
#echo "Data patch"
#mysql -h $DB_RES_HOST       -u $USER -P $PORT --database=$DB_RES_NAME < sql_scripts/mod16_vm_data.sql

echo "Finished"


