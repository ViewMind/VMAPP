import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"

Item {

    id: userManualDialogSelect;
    anchors.fill: parent
    visible: false
    clip: true
    z: 10

    ////////////////////////// BASE DIALOG FUNCTIONS ////////////////////////
    function open(){
        visible = true
    }

    function close(){
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
        width: VMGlobals.adjustWidth(500)
        height: VMGlobals.adjustHeight(200)
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
        font.pixelSize: VMGlobals.vmFontLarger
        text: loader.getStringForKey("viewstart_umanual_lang_select")
        height: VMGlobals.adjustHeight(37)
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.top: dialog.top
        anchors.topMargin: VMGlobals.adjustHeight(34)
        anchors.horizontalCenter: dialog.horizontalCenter
        //anchors.left: dialog.left
        //anchors.leftMargin: VMGlobals.adjustWidth(15);
    }

    Column {

        id: linkColumns
        //anchors.left: dialogTitle.left
        anchors.top: dialogTitle.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)
        anchors.horizontalCenter: dialog.horizontalCenter
        spacing: VMGlobals.adjustHeight(10);

        VMUserManualHyperLink {
            id: english
            vmHyperLinkText: "English"
            anchors.horizontalCenter: linkColumns.horizontalCenter
            onClicked: {
                loader.openUserManual("en");
                close();
            }
        }

        VMUserManualHyperLink {
            id: spanish
            vmHyperLinkText: "Español"
            anchors.horizontalCenter: linkColumns.horizontalCenter
            onClicked: {
                loader.openUserManual("es");
                close();
            }
        }

        VMUserManualHyperLink {
            id: portuguese
            vmHyperLinkText: "Português"
            anchors.horizontalCenter: linkColumns.horizontalCenter
            onClicked: {
                loader.openUserManual("pr");
                close();
            }
        }

    }



}
