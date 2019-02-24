#!/bin/bash

############## ACTUAL PATCH LOGIC #################

if [[ $1 == "dev" ]] || [[ $1 == "prod" ]]; then
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

if [[ $1 == "dev" ]] || [[ $1 == "ldev"  ]]; then
   DB_RES_NAME="viewmind_develop";
   DB_ID_NAME="viewmind_id_dev";
   DB_PATDATA_NAME="viewmind_patdata_dev";
   
else
   DB_RES_NAME="viewmind_data";
   DB_ID_NAME="viewmind_id";
   DB_PATDATA_NAME="viewmind_patdata";  
fi

USER="root"
PORT=3306

# Setting up the password access
export MYSQL_PWD=$PASSWD

echo ">> APPLYING PATCH 05"
echo ">> Constructing doctor institution map logic"
queryres=($(mysql -h $DB_RES_HOST -u $USER -P $PORT --database=$DB_RES_NAME -e "SELECT DISTINCT 
                                                                                    a.uid AS duid, 
                                                                                    a.keyid, 
                                                                                    a.medicalinstitution AS medid
                                                                              FROM tInstitution i, tDoctors a
                                                                              INNER JOIN (
                                                                                 SELECT uid, MAX(keyid) maxkeyid
                                                                                 FROM tDoctors
                                                                                 GROUP BY uid
                                                                              ) b ON a.uid = b.uid AND a.keyid = b.maxkeyid
                                                                             WHERE i.uid = a.medicalinstitution"))

# The firs three elements are the column names.
unset 'queryres[0]'
unset 'queryres[1]'
unset 'queryres[2]'

declare -A drmedidmap; 
counter=0
drid=""
for i in "${queryres[@]}"
do   
#   echo $i
   if (( $counter == 0 )); then
      #echo "DRID: $i"
      drid=$i
      counter=$( expr $counter + 1 )
   elif (( $counter == 2 )); then
      #echo "MEDID: $i"
      drmedidmap[$drid]=$i
      counter=0
   else
     counter=$( expr $counter + 1 )
   fi
done        

for K in "${!drmedidmap[@]}"; 
do 
  value=${drmedidmap[$K]}
  newvalue=$K"_"$value
  echo "$K -> $value -> $newvalue"
done

echo ">> Changing all doctor's id from the tDoctors table"
for K in "${!drmedidmap[@]}"; 
do 
  value=${drmedidmap[$K]}
  newvalue=$K"_"$value
  mysql -h $DB_RES_HOST -u $USER -P $PORT --database=$DB_RES_NAME -e "UPDATE tDoctors SET uid = '$newvalue' WHERE uid = '$K'"
done

echo ">> Changing all doctor's id from the tEyeResults table"
for K in "${!drmedidmap[@]}"; 
do 
  value=${drmedidmap[$K]}
  newvalue=$K"_"$value
  mysql -h $DB_RES_HOST -u $USER -P $PORT --database=$DB_RES_NAME -e "UPDATE tEyeResults SET doctorid = '$newvalue' WHERE doctorid = '$K'"
done
echo ">> Changing all doctor's id from the tPatientData table"
for K in "${!drmedidmap[@]}"; 
do 
  value=${drmedidmap[$K]}
  newvalue=$K"_"$value
  mysql -h $DB_PATDATA_HOST -u $USER -P $PORT --database=$DB_PATDATA_NAME -e "UPDATE tPatientData SET doctorid = '$newvalue' WHERE doctorid = '$K'"
done

# mysql -h $DB_RES_HOST -u $USER -P $PORT --database=$DB_RES_NAME -e "ALTER TABLE tPlacedProducts ADD COLUMN enabled TINYINT;"
# mysql -h $DB_RES_HOST -u $USER -P $PORT --database=$DB_RES_NAME -e "UPDATE tPlacedProducts SET enabled = '1';"

echo "Finished"
