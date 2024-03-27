import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"

Item {
    id: confirmationDialog;
    anchors.fill: parent
    visible: false
    clip: true
    z: 10

    signal confirm();
    signal reject();

    property string vmTitle : "TITLE"
    property string vmMessage: "MESSAGE"

    ////////////////////////// CONVERSION FUNCTIONS: 2D HMD Data to Plot Data ////////////////////////

    function askForConfirmation(title, message){
        console.log("Asking for confirmation with message '" + message + "'")
        vmTitle = title;
        vmMessage = message
        open()
    }

    ////////////////////////// BASE DIALOG FUNCTIONS ////////////////////////
    function open(){
        flowControl.hideRenderWindow();
        visible = true
    }

    function close(rejected){
        if (rejected) flowControl.showRenderWindow();
        visible = false
    }

    MouseArea {
        id: mouseCatcher
        anchors.fill: parent
        hoverEnabled: true
    }

    Rectangle {
        id: overlay
        color: VMGlobals.vmGrayDialogOverlay
        anchors.fill: parent
    }

    Rectangle {
        id: dialog
        width: VMGlobals.adjustWidth(570)
        height: VMGlobals.adjustHeight(250)
        anchors.centerIn: parent
        radius: VMGlobals.adjustWidth(10);
        color: "#ffffff"
    }

    /////////////////////////////////////////////////////////////////////

    Text {
        id: dialogTitle
        color: VMGlobals.vmBlackText
        font.weight: 600
        font.pixelSize: VMGlobals.vmFontVeryLarge
        text: vmTitle
        height: VMGlobals.adjustHeight(32)
        verticalAlignment: Text.AlignVCenter
        anchors.top: dialog.top
        anchors.topMargin: VMGlobals.adjustHeight(30)
        anchors.left: dialog.left
        anchors.leftMargin: VMGlobals.adjustWidth(30)
    }

    Text {
        id: dialotMessage
        color: VMGlobals.vmBlackText
        textFormat: Text.RichText
        text: vmMessage
        font.weight: 400
        font.pixelSize: VMGlobals.vmFontLarge
        anchors.top: dialogTitle.bottom
        anchors.topMargin: VMGlobals.adjustHeight(20)
        anchors.left: dialogTitle.left
    }

    /////////////////////////////////////////////////////////////////////

    Rectangle {
        id: divider
        width: dialog.width
        height: VMGlobals.adjustHeight(1)
        anchors.bottom: dialog.bottom
        anchors.left: dialog.left
        anchors.bottomMargin: VMGlobals.adjustHeight(84) - height
        border.width: 0
        color: VMGlobals.vmGrayDialogDivider
    }

    VMButton {
        id: btnYes
        vmButtonType: vmTypeSecondary
        vmText: loader.getStringForKey("viewevaluation_comfirm_abort_yes");
        anchors.bottom: dialog.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(20)
        anchors.right: dialog.right
        anchors.rightMargin: VMGlobals.adjustWidth(30)
        onClickSignal: {
            close(false)
            confirm()
        }
    }

    VMButton {
        id: btnNo
        vmText: loader.getStringForKey("viewevaluation_comfirm_abort_no")
        anchors.top: btnYes.top
        anchors.left: dialog.left
        anchors.leftMargin: VMGlobals.adjustWidth(30)
        onClickSignal: {
            close(true)
            reject()
        }
    }



}
