import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"

ViewBase {

    id: addEditProtocolView

    property string vmCurrentlyLoadedProtocol: ""

    function clear(){
        protocolName.clear()
        protocolID.clear()
        protocolID.vmEnabled = true;
        vmCurrentlyLoadedProtocol = ""
    }

    function loadProtocol(id,name){

        clear()

        vmCurrentlyLoadedProtocol = id;

        protocolName.setText(name)
        protocolID.setText(id)
        protocolID.vmEnabled = false;

    }

    function checkAndSave(){

        if (protocolName.vmCurrentText === ""){
            protocolName.vmErrorMsg = loader.getStringForKey("viewpatform_cannotbeempty")
            return;
        }

        if (protocolName.vmCurrentText.length > 30){
            protocolName.vmErrorMsg = loader.getStringForKey("protocol_name_too_long");
            return;
        }

        if (protocolID.vmCurrentText === ""){
            protocolID.vmErrorMsg = loader.getStringForKey("viewpatform_cannotbeempty")
            return;
        }

        if (vmCurrentlyLoadedProtocol != ""){
            loader.editProtocol(vmCurrentlyLoadedProtocol,protocolName.vmCurrentText)
        }
        else {
            if (!loader.addProtocol(protocolName.vmCurrentText,protocolID.vmCurrentText)){
                protocolID.vmErrorMsg = loader.getStringForKey("protocol_err_exists")
                return;
            }
        }

        var message = "";
        if (vmCurrentlyLoadedProtocol == "")  message = loader.getStringForKey("protocol_successful_created");
        else message = loader.getStringForKey("protocol_successful_edit");
        message = message.replace("<b></b>","<b>" + protocolName.vmCurrentText + "</b>")
        //viewMainSetup.showProtocols(false)
        mainWindow.popUpNotify(VMGlobals.vmNotificationGreen,message);
        mainWindow.swipeTo(VMGlobals.vmSwipeIndexMainScreen)

    }

    VMButton {
        id: backButton
        vmText: loader.getStringForKey("viewlogin_back")
        vmIconSource: "arrow"
        vmButtonType: backButton.vmTypeTertiary
        anchors.bottom: parent.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(592)
        anchors.left: parent.left
        anchors.leftMargin: VMGlobals.adjustWidth(5)
        onClickSignal: {
            mainWindow.swipeTo(VMGlobals.vmSwipeIndexMainScreen)
        }
    }

    Rectangle {
        property double vmBorderPadding: VMGlobals.adjustWidth(1)
        id: mainRect
        clip: true
        radius: VMGlobals.adjustHeight(10)
        width: VMGlobals.adjustWidth(969);
        height: VMGlobals.adjustHeight(670+10);
        anchors.right: parent.right
        anchors.rightMargin: VMGlobals.adjustWidth(15)
        anchors.bottom: parent.bottom
        anchors.bottomMargin: -radius
        border.width: vmBorderPadding
        border.color: VMGlobals.vmGrayUnselectedBorder

        Text {
            id: title
            text: (vmCurrentlyLoadedProtocol == "") ? loader.getStringForKey("protocol_add") : loader.getStringForKey("protocol_edit")
            color: VMGlobals.vmBlackText
            height: VMGlobals.adjustHeight(42)
            font.pixelSize: VMGlobals.vmFontExtraExtraLarge
            font.weight: 600
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: VMGlobals.adjustWidth(30)
            anchors.topMargin: VMGlobals.adjustHeight(31)
        }


        Column {
            id: formColumn
            spacing: VMGlobals.adjustHeight(54)
            width: VMGlobals.adjustWidth(340)
            anchors.top: title.bottom
            anchors.topMargin: VMGlobals.adjustHeight(44)
            anchors.left: title.left

            VMTextInput {
                id: protocolName
                width: parent.width
                vmLabel: loader.getStringForKey("viewpatlist_name")
                vmPlaceHolderText: loader.getStringForKey("protocol_name_ph")
                Keys.onTabPressed: protocolID.vmFocus = true
            }

            VMTextInput {
                id: protocolID
                width: parent.width
                vmLabel: loader.getStringForKey("protocol_id")
                vmPlaceHolderText: loader.getStringForKey("protocol_id_ph")
                Keys.onTabPressed: protocolName.vmFocus = true
            }

        }

        Rectangle {
            id: horizontalDivider
            height: VMGlobals.adjustHeight(1)
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.bottomMargin: VMGlobals.adjustHeight(84)
            width: parent.width
            border.width: 0
            color: VMGlobals.vmGrayUnselectedBorder
        }

        VMButton {
            id: saveButton
            vmText: loader.getStringForKey("viewaccount_save")
            vmCustomWidth: VMGlobals.adjustWidth(120)
            anchors.top: horizontalDivider.top
            anchors.topMargin: VMGlobals.adjustHeight(20)
            anchors.right: parent.right
            anchors.rightMargin: VMGlobals.adjustWidth(29)
            onClickSignal: {
                checkAndSave()
            }
        }

        VMButton {
            id: cancelButton
            vmText: loader.getStringForKey("viewsettings_cancel")
            vmButtonType: cancelButton.vmTypeSecondary
            vmCustomWidth: VMGlobals.adjustWidth(120)
            anchors.top: saveButton.top
            anchors.right: saveButton.left
            anchors.rightMargin: VMGlobals.adjustWidth(20)
            onClickSignal: {
                mainWindow.swipeTo(VMGlobals.vmSwipeIndexMainScreen)
            }
        }

    }

}
