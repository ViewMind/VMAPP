import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"

Item {
    id: genericDialog;
    anchors.fill: parent
    visible: false
    clip: true
    z: 10

    property string vmTitle: ""
    property string vmText: ""
    property bool vmLarge: false
    property bool vmUseRedAlert: false

    signal dismissed();

    ////////////////////////// BASE DIALOG FUNCTIONS ////////////////////////
    function open(){
        visible = true
    }

    function close(){
        visible = false
    }

    function loadFromKey(key){
        var list = loader.getStringListForKey(key)
        //console.log(JSON.stringify(list));
        //console.log("List length " + list.length)
        if (list.length !== 2){
            vmTitle = "Unknown"
            vmText = "Unknown"
            return
        }
        vmTitle = list[0]
        vmText = list[1]
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
        width: VMGlobals.adjustWidth(615)
        height: vmLarge ? VMGlobals.adjustHeight(308) : VMGlobals.adjustHeight(227)
        anchors.centerIn: parent
        radius: VMGlobals.adjustWidth(10);
        color: "#ffffff"
    }

    /////////////////////////////////////////////////////////////////////

    Text {
        id: dialogTitle
        color: VMGlobals.vmBlackText
        font.weight: 600
        font.pixelSize: VMGlobals.vmFontExtraLarge
        text: vmTitle
        height: VMGlobals.adjustHeight(37)
        verticalAlignment: Text.AlignVCenter
        anchors.top: dialog.top
        anchors.topMargin: VMGlobals.adjustHeight(34)
        anchors.left: infoIcon.right
        anchors.leftMargin: VMGlobals.adjustWidth(22.74)
    }

    Image {
        id: infoIcon
        source: vmUseRedAlert ? "qrc:/images/alert-triangle-red.png" : "qrc:/images/alert-triangle.png"
        height: VMGlobals.adjustHeight(67)
        fillMode: Image.PreserveAspectFit
        anchors.top: dialog.top
        anchors.topMargin: VMGlobals.adjustHeight(37.34)
        anchors.left: dialog.left
        anchors.leftMargin: VMGlobals.adjustWidth(32.74)
    }

    Text {
        text: vmText
        font.pixelSize: VMGlobals.vmFontLarge
        font.weight: 400
        color: VMGlobals.vmGrayPlaceholderText
        anchors.top: dialogTitle.bottom
        anchors.topMargin: VMGlobals.adjustHeight(15)
        anchors.left: dialogTitle.left
    }

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
        id: okButton
        vmText: "OK";
        anchors.bottom: dialog.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(20)
        anchors.right: dialog.right
        anchors.rightMargin: VMGlobals.adjustWidth(30)
        onClickSignal: {
            dismissed()
        }
    }


}
