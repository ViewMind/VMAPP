:: Update Script for EyeExperimentner Application

:: Version: 1.0.2
:: Date (DD/MM/YYYY) : 04/07/2023
:: -> Removed the copying of the settings file as it is no longer necessary as of version 26.0.0 

:: Version: 1.0.1
:: Date (DD/MM/YYYY) : 12/08/2021

:: Getting rid of anoying echo. 
@ECHO OFF

:: Defining constants. 
SET VERSION=1.0.3
SET CURRENT_DIR=%cd%

:: Local Paths
SET APP_DIRECTORY=EyeExperimenter
SET VMTOOLDIR=VMMaintenanceTool

:: Full pathds
SET FULL_PATH_APP_DIR=%CURRENT_DIR%\%BKP_DIR%
SET FULL_PATH_NEW_APP_DIR=%CURRENT_DIR%\%APP_DIRECTORY%

SET VMET_DATA=viewmind_etdata
SET VMCONFINGS=vmconfiguration
SET VMSETTINGS=vmsettings
SET EXE=EyeExperimenter.exe
SET TOOLEXE=VMMaintenanceTool.exe
SET LOG=updatelog_file.log


ECHO ======================================================
ECHO =        VIEWMIND SELF UPDATE. Version %VERSION%         =
ECHO ======================================================
ECHO -Viewmind Self Update. Version %VERSION% > %LOG%
ECHO -Date: %date%

ECHO Getting somethings ready ...

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

:: Waiting a few seconds to ensure the OS Frees up the directory. 
TIMEOUT 5 /nobreak > nul
CD %VMTOOLDIR%
START %TOOLEXE% update
