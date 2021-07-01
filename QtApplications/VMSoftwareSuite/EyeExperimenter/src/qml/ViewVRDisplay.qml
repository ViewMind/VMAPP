import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import com.qml 1.0

VMBase {

    id: viewVRDisplay


    function disableStartStudyButton(){
        btnStartStudy.enabled = false;
        btnStartStudy.visible = true;
    }

    function finishedStudySucessfully(){
        btnStartStudy.visible = true;
        btnStartStudy.enabled = true;
        btnCalibrate.enabled = true;
        swiperControl.currentIndex = swiperControl.vmIndexPresentExperiment;
    }

    function startStudy(){
        btnStartStudy.enabled = false;
        btnStartStudy.visible = true;
        btnCalibrate.enabled = true;
    }

    function testCalibrationFailedDialog(){
        calibrationFailedDialog.vmLeftEyePassed = true;
        calibrationFailedDialog.vmRightEyePassed = false;
        calibrationFailedDialog.open();
    }

    readonly property string keysearch: "viewvrscreen_"
    property string vmStudyTitle: "NO TITLE SET"

    Connections{
        target: flowControl
        onNewImageAvailable: {
            hmdView.image = flowControl.image;
        }
    }

    Connections{
        target: flowControl
        onConnectedToEyeTracker:{
            if (swiperControl.currentIndex != swiperControl.vmIndexVRDisplay) return;
            //console.log("Connected to eyetracker in ViewVRDisplay")
            var titleMsg;
            if (!flowControl.isConnected()){
                vmErrorDiag.vmErrorCode = vmErrorDiag.vmErrorCodeNotClose;
                titleMsg = viewHome.getErrorTitleAndMessage("error_et_connect");
                vmErrorDiag.vmErrorMessage = titleMsg[1];
                vmErrorDiag.vmErrorTitle = titleMsg[0];
                vmErrorDiag.open();
                btnCalibrate.enabled = true;
                return;
            }
            // All is good so the calibration is requested.
            flowControl.calibrateEyeTracker(viewCalibrationStart.vmSelectedEye);
        }
        onCalibrationDone: {
            if (swiperControl.currentIndex != swiperControl.vmIndexVRDisplay) return;
            //console.log("Calibration done in ViewVRDisplay")
            var titleMsg;
            if (!flowControl.isCalibrated()){
                calibrationFailedDialog.vmLeftEyePassed = flowControl.isLeftEyeCalibrated();
                calibrationFailedDialog.vmRightEyePassed = flowControl.isRightEyeCalibrated();
                calibrationFailedDialog.open();
                btnCalibrate.enabled = true;
                return;
            }
            flowControl.generateWaitScreen(loader.getStringForKey("waitscreenmsg_calibrationEnd"));
            btnCalibrate.enabled = true;
            btnStartStudy.enabled = true;
            btnStartStudy.visible = true;
        }

    }

    VMCalibrationFailedDialog{
        id: calibrationFailedDialog
        x: (mainWindow.width-width)/2
        y: (mainWindow.height-height)/2
    }

    // The title of this slide should be the current experiment
    Text{
        id: slideTitle
        text: vmStudyTitle
        font.family: viewVRDisplay.gothamM.name
        font.pixelSize: 43*viewHome.vmScale
        color: "#297fca"
        anchors.top: vmBanner.bottom
        anchors.topMargin: mainWindow.height*0.01
        anchors.horizontalCenter: parent.horizontalCenter
    }


    QImageDisplay {
        id: hmdView
        width: parent.width*0.8;
        height: parent.height*0.7;
        anchors.top: slideTitle.bottom
        anchors.topMargin: parent.height*0.002
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Row{

        id: buttonRow
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: mainWindow.height*0.02
        spacing: mainWindow.width*0.004

        VMButton{
            id: btnStartStudy
            vmText: loader.getStringForKey(keysearch + "btnStartExperiment")
            vmSize: [mainWindow.width*0.141, mainWindow.height*0.072]
            vmInvertColors: true
            vmFont: viewVRDisplay.gothamM.name
            visible: true;
            onClicked: {
                btnCalibrate.enabled = false;
                btnStartStudy.visible = false;
                viewPresentExperimet.startNextStudy();
            }
        }

        VMButton{
            id: btnStopStudy
            vmText: loader.getStringForKey(keysearch + "btnStopExperiment")
            vmSize: [mainWindow.width*0.141, mainWindow.height*0.072]
            vmInvertColors: true
            vmFont: viewVRDisplay.gothamM.name
            visible: !btnStartStudy.visible
            onClicked: {
                btnCalibrate.enabled = true;
                //swiperControl.currentIndex = swiperControl.vmIndexPresentExperiment;
                flowControl.keyboardKeyPressed(Qt.Key_Escape); // This is the equivalent of pressing the escape key.
            }
        }

        VMButton{
            id: btnCalibrate
            vmText: loader.getStringForKey(keysearch + "btnStartCalibration")
            vmSize: [mainWindow.width*0.141, mainWindow.height*0.072]
            vmInvertColors: true
            vmFont: viewVRDisplay.gothamM.name
            onClicked: {
                btnStartStudy.enabled = false;
                btnCalibrate.enabled = false;
                if (!flowControl.isConnected()) flowControl.connectToEyeTracker();
                else flowControl.calibrateEyeTracker(viewCalibrationStart.vmSelectedEye);
            }
        }

    }

    Keys.onPressed: {
        flowControl.keyboardKeyPressed(event.key);
    }



}
