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
            Qt.quit()
        }
    }

    // Calibration error dialog
    VMMessageDialog {
        id: calibrationErrorDialog
        onDismissed: {
            calibrationErrorDialog.close()
        }
//        Component.onCompleted: {
//            showCalibrationError(true,false)
//        }
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

        onCurrentIndexChanged: {
            switch (currentIndex){
            case VMGlobals.vmSwipeIndexHome:
                loader.logOut();
                break;
            case VMGlobals.vmSwipeIndexMainScreen:
                flowControl.stopRenderingVR(); // Safe place to ensure we are not reandering and gathering data ALL the time.
                viewMainSetup.enableStudyStart(true)
                if (!flowControl.isVROk()){
                    viewMainSetup.enableStudyStart(false)
                    var message = loader.getStringForKey("viewpatlist_vr_failed");
                    popUpNotify(VMGlobals.vmNotificationRed,message);
                }
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

    function popUpNotify(type,message){
        notification.show(type,message)
    }

    function showErrorMessage(key){
        messageDiag.loadFromKey(key);
        messageDiag.open();
    }

    function getCurrentSwipeIndex(){
        return swiperControl.currentIndex;
    }

    function showCalibrationError(leftEye,rightEye){
        var title = loader.getStringForKey("viewevaluation_err_calib_title");
        var body  = loader.getStringForKey("viewevaluation_err_calib_exp");

        if (leftEye) body = body.replace("llee",loader.getStringForKey("viewevaluation_err_calib_ok"))
        else body = body.replace("llee",loader.getStringForKey("viewevaluation_err_calib_fail"))

        if (rightEye) body = body.replace("rree",loader.getStringForKey("viewevaluation_err_calib_ok"))
        else body = body.replace("rree",loader.getStringForKey("viewevaluation_err_calib_fail"))

        //console.log(body)

        calibrationErrorDialog.vmTitle = title
        calibrationErrorDialog.vmText = body
        calibrationErrorDialog.vmLarge = true
        calibrationErrorDialog.vmUseRedAlert = true

        calibrationErrorDialog.open()

    }

}
