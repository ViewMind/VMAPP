#!/bin/bash 
pushd ../../pdf_report_generation/resource_generator/
./create.sh
popd
php pdftest.php
