import Qt.labs.platform 1.1 as Platform
import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects

Dialog {

    property bool vmInvalidRepoError: false;
    property string vmLoadLanguage: "";
    property string vmDefaultCountry: "";
    property bool vmRestartRequired: false;
    property bool vmPreviousMouseCheckValue: false

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

    // The configure settings title
    Text {
        id: diagTitle
        font.pixelSize: 18*viewHome.vmScale
        font.family: viewHome.gothamB.name
        color: "#297FCA"
        text: loader.getStringForKey("viewsettings_diagTitle");
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
        text: loader.getStringForKey("viewsettings_diagLabelDefaultCountry");
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
        //height: mainWindow.height*0.043
        vmNoLabelHeight: mainWindow.height*0.043
        z: 2
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
        text: loader.getStringForKey("viewsettings_diagLabelLang");
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
            var lang = loader.getConfigurationString("ui_language")
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
        text: loader.getStringForKey("viewsettings_diagLabelET");
        font.family: viewHome.robotoB.name
        font.pixelSize: 13*viewHome.vmScale
        font.bold: true
        anchors.top: diagCBLang.bottom
        anchors.topMargin: mainWindow.height*0.036
        anchors.left: diagDBDefaultCountry.left
    }

    // The double monitor, demo mode and use mouse selections

    VMCheckBox{
        id: diagCboxDualMonitor
        text: loader.getStringForKey("viewsettings_diagCboxDualMonitor");
        font.family: viewHome.robotoR.name
        font.pixelSize: 13*viewHome.vmScale
        anchors.top: diagLabelET.bottom
        anchors.left: diagCboxUseMouse.right
        anchors.leftMargin: mainWindow.width*0.016
        visible: false
        checked:  loader.getConfigurationBoolean("dual_monitor_mode");
    }

    VMCheckBox{
        id: diagCboxUseMouse
        text: loader.getStringForKey("viewsettings_diagCboxUseMouse");
        font.family: viewHome.robotoR.name
        font.pixelSize: 13*viewHome.vmScale
        anchors.top: diagLabelET.bottom
        anchors.topMargin: mainWindow.height*0.022
        anchors.left: diagDBDefaultCountry.left
        anchors.leftMargin: mainWindow.width*0.016
        checked: {
           vmPreviousMouseCheckValue = loader.getConfigurationBoolean("use_mouse");
           return vmPreviousMouseCheckValue;
        }
        visible: false
    }

    VMButton{
        id: diagBtnOK
        vmFont: viewHome.gothamM.name
        vmSize: [mainWindow.width*0.09, mainWindow.height*0.068]
        vmText: loader.getStringForKey("viewsettings_diagBtnOK");
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: mainWindow.height*0.02;
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
        vmSize: [mainWindow.width*0.09, mainWindow.height*0.068]
        vmInvertColors: true
        vmText: loader.getStringForKey("viewsettings_diagAbout");
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: mainWindow.height*0.02;
        anchors.right: diagDBDefaultCountry.right
        onClicked: {
            viewSettings.close();
            viewAbout.open();
        }
    }


    Platform.MessageDialog {
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

        loader.setSettingsValue("ui_language",diagCBLang.vmCurrentText);
        loader.setSettingsValue("default_country",newCountry);
        loader.setSettingsValue("use_mouse",diagCboxUseMouse.checked);

        // Signal to update dropdown menu information.
        updateMenus();

        // Restart required
        if ((vmLoadLanguage !== diagCBLang.vmCurrentText) || (vmDefaultCountry !== newCountry)){
            viewSettings.vmRestartRequired = true;
        }
        else{
            // Check if et changed.
            if (vmPreviousMouseCheckValue !== diagCboxUseMouse.checked){
                flowControl.eyeTrackerChanged()
            }
        }

        return true;
    }

}
