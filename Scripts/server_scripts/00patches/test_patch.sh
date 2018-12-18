#!/bin/bash

#################### CONFIG DATA: Starting bkp and patch to test. Avoids using parameters
DB_BKP="prod_2018_12_15_06_26.sql" 
PATCH_TO_TEST="mod03.sql"

############## LOCAL PRODUCTION DB INFO #################
#LOCALPASS="vmdb"
#LOCALUSER="vmdb"
LOCALPASS="givnar"
LOCALUSER="root"
DBNAME="viewmind_data"
DEST="../bkpProd"

# Setting up the password access
export MYSQL_PWD=$LOCALPASS

# Restoring to the local database.
echo ">> RESTORING TO BKP TO LOCAL DB"
mysql -u $LOCALUSER < $DEST/$DB_BKP 

echo ">> APPLYING PATCH $PATCH_TO_TEST"
mysql -u $LOCALUSER < $PATCH_TO_TEST
