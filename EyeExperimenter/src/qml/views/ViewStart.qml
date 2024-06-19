import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"

ViewBase {

    readonly property int vmHMD_CHANGE_STATUS_OK: 0
    readonly property int vmHMD_CHANGE_STATUS_BAD_SN: 1
    readonly property int vmHMD_CHANGE_STATUS_CHANGE: 2

    Connections {
        target: loader
        function onFinishedRequest () {
            // Close the connection dialog and open the user selection dialog.
            if ( (loader.getLastAPIRequest() === VMGlobals.vmAPI_OPERATING_INFO) ||
                    (loader.getLastAPIRequest() === VMGlobals.vmAPI_OPERATING_INFO_AND_LOG)){

                mainWindow.closeWait()

                let hmd_status = loader.checkHMDChangeStatus();

                // We need to check the disabled status.
                if (loader.instanceDisabled()){
                    mainWindow.swipeTo(VMGlobals.vmSwipeIndexInstanceDisabled);
                }
                else if (hmd_status !== vmHMD_CHANGE_STATUS_OK){
                    mainWindow.swipeTo(VMGlobals.vmSwipeIndexFunctionalVerif);
                    viewFuncCtl.setIntent(viewFuncCtl.vmINTENT_HMD_CHANGE);
                }
                else {
                    if (loader.getNewUpdateVersionAvailable() !== ""){
                        requestUpdateDialog.vmVersion = loader.getNewUpdateVersionAvailable()
                        requestUpdateDialog.vmTimesRemaining = loader.getRemainingUpdateDenials();
                        //console.log("New version Available " + loader.getNewUpdateVersionAvailable())
                        requestUpdateDialog.open();
                    }
                    else {
                        mainWindow.swipeTo(VMGlobals.vmSwipeIndexLogin)
                    }
                }

            }
            else if (loader.getLastAPIRequest() === VMGlobals.vmAPI_UPDATE_REQUEST){
                // If the application got here, we move on. But it means that the update failed.
                mainWindow.popUpNotify(VMGlobals.vmNotificationRed,loader.getStringForKey("update_failed_message"));
                mainWindow.closeWait()
                mainWindow.swipeTo(VMGlobals.vmSwipeIndexLogin)
            }
            else if (loader.getLastAPIRequest() === VMGlobals.vmAPI_REQUEST_REPORT){
                if (mainWindow.getCurrentSwipeIndex() === VMGlobals.vmSwipeIndexHome){
                    // This is from uploaded old evaluations. WE need to move to login.
                    mainWindow.closeWait()
                    mainWindow.swipeTo(VMGlobals.vmSwipeIndexLogin)
                }
            }
        }

    }

    Connections {
        target: flowControl

        function onRenderServerDisconnect(){
            mainWindow.swipeTo(VMGlobals.vmSwipeIndexHome);
            var message = loader.getStringForKey("viewstart_vr_disconnect");
            mainWindow.popUpNotify(VMGlobals.vmNotificationRed,message,false);
            btnLogin.vmEnabled = false;
        }

        function onCheckOnRRSFailed() {
            criticalFailure.loadFromKey("viewstart_on_RRS_check_failed");
            criticalFailure.open()
            return;
        }

    }

    VMMessageDialog {
        id: criticalFailure
        vmLarge: true
        onDismissed: {
            messageDiag.close();
            Qt.quit()
        }
    }

    ViewChangeLog {
        id: changelogDialog
    }


    Component.onCompleted: {
        // Checking that the everything was loaded correctly.
        if (loader.getLoaderError()){
            criticalFailure.loadFromText("Detected Database Corruption","ViewMind Atlas has detected possible corruption of it's local database\nPlease contact ViewMind Support: ");
            criticalFailure.open()
            return;
        }

        if (!loader.isVMConfigPresent()){
            // VM Config is Not present. We need to go the licensce screen.
            mainWindow.swipeTo(VMGlobals.vmSwipeIndexGetVMConfig)
            return;
        }

        // If the instance is disabled we need to go to that screen.
        if (loader.instanceDisabled()){
            mainWindow.swipeTo(VMGlobals.vmSwipeIndexInstanceDisabled)
            return;
        }

        // If this is the first time running this version the changes are shown.
        if (loader.isFirstTimeRun()){
            //if (1 === 1){
            var title_and_body = loader.getLatestVersionChanges();

            let versionText = title_and_body[0];
            let titleParts = versionText.split(" -- ")
            changelogDialog.vmVersion = "<b>" + titleParts[0] + "</b> " + titleParts[1];

            let changes = title_and_body[1].split("\r\n")
            //console.log(JSON.stringify(changes));
            let bullets = [];
            for (var i = 0; i < changes.length; i++){
                let line = changes[i];
                if (line === "") continue;
                if (line === undefined) continue;
                line = line.replace("-- ","");
                bullets.push(line);
            }

            changelogDialog.setChangedItems(bullets)
            changelogDialog.open()

        }

    }


    Text {
        id: welcomeText
        text: loader.getStringForKey("viewstart_welcome")
        font.pixelSize: VMGlobals.vmFontHuge
        font.weight: 600
        color: VMGlobals.vmBlackText
        anchors.left: viewmindLogo.left
        anchors.top: viewmindLogo.bottom
        anchors.topMargin: VMGlobals.adjustHeight(100)
    }

    Text {
        id: welcomeTextTM
        text: "™"
        font.pixelSize: VMGlobals.vmFontExtraLarge
        font.weight: 400
        color: VMGlobals.vmBlackText
        anchors.left: welcomeText.right
        anchors.top: welcomeText.top
    }

    Text {
        id: cognitionText
        text: "Cognition"
        font.pixelSize: VMGlobals.vmFontHuge
        font.weight: 600
        color: VMGlobals.vmBlackText
        anchors.left: welcomeTextTM.right
        anchors.bottom: welcomeText.bottom
    }

    Text {
        id: instructionText
        text: loader.getStringForKey("viewstart_instruction")
        font.pixelSize: VMGlobals.vmFontLarge
        font.weight: 400
        color: VMGlobals.vmGrayPlaceholderText
        anchors.left: viewmindLogo.left
        anchors.top: welcomeText.bottom
        anchors.topMargin: VMGlobals.adjustHeight(15)
    }

    VMButton {
        id: btnLogin
        vmText: loader.getStringForKey("viewstart_login")
        vmCustomWidth: VMGlobals.adjustHeight(150)
        anchors.left: viewmindLogo.left
        anchors.top: instructionText.bottom
        anchors.topMargin: VMGlobals.adjustHeight(30)
        onClickSignal: {
            mainWindow.openWait(loader.getStringForKey("viewwait_synching"))
            loader.requestOperatingInfo();
        }
    }

    VMButton {
        id: btnAddNew
        vmButtonType: btnAddNew.vmTypeSecondary
        vmText: loader.getStringForKey("viewstart_addneweval")
        vmCustomWidth: VMGlobals.adjustHeight(221)
        anchors.left: btnLogin.right
        anchors.top: btnLogin.top
        anchors.leftMargin: VMGlobals.adjustWidth(20)
        onClickSignal: {
            mainWindow.swipeTo(VMGlobals.vmSwipeIndexAddEval);
        }
    }

    VMEvaluationTaskDialog {
        id: evaluationTaskDialog
    }


    // The ViewMind Logo.
    Image {
        id: viewmindLogo
        height: VMGlobals.vmControlsHeight
        fillMode: Image.PreserveAspectFit
        anchors.left: parent.left
        anchors.leftMargin: VMGlobals.adjustWidth(100)
        anchors.top: parent.top
        anchors.topMargin: VMGlobals.adjustHeight(148)
        source: "qrc:/images/logo.png"
    }

    Text {
        id: registeredCircleText
        text: "®"
        font.pixelSize: VMGlobals.vmFontLarger
        font.weight: 400
        color: VMGlobals.vmBlackText
        anchors.left: viewmindLogo.right
        anchors.top: viewmindLogo.top
    }

    // The Eye Design
    Image {
        id: viewmindEye
        width: VMGlobals.adjustWidth(558)
        fillMode: Image.PreserveAspectFit
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: VMGlobals.adjustWidth(100)
        anchors.topMargin: VMGlobals.adjustHeight(167)
        source: "qrc:/images/eye.png"
    }

    // The Settings button
    VMButton {
        id: settingsButton
        vmIconSource: "qrc:/images/Settings.png"
        vmIconToTheRight: false
        vmButtonType: settingsButton.vmTypeTertiary
        vmText: loader.getStringForKey("viewstart_settings");
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.leftMargin: VMGlobals.adjustWidth(32.53)
        anchors.bottomMargin: VMGlobals.adjustHeight(32.75)
        onClickSignal: {
            settingsDialog.open()
        }
    }

    // The Settings button
    VMButton {
        id: supportButton
        vmIconSource: "qrc:/images/tech_support_gray.png"
        vmIconToTheRight: false
        vmButtonType: settingsButton.vmTypeTertiary
        vmText: loader.getStringForKey("viewsettings_tech_support");
        anchors.left: settingsButton.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: VMGlobals.adjustWidth(10)
        anchors.bottomMargin: VMGlobals.adjustHeight(32.75)
        onClickSignal: {
            vmSendSupportEmail.open();
            vmSendSupportEmail.setCurrentEvaluator("");
        }
    }

    VMUserManualHyperLink {
        id: userManualHyperLink
        anchors.verticalCenter: supportButton.verticalCenter
        anchors.left: supportButton.right
        anchors.leftMargin: VMGlobals.adjustWidth(10)
        onClicked: {
            userManualLangSelection.open();
        }
    }

//    //     We add test stuff to main when developing components as we can test it as sson as the app starts.
//    //     I leave here the commented the last test item so hat It appears right beside the add new evaluator button.
//    VMButton {
//        id: testBox
//        vmText: "Test Stuff";
//        width: VMGlobals.adjustWidth(100)
//        anchors.left: btnAddNew.right
//        anchors.top: btnAddNew.top
//        anchors.leftMargin: 10
//        onClickSignal: {

////            mainWindow.openEvaluationTaskDialog("1_9_20240408_144816")
////            messageDiag.vmLarge = true;
////            messageDiag.loadFromKey("viewevaluation_badqci_middle_eval",true);
////            messageDiag.open(true)
//        }
//    }


}
