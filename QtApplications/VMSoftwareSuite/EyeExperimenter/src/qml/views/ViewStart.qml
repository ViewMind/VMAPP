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
            if (loader.getLastAPIRequest() === VMGlobals.vmAPI_OPINFO_REQUEST){
                //console.log("Last request was an OP INFO call")
                mainWindow.closeWait()

                if (loader.getNewUpdateVersionAvailable() !== ""){
                    console.log("New version Available " + loader.getNewUpdateVersionAvailable())
                }
                else {
                    mainWindow.swipeTo(VMGlobals.vmSwipeIndexLogin)
                }
            }
            else if (loader.getLastAPIRequest() === VMGlobals.vmAPI_UPDATE_REQUEST){
                // If the application got here, we move on. But it means that the update failed.
                mainWindow.closeWait()
                mainWindow.swipeTo(VMGlobals.vmSwipeIndexLogin)
            }
        }

        function onPartnerSequenceDone() {
            mainWindow.closeWait();
            if (!allok){
                console.log("ERROR: Partener sequence failed");
            }
        }
    }

    Component.onCompleted: {
        // Checking that the everything was loaded correctly.
        if (loader.getLoaderError()){
            console.log("ERROR loading the configuration. Dialog Missing");
        }

        // If this is the first time running this version the changes are shown.
        if (loader.isFirstTimeRun()){
            var title_and_body = loader.getLatestVersionChanges();
            console.log("Showing ChangeLog for version " + title_and_body[0])
            console.log(title_and_body[1])
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

}
