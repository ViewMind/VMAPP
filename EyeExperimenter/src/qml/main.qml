import QtQuick
import QtQuick.Controls
import QtQuick.Window
import "./components"
import "./views"
import "."

ApplicationWindow {
    id: mainWindow
    visible: true
    title: qsTr("EyeExplorer - ") + loader.getWindowTilteVersion()
    visibility: Window.Maximized

    readonly property alias vmSegoeNormal: segoeui_normal
    readonly property alias vmSegoeHeavy: segoeui_heavy
    readonly property alias vmSegoeBold: segoeui_bold

    FontLoader {
        id: segoeui_bold
        source: "qrc:/font/segoe_ui_bold.ttf"
    }

    FontLoader{
        id: segoeui_normal
        source: "qrc:/font/segoe_ui_normal.ttf"
    }

    FontLoader{
        id: segoeui_heavy
        source: "qrc:/font/segoe_ui_semibold.ttf"
    }

    Component.onCompleted: {
        flowControl.resolutionCalculations();
        // This ensures that no resizing is possible.
        minimumHeight = height;
        maximumHeight = height;
        minimumWidth = width;
        maximumWidth = width;
        VMGlobals.mainHeight = height;
        VMGlobals.mainWidth = width;
    }

    // For showing restart message.
    VMMessageDialog {
        id: messageDiag
        onDismissed: {
            messageDiag.close();
            if (vmCloseAppOnDismissed){
                Qt.quit()
            }
            else {
                messageDiag.vmCloseAppOnDismissed = true;
            }

        }
    }

    // Calibration Validation dialog
    VMCalibrationValidation {
        id: calibrationValidation
        onCalibrationValidated: {
            close();
            viewEvaluations.calibrationValidated();
        }
        onRequestReCalibration: {
            viewEvaluations.setCalibrationSpeedToSlow(slow);
            close()
        }
    }

    VMNotification {
        id: notification
    }

    // Settings is a global dialog.
    ViewSettings {
        id: settingsDialog
        onRestartRequired: {
            messageDiag.loadFromKey("viewsettings_restart_msg")
            messageDiag.open();
        }
    }

    // The wait screen
    ViewWait {
        id: waitScreen
    }

    SwipeView {

        id: swiperControl
        //currentIndex: vmIndexHome
        interactive: false
        anchors.fill: parent

        Item{
            ViewStart{
                id: viewHome
            }
        }

        Item{
            ViewLogin{
                id: viewLogin
            }
        }

        Item {
            ViewAddEvaluator {
                id: viewAddEval
            }
        }

        Item {
            ViewMainSetup {
                id: viewMainSetup
            }
        }

        Item {
            ViewAddPatient {
                id: viewAddPatient
            }
        }

        Item {
            ViewEvaluations {
                id: viewEvaluations
            }
        }

        Item {
            ViewQC {
                id: viewQC
            }
        }

        Item {
            ViewEditProtocol {
                id: viewEditProtocol
            }
        }

        onCurrentIndexChanged: {

            flowControl.setRenderWindowState(true);
            flowControl.hideRenderWindow();

            switch (currentIndex){
            case VMGlobals.vmSwipeIndexHome:
                loader.logOut();
                break;
            case VMGlobals.vmSwipeIndexMainScreen:

                flowControl.stopRenderingVR(); // Safe place to ensure we are not reandering and gathering data ALL the time.
                viewMainSetup.enableStudyStart(true)

                if (!flowControl.isVROk()){
                    viewMainSetup.enableStudyStart(false)
                    let message = loader.getStringForKey("viewpatlist_vr_failed");
                    popUpNotify(VMGlobals.vmNotificationRed,message);
                }
                else if (!loader.processingParametersArePresent()){
                    viewMainSetup.enableStudyStart(false)
                    let message = loader.getStringForKey("viewevaluation_err_no_pp");
                    popUpNotify(VMGlobals.vmNotificationRed,message);
                }

                ///console.log("Calling swipe into main due to setting the global swipe view");
                viewMainSetup.swipeIntoMain()
                break;
            case VMGlobals.vmSwipeIndexLogin:
                viewLogin.updateProfileList();
                break;
            case VMGlobals.vmSwipeIndexAddEval:
                viewAddEval.clear()
                break;
            case VMGlobals.vmSwipeIndexEvalView:
                viewEvaluations.setPatientForEvaluation()
                break;
            }
        }

    }

    function swipeTo(index){
        swiperControl.currentIndex = index;
    }

    function openWait(message){
        waitScreen.vmText = message;
        waitScreen.show()
    }

    function closeWait(){
        waitScreen.hide();
    }

    function popUpNotify(type,message,showOnCenter = false){
        notification.show(type,message,showOnCenter)
    }

    function showErrorMessage(key){
        messageDiag.loadFromKey(key);
        messageDiag.open();
    }

    function getCurrentSwipeIndex(){
        return swiperControl.currentIndex;
    }

    function showCalibrationValidation(){

        if (flowControl.autoValidateCalibration()){
            viewEvaluations.calibrationValidated();
            return;
        }

        var map = flowControl.getCalibrationValidationData();
        let W = map["W"];
        let H = map["H"];

        let no_data_key = "calibration_points_with_too_few_data_points";
        if (!(no_data_key in map)){
            console.log("WARNING: Too Low Data POints Key Not Present in Calibration Validation Data Map. Moving on as if all ok");
        }
        else {

            // We need to check what happened.
            let no_data_calib_points = map[no_data_key];
            let Nfail = no_data_calib_points.length

            if (Nfail > 0){

                // Something went wrong.
                let upper_points = [0,1,2];
                let lower_points = [6,7,8];

                // Checking if at least two points are present from the upper points.
                let upper_include = 0;
                let i = 0;
                for (i = 0; i < upper_points.length; i++){
                    if (no_data_calib_points.includes(upper_points[i])) upper_include++;
                }

                // Checking if at least two points are present from the lower points.
                let lower_include = 0;
                for (i = 0; i < upper_points.length; i++){
                    if (no_data_calib_points.includes(lower_points[i])) lower_include++;
                }

                if ((upper_include >= 2) && (lower_include === 0)){
                    // Headset is too low.
                    messageDiag.loadFromKey("viewevaluation_calib_suggest_too_low",true);
                }
                else if ((upper_include == 2) && (lower_include >= 2)){
                    // Headset is too high.
                    messageDiag.loadFromKey("viewevaluation_calib_suggest_too_high",true);
                }
                else if (Nfail === 1){
                    // Generic retry. Likely a headset issue.
                    messageDiag.loadFromKey("viewevaluation_calib_suggest_retry",true);
                }
                else {
                    // Generic bad heaset. Adjust and retry.
                    messageDiag.loadFromKey("viewevaluation_calib_suggest_adjust",true);
                }

                messageDiag.open(true) // The flag is to stop message diag from closing the applicaiton.
                return;

            }

        }

        calibrationValidation.configuringRenderingParameters(map,W,H);
        calibrationValidation.redrawCanvas()
        calibrationValidation.open()
    }

    onClosing: {
        flowControl.closeApplication();
    }

}
