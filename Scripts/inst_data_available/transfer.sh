#!/bin/bash 


SOURCE=$1
DESTIP=$2
USER=$3

# SOURCE="OUTPUTS/989302458/"
# DESTIP="68.183.67.134"
# USER="novartis"

# The actual transfer
scp -r $SOURCE root@$DESTIP:/home/$USER

# Changing permissions
CHANGE_PERMISSIONS="chown -R $USER:$USER /home/$USER"   
ssh root@$DESTIP "$CHANGE_PERMISSIONS"   
