import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.1

Dialog {

    readonly property string keybase: "viewsettings_"
    property bool vmInvalidRepoError: false;
    property string vmLoadLanguage: "";
    property string vmLoadET: "";
    property bool vmRestartRequired: false;

    // Indexes for the list and to identify the eys.
    readonly property int vmEYE_L:    0
    readonly property int vmEYE_R:    1
    readonly property int vmEYE_BOTH: 2

    signal updateMenus();

    id: viewSettings
    modal: true
    width: 614
    height: 680
    closePolicy: Popup.CloseOnEscape

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
        anchors.left: parent.left
        anchors.leftMargin: 150
    }

    VMConfigTextInput {
        id: diagTIRepo
        vmFont: viewHome.robotoR.name
        vmAcceptInput: false
        width: 314
        height: 20
        anchors.bottom: diagLabelRepo.bottom
        anchors.bottomMargin: -35
        anchors.left: parent.left
        anchors.leftMargin: 150
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

    // Doctor's label and input field
    Text {
        id: diagLabelDoctor
        text: loader.getStringForKey(keybase+"diagLabelDoctor");
        font.family: viewHome.robotoB.name
        font.pixelSize: 13
        font.bold: true
        anchors.top: diagTIRepo.bottom
        anchors.topMargin: 45
        anchors.left: parent.left
        anchors.leftMargin: 150
    }

    VMConfigTextInput {
        id: diagTIDoctor
        vmTextField.text: loader.getConfigurationString(vmDefines.vmCONFIG_DOCTOR_NAME);
        vmFont: viewHome.robotoR.name
        width: 314
        height: 20
        anchors.bottom: diagLabelDoctor.bottom
        anchors.bottomMargin: -35
        anchors.left: parent.left
        anchors.leftMargin: 150
    }


    // Doctor's label and input field
    Text {
        id: diagLabelEmail
        text: loader.getStringForKey(keybase+"diagLabelEmail");
        font.family: viewHome.robotoB.name
        font.pixelSize: 13
        font.bold: true
        anchors.top: diagTIDoctor.bottom
        anchors.topMargin: 35
        anchors.left: parent.left
        anchors.leftMargin: 150
    }

    VMConfigTextInput {
        id: diagTIEmail
        vmFont: viewHome.robotoR.name
        vmTextField.text: loader.getConfigurationString(vmDefines.vmCONFIG_DOCTOR_EMAIL);
        width: 314
        height: 20
        anchors.top: diagLabelEmail.bottom
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 150
    }

    // Combo box for language selection and label
    Text {
        id: diagLabelLang
        text: loader.getStringForKey(keybase+"diagLabelLang");
        font.family: viewHome.robotoB.name
        font.pixelSize: 13
        font.bold: true
        anchors.top: diagTIEmail.bottom
        anchors.topMargin: 38
        anchors.left: parent.left
        anchors.leftMargin: 150
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
        anchors.topMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 150
    }

    // Combo box for eyetracker selection and label
    Text {
        id: diagLabelET
        text: loader.getStringForKey(keybase+"diagLabelET");
        font.family: viewHome.robotoB.name
        font.pixelSize: 13
        font.bold: true
        anchors.top: diagCBLang.bottom
        anchors.topMargin: 38
        anchors.left: parent.left
        anchors.leftMargin: 150
    }

    VMComboBox{
        id: diagCBET
        vmModel: ["REDm", "Mouse", "GP3HD"]
        currentIndex: {
            var sel = loader.getConfigurationString(vmDefines.vmCONFIG_SELECTED_ET)
            for (var i = 0; i < vmModel.length; i++){
                if (vmModel[i] === sel) {
                    vmLoadET = vmModel[i];
                    return i;
                }
            }
            return 0;
        }
        font.family: viewHome.robotoR.name
        font.pixelSize: 13
        anchors.top: diagLabelET.bottom
        anchors.topMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 150
    }

    // Combo box for eyetracker selection and label
    Text {
        id: diagLabelEye
        text: loader.getStringForKey(keybase+"diagLabelEye");
        font.family: viewHome.robotoB.name
        font.pixelSize: 13
        font.bold: true
        anchors.top: diagCBLang.bottom
        anchors.topMargin: 38
        anchors.left: diagLabelET.right
        anchors.leftMargin: 20
    }

    VMComboBox{
        id: diagCBEyeType
        vmModel: {
            //["REDm", "Mouse", "GP3HD"]
            var options = [];
            var id = keybase+"diagCBEyeType";
            options.push(loader.getStringForKey(id,vmEYE_L));
            options.push(loader.getStringForKey(id,vmEYE_R));
            options.push(loader.getStringForKey(id,vmEYE_BOTH));
            return options;
        }
        currentIndex: {
            var sel = loader.getConfigurationString(vmDefines.vmCONFIG_VALID_EYE)
            if (sel === "0") return vmEYE_L;
            if (sel === "1") return vmEYE_R;
            return vmEYE_BOTH;
        }
        font.family: viewHome.robotoR.name
        font.pixelSize: 13
        anchors.top: diagLabelEye.bottom
        anchors.topMargin: 5
        anchors.left: diagLabelEye.left
    }


    // The double monitor and demo mode selections

    VMCheckBox{
        id: diagCboxDemo
        text: loader.getStringForKey(keybase+"diagCboxDemo");
        font.family: viewHome.robotoR.name
        font.pixelSize: 13
        anchors.top: diagCBET.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 150
        checked: loader.getConfigurationBoolean(vmDefines.vmCONFIG_DEMO_MODE);
    }

    VMCheckBox{
        id: diagCboxDualMonitor
        text: loader.getStringForKey(keybase+"diagCboxDualMonitor");
        font.family: viewHome.robotoR.name
        font.pixelSize: 13
        anchors.top: diagCBET.bottom
        anchors.topMargin: 20
        anchors.left: diagCboxDemo.right
        anchors.leftMargin: 20
        checked: loader.getConfigurationBoolean(vmDefines.vmCONFIG_DUAL_MONITOR_MODE);
    }

    VMButton{
        id: diagBtnOK
        vmFont: viewHome.gothamM.name
        vmSize: [120, 49]
        vmText: loader.getStringForKey(keybase+"diagBtnOK");
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 150
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
        if (diagTIDoctor.vmTextField.text === ""){
            diagTIDoctor.vmErrorMsg = loader.getStringForKey(keybase+"error_invalid_dr");
            return false;
        }

        // Saving the configuration.
        loader.setConfigurationString(vmDefines.vmCONFIG_DOCTOR_NAME,diagTIDoctor.vmTextField.text);
        loader.setConfigurationString(vmDefines.vmCONFIG_DOCTOR_EMAIL,diagTIEmail.vmTextField.text);
        loader.setConfigurationString(vmDefines.vmCONFIG_SELECTED_ET,diagCBET.currentText);
        loader.setConfigurationString(vmDefines.vmCONFIG_REPORT_LANGUAGE,diagCBLang.currentText);
        loader.setConfigurationBoolean(vmDefines.vmCONFIG_DUAL_MONITOR_MODE,diagCboxDualMonitor.checked);
        loader.setConfigurationBoolean(vmDefines.vmCONFIG_DEMO_MODE,diagCboxDemo.checked);
        if (diagCBEyeType.currentIndex == vmEYE_L){
            loader.setConfigurationInt(vmDefines.vmCONFIG_VALID_EYE,vmEYE_L);
        }
        else if (diagCBEyeType.currentIndex == vmEYE_R){
            loader.setConfigurationInt(vmDefines.vmCONFIG_VALID_EYE,vmEYE_R);
        }
        else{
            loader.setConfigurationInt(vmDefines.vmCONFIG_VALID_EYE,vmEYE_BOTH);
        }

        // Signal to update dropdown menu information.
        updateMenus()

        // Restart required
        if (vmLoadLanguage !== diagCBLang.currentText){
            viewSettings.vmRestartRequired = true;
        }
        else{
            // Check if et changed.
            if (diagCBET.currentText !== vmLoadET){
                flowControl.eyeTrackerChanged()
            }
        }

        return true;
    }

}
