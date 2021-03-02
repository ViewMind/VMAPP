#!/bin/bash
DNS="3.139.105.122"
#USER="ubuntu"
USER="ec2-user"
#ssh -i aws_ec2_first_time.pem $USER@$DNS
ssh $USER@$DNS
