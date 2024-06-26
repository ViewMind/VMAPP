import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"
import "../subscreens"

Item {

    id: settingsDialog;
    anchors.fill: parent
    visible: false
    clip: true
    z: 10

    property bool vmAnyChangeState: false

    signal restartRequired()
    signal requestEyeCheck();

    ////////////////////////// BASE DIALOG FUNCTIONS ////////////////////////
    function open(){
        visible = true
        advanced.reset();
    }

    function close(){
        visible = false
    }

    MouseArea {
        id: mouseCatcher
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {

        }
    }

    Rectangle {
        id: overlay
        color: VMGlobals.vmGrayDialogOverlay
        anchors.fill: parent
    }

    Rectangle {
        id: dialog
        width: VMGlobals.adjustWidth(821)
        height: VMGlobals.adjustHeight(650)
        anchors.centerIn: parent
        radius: VMGlobals.adjustWidth(10);
        color: "#ffffff"
    }
    /////////////////////////////////////////////////////////////////////

    Text {
        id: panelTitle
        text: loader.getStringForKey("viewstart_settings");
        font.pixelSize: VMGlobals.vmFontVeryLarge
        font.weight: 600
        anchors.left: dialog.left
        anchors.top: dialog.top
        anchors.topMargin: VMGlobals.adjustHeight(20);
        anchors.leftMargin: VMGlobals.adjustWidth(20);
        verticalAlignment: Text.AlignVCenter
        height: VMGlobals.adjustHeight(32)
    }

    // Right hand side panel
    Rectangle {
        id: clipRect
        clip: true
        height: dialog.height - buttonPanel.height
        width: dialog.width - VMGlobals.adjustWidth(300);
        anchors.top: dialog.top
        anchors.right: dialog.right
        radius: dialog.radius

        SwipeView {
            id: viewer
            interactive: true
            currentIndex: 0
            anchors.fill: parent

            Item {
                SCSettingsAdvanced {
                    id: advanced
                    anchors.fill: parent
                }
            }

            Item {
                SCSettingsAbout {
                    id: about
                    anchors.fill: parent
                }
            }


        }
    }

    Rectangle {
        id: verticalDivider
        height: dialog.height
        anchors.right: clipRect.left
        anchors.top: dialog.top
        width: VMGlobals.adjustWidth(1)
        border.width: 0
        color: VMGlobals.vmGrayUnselectedBorder
    }

    Rectangle {
        id: horizontalDivider
        height: verticalDivider.width
        anchors.right: clipRect.right
        anchors.bottom: buttonPanel.top
        width: buttonPanel.width
        border.width: 0
        //color: advanced.vmAnyChanges ? verticalDivider.color : advanced.color
        color: verticalDivider.color
    }


    Rectangle {
        id: buttonPanel
        width: clipRect.width
        height: VMGlobals.adjustWidth(74)
        anchors.bottom: dialog.bottom
        anchors.right: dialog.right
        border.width: 0
        color: advanced.vmAnyChanges ? dialog.color : advanced.color

        VMButton {
            id: btnCancel
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: btnApply.left
            anchors.rightMargin: VMGlobals.adjustWidth(15)
            vmText: loader.getStringForKey("viewsettings_cancel")
            vmButtonType: btnCancel.vmTypeSecondary
            //visible: advanced.vmAnyChanges
            onClickSignal: {
                close()
            }
        }

        VMButton {
            id: btnApply
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: VMGlobals.adjustWidth(20)
            //visible: advanced.vmAnyChanges
            //vmText: advanced.vmRestartRequired ? loader.getStringForKey("viewsettings_restart") : loader.getStringForKey("viewsettings_apply")
            vmText: loader.getStringForKey("viewsettings_apply")
            onClickSignal: {
                advanced.saveData();
                settingsDialog.close()
                if (advanced.vmRestartRequired){
                    restartRequired()
                }
            }
        }

    }

    // The navigation options to the left.
    VMSideNavigationBar {
        id: sideNavigationBar
        width: VMGlobals.adjustWidth(260)
        anchors.left: panelTitle.left
        anchors.top: panelTitle.bottom
        anchors.topMargin: VMGlobals.adjustHeight(20)

        Component.onCompleted: {
            var list = [];
            var item = {};

            //console.log("Getting the text for the sliding menu in settings");

            item["text"]        = loader.getStringForKey("viewsettings_advanced");
            item["icon_normal"] = "qrc:/images/sliders.png"
            item["icon_selected"] = "qrc:/images/sliders_blue.png"
            //console.log(JSON.stringify(item))
            list.push(item)
            //console.log(JSON.stringify(list))

            item = {};
            item["text"]        = loader.getStringForKey("viewsettings_about");
            item["icon_normal"] = "qrc:/images/info.png"
            item["icon_selected"] = "qrc:/images/info_blue.png"
            //console.log(JSON.stringify(item))
            list.push(item)
            //console.log(JSON.stringify(list))

            sideNavigationBar.setItems(list);
            sideNavigationBar.setItemSelected(0)
        }
        onVmCurrentIndexChanged: {
            if (sideNavigationBar.vmCurrentIndex == 0){
                advanced.vmAnyChanges = vmAnyChangeState;
            }
            else{
                vmAnyChangeState = advanced.vmAnyChanges;
                advanced.vmAnyChanges = false;
            }
            viewer.setCurrentIndex(sideNavigationBar.vmCurrentIndex)
        }
    }


    // The close button for the dialog
    VMCloseButton {
        id: closeDialog
        anchors.right: dialog.right
        anchors.top: dialog.top
        anchors.topMargin: VMGlobals.adjustHeight(29.75)
        anchors.rightMargin: VMGlobals.adjustWidth(29.75)
        onCloseSignal: {
            settingsDialog.close();
        }
    }


    VMUserManualHyperLink {
        id: userManualHyperLink
        anchors.bottom: dialog.bottom
        anchors.left: dialog.left
        anchors.bottomMargin: VMGlobals.adjustHeight(21)
        anchors.leftMargin: VMGlobals.adjustWidth(20)
        onClicked: {
            settingsDialog.close()
            userManualLangSelection.open();
        }
    }

}

