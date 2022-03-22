import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"
import "../subscreens"

Item {

    id: notificationUpdate;
    anchors.fill: parent
    visible: false
    clip: true
    z: 10

    property int vmTimesRemaining: 0
    property string vmVersion: ""

    ////////////////////////// BASE DIALOG FUNCTIONS ////////////////////////
    function open(){

        let text = loader.getStringForKey("update_message");
        text = text.replace("<<N>>",vmVersion)
        text = text.replace("<<M>>",vmTimesRemaining)
        message.text = text

        visible = true
    }

    function close(){
        visible = false
    }

    MouseArea {
        id: mouseCatcher
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            //console.log("Mouse catcher");
        }
    }

    Rectangle {
        id: overlay
        color: VMGlobals.vmGrayDialogOverlay
        anchors.fill: parent
    }

    Rectangle {
        id: dialog
        width: VMGlobals.adjustWidth(615)
        height: VMGlobals.adjustHeight(285)
        anchors.centerIn: parent
        radius: VMGlobals.adjustWidth(10);
        color: "#ffffff"
    }

    /////////////////////////////////////////////////////////////////////

    Image {
        id: icon
        source: "qrc:/images/icon.png"
        height: VMGlobals.adjustHeight(82)
        fillMode: Image.PreserveAspectFit
        anchors.top: dialog.top
        anchors.topMargin: VMGlobals.adjustHeight(30)
        anchors.left: dialog.left
        anchors.leftMargin: VMGlobals.adjustWidth(30)

    }

    Text {
        id: title
        text: loader.getStringForKey("update_title")
        color: VMGlobals.vmBlackText
        font.weight: 600
        font.pixelSize: VMGlobals.vmFontExtraLarge
        height: VMGlobals.adjustHeight(37)
        verticalAlignment: Text.AlignVCenter
        anchors.top: dialog.top
        anchors.topMargin: VMGlobals.adjustHeight(34)
        anchors.left: icon.right
        anchors.leftMargin: VMGlobals.adjustWidth(20)
    }

    Text {
        id: message
        color: VMGlobals.vmGrayPlaceholderText
        font.weight: 400
        font.pixelSize: VMGlobals.vmFontLarge
        height: VMGlobals.adjustHeight(84)
        verticalAlignment: Text.AlignVCenter
        anchors.top: title.bottom
        anchors.topMargin: VMGlobals.adjustHeight(15)
        anchors.left: title.left
    }


    // The footer divider
    Rectangle {
        id: divider
        width: dialog.width
        height: VMGlobals.adjustHeight(1)
        color: VMGlobals.vmGrayUnselectedBorder
        anchors.right: dialog.right
        anchors.bottom: dialog.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(86)
    }

    VMButton {
        id: updatenow
        vmText: loader.getStringForKey("update_now")
        anchors.right: dialog.right
        anchors.rightMargin: VMGlobals.adjustWidth(30)
        anchors.top: divider.bottom
        anchors.topMargin: VMGlobals.adjustHeight(20)
        onClickSignal: {
            close();
            mainWindow.openWait(loader.getStringForKey("update_download_wait"));
            loader.startUpdate();
        }
    }

    VMButton {
        id: remindme
        vmText: loader.getStringForKey("update_later")
        vmButtonType: remindme.vmTypeSecondary
        anchors.right: updatenow.left
        anchors.rightMargin: VMGlobals.adjustWidth(14)
        anchors.top: updatenow.top
        vmEnabled: vmTimesRemaining > 0
        onClickSignal: {
            loader.updateDenied();
            close();
            mainWindow.swipeTo(VMGlobals.vmSwipeIndexLogin)
        }
    }

}
