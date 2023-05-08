import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"

ViewBase {

    id: viewGetVMConfig

    signal successActivation()

    Connections {
        target: loader
        function onFinishedRequest () {
            if (mainWindow.getCurrentSwipeIndex() !== VMGlobals.vmSwipeIndexGetVMConfig) return;

            // Close the connection dialog and open the user selection dialog.
            mainWindow.closeWait()
            var failCode = loader.wasThereAnProcessingUploadError();

            // This check needs to be done ONLY when on this screen.
            if (failCode !== VMGlobals.vmFAIL_CODE_NONE){
                popUpNotify(VMGlobals.vmNotificationRed,loader.getStringForKey("viewgetconfig_error"));
                return;
            }

            //console.log("All good with the activation!!!");
            successActivation();

        }
    }

    // Function to realod the strings of this screen whenever a language change is done.
    function reloadStrings(){
        welcomeText.text = loader.getStringForKey("viewstart_welcome");
        instructionText.text = loader.getStringForKey("viewgetconfig_instructions")
        institution.vmPlaceHolderText = loader.getStringForKey("viewgetconfig_institution_ph")
        institution.vmLabel = loader.getStringForKey("viewgetconfig_institution")
        instance.vmPlaceHolderText = loader.getStringForKey("viewgetconfig_instance_ph")
        instance.vmLabel = loader.getStringForKey("viewgetconfig_instance")
        activation_code.vmPlaceHolderText = loader.getStringForKey("viewgetconfig_code_ph")
        activation_code.vmLabel = loader.getStringForKey("viewgetconfig_code")
        btnGetLicence.vmText = loader.getStringForKey("viewgetconfig_get")
        uiLanguage.vmLabel = loader.getStringForKey("viewsettings_language")
    }

    // The ViewMind Logo.
    Image {
        id: viewmindLogo
        height: VMGlobals.vmControlsHeight
        fillMode: Image.PreserveAspectFit
        anchors.left: parent.left
        anchors.leftMargin: VMGlobals.adjustWidth(100)
        anchors.top: parent.top
        anchors.topMargin: VMGlobals.adjustHeight(80)
        source: "qrc:/images/logo.png"
    }

    // The Eye Design
    Image {
        id: viewmindEye
        width: VMGlobals.adjustWidth(558)
        fillMode: Image.PreserveAspectFit
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: VMGlobals.adjustWidth(100)
        anchors.topMargin: VMGlobals.adjustHeight(167)
        source: "qrc:/images/eye.png"
    }

    Text {
        id: welcomeText
        text: loader.getStringForKey("viewstart_welcome")
        font.pixelSize: VMGlobals.vmFontHuge
        font.weight: 600
        color: VMGlobals.vmBlackText
        anchors.left: viewmindLogo.left
        anchors.top: viewmindLogo.bottom
        anchors.topMargin: VMGlobals.adjustHeight(50)
    }


    Text {
        id: instructionText
        text: loader.getStringForKey("viewgetconfig_instructions")
        font.pixelSize: VMGlobals.vmFontLarge
        font.weight: 400
        color: VMGlobals.vmGrayPlaceholderText
        anchors.left: viewmindLogo.left
        anchors.top: welcomeText.bottom
        anchors.topMargin: VMGlobals.adjustHeight(15)
    }

    Column {

        id: inputColumn
        width: VMGlobals.adjustWidth(320)
        spacing: VMGlobals.adjustHeight(54)
        anchors.left: instructionText.left
        anchors.top: instructionText.bottom
        anchors.topMargin: VMGlobals.adjustHeight(55)

        VMTextInput {
            id: institution
            vmPlaceHolderText: loader.getStringForKey("viewgetconfig_institution_ph")
            vmLabel: loader.getStringForKey("viewgetconfig_institution")
            width: parent.width
            Keys.onTabPressed: instance.vmFocus = true;
        }

        VMTextInput {
            id: instance
            width: parent.width
            vmPlaceHolderText: loader.getStringForKey("viewgetconfig_instance_ph")
            vmLabel: loader.getStringForKey("viewgetconfig_instance")
            Keys.onTabPressed: activation_code.vmFocus = true;
        }

        VMTextInput {
            id: activation_code
            width: parent.width
            vmPlaceHolderText: loader.getStringForKey("viewgetconfig_code_ph")
            vmLabel: loader.getStringForKey("viewgetconfig_code")
            Keys.onTabPressed: institution.vmFocus = true;
        }


    }

    Row {
        id: rowButtonAndLang
        anchors.left: inputColumn.left
        anchors.bottom: parent.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(60)
        spacing: VMGlobals.adjustWidth(900)
        VMButton {
            id: btnGetLicence
            vmText: loader.getStringForKey("viewgetconfig_get")
            vmCustomWidth: VMGlobals.adjustHeight(150)
            onClickSignal: {

                // We need to check all values are here.
                let inst   = institution.vmCurrentText
                let number = instance.vmCurrentText
                let code   = activation_code.vmCurrentText

                // For quick testing. Developing the endpoint itself
                //inst = 1;
                //number = 6;
                //code = "ZP1KY-OGQFG-UFFSV-A4SPO-YONOK-NGOA0"

                if (inst == ""){
                    institution.vmErrorMsg = loader.getStringForKey("viewaddeval_err_empty")
                    return;
                }

                if (number == ""){
                    instance.vmErrorMsg = loader.getStringForKey("viewaddeval_err_empty")
                    return;
                }

                if (code == ""){
                    activation_code.vmErrorMsg = loader.getStringForKey("viewaddeval_err_empty")
                    return;
                }


                mainWindow.openWait(loader.getStringForKey("viewait_get_license"))

                // Requesting the vm configuration data.
                loader.requestActivation(inst,number,code);

            }
        }

        VMComboBox {
            id: uiLanguage
            vmLabel: loader.getStringForKey("viewsettings_language")
            width: VMGlobals.adjustWidth(150)
            vmMaxDisplayItems: 1
            Component.onCompleted: {
                var langs = loader.getStringListForKey("viewsettings_langs")
                setModelList(loader.getStringListForKey("viewsettings_langs"))
                var lang = loader.getConfigurationString("ui_language")
                for (var i = 0; i < langs.length; i++){
                    if (langs[i] === lang){
                        uiLanguage.setSelection(i)
                        break;
                    }
                }
            }
            onVmCurrentIndexChanged: {
                loader.changeGetVMConfigScreenLanguage(uiLanguage.vmCurrentText);
                reloadStrings();
            }
        }

    }






}
