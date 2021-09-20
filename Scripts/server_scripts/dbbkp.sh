#!/bin/bash

BASTION_IP="3.139.215.159"
BASTION_USER_NAME="ec2-user"
BASTION_SRC_FOLDER="/home/ec2-user/viewmind_projects/Scripts/AWS"
BASTION_WORK_DIR="$BASTION_SRC_FOLDER/SQL_BKPS"

CURRENT_FOLDER=$(date +"%Y_%m_%d_%H_%M")
#CURRENT_FOLDER="2021_05_24_16_33"

BASTION_BKP_DIR="$BASTION_WORK_DIR/$CURRENT_FOLDER" 
DEST="bkpProd"

# Array of database names to Backup. 
declare -a DB_NAMES=("viewmind_data" "viewmind_id" "viewmind_patdata" "viewmind_dashboard")
declare -a CNF_FILES=("vm-data.cnf"  "vm-id.cnf"   "vm-patdata.cnf"    "vm-dashboard.cnf")

# Prepraing remote
echo "Preparing remote ..."
CLEAN_CMD="rm -rf $BASTION_WORK_DIR; mkdir -p $BASTION_BKP_DIR"
ssh $BASTION_USER_NAME@$BASTION_IP "$CLEAN_CMD"

echo "Doing DB Backups"

# Connect to the Bastion and do the Backups.
total=${#DB_NAMES[@]}
for (( i=0; i<$total; i++ )); do
   
   # The database to backup.
   DB_NAME=${DB_NAMES[$i]}

   # The credentials file
   CNF=${CNF_FILES[$i]}

   CNF="$BASTION_SRC_FOLDER/CNF/$CNF"

   # Output file name is the dababase name + sql
   BKP_FILE_NAME="$BASTION_BKP_DIR/$DB_NAME.sql"

   # Database backup command. 
   BKP_CMD="mysqldump --defaults-extra-file=$CNF $DB_NAME > $BKP_FILE_NAME"

   # Running the command. 
   echo "   Backing up DB: $DB_NAME"
   ssh $BASTION_USER_NAME@$BASTION_IP "$BKP_CMD"
   
done

echo "Copying bkp directory locally..."
scp -r $BASTION_USER_NAME@$BASTION_IP:$BASTION_BKP_DIR $DEST 

echo "Restoring backups locally ... "
for (( i=0; i<$total; i++ )); do
      
   # The database to backup.
   DB_NAME=${DB_NAMES[$i]}

   # The corresponding backup file.
   SQL_FILE="$DEST/$CURRENT_FOLDER/$DB_NAME.sql"

   # Running the command.
   echo "   Restoring $DB_NAME locally ..."   
   mysql --defaults-extra-file=local.cnf $DB_NAME < $SQL_FILE

done

echo "Finished"
