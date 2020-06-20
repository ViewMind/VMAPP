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

USER="root"
PORT=3306

# Setting up the password access
export MYSQL_PWD=$PASSWD

echo ">> APPLYING PATCH 15"

mysql -h $DB_RES_HOST -u $USER -P $PORT --database=$DB_RES_NAME -e "ALTER TABLE tEyeResults ADD nbrt_fix_enc INT;
                                                                    ALTER TABLE tEyeResults ADD nbrt_fix_ret INT;
                                                                    ALTER TABLE tEyeResults ADD nbrt_inhib_prob FLOAT;
                                                                    ALTER TABLE tEyeResults ADD nbrt_seq_complete FLOAT;
                                                                    ALTER TABLE tEyeResults ADD nbrt_target_hit FLOAT;
                                                                    ALTER TABLE tEyeResults ADD nbrt_mean_resp_time FLOAT;
                                                                    ALTER TABLE tEyeResults ADD nbrt_mean_sac_amp FLOAT;"

echo "Finished"


