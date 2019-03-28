import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.1

Dialog {

    readonly property string keybase: "viewsettings_"
    property bool vmInvalidRepoError: false;
    property string vmLoadLanguage: "";
    property string vmDefaultCountry: "";
    property bool vmOfflineModeStatus: false;
    property bool vmRestartRequired: false;

    // Indexes for the list and to identify the eys.
    readonly property int vmEYE_L:    0
    readonly property int vmEYE_R:    1
    readonly property int vmEYE_BOTH: 2

    signal updateMenus();

    id: viewSettings
    modal: true
    width: 614
    height: 560
    closePolicy: Popup.NoAutoClose

    MouseArea {
        anchors.fill: parent
        onClicked: {
            focus = true
        }
    }

    contentItem: Rectangle {
        id: rectDialog
        anchors.fill: parent
        layer.enabled: true
        layer.effect: DropShadow{
            radius: 5
        }
    }


    VMDefines{
        id: vmDefines
    }

    onOpened: {
        if (loader.getViewAllFlag()) diagBtnEnableViewAll.vmText = loader.getStringForKey(keybase+"diagDisableViewAll");
        else diagBtnEnableViewAll.vmText = loader.getStringForKey(keybase+"diagEnableViewAll");
        instPassword.setText("");
        instPassword.vmErrorMsg = "";
    }

    // The configure settings title
    Text {
        id: diagTitle
        font.pixelSize: 18
        font.family: viewHome.gothamB.name
        color: "#297FCA"
        text: loader.getStringForKey(keybase+"diagTitle");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 70
    }

    // Creating the close button
    VMDialogCloseButton {
        id: btnClose
        anchors.top: parent.top
        anchors.topMargin: 22
        anchors.right: parent.right
        anchors.rightMargin: 25
        onClicked: {
            // The dialog can be closed ONLY if the the output repo is valid.
            if (!vmInvalidRepoError) viewSettings.close()
        }
    }

    // Combo box for selecting the default country.
    Text {
        id: diagLabelDefaultCountry
        text: loader.getStringForKey(keybase+"diagLabelDefaultCountry");
        font.family: viewHome.robotoB.name
        font.pixelSize: 13
        font.bold: true
        anchors.top: diagTitle.bottom
        anchors.topMargin: 25
        anchors.left: diagDBDefaultCountry.left
    }

    VMAutoCompleteComboBox{
        id: diagDBDefaultCountry
        width: 440
        height: 30
        z: 1
        vmList: loader.getCountryList()
        vmValues: loader.getCountryCodeList()
        anchors.top: diagLabelDefaultCountry.bottom
        anchors.topMargin: 12
        anchors.horizontalCenter: parent.horizontalCenter
        onVmValuesChanged:diagDBDefaultCountry.setCurrentIndex(loader.getDefaultCountry(false))
        onVmListChanged: diagDBDefaultCountry.setCurrentIndex(loader.getDefaultCountry(false))
        Component.onCompleted: {
            vmDefaultCountry = loader.getCountryCodeForCountry(diagDBDefaultCountry.vmCurrentText)
        }
    }

    // Combo box for language selection and label
    Text {
        id: diagLabelLang
        text: loader.getStringForKey(keybase+"diagLabelLang");
        font.family: viewHome.robotoB.name
        font.pixelSize: 13
        font.bold: true
        anchors.top: diagDBDefaultCountry.bottom
        anchors.topMargin: 25
        anchors.left: diagDBDefaultCountry.left
    }

    VMComboBox{
        id: diagCBLang
        vmModel: ["English", "Spanish"]
        currentIndex: {
            var lang = loader.getConfigurationString(vmDefines.vmCONFIG_REPORT_LANGUAGE)
            for (var i = 0; i < vmModel.length; i++){
                if (vmModel[i] === lang){
                    vmLoadLanguage = lang;
                    return i;
                }
            }
            return 0;
        }
        font.family: viewHome.robotoR.name
        font.pixelSize: 13
        anchors.top: diagLabelLang.bottom
        anchors.topMargin: 15
        anchors.left: diagDBDefaultCountry.left
    }

    // Combo box for eyetracker selection and label
    Text {
        id: diagLabelET
        text: loader.getStringForKey(keybase+"diagLabelET");
        font.family: viewHome.robotoB.name
        font.pixelSize: 13
        font.bold: true
        anchors.top: diagCBLang.bottom
        anchors.topMargin: 25
        anchors.left: diagDBDefaultCountry.left
    }

    // The double monitor, demo mode and use mouse selections

    VMCheckBox{
        id: diagCboxDemo
        text: loader.getStringForKey(keybase+"diagCboxDemo");
        font.family: viewHome.robotoR.name
        font.pixelSize: 13
        anchors.top: diagLabelET.bottom
        anchors.topMargin: 20
        anchors.left: diagDBDefaultCountry.left
        checked: loader.getConfigurationBoolean(vmDefines.vmCONFIG_DEMO_MODE);
    }

    VMCheckBox{
        id: diagCboxDualMonitor
        text: loader.getStringForKey(keybase+"diagCboxDualMonitor");
        font.family: viewHome.robotoR.name
        font.pixelSize: 13
        anchors.top: diagCboxDemo.top
        anchors.left: diagCboxUseMouse.right
        anchors.leftMargin: 20
        visible: false
        checked: loader.getConfigurationBoolean(vmDefines.vmCONFIG_DUAL_MONITOR_MODE);        
    }

    VMCheckBox{
        id: diagCboxUseMouse
        text: loader.getStringForKey(keybase+"diagCboxUseMouse");
        font.family: viewHome.robotoR.name
        font.pixelSize: 13
        anchors.top: diagCboxDemo.top
        anchors.left: diagCboxDemo.right
        anchors.leftMargin: 20
        checked: loader.getConfigurationBoolean(vmDefines.vmCONFIG_USE_MOUSE)
        visible: true
    }

    Text {
        id: diagLabelEnableViewAll
        text: loader.getStringForKey(keybase+"diagLabelEnableViewAll");
        font.family: viewHome.robotoB.name
        font.pixelSize: 13
        font.bold: true
        anchors.top: diagCboxUseMouse.bottom
        anchors.topMargin: 20
        anchors.left: diagDBDefaultCountry.left
    }

    Row{
        id: viewAllEnableRow
        anchors.left: diagCBLang.left
        anchors.top: diagLabelEnableViewAll.bottom
        anchors.topMargin: 20
        spacing: 10

        VMPasswordField{
            id: instPassword
            vmLabelText: loader.getStringForKey(keybase+"diagInstPassword");
            width: 150
        }

        VMButton{
            id: diagBtnEnableViewAll
            vmFont: viewHome.gothamM.name
            vmSize: [250, 30]
            vmText: ""
            onClicked: {

                if (loader.verifyInstitutionPassword(instPassword.getText())){
                    var current = loader.getViewAllFlag();
                    loader.setViewAllFlag(!current)
                    viewSettings.close();
                }
                else{
                    instPassword.vmErrorMsg = loader.getStringForKey(keybase + "diagWrongPass")
                }

            }
            anchors.bottom: instPassword.bottom
        }

    }

    VMButton{
        id: diagBtnOK
        vmFont: viewHome.gothamM.name
        vmSize: [120, 49]
        vmText: loader.getStringForKey(keybase+"diagBtnOK");
        anchors.top: viewAllEnableRow.bottom
        anchors.topMargin: 45
        anchors.left: diagDBDefaultCountry.left
        onClicked: {
            if (checkAllOk()) close();
            if (vmRestartRequired){
                restartRequiredDiag.open();
            }
        }
    }

    MessageDialog {
        id: restartRequiredDiag
        onAccepted: {
            Qt.quit()
        }
        Component.onCompleted: {
            var titleMsg = viewHome.getErrorTitleAndMessage("msg_change_lang");
            restartRequiredDiag.text = titleMsg[1];
            restartRequiredDiag.title = titleMsg[0];
        }
    }

    // Checks that everything is ok
    function checkAllOk(save){

        // Saving the settings.
        var newCountry = loader.getCountryCodeForCountry(diagDBDefaultCountry.vmCurrentText);
        loader.setSettingsValue(vmDefines.vmCONFIG_REPORT_LANGUAGE,diagCBLang.currentText);
        loader.setSettingsValue(vmDefines.vmCONFIG_DUAL_MONITOR_MODE,diagCboxDualMonitor.checked);
        loader.setSettingsValue(vmDefines.vmCONFIG_DEMO_MODE,diagCboxDemo.checked);
        loader.setSettingsValue(vmDefines.vmCONFIG_DEFAULT_COUNTRY,newCountry);
        loader.setSettingsValue(vmDefines.vmCONFIG_USE_MOUSE,diagCboxUseMouse.checked);

        // Signal to update dropdown menu information.
        updateMenus();

        // Restart required
        if ((vmLoadLanguage !== diagCBLang.currentText) || (vmDefaultCountry !== newCountry)){
            viewSettings.vmRestartRequired = true;
        }
        else{
            // Check if et changed.
            if (loader.checkETChange()){
                flowControl.eyeTrackerChanged()
            }
        }

        return true;
    }

}
