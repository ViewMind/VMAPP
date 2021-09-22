import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects

VMBase {

    id: viewCalibrationStart

    property int vmSelectedEye;

    Connections{
        target: flowControl
        function onConnectedToEyeTracker () {
            if (swiperControl.currentIndex !== swiperControl.vmIndexCalibrationStart) return;
            //console.log("Connected to eyetracker in ViewCalibration")
            var titleMsg;
            if (!flowControl.isConnected()){
                vmErrorDiag.vmErrorCode = vmErrorDiag.vmERROR_CONNECT_ET;
                titleMsg = viewHome.getErrorTitleAndMessage("error_et_connect");
                vmErrorDiag.vmErrorMessage = titleMsg[1];
                vmErrorDiag.vmErrorTitle = titleMsg[0];
                vmErrorDiag.open();
                return;
            }
            // All is good so the calibration is requested.
            flowControl.calibrateEyeTracker(vmSelectedEye);
        }
        function onCalibrationDone () {
            if (swiperControl.currentIndex !== swiperControl.vmIndexCalibrationStart) return;
            //console.log("Calibration done in ViewCalibration")
            var titleMsg;
            if (!flowControl.isCalibrated()){
                calibrationFailedDialog.vmLeftEyePassed = flowControl.isLeftEyeCalibrated();
                calibrationFailedDialog.vmRightEyePassed = flowControl.isRightEyeCalibrated();
                viewCalibrationStartDiag.close();
                calibrationFailedDialog.open();
                return;
            }
            else{
                viewCalibrationStartDiag.close()
                swiperControl.currentIndex = swiperControl.vmIndexCalibrationDone;
            }
        }

    }

    function openDiag(){
        viewCalibrationStartDiag.open()
    }

    VMCalibrationFailedDialog{
        id: calibrationFailedDialog
        x: (mainWindow.width-width)/2
        y: (mainWindow.height-height)/2
    }

    VMDialogBase {

        id: viewCalibrationStartDiag
        vmNoCloseButton: true
        width: mainWindow.width*0.479
        height: mainWindow.height*0.758

        onWidthChanged: repositionImage();
        onHeightChanged: repositionImage();

        // The configure settings title
        Text {
            id: viewTitle
            font.pixelSize: 43*viewHome.vmScale
            font.family: viewCalibrationStart.gothamB.name
            color: "#297FCA"
            text: loader.getStringForKey("viewcalibstart_viewTitle");
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: mainWindow.height*0.124
        }

        // The configure settings title
        Text {
            id: viewSubTitle
            font.pixelSize: 13*viewHome.vmScale
            font.family: viewCalibrationStart.robotoR.name
            color: "#297FCA"
            text: loader.getStringForKey("viewcalibstart_viewSubTitle");
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: viewTitle.bottom
            anchors.topMargin: mainWindow.height*0.040
        }

        // Loading the image
        Image {
            id: imgEye
            source: "qrc:/images/OJO.png"
            scale: viewHome.vmScale
            transformOrigin: Item.TopLeft
            onScaleChanged: {
                viewCalibrationStartDiag.repositionImage();
            }
        }

        function repositionImage(){
            imgEye.x = (viewCalibrationStartDiag.width - imgEye.width*viewHome.vmScale)/2;
            imgEye.y = viewSubTitle.y + viewSubTitle.height + mainWindow.height*0.054;
        }


        VMButton{
            id: btnStart
            vmFont: gothamM.name            
            vmSize: [mainWindow.width*0.148, mainWindow.height*0.072]
            vmText: loader.getStringForKey("viewcalibstart_btnStart");
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: mainWindow.height*0.061
            onClicked: {
                if (!flowControl.isConnected()) flowControl.connectToEyeTracker();
                else flowControl.calibrateEyeTracker(vmSelectedEye);
            }
        }

    }

}
