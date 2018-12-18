#!/bin/bash
DNS="18.191.142.5"
REPO="/home/ariela/repos/viewmind_projects"
KEY="/home/ariela/Dropbox/ViewMindData/awspem/aws_ec2_first_time.pem"

# Programs
SSLSERVER=$REPO"/EyeServer/bin/EyeServer"
EYEREPGEN=$REPO"/EyeReportGenerator/bin/EyeReportGen"
DBMNG=$REPO"/DBManager/bin/DBManager"

# Configuration files
SSLSERVER_CONFIG=$REPO"/EyeServer/bin/configuration"

# Scripts
SQLSCRIPTS=$REPO"/Scripts/dbscripts"

# Copying
scp -r -i $KEY ec2-user@$DNS:/home/ec2-user/EyeServer/s3bkp .

#scp -i $KEY $DBMNG     ec2-user@$DNS:/home/ec2-user/DBMng/
#scp -i $KEY $EYEREPGEN ec2-user@$DNS:/home/ec2-user/EyeReportGenerator
##### OLD STUFF
#scp -i $KEY  $SSLSERVER_CONFIG ec2-user@$DNS:/home/ec2-user/EyeServer/configuration
#scp -i $KEY  $EYEREPGEN ec2-user@$DNS:/home/ec2-user/EyeReportGenerator
#scp -i $KEY -r $SQLSCRIPTS ec2-user@$DNS:/home/ec2-user/EyeServer/
#scp -i $KEY  "/home/ariela/Workspace/TestSingleCLICommand/bin/testsrc" ec2-user@$DNS:/home/ec2-user/
# Copying the scripts to db
#scp -i $KEY -r $SQLSCRIPTS/mod02.sql ec2-user@$DNS:/home/ec2-user/sqlscripts/mod02.sql


