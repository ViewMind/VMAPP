#!/bin/bash

############## INPUT PARAMETERS #################
INSTUID="1242673082"
LOWERDATE="2019-01-01"

clear

############## DEFINING CONSTANTS #################
DNS="18.220.30.34"
USER="ec2-user"
BASE_HOST_NAME=".cdqlb2rkfdvi.us-east-2.rds.amazonaws.com"
DB_RES_HOST="viewminddb$BASE_HOST_NAME"
DB_ID_HOST="viewmind-id$BASE_HOST_NAME"
DB_PATDATA_HOST="viewmind-patdata$BASE_HOST_NAME"
PASSWD="vimiroot"
DB_RES_NAME="viewmind_data";
DB_ID_NAME="viewmind_id";
DB_PATDATA_NAME="viewmind_patdata";  
DBUSER="root"
PORT=3306
KEY="/home/ariela/Dropbox/ViewMindData/awspem/aws_ec2_first_time.pem"
BUCKET="viewmind-raw-eye-data"
EYEPROCESSOR_PATH="/home/ariela/repos/viewmind_projects/EyeReportGenerator/bin"
EYEPROCESSOR_EXE="EyeReportGen"

#################### Setting up the password access
MYSQL_BASEDB="export MYSQL_PWD=$PASSWD; mysql -h $DB_RES_HOST -u $DBUSER -P $PORT --database=$DB_RES_NAME -e"
MYSQL_PATIDDB="export MYSQL_PWD=$PASSWD; mysql -h $DB_ID_HOST -u $DBUSER -P $PORT --database=$DB_ID_NAME -e"
MYSQL_PATDATADB="export MYSQL_PWD=$PASSWD; mysql -h $DB_PATDATA_HOST -u $DBUSER -P $PORT --database=$DB_PATDATA_NAME -e"


#################### File Names and Paths
FILE_TABLE_RESULT="results_$INSTUID""_from_$LOWERDATE"".csv"
FILE_TABLE_PATDATA="patient_information_$INSTUID""_from_$LOWERDATE"".csv"
FILE_PUID_LIST="puid_list"

QUERYRES_RESULTS="result_query_res"
QUERYRES_ID="id_query_res"
QUERYRES_PATDATA="patdata_query_res"

RAWDATA_DIR="rawdata"

#################### Running the script
echo ">> SEARCHING RESULT TABLE"

query="$MYSQL_BASEDB \"SELECT DISTINCT r.* FROM tEyeResults AS r, tDoctors AS d WHERE r.study_date >= '$LOWERDATE' AND r.doctorid = d.uid AND d.medicalinstitution = '$INSTUID'\""
ssh -i $KEY $USER@$DNS "$query > $QUERYRES_RESULTS";
scp -i $KEY -q $USER@$DNS:"$QUERYRES_RESULTS" .
       
echo ">> GENERATING RESULT TABLE AND PUID LIST"
python queryRes2CSV.py $QUERYRES_RESULTS $FILE_TABLE_RESULT
python generatePUIDList.py $FILE_PUID_LIST

echo ">> SEARCHING PUID INFORMATION"
puids=$(cat $FILE_PUID_LIST)
query="$MYSQL_PATIDDB \"SELECT DISTINCT * FROM tPatientIDs WHERE keyid IN ($puids)\""
ssh -i $KEY $USER@$DNS "$query > $QUERYRES_ID";
scp -i $KEY -q $USER@$DNS:"$QUERYRES_ID" .

echo ">> CREATING PUID HASH MAP"
hashmap_file=($(cat $QUERYRES_ID))
value=""
let counter=0
declare -A hashmap
for h in "${hashmap_file[@]}"; 
do
   if (( (counter % 2) == 0 )); then      
      value=$h
      while (( ${#value} < 8)); do
         value="0$value"
      done
      #echo "value: $value" 
   else
      #echo "key: $h"
      hashmap[$h]=$value;
   fi
   counter=$((counter + 1))
done

query="$MYSQL_PATDATADB \"SELECT DISTINCT * FROM tPatientData WHERE puid IN ($puids)\""
ssh -i $KEY $USER@$DNS "$query > $QUERYRES_PATDATA";
scp -i $KEY -q $USER@$DNS:"$QUERYRES_PATDATA" .

echo ">> GENERATING PATIENT TABLE"
python queryRes2CSV.py $QUERYRES_PATDATA $FILE_TABLE_PATDATA

echo ">> DOWNLOADING AWS FOLDERS"
mkdir $RAWDATA_DIR
let i=0
declare -a hashes
while IFS=$'\n' read -r line_data; do
     if (( $i > 0 )); then
        skip=0;
        for word in $line_data
        do
           if (( skip == 1 )); then
                hashes[$i]=$word
           else
              skip=1
           fi          
        done
     fi
     ((++i))
done < $QUERYRES_ID

for word in "${hashes[@]}"
do
   echo "  >> Downloading: $word"
   ssh -i $KEY $USER@$DNS "mkdir $word; aws s3 cp --recursive s3://$BUCKET/$word $word"  > /dev/null 2>&1
   echo "  >> Copying locally: $word"
   scp -i $KEY -q -r $USER@$DNS:"$word" $RAWDATA_DIR
   echo "  >> Deleting remote: $word"
   ssh -i $KEY $USER@$DNS "rm -rf $word"  > /dev/null 2>&1   
   puid=${hashmap[$word]}
   echo "  >> Renaming downloaded directory to $puid"
   mv $RAWDATA_DIR/$word $RAWDATA_DIR/$puid;   
done

echo ">> CALLING THE EYEPROCESSOR ON EACH OF THE FOLDER"
filelist=($(find $RAWDATA_DIR -name "eye_rep_gen_conf."))
base_path=$(pwd)
cd $EYEPROCESSOR_PATH
for f in "${filelist[@]}"
do
   repgen_file_path="$base_path/$f"
   echo $pwd
   ./$EYEPROCESSOR_EXE $repgen_file_path
done

echo ">> DOING CLEANUP"
cd $base_path
mv $QUERYRES_ID        ./$RAWDATA_DIR
mv $QUERYRES_PATDATA   ./$RAWDATA_DIR
mv $QUERYRES_RESULTS   ./$RAWDATA_DIR
mv $FILE_TABLE_RESULT  ./$RAWDATA_DIR
mv $FILE_TABLE_PATDATA ./$RAWDATA_DIR
rm $FILE_PUID_LIST

