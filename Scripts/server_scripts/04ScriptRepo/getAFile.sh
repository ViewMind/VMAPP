#!/bin/bash

## Production IP
DNS="18.220.30.34"
USER="ec2-user"
KEY="/home/ariela/Dropbox/ViewMindData/awspem/aws_ec2_first_time.pem"

FILE_TO_GET="/home/ec2-user/EyeServer/OUTPUTS/fc223a8816556ced8e11e03419fdd770f9e41c6cfd98ffb7b798cd985dcca2935a721f62bff455b1f5dab4875f6ae668a25382e3d76814356b642ecae1e0f0ae/2020_05_17_23_17_09/reading_es_2_2020_05_18_01_13.csv";

# Command to downlaod an S3 folder and then copy it locally
scp -i $KEY $USER@$DNS:"$FILE_TO_GET" .




