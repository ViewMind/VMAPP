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

    Connections {
        target: loader
        function onTitleBarUpdate(){
            title = qsTr("EyeExplorer - ") + loader.getWindowTilteVersion()
        }

        function onDownloadProgressUpdate(progress, hours, minutes, seconds, bytesDowloaded, bytesTotal){
            let text = loader.getStringForKey("viewwait_download_remaining")
            let MB = Math.round(bytesDowloaded/(1024*1024))
            let MBT = Math.round(bytesTotal/(1024*1024));
            text = text.replace("<M>",Math.round(minutes));
            text = text.replace("<S>",Math.round(seconds));
            text = text.replace("<MB>",MB);
            text = text.replace("<MBT>",MBT);
            waitScreen.updateProgress(Math.ceil(progress),text);
        }
    }

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

    VMSendSupportEmail {
        id: vmSendSupportEmail
    }

    // Calibration Validation dialog
    VMCalibrationValidation {
        id: calibrationValidation
        onCalibrationValidated: {
            close();
            viewEvaluations.calibrationValidated();
        }
        onRequestReCalibration: function (slow) {
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
            messageDiag.vmLarge = false
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

        Item {
            ViewGetVMConfig {
                id: viewGetVMConfig
                onSuccessActivation: {
                    messageDiag.vmLarge = false;
                    messageDiag.loadFromKey("viewgetconfig_success")
                    messageDiag.open();
                }
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

                viewMainSetup.enableStudyStart(true)

                if (!flowControl.isRenderServerWorking()){
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
            case VMGlobals.vmSwipeIndexQCView:
                viewQC.clear();
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

    function openWaitAsProgress(message){
        waitScreen.vmText = message
        waitScreen.showWithProgress();
        waitScreen.updateProgress(0,"");
    }

    function closeWait(){
        waitScreen.hide();
    }

    function popUpNotify(type,message,showOnCenter = false){
        notification.show(type,message,showOnCenter)
    }

    function showErrorMessage(key){
        messageDiag.vmLarge = false;
        messageDiag.loadFromKey(key);
        messageDiag.open();
    }

    function getCurrentSwipeIndex(){
        return swiperControl.currentIndex;
    }

    function showCalibrationValidation(){

        var map = flowControl.getCalibrationValidationData();

        let no_data_key = "calibration_points_with_too_few_data_points";
        let calib_target_key = "calibration_target_location";
        let math_issues_key = "math_issues_for_calibration";
        let keys_to_check = [no_data_key, calib_target_key, math_issues_key];
        let do_pts_check = true;
        for (let i in keys_to_check){
            if (!(keys_to_check[i] in map)){
                do_pts_check = false
                loader.logUIMessage("WARNING: key " + keys_to_check + " in Calibration Validation Data Map. Moving on as if all ok",true);
            }
        }


        //console.log("MAP DATA FOR CALIB VALIDATION");
        //console.log(JSON.stringify(map,null,1));
        //console.log(JSON.stringify(map));

        let W = map["W"];
        let H = map["H"];

        if (do_pts_check){

            // We need to check if the total number of calibration points with no values is the same as the total number of points.
            let no_data_calib_points = map[no_data_key];
            let Nfail = no_data_calib_points.length
            let calib_target_locs = map[calib_target_key]
            let NPts  = calib_target_locs.length;
            let math_issues = map[math_issues_key];

            //console.log("NPts,NFail->" + NPts + ", " + Nfail + ". Issues: " + math_issues);

            if ((NPts === Nfail) || math_issues){
                messageDiag.vmLarge = true; // Makes it a bit taller, so that more text can fit.
                messageDiag.loadFromKey("viewevaluation_calib_suggest_retry",true);
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
