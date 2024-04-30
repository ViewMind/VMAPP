import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"

ViewBase {

    id: viewAddEvaluator

    readonly property int vmINTENT_FUNC_VERIF: 0
    readonly property int vmINTENT_HMD_CHANGE: 1

    readonly property int vmETCHECK_CHECKING: 0
    readonly property int vmETCHECK_PASS:     1
    readonly property int vmETCHECK_FAILED:   2
    readonly property int vmETCHECK_NOTRUN:   3

    readonly property string vmKEY_TO_CHECK: "update_sampling_frequecy"

    readonly property string vmKEY_PC_SN: "serial_number";
    readonly property string vmKEY_ET_SN: "hmd_sn";
    readonly property string vmKEY_HDD_SN: "hdd_sn"

    property int vmETCheckState: vmETCHECK_NOTRUN
    property int vmIntent: vmINTENT_FUNC_VERIF;
    property bool vmLoadedSN: false

    /**
      This function should be called each time this comes into view.
      Hence it will always start in the et check not run state.
      */
    function setIntent(intent){
        vmIntent = intent;
        vmETCheckState = vmETCHECK_NOTRUN;

        // We try to get the HW Info. For security we disable the action button-
        vmLoadedSN = false

        let hwinfo = loader.getHWInfo();

        // Now we get the PC SN and the HMD SN.
        // console.log(JSON.stringify(hwinfo))

        let pc_sn = hwinfo[vmKEY_PC_SN];
        let et_sn = hwinfo[vmKEY_ET_SN];
        let hdd_sn = hwinfo[vmKEY_HDD_SN];


        // Simple test. Might fail. But basically there are no numbers in the return strings for Desktop computers in this fields
        // So if it has numbers its a laptop and we can use the serial number.
        // If it doesn't we need to use the hdd.
        let hasNumbers = /\d/.test(pc_sn);
        if (hasNumbers){
            pc_sn_input.setText(pc_sn);
        }
        else {
            pc_sn_input.setText(hdd_sn);
        }
        hmd_sn_input.setText(et_sn)

        if ((pc_sn_input.vmCurrentText != "") && (hmd_sn_input.vmCurrentText != "")){
            vmLoadedSN = true
        }
        else if (hmd_sn_input.vmCurrentText == "") {
            //popUpNotify(VMGlobals.vmNotificationRed,loader.getStringForKey("viewfuncctl_btn_no_hmd_error"),false);
            messageDiag.vmLarge = false;
            messageDiag.loadFromKey("viewfuncctl_btn_no_hmd_error");
            messageDiag.open(true);
            runETCheckButton.vmEnabled = false;
        }

        if (pc_sn_input.vmCurrentText == ""){
            // Hope that this NEVER happens.
            loader.logUI("UNABLE TO FIND A S/N for the COMPUTER",true);
        }

        if (vmIntent == vmINTENT_HMD_CHANGE){
            // If the intent is HMD Check we need to load the institution and instance as well.
            let instance_uid = loader.getInstanceUID();
            let parts = instance_uid.split(".")
            if (parts.length === 2){
                instance.setText(parts[1])
                institution.setText(parts[0])
                instance.vmEnabled = false;
                institution.vmEnabled = false;
            }
        }

        // Uncomment this to test out.
        // setDebugData();

    }

    function setDebugData(){
        instance.setText("9");
        institution.setText("1");
        email.setText("ariel.arelovich@viewmind.ai")
        if (vmIntent == vmINTENT_FUNC_VERIF){
            password.vmCurrentText = "1234";
        }
        else {
            lname.setText("Ariel")
            fname.setText("Arelovich")
        }
        actionButton.vmEnabled = true;
        vmETCheckState = vmETCHECK_PASS;
    }


    Connections {
        target: loader
        function onFinishedRequest () {
            // Close the connection dialog and open the user selection dialog.
            //console.log("Finished request")

            mainWindow.closeWait()
            let last_api_request = loader.getLastAPIRequest();

            if ( (last_api_request === VMGlobals.vmAPI_FUNC_CTL_NEW) ||
                 (last_api_request === VMGlobals.vmAPI_FUNC_CTL_HMD_CHANGE)){

                mainWindow.closeWait()

                // We check for errors.
                let http_code = loader.getLastHTTPCodeReceived();
                if (http_code === VMGlobals.vmHTTP_CODE_OK){
                    if (last_api_request === VMGlobals.vmAPI_FUNC_CTL_NEW) swiperControl.setCurrentIndex(VMGlobals.vmSwipeIndexGetVMConfig)
                    else swiperControl.setCurrentIndex(VMGlobals.vmSwipeIndexHome)

                    popUpNotify(VMGlobals.vmNotificationGreen,loader.getStringForKey("viewfuncctl_success"))

                    return;
                }
                else if (http_code === VMGlobals.vmHTTP_CODE_FUNC_VERIF_FAIL_BADSN){
                    popUpNotify(VMGlobals.vmNotificationRed,loader.getStringForKey("viewfuncctl_error_bad_sn"))
                }
                else if (http_code === VMGlobals.vmHTTP_CODE_FUNC_VERIF_FAIL_BAD_PASSWD){
                    popUpNotify(VMGlobals.vmNotificationRed,loader.getStringForKey("viewfuncctl_error_bad_password"))
                }
                else if (http_code === VMGlobals.vmHTTP_CODE_FUNC_VERIF_FAIL_UNAUTHUSER){
                    popUpNotify(VMGlobals.vmNotificationRed,loader.getStringForKey("viewfuncctl_error_bad_user"))
                }
                else {
                    popUpNotify(VMGlobals.vmNotificationRed,loader.getStringForKey("viewfuncctl_error_generic"))
                }

            }
        }
    }

    Connections {
        target: flowControl
        function onNotifyLoader(data){
            //console.log("Got Loader Notification for " + JSON.stringify(data));
            if (!timer.running) return; // Nothing to do.
            if (vmKEY_TO_CHECK in data){
                if (data[vmKEY_TO_CHECK] > 100){
                    // We are good.
                    timer.running = false;
                    vmETCheckState = vmETCHECK_PASS;
                }
            }
        }
    }


    Text {
        id: title
        text: loader.getStringForKey("viewfuncctl_title")
        font.pixelSize: VMGlobals.vmFontExtraExtraLarge
        font.weight: 600
        color: VMGlobals.vmBlackText
        height: VMGlobals.adjustHeight(43)
        verticalAlignment: Text.AlignVCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(645)
    }

    Text {
        id: subtitle
        text: loader.getStringForKey("viewgetconfig_instructions")
        font.pixelSize: VMGlobals.vmFontLarge
        font.weight: 400
        color: VMGlobals.vmGrayPlaceholderText
        verticalAlignment: Text.AlignVCenter
        height: VMGlobals.adjustHeight(18)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: title.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)
    }

    Row {

        id: formMain
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: subtitle.bottom
        anchors.topMargin: VMGlobals.adjustHeight(70)
        spacing: VMGlobals.adjustWidth(20)

        Column {

            id: inputColumnLeft
            width: VMGlobals.adjustWidth(400)
            spacing: VMGlobals.adjustHeight(60)

            VMTextInput {
                id: institution
                vmPlaceHolderText: loader.getStringForKey("viewgetconfig_institution_ph")
                vmLabel: loader.getStringForKey("viewgetconfig_institution")
                width: parent.width
                vmAlignErrorLeft: false
                Keys.onTabPressed: lname.vmFocus = true;
            }

            VMTextInput {
                id: hmd_sn_input
                vmLabel: loader.getStringForKey("viewfuncctl_hmdsn")
                width: parent.width
                vmAlignErrorLeft: false
                Keys.onTabPressed: lname.vmFocus = true;
                vmEnabled: false
            }

        }


        Column {

            id: inputColumnRight
            width: VMGlobals.adjustWidth(320)
            spacing: VMGlobals.adjustHeight(60)

            VMTextInput {
                id: instance
                width: parent.width
                vmPlaceHolderText: loader.getStringForKey("viewgetconfig_instance_ph")
                vmLabel: loader.getStringForKey("viewgetconfig_instance")
                vmAlignErrorLeft: false
                Keys.onTabPressed: email.vmFocus = true;
            }

            VMTextInput {
                id: pc_sn_input
                width: parent.width
                vmLabel: loader.getStringForKey("viewfuncctl_pcsn")
                vmAlignErrorLeft: false                
                Keys.onTabPressed: email.vmFocus = true;
                vmEnabled: false
            }

        }

    }

    Rectangle {
        id: firstDivider
        width: formMain.width
        height: VMGlobals.adjustHeight(1)
        border.width: 0
        color: VMGlobals.vmGrayLightGrayText
        anchors.left: formMain.left
        anchors.top: formMain.bottom
        anchors.topMargin: VMGlobals.adjustHeight(20)
    }

    ///////////////////////////////////////////// EYE TRACKING CHECK ////////////////////////////////////////////

    Text {
        id: eyeTrackingText
        color: VMGlobals.vmBlackText
        font.weight: 400
        font.pixelSize: VMGlobals.vmFontExtraLarge
        text: {
            if (vmETCheckState == vmETCHECK_FAILED) return loader.getStringForKey("viewevaluation_checket_title_fail")
            else if (vmETCheckState == vmETCHECK_CHECKING) return loader.getStringForKey("viewevaluation_checket_title")
            else if (vmETCheckState == vmETCHECK_PASS) return loader.getStringForKey("viewfuncctl_text_pass");
            else return loader.getStringForKey("viewfuncctl_text_not_run")
        }
        height: VMGlobals.adjustHeight(37)
        verticalAlignment: Text.AlignVCenter
        anchors.top: firstDivider.bottom
        anchors.topMargin: VMGlobals.adjustHeight(20)
        anchors.left: firstDivider.left
    }

    AnimatedImage {
        id: waitingAnimation
        source: "qrc:/images/loader.gif"
        height: eyeTrackingText.height
        fillMode: Image.PreserveAspectFit
        anchors.verticalCenter: eyeTrackingText.verticalCenter
        anchors.right: runETCheckButton.left
        anchors.rightMargin: VMGlobals.adjustWidth(10);
        visible: (vmETCheckState == vmETCHECK_CHECKING)
    }

    Timer {
        id: timer
        interval: 8000
        running: false
        repeat: false
        onTriggered: {
            // This means there was a time out.
            timer.running = false;
            vmETCheckState = vmETCHECK_FAILED;
        }
    }

    Image {
        id: result
        source: {
            if (vmETCheckState == vmETCHECK_FAILED) return "qrc:/images/alert-triangle-red.png"
            else return "qrc:/images/check-circle.png"
        }
        height: waitingAnimation.height
        fillMode: Image.PreserveAspectFit
        anchors.verticalCenter: waitingAnimation.verticalCenter
        anchors.left: waitingAnimation.left
        visible: (vmETCheckState == vmETCHECK_FAILED) || (vmETCheckState == vmETCHECK_PASS)
    }


    VMButton {
        id: runETCheckButton
        vmText: loader.getStringForKey("viewfuncctl_runcheck")
        anchors.right: formMain.right
        anchors.verticalCenter: eyeTrackingText.verticalCenter
        onClickSignal: {
            timer.running = true;
            vmETCheckState = vmETCHECK_CHECKING;
        }
        vmEnabled: (vmETCheckState != vmETCHECK_CHECKING)
    }

    Rectangle {
        id: secondDivider
        width: firstDivider.width
        height: firstDivider.height
        border.width: 0
        color: VMGlobals.vmGrayLightGrayText
        anchors.left: firstDivider.left
        anchors.top: eyeTrackingText.bottom
        anchors.topMargin: VMGlobals.adjustHeight(20)
    }

    ///////////////////////////////////////////// LOGIN PART OF THE FORM ////////////////////////////////////////////

    Row {

        id: formLogin
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: secondDivider.bottom
        anchors.topMargin: VMGlobals.adjustHeight(70)
        spacing: VMGlobals.adjustWidth(20)

        Column {

            id: inputColumnLeftLogin
            width: inputColumnLeft.width
            spacing: VMGlobals.adjustHeight(60)


            VMTextInput {
                id: fname
                vmPlaceHolderText: loader.getStringForKey("viewaddeval_fname_ph")
                vmLabel: loader.getStringForKey("viewaddeval_fname")
                width: parent.width
                vmAlignErrorLeft: false
                visible: vmIntent == vmINTENT_HMD_CHANGE
            }

            VMTextInput {
                id: email
                width: parent.width
                vmPlaceHolderText: loader.getStringForKey("viewaddeval_email_ph")
                vmLabel: loader.getStringForKey("viewaddeval_email")
                vmAlignErrorLeft: false
            }

        }


        Column {

            id: inputColumnRightLogin
            width: inputColumnRight.width
            spacing: VMGlobals.adjustHeight(60)

            VMTextInput {
                id: lname
                width: parent.width
                vmPlaceHolderText: loader.getStringForKey("viewaddeval_lname_ph")
                vmLabel: loader.getStringForKey("viewaddeval_lname")
                vmAlignErrorLeft: false
                visible: fname.visible
            }

            VMPasswordInput {
                id: password
                width: parent.width
                vmLabel: loader.getStringForKey("viewlogin_label_pass")
                vmPlaceHolderText: loader.getStringForKey("viewlogin_placeholder_password")
                vmShowText: loader.getStringForKey("viewlogin_show")
                vmHideText: loader.getStringForKey("viewlogin_hide")
                vmAlignErrorLeft: false
                Keys.onTabPressed: verifyPassword.vmFocus = true;
                visible: !fname.visible
            }

        }

    }



    VMButton {
        id: actionButton
        vmText: loader.getStringForKey("viewfuncctl_btn_send")
        anchors.bottom: parent.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(40)
        anchors.right: parent.right
        anchors.rightMargin: VMGlobals.adjustWidth(40)
        vmEnabled: (vmLoadedSN && (vmETCheckState == vmETCHECK_PASS))
        onClickSignal: {           

            // We need to verify that the institution and instance number are loaded.
            // And then the rest will depend on the intent.
            if (institution.vmCurrentText == ""){
                institution.vmErrorMsg = loader.getStringForKey("viewsendsupport_email_error")
                return;
            }

            if (instance.vmCurrentText == ""){
                instance.vmErrorMsg = loader.getStringForKey("viewsendsupport_email_error")
                return;
            }

            if (email.vmCurrentText == ""){
                email.vmErrorMsg = loader.getStringForKey("viewsendsupport_email_error")
                return;
            }

            if (vmIntent == vmINTENT_FUNC_VERIF){
                if (password.vmCurrentText == ""){
                    password.vmErrorMsg = loader.getStringForKey("viewsendsupport_email_error")
                    return;
                }
            }
            else {

                // We are talking about an HMD change.
                if (lname.vmCurrentText == ""){
                    lname.vmErrorMsg = loader.getStringForKey("viewsendsupport_email_error")
                    return;
                }

                if (fname.vmCurrentText == ""){
                    fname.vmErrorMsg = loader.getStringForKey("viewsendsupport_email_error")
                    return;
                }

            }

            //console.log("Actually send the API Request");
            mainWindow.openWait(loader.getStringForKey("viewwait_synching"))
            loader.sendFunctionalControlData(institution.vmCurrentText,instance.vmCurrentText,email.vmCurrentText,password.vmCurrentText,fname.vmCurrentText,lname.vmCurrentText)

        }
    }

}
