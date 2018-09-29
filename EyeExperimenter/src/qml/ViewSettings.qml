import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.1

Dialog {

    readonly property string keybase: "viewsettings_"
    property bool vmInvalidRepoError: false;
    property string vmLoadLanguage: "";
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
    height: 580
    closePolicy: Popup.NoAutoClose

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

    function checkOutputRepo(location){
        var dirLoc = loader.hasValidOutputRepo(location);
        if (dirLoc === ""){
            diagTIRepo.vmErrorMsg = loader.getStringForKey(keybase+"error_invalid_repo");
            vmInvalidRepoError = true;
        }
        else {
            diagTIRepo.vmTextField.text = dirLoc;
            diagTIRepo.vmTextField.ensureVisible(0);
            vmInvalidRepoError = false;
        }
    }

    // The file selector dialog
    FileDialog {
        id: fileDialog
        title: loader.getStringForKey(keybase+"error_invalid_repo");
        folder: diagTIRepo.vmTextField.text
        selectExisting: true
        selectFolder: true
        selectMultiple: false
        onAccepted: {
            checkOutputRepo(fileDialog.fileUrl)
            close();
        }
        onRejected: {
            checkOutputRepo("");
            close();
        }
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
        anchors.topMargin: 50
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

    // Doctor's label and input field
    Text {
        id: diagLabelRepo
        text: loader.getStringForKey(keybase+"diagLabelRepo");
        font.family: viewHome.robotoB.name
        font.pixelSize: 13
        font.bold: true
        anchors.top: diagTitle.bottom
        anchors.topMargin: 45
        anchors.left: diagTIRepo.left
    }

    VMConfigTextInput {
        id: diagTIRepo
        vmFont: viewHome.robotoR.name
        vmAcceptInput: false
        width: 500
        height: 20
        anchors.top: diagLabelRepo.bottom
        anchors.topMargin: 15
        anchors.horizontalCenter: parent.horizontalCenter
        vmErrorMsg: {
            if(vmInvalidRepoError){
                return loader.getStringForKey(keybase+"error_invalid_repo")
            }
            else{
                return "";
            }
        }
        vmTextField.text: loader.getConfigurationString(vmDefines.vmCONFIG_OUTPUT_DIR);
        onGotClicked: {
            fileDialog.open();
        }
    }

    // Combo box for selecting the default country.
    Text {
        id: diagLabelDefaultCountry
        text: loader.getStringForKey(keybase+"diagLabelDefaultCountry");
        font.family: viewHome.robotoB.name
        font.pixelSize: 13
        font.bold: true
        anchors.top: diagTIRepo.bottom
        anchors.topMargin: 25
        anchors.left: diagTIRepo.left
    }

    VMComboBox{
        id: diagDBDefaultCountry
        width: 440
        vmModel: {
            var data = loader.getCountryList();
            return data;
        }
        currentIndex: loader.getDefaultCountry(false);
        font.family: viewHome.robotoR.name
        font.pixelSize: 13
        anchors.top: diagLabelDefaultCountry.bottom
        anchors.topMargin: 22
        anchors.left: diagTIRepo.left
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
        anchors.left: diagTIRepo.left
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
        anchors.left: diagTIRepo.left
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
        anchors.left: diagTIRepo.left
    }

    // The double monitor, demo mode and use mouse selections

    VMCheckBox{
        id: diagCboxDemo
        text: loader.getStringForKey(keybase+"diagCboxDemo");
        font.family: viewHome.robotoR.name
        font.pixelSize: 13
        anchors.top: diagLabelET.bottom
        anchors.topMargin: 20
        anchors.left: diagTIRepo.left
        checked: loader.getConfigurationBoolean(vmDefines.vmCONFIG_DEMO_MODE);
    }

    VMCheckBox{
        id: diagCboxDualMonitor
        text: loader.getStringForKey(keybase+"diagCboxDualMonitor");
        font.family: viewHome.robotoR.name
        font.pixelSize: 13
        anchors.top: diagCboxDemo.top
        anchors.left: diagCboxDemo.right
        anchors.leftMargin: 20
        checked: loader.getConfigurationBoolean(vmDefines.vmCONFIG_DUAL_MONITOR_MODE);
    }

    VMCheckBox{
        id: diagCboxUseMouse
        text: loader.getStringForKey(keybase+"diagCboxUseMouse");
        font.family: viewHome.robotoR.name
        font.pixelSize: 13
        anchors.top: diagCboxDemo.top
        anchors.left: diagCboxDualMonitor.right
        anchors.leftMargin: 20
        checked: loader.getConfigurationBoolean(vmDefines.vmCONFIG_USE_MOUSE)
    }

    VMButton{
        id: diagBtnOK
        vmFont: viewHome.gothamM.name
        vmSize: [120, 49]
        vmText: loader.getStringForKey(keybase+"diagBtnOK");
        anchors.top: diagCboxDualMonitor.bottom
        anchors.topMargin: 45
        anchors.left: diagTIRepo.left
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
        if (vmInvalidRepoError){
            diagTIRepo.vmErrorMsg = loader.getStringForKey(keybase+"error_invalid_repo");
            return false;
        }

        // Saving the settings.
        loader.setSettingsValue(vmDefines.vmCONFIG_OUTPUT_DIR,diagTIRepo.vmTextField.text);        
        loader.setSettingsValue(vmDefines.vmCONFIG_REPORT_LANGUAGE,diagCBLang.currentText);
        loader.setSettingsValue(vmDefines.vmCONFIG_DUAL_MONITOR_MODE,diagCboxDualMonitor.checked);
        loader.setSettingsValue(vmDefines.vmCONFIG_DEMO_MODE,diagCboxDemo.checked);
        loader.setSettingsValue(vmDefines.vmCONFIG_DEFAULT_COUNTRY,loader.getCountryCodeForCountry(diagDBDefaultCountry.currentText));
        loader.setSettingsValue(vmDefines.vmCONFIG_USE_MOUSE,diagCboxUseMouse.checked);

        // Signal to update dropdown menu information.
        updateMenus();

        // Restart required
        if (vmLoadLanguage !== diagCBLang.currentText){
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
