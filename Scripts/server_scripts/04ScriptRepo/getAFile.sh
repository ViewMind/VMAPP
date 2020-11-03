#!/bin/bash

## Production IP
DNS="18.220.30.34"
USER="ec2-user"

FILE_TO_GET="/home/ec2-user/EyeUpdateServer/etdir/989302458/0001/dbbkps/localdb.dat.2020_10_21_10_07";

# Command to downlaod an S3 folder and then copy it locally
#scp -i $KEY $USER@$DNS:"$FILE_TO_GET" .

scp $USER@$DNS:"$FILE_TO_GET" .



