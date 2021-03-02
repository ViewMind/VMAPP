#!/bin/bash

LOG=$(date '+%Y_%m_%d_%H_%M_%S.log')
php php_log_tests.php > $LOG 2>&1
