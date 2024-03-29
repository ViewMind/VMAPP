import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"

ViewBase {

    Connections {
        target: loader
        function onFinishedRequest () {
            // Close the connection dialog and open the user selection dialog.
            //console.log("Finished request")
            if ( (loader.getLastAPIRequest() === VMGlobals.vmAPI_OPINFO_REQUEST) ||
                 (loader.getLastAPIRequest() === VMGlobals.vmAPI_OPERATING_INFO_AND_LOG)){
                //console.log("Last request was an OP INFO call")
                mainWindow.closeWait()

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
            else if (loader.getLastAPIRequest() === VMGlobals.vmAPI_UPDATE_REQUEST){
                // If the application got here, we move on. But it means that the update failed.
                mainWindow.popUpNotify(VMGlobals.vmNotificationRed,loader.getStringForKey("update_failed_message"));
                mainWindow.closeWait()
                mainWindow.swipeTo(VMGlobals.vmSwipeIndexLogin)
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
    }


    VMMessageDialog {
        id: criticalFailure
        onDismissed: {
            messageDiag.close();
            Qt.quit()
        }
    }

    ViewChangeLog {
        id: changelogDialog
    }

    ViewRequestUpdate{
        id: requestUpdateDialog
    }

    Component.onCompleted: {
        // Checking that the everything was loaded correctly.
        if (loader.getLoaderError()){
            criticalFailure.loadFromKey("viewstart_configuration_failed");
            criticalFailure.open()
            return;
        }

        if (!loader.isVMConfigPresent()){
            // VM Config is Not present. We need to go the licensce screent.
            mainWindow.swipeTo(VMGlobals.vmSwipeIndexGetVMConfig)
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
            loader.requestOperatingInfo(false);
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

// We add test stuff to main when developing components as we can test it as sson as the app starts.
// I leave here the commented the last test item so hat It appears right beside the add new evaluator button.
//        Text {
//            id: testBox
//            width: VMGlobals.adjustWidth(100)
//            anchors.left: btnAddNew.right
//            anchors.top: btnAddNew.top
//            anchors.leftMargin: 10
//            linkColor: "#0000aa"
//            text: "Hello. This is a <a href='www.viewmind.com'>Link</a> to I don't know where"
//            onLinkActivated: {
//                console.log("Link clicked");
//            }
//        }




}
