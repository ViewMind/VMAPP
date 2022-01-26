import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"
import "../subscreens"

Item {

    id: changelogDialog;
    anchors.fill: parent
    visible: false
    clip: true
    z: 10

    property string vmVersion: ""
    property int vmItemHeight: 0

    readonly property int vmMAX_ITEM_HEIGHT: VMGlobals.adjustHeight(200)
    readonly property int vmBASE_DIALOG_HEIGHT: VMGlobals.adjustHeight(306-71)

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
        anchors.centerIn: parent
        radius: VMGlobals.adjustWidth(10);
        color: "#ffffff"
    }

    /////////////////////////////////////////////////////////////////////

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
        vmText: loader.getStringForKey("update_close")
        anchors.right: dialog.right
        anchors.rightMargin: VMGlobals.adjustWidth(30)
        anchors.top: divider.bottom
        anchors.topMargin: VMGlobals.adjustHeight(20)
        onClickSignal: {
            close();
        }
    }

    Image {
        id: checkMark
        source: "qrc:/images/check-circle.png"
        height: VMGlobals.adjustHeight(65)
        fillMode: Image.PreserveAspectFit
        anchors.top: dialog.top
        anchors.topMargin: VMGlobals.adjustHeight(40)
        anchors.left: dialog.left
        anchors.leftMargin: VMGlobals.adjustWidth(37)
    }

    Text {
        id: title
        text: loader.getStringForKey("update_success")
        color: VMGlobals.vmBlackText
        font.weight: 600
        font.pixelSize: VMGlobals.vmFontExtraLarge
        height: VMGlobals.adjustHeight(37)
        verticalAlignment: Text.AlignVCenter
        anchors.top: dialog.top
        anchors.topMargin: VMGlobals.adjustHeight(30)
        anchors.left: checkMark.right
        anchors.leftMargin: VMGlobals.adjustWidth(30.25)
    }

    Text {
        id: version
        text: vmVersion
        color: VMGlobals.vmGrayPlaceholderText
        font.weight: 600
        font.pixelSize: VMGlobals.vmFontLarge
        height: VMGlobals.adjustHeight(21)
        verticalAlignment: Text.AlignVCenter
        anchors.top: title.bottom
        anchors.topMargin: VMGlobals.adjustHeight(15)
        anchors.left: title.left
    }

//    Rectangle {
//        anchors.fill: contentScroll
//        border.width: 1
//        border.color: "#ff0000"
//    }

    ScrollView {
        id: contentScroll
        clip: true
        width: dialog.width - VMGlobals.adjustWidth(132)
        height: vmItemHeight
        anchors.top: version.bottom
        anchors.topMargin: VMGlobals.adjustHeight(15)
        anchors.left: title.left

        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

        Text {
            id: content
            anchors.left: parent.left
            anchors.leftMargin: -VMGlobals.adjustWidth(10)
            //anchors.centerIn: parent
            text: loader.getStringForKey("update_success")
            textFormat: Text.RichText
            width: VMGlobals.adjustWidth(453)
            //implicitWidth: width
            color: VMGlobals.vmGrayPlaceholderText
            font.weight: 400
            font.pixelSize: VMGlobals.vmFontLarge
            verticalAlignment: Text.AlignTop
            wrapMode: Text.WordWrap
            //readOnly: true
        }
    }

    function setChangedItems(list){

        let html = '<ul style="padding-left: 0">';
        for (var i = 0; i < list.length; i++){
            html = html + '<li>' + list[i] + "</li>";
        }
        html = html + "</ul>"

        content.text = html;

        vmItemHeight  = (content.height > vmMAX_ITEM_HEIGHT) ? vmMAX_ITEM_HEIGHT : content.height
        dialog.height = vmBASE_DIALOG_HEIGHT + vmItemHeight

        //console.log("Content height " + content.height + " and max is "  + vmMAX_ITEM_HEIGHT + " and result is " + vmItemHeight + " and dialog is " + dialog.height)

    }

}
