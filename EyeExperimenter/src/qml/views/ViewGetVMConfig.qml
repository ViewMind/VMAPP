import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"

ViewBase {

    id: viewGetVMConfig

    // Map keys.
    readonly property string vmINSTITUTION_ID: "institution_id";
    readonly property string vmINSTANCE_NUM: "instance_number";

    signal successActivation()

    Connections {
        target: loader
        function onFinishedRequest () {            
            if (mainWindow.getCurrentSwipeIndex() !== VMGlobals.vmSwipeIndexGetVMConfig) return;

            if (loader.getLastAPIRequest() !== VMGlobals.vmAPI_ACTIVATE) return; // Prevents from activating the code below in functional control

            // Close the connection dialog and open the user selection dialog.
            mainWindow.closeWait()
            var failCode = loader.wasThereAnProcessingUploadError();

            let httpCode = loader.getLastHTTPCodeReceived();
            if (httpCode === VMGlobals.vmHTTP_CODE_OK){
                successActivation();
            }
            else if (httpCode === VMGlobals.vmHTTP_CODE_ACT_FAILED_INVALID_SN){
                popUpNotify(VMGlobals.vmNotificationRed,loader.getStringForKey("viewgetconfig_error_sn"));
            }
            else if (httpCode === VMGlobals.vmHTTP_CODE_ACT_FAILED_NOT_LIBERATED){
                let msg = loader.getStringForKey("viewgetconfig_error_lib");
                let uid =  institution.vmCurrentText + "." + instance.vmCurrentText;
                msg = msg.replace("???",uid);
                //console.log("Message is '" + msg + "' and uid is " + uid);
                popUpNotify(VMGlobals.vmNotificationRed,msg);
            }
            else {
                popUpNotify(VMGlobals.vmNotificationRed,loader.getStringForKey("viewgetconfig_error"));
            }

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
        btnFuncVerif.vmText = loader.getStringForKey("viewgetconfig_func_verif");
    }

    function checkOnVMID() {
        var id_data = loader.getInstIDFileInfo();
        instance.vmEnabled = false;
        institution.vmEnabled = false;
        if (!(vmINSTANCE_NUM in id_data)) {
            activation_code.vmEnabled = false;
            btnGetLicence.vmEnabled = false;
            btnFuncVerif.visible = true;
            return;
        }
        else {
            instance.setText(id_data[vmINSTANCE_NUM])
            institution.setText(id_data[vmINSTITUTION_ID])
            btnGetLicence.vmEnabled = true;
            btnFuncVerif.visible = false;
        }
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

    VMButton {
        id: btnGetLicence
        anchors.left: inputColumn.left
        anchors.bottom: parent.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(60)
        vmText: loader.getStringForKey("viewgetconfig_get")
        vmCustomWidth: VMGlobals.adjustHeight(150)
        onClickSignal: {

            // We need to check all values are here.
            let inst   = institution.vmCurrentText
            let number = instance.vmCurrentText
            let code   = activation_code.vmCurrentText

            // For quick testing. Developing the endpoint itself
            //                inst = 1;
            //                number = 9;
            //                code = "PIDTB-WSJIE-7YFVB-WLSJU-U2XWZ-SVMHD"

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


    VMButton {
        id: btnFuncVerif
        anchors.left: btnGetLicence.right
        anchors.leftMargin: VMGlobals.adjustWidth(15)
        anchors.bottom: btnGetLicence.bottom
        vmText: loader.getStringForKey("viewgetconfig_func_verif")
        vmCustomWidth: VMGlobals.adjustHeight(250)
        vmButtonType: btnFuncVerif.vmTypeSecondary;
        onClickSignal: {
            swiperControl.setCurrentIndex(VMGlobals.vmSwipeIndexFunctionalVerif);
            viewFuncCtl.setIntent(viewFuncCtl.vmINTENT_FUNC_VERIF)
        }
    }


    VMComboBox {
        id: uiLanguage
        vmLabel: loader.getStringForKey("viewsettings_language")
        width: VMGlobals.adjustWidth(150)
        anchors.bottom: parent.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(60)
        anchors.left: btnGetLicence.left
        anchors.leftMargin: VMGlobals.adjustWidth(800)
        vmMaxDisplayItems: 1
        Component.onCompleted: {
            var langs = loader.getStringListForKey("viewsettings_langs")
            this.setModelList(langs)
            uiLanguage.setSelection(0);
        }
        onVmCurrentIndexChanged: {
            if (loader.isVMConfigPresent()) return; // We do this because on normal operation the line below screws with language selection
            loader.changeGetVMConfigScreenLanguage(uiLanguage.vmCurrentText);
            reloadStrings();
        }
    }







}
