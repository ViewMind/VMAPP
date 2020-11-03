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
    width: mainWindow.width*0.48
    height: mainWindow.height*0.812
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
        font.pixelSize: 18*viewHome.vmScale
        font.family: viewHome.gothamB.name
        color: "#297FCA"
        text: loader.getStringForKey(keybase+"diagTitle");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: mainWindow.height*0.101
    }

    // Creating the close button
    VMDialogCloseButton {
        id: btnClose
        anchors.top: parent.top
        anchors.topMargin: mainWindow.height*0.032
        anchors.right: parent.right
        anchors.rightMargin: mainWindow.width*0.02
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
        font.pixelSize: 13*viewHome.vmScale
        font.bold: true
        anchors.top: diagTitle.bottom
        anchors.topMargin: mainWindow.height*0.036
        anchors.left: diagDBDefaultCountry.left
    }

    VMAutoCompleteComboBox{
        id: diagDBDefaultCountry
        width: mainWindow.width*0.344
        height: mainWindow.height*0.043
        z: 1
        vmList: loader.getCountryList()
        vmValues: loader.getCountryCodeList()
        anchors.top: diagLabelDefaultCountry.bottom
        anchors.topMargin: mainWindow.height*0.017
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
        font.pixelSize: 13*viewHome.vmScale
        font.bold: true
        anchors.top: diagDBDefaultCountry.bottom
        anchors.topMargin: mainWindow.height*0.036
        anchors.left: diagDBDefaultCountry.left
    }

    VMComboBox2{
        id: diagCBLang
        z: 1
        width: mainWindow.width*0.078
        anchors.top: diagLabelLang.bottom
        anchors.topMargin: mainWindow.height*0.022
        anchors.left: diagDBDefaultCountry.left
        Component.onCompleted: {
            var langs = ["English", "Spanish"]
            diagCBLang.setModelList(langs)
            var lang = loader.getConfigurationString(vmDefines.vmCONFIG_REPORT_LANGUAGE)
            for (var i = 0; i < langs.length; i++){
                if (langs[i] === lang){
                    vmLoadLanguage = lang;
                    diagCBLang.setSelection(i)
                    return;
                }
            }
        }
    }

    // Combo box for eyetracker selection and label
    Text {
        id: diagLabelET
        text: loader.getStringForKey(keybase+"diagLabelET");
        font.family: viewHome.robotoB.name
        font.pixelSize: 13*viewHome.vmScale
        font.bold: true
        anchors.top: diagCBLang.bottom
        anchors.topMargin: mainWindow.height*0.036
        anchors.left: diagDBDefaultCountry.left
    }

    // The double monitor, demo mode and use mouse selections

    VMCheckBox{
        id: diagCboxDemo
        text: loader.getStringForKey(keybase+"diagCboxDemo");
        font.family: viewHome.robotoR.name
        font.pixelSize: 13*viewHome.vmScale
        anchors.top: diagLabelET.bottom
        anchors.topMargin: mainWindow.height*0.029
        anchors.left: diagDBDefaultCountry.left
        checked: loader.getConfigurationBoolean(vmDefines.vmCONFIG_DEMO_MODE);
    }

    VMCheckBox{
        id: diagCboxDualMonitor
        text: loader.getStringForKey(keybase+"diagCboxDualMonitor");
        font.family: viewHome.robotoR.name
        font.pixelSize: 13*viewHome.vmScale
        anchors.top: diagCboxDemo.top
        anchors.left: diagCboxUseMouse.right
        anchors.leftMargin: mainWindow.width*0.016
        visible: {
            if (uimap.getSecondMonitorOption() === "E") return true;
            else if (uimap.getSecondMonitorOption() === "D") return false;
        }
        checked: {
            if (uimap.getSecondMonitorOption() === "E") return loader.getConfigurationBoolean(vmDefines.vmCONFIG_DUAL_MONITOR_MODE);
            else if (uimap.getSecondMonitorOption() === "D") return false;
        }
    }

    VMCheckBox{
        id: diagCboxUseMouse
        text: loader.getStringForKey(keybase+"diagCboxUseMouse");
        font.family: viewHome.robotoR.name
        font.pixelSize: 13*viewHome.vmScale
        anchors.top: diagCboxDemo.top
        anchors.left: diagCboxDemo.right
        anchors.leftMargin: mainWindow.width*0.016
        checked: loader.getConfigurationBoolean(vmDefines.vmCONFIG_USE_MOUSE)
        visible: true
    }

    Text {
        id: diagLabelEnableViewAll
        text: loader.getStringForKey(keybase+"diagLabelEnableViewAll");
        font.family: viewHome.robotoB.name
        font.pixelSize: 13*viewHome.vmScale
        font.bold: true
        anchors.top: diagCboxUseMouse.bottom
        anchors.topMargin: mainWindow.height*0.029
        anchors.left: diagDBDefaultCountry.left
    }

    Row{
        id: viewAllEnableRow
        anchors.left: diagCBLang.left
        anchors.top: diagLabelEnableViewAll.bottom
        anchors.topMargin: mainWindow.height*0.029
        spacing: 10

        VMPasswordField{
            id: instPassword
            vmLabelText: loader.getStringForKey(keybase+"diagInstPassword");
            width: mainWindow.width*0.117
        }

        VMButton{
            id: diagBtnEnableViewAll
            vmFont: viewHome.gothamM.name
            vmSize: [mainWindow.width*0.195, mainWindow.height*0.043]
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
        anchors.topMargin: mainWindow.height*0.065
        anchors.left: diagDBDefaultCountry.left
        onClicked: {
            if (checkAllOk()) close();
            if (vmRestartRequired){
                restartRequiredDiag.open();
            }
        }
    }

    VMButton{
        id: diagBtnAbout
        vmFont: viewHome.gothamM.name
        vmSize: [140, 49]
        vmInvertColors: true
        vmText: loader.getStringForKey(keybase+"diagAbout");
        anchors.top: viewAllEnableRow.bottom
        anchors.topMargin: mainWindow.height*0.065
        anchors.right: diagDBDefaultCountry.right
        onClicked: {
            viewSettings.close();
            viewAbout.open();
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
        loader.setSettingsValue(vmDefines.vmCONFIG_REPORT_LANGUAGE,diagCBLang.vmCurrentText);
        loader.setSettingsValue(vmDefines.vmCONFIG_DUAL_MONITOR_MODE,diagCboxDualMonitor.checked);
        loader.setSettingsValue(vmDefines.vmCONFIG_DEMO_MODE,diagCboxDemo.checked);
        loader.setSettingsValue(vmDefines.vmCONFIG_DEFAULT_COUNTRY,newCountry);
        loader.setSettingsValue(vmDefines.vmCONFIG_USE_MOUSE,diagCboxUseMouse.checked);

        // Signal to update dropdown menu information.
        updateMenus();

        // Restart required
        if ((vmLoadLanguage !== diagCBLang.vmCurrentText) || (vmDefaultCountry !== newCountry)){
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
