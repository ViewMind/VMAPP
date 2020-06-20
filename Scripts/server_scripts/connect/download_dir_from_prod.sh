#!/bin/bash

SOURCE="/home/ec2-user/EyeServer/OUTPUTS/89d60465c657dea363a37dd4b6294c943720027498334f37ebe102282c7939795e058e9055d53fadebab85089fb7295a2f4c47c5c72178c3d3fef8952083696b/2020_06_17_19_09_40"

DNS="18.220.30.34"
USER="ec2-user"
scp -i /home/ariela/Dropbox/ViewMindData/awspem/aws_ec2_first_time.pem -r $USER@$DNS:$SOURCE . 
