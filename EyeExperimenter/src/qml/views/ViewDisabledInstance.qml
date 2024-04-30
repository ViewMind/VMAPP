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
            if ( (loader.getLastAPIRequest() === VMGlobals.vmAPI_OPERATING_INFO) ||
                 (loader.getLastAPIRequest() === VMGlobals.vmAPI_OPERATING_INFO_AND_LOG)){

                mainWindow.closeWait()

                if (loader.instanceDisabled()){

                    var message = loader.getStringForKey("viewdisabled_recheck_failed");
                    mainWindow.popUpNotify(VMGlobals.vmNotificationRed,message,false);

                }
                else {
                    // This is important cause otherwise we will go to login when we shouldn't.
                    if (loader.getNewUpdateVersionAvailable() === ""){
                       mainWindow.swipeTo(VMGlobals.vmSwipeIndexLogin)
                    }
                }

            }
        }

    }

    Text {
        id: welcomeText
        text: loader.getStringForKey("viewdisabled_title")
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
        text: loader.getStringForKey("viewdisabled_messsage")
        font.pixelSize: VMGlobals.vmFontLarge
        font.weight: 400
        color: VMGlobals.vmGrayPlaceholderText
        anchors.left: viewmindLogo.left
        anchors.top: welcomeText.bottom
        anchors.topMargin: VMGlobals.adjustHeight(15)
    }

    VMButton {
        id: btnRecheck
        vmText: loader.getStringForKey("viewdisabled_btn_retry")
        vmCustomWidth: VMGlobals.adjustHeight(250)
        anchors.left: viewmindLogo.left
        anchors.top: instructionText.bottom
        anchors.topMargin: VMGlobals.adjustHeight(30)
        onClickSignal: {
            mainWindow.openWait(loader.getStringForKey("viewwait_synching"))
            loader.requestOperatingInfo();
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
        id: supportButton
        vmIconSource: "qrc:/images/tech_support_gray.png"
        vmIconToTheRight: false
        vmButtonType: supportButton.vmTypeTertiary
        vmText: loader.getStringForKey("viewsettings_tech_support");
        anchors.left: btnRecheck.left
        anchors.bottom: parent.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(32.75)
        onClickSignal: {
            vmSendSupportEmail.open();
            vmSendSupportEmail.setCurrentEvaluator("");
        }
    }



}
