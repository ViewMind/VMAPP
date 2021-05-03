#!/bin/bash
### CRONJOB LINE: 0 5 * * * /home/ec2-user/viewmind_projects/Scripts/inst_data_available/cronjob.sh
LOG=$(date '+%Y_%m_%d_%H_%M_%S.log')
cd /home/ec2-user/viewmind_projects/Scripts/inst_data_available
php create_sync_inst_info.php > $LOG 2>&1
