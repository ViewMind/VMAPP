#!/bin/bash

## Production IP
DNS="18.220.30.34"
USER="ec2-user"
KEY="/home/ariela/Dropbox/ViewMindData/awspem/aws_ec2_first_time.pem"

# Command to downlaod an S3 folder and then copy it locally
scp -i $KEY $1 $USER@$DNS:"$2" 




