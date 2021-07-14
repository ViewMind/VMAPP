:: Update Script for EyeExperimentner Application
:: Version: 1
:: Date (DD/MM/YYYY) : 12/08/2021

:: Getting rid of anoying echo. 
@ECHO OFF

:: Defining constants. 
SET VERSION=1.0.0
SET CURRENT_DIR=%cd%

:: Local Paths
SET APP_DIRECTORY=App
SET NEW_APP_DIR_NAME=%APP_DIRECTORY%_
SET BKP_DIR=%APP_DIRECTORY%_bkp

:: Full pathds
SET FULL_PATH_APP_DIR=%CURRENT_DIR%\%APP_DIRECTORY%
SET FULL_PATH_NEW_APP_DIR=%CURRENT_DIR%\%NEW_APP_DIR_NAME%
:: SET FULL_PATH_BKP_APP_DIR=%CURRENT_DIR%\%BKP_DIR%

SET VMET_DATA=viewmind_etdata
SET VMCONFINGS=vmconfiguration
SET VMSETTINGS=vmsettings
SET EXE=App.exe
SET LOG=updatelog_file.log


ECHO ======================================================
ECHO =        VIEWMIND SELF UPDATE. Version %VERSION%         =
ECHO ======================================================
ECHO -Viewmind Self Update. Version %VERSION% > %LOG%

:: Waiting 2 seconds just in case. 
ECHO Getting somethings ready ...
TIMEOUT 2 /nobreak > nul

:: Killing the current task. 
taskkill/im %EXE%
if %ERRORLEVEL% neq 0 (
   if %ERRORLEVEL% neq 128 (
      echo Updated failed: Please contact Viewmind Support
      echo - Could not kill current process: %ERRORLEVEL% >> %LOG%
      PAUSE
      exit
   )
   ECHO - Application %EXE% was not found, moving on. >> %LOG%
)

:: Doing the copying. 
:: Copying the ET Data Dir. 
SET cdir=%cd%
SET src=%FULL_PATH_APP_DIR%\%VMET_DATA%
SET dest=%FULL_PATH_NEW_APP_DIR%\%VMET_DATA%\
XCOPY %src% %dest% /E/S/H/Y > NUL
ECHO -Porting viewmind_etdata directory: XCOPY %src% %dest% >> %LOG%
if %ERRORLEVEL% neq 0 (
   echo Updated failed: Please contact Viewmind Support
   echo - The following error was encountered when copying the application directory:%ERRORLEVEL% >> %LOG%
   PAUSE
   exit
)

:: Copying the Configuration file. 
SET src=%FULL_PATH_APP_DIR%\%VMCONFINGS%
SET dest=%FULL_PATH_NEW_APP_DIR%
XCOPY %src% %dest% /y > NUL
ECHO -Porting configuration directory: XCOPY %src% %dest% >> %LOG%
if %ERRORLEVEL% neq 0 (
   echo Updated failed: Please contact Viewmind Support
   echo - The following error was encountered when copying the configuration file: %ERRORLEVEL% >> %LOG%
   PAUSE
   exit
)


:: Copying the Settings file.
SET src=%FULL_PATH_APP_DIR%\%VMSETTINGS%
SET dest=%FULL_PATH_NEW_APP_DIR%
XCOPY %src% %dest% /y > NUL
ECHO -Porting settings directory: XCOPY %src% %dest% >> %LOG%
if %ERRORLEVEL% neq 0 (
   echo Updated failed: Please contact Viewmind Support
   echo - The following error was encountered when copying the settings file: %ERRORLEVEL% >> %LOG%
   PAUSE
   exit
)

:: Deleting the old directory if exists. 
ECHO -Deleting backup location: rmdir %BKP_DIR% >> %LOG%
rmdir /s /q %BKP_DIR% > NUL

:: Renaming the current location to be the backup location. 
RENAME %APP_DIRECTORY% %BKP_DIR%
ECHO -Renaming current application directory to be backup: RENAME %APP_DIRECTORY% %BKP_DIR% >> %LOG%
if %ERRORLEVEL% neq 0 (
   echo Updated failed: Please contact Viewmind Support
   echo - The following error was encountered when renaming the current directory to backup: %ERRORLEVEL% >> %LOG%
   PAUSE
   exit
)

:: Renaming the new location to be the current location. 
RENAME %NEW_APP_DIR_NAME% %APP_DIRECTORY%
ECHO -Renaming new application directory to be current: RENAME %NEW_APP_DIR_NAME% %APP_DIRECTORY% >> %LOG%
if %ERRORLEVEL% neq 0 (
   echo Updated failed: Please contact Viewmind Support
   echo - The following error was encountered when renaming the new directory to current: %ERRORLEVEL% >> %LOG%
   PAUSE
   exit
)

:: Finally the application is restarted. 
cd %APP_DIRECTORY%
START %EXE%

echo Finished
echo Update finalized with success starting the new application >> %LOG%
exit

