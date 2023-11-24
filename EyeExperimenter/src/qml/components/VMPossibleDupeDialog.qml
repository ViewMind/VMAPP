import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"

Item {

    id: possibleDupeDialog;
    anchors.fill: parent
    visible: false
    clip: true
    z: 10

    signal saveNewPatient()
    signal goBackToPatientList()

    ////////////////////////// BASE DIALOG FUNCTIONS ////////////////////////
    function open(){
        visible = true
    }

    function close(){
        visible = false
    }

    function setNameList (list){
        nameList.setList(list);
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
        width: VMGlobals.adjustWidth(500)
        height: VMGlobals.adjustHeight(400)
        anchors.centerIn: parent
        radius: VMGlobals.adjustWidth(10);
        color: "#ffffff"
    }

    ////////////////////////// GRAPHICAL ELEMENTS ////////////////////////

    // The close button for the dialog
    VMCloseButton {
        id: closeDialog
        anchors.right: dialog.right
        anchors.top: dialog.top
        anchors.topMargin: VMGlobals.adjustHeight(29.75)
        anchors.rightMargin: VMGlobals.adjustWidth(29.75)
        onCloseSignal: {
            close();
        }
    }

    Text {
        id: dialogTitle
        color: VMGlobals.vmBlackText
        font.weight: 600
        font.pixelSize: VMGlobals.vmFontExtraLarge
        text: loader.getStringForKey("viewpatform_dupe_title")
        height: VMGlobals.adjustHeight(37)
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.top: dialog.top
        anchors.topMargin: VMGlobals.adjustHeight(34)
        anchors.horizontalCenter: dialog.horizontalCenter
    }

    Text {
        id: dialogSubTitle
        color: VMGlobals.vmGrayLightGrayText
        font.weight: 400
        font.pixelSize: VMGlobals.vmFontBaseSize
        text: loader.getStringForKey("viewpatform_dupe_warning")
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.top: dialogTitle.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)
        anchors.horizontalCenter: dialog.horizontalCenter

    }

    VMSimpleNameList {
        id: nameList
        width: 0.94*dialog.width
        height: 0.45*dialog.height
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: dialogSubTitle.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)
    }


    /////////////////////////////////////////////////////////////////////

    Rectangle {
        id: divider
        width: dialog.width
        height: VMGlobals.adjustHeight(1)
        anchors.top: nameList.bottom
        anchors.left: dialog.left
        anchors.topMargin: VMGlobals.adjustHeight(10)
        border.width: 0
        color: VMGlobals.vmGrayDialogDivider
    }

    VMButton {
        id: btnGoBack
        vmText: loader.getStringForKey("viewpatform_dupe_back");
        vmButtonType: vmTypeSecondary
        anchors.left: dialog.left
        anchors.leftMargin: VMGlobals.adjustWidth(30)
        anchors.top: divider.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)
        onClickSignal: {
            goBackToPatientList()
        }
    }

    VMButton {
        id: btnSavePatient
        vmText: loader.getStringForKey("viewpatform_dupe_add");
        anchors.top: btnGoBack.top
        anchors.right: dialog.right
        anchors.rightMargin: VMGlobals.adjustWidth(30)
        onClickSignal: {
            saveNewPatient()
        }
    }



}
