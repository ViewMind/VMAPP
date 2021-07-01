import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

VMBase {

    id: viewDoctorInformation
    width: viewDoctorInformation.vmWIDTH
    height: viewDoctorInformation.vmHEIGHT

    readonly property string keybase: "viewdrinfo_"
    property string oldemail: ""

    function clearAllFields(){
        labelLastName.clear();
        labelMail.clear();
        labelName.clear();
        labelPassword.clear();
        labelVerifyPassword.clear();

        // Clearing error just in case
        labelMail.vmErrorMsg = "";
        labelName.vmErrorMsg = "";
        labelLastName.vmErrorMsg = "";
        labelPassword.vmErrorMsg = "";
    }

    function newEvaluator(){
        // Just to make doubly sure NO one is logged in.
        loader.logOut();

        // We shouldn't see the menu
        setMenuVisibility(false);

        // This function is called.
        oldemail = "";
    }

    function loadDoctorInformation(){

        clearAllFields();

        var drInfo = loader.getCurrentEvaluatorInfo();

        // Saving the old email to check for a change in it.
        oldemail = drInfo.email;
        labelMail.setText(drInfo.email);
        labelName.setText(drInfo.name);
        labelLastName.setText(drInfo.lastname);

        setMenuVisibility(true);
    }

    function checkAndSave(){
        if (labelPassword.getText() !== labelVerifyPassword.getText()){
            labelPassword.vmErrorMsg = loader.getStringForKey(keybase + "password_match");
            return;
        }

        if (labelName.vmEnteredText === ""){
            labelName.vmErrorMsg = loader.getStringForKey(keybase + "errorEmpty");
            return;
        }

        if (labelLastName.vmEnteredText === ""){
            labelLastName.vmErrorMsg = loader.getStringForKey(keybase + "errorEmpty");
            return;
        }

        if (labelMail.vmEnteredText === ""){
            labelMail.vmErrorMsg = loader.getStringForKey(keybase + "errorEmpty");
            return;
        }

        //console.log("oldemail: " + oldemail)

        if (oldemail === ""){
            // If the old email is empty this is a new user. In this case the password cannot be empty.
            if (labelPassword.getText() === ""){
                labelPassword.vmErrorMsg = loader.getStringForKey(keybase + "errorEmpty");
                return;
            }
            if (loader.checkIfEvaluatorEmailExists(labelMail.vmEnteredText)){
                labelMail.vmErrorMsg = loader.getStringForKey(keybase + "drexists");
                return;
            }
        }
        else{
            if (oldemail != labelMail.vmEnteredText){
                // The evaluator want to change it's email and hence, we need to verify it's not overstepping on another user
                if (loader.checkIfEvaluatorEmailExists(labelMail.vmEnteredText)){
                    labelMail.vmErrorMsg = loader.getStringForKey(keybase + "drexists");
                    return;
                }
            }
        }


        loader.addOrModifyEvaluator(labelMail.vmEnteredText,oldemail,labelPassword.getText(),labelName.vmEnteredText,labelLastName.vmEnteredText);
        loader.updateCurrentEvaluator(labelMail.vmEnteredText);
        viewHome.updateDrMenuText();
        swiperControl.currentIndex = swiperControl.vmIndexPatientList;
    }


    // The Doctor Information Title and subtitle
    Text {
        id: diagTitle
        font.pixelSize: 43*viewHome.vmScale
        font.family: gothamB.name
        color: "#297FCA"
        text: loader.getStringForKey(keybase+"title");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: vmBanner.bottom
        anchors.topMargin: mainWindow.height*0.043
    }

    Text {
        id: diagSubTitle
        font.pixelSize: 11*viewHome.vmScale
        font.family: gothamR.name
        color: "#cfcfcf"
        text: loader.getStringForKey(keybase+"subtitle");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: diagTitle.bottom
        anchors.topMargin: mainWindow.height*0.015
    }

    Column {

        id: mainForm
        width: mainWindow.width*0.340
        anchors.top: diagSubTitle.bottom
        anchors.topMargin: mainWindow.height*0.028
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: mainWindow.height*0.057

        // The form fields

        // Name, username and email.
        VMTextDataInput{
            id: labelName
            width: parent.width
            vmPlaceHolder: loader.getStringForKey(keybase+"labelName");
            Keys.onTabPressed: labelLastName.vmFocus = true;
        }

        VMTextDataInput{
            id: labelLastName
            width: parent.width
            vmPlaceHolder: loader.getStringForKey(keybase+"labelLastName");
            Keys.onTabPressed: labelMail.vmFocus = true;
        }

        VMTextDataInput{
            id: labelMail
            width: parent.width
            vmPlaceHolder: loader.getStringForKey(keybase+"labelMail");
            Keys.onTabPressed: labelPassword.vmFocus = true;
        }

        VMPasswordField{
            id: labelPassword;
            width: parent.width
            vmLabelText: loader.getStringForKey(keybase+"password");
            Keys.onTabPressed: labelVerifyPassword.vmFocus = true;
        }

        VMPasswordField{
            id: labelVerifyPassword
            width: parent.width
            vmLabelText: loader.getStringForKey(keybase+"verify_password");
            Keys.onTabPressed: labelName.vmFocus = true;
        }

    }


    // Buttons
    Row{
        id: rowButtons
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: mainForm.bottom
        anchors.topMargin: mainWindow.height*0.072
        spacing: mainWindow.width*0.011

        VMButton{
            id: btnBack
            height: mainWindow.height*0.072
            vmText: loader.getStringForKey(keybase+"btnBack");
            vmFont: gothamM.name
            vmInvertColors: true
            onClicked: {
                if (loader.isLoggedIn()){
                    swiperControl.currentIndex = swiperControl.vmIndexPatientList;
                }
                else{
                    swiperControl.currentIndex = swiperControl.vmIndexHome
                }
            }
        }

        VMButton{
            id: btnSave
            vmText: loader.getStringForKey(keybase+"btnSave");
            vmFont: gothamM.name
            onClicked: {
                checkAndSave();
            }
        }
    }

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//    Dialog {
//        id: showMsgDialog;
//        modal: true
//        width: mainWindow.width*0.479
//        height: mainWindow.height*0.405
//        y: (parent.height - height)/2
//        x: (parent.width - width)/2
//        closePolicy: Popup.NoAutoClose

//        contentItem: Rectangle {
//            id: rectShowMsgDialog
//            anchors.fill: parent
//            layer.enabled: true
//            layer.effect: DropShadow{
//                radius: 5
//            }
//        }

//        VMDialogCloseButton {
//            id: btnClose
//            anchors.top: parent.top
//            anchors.topMargin: mainWindow.height*0.031
//            anchors.right: parent.right
//            anchors.rightMargin: mainWindow.width*0.019
//            onClicked: {
//                showMsgDialog.close();
//            }
//        }

//        // The instruction text
//        Text {
//            id: showMsgDialogTitle
//            font.family: viewHome.gothamB.name
//            font.pixelSize: 43*viewHome.vmScale
//            anchors.top: parent.top
//            anchors.topMargin: mainWindow.height*0.072
//            anchors.left: parent.left
//            anchors.leftMargin: mainWindow.width*0.015
//            color: "#297fca"
//            text: loader.getStringListForKey(keybase + "disable_warning")[0];
//        }

//        // The instruction text
//        Text {
//            id: showMsgDialogMessage
//            font.family: viewHome.robotoR.name
//            font.pixelSize: 13*viewHome.vmScale
//            textFormat: Text.RichText
//            anchors.top:  showMsgDialogTitle.bottom
//            anchors.topMargin: mainWindow.height*0.028
//            anchors.left: showMsgDialogTitle.left
//            text: loader.getStringListForKey(keybase + "disable_warning")[1];
//        }

//        // Buttons
//        Row{
//            id: rowButtonsDiag
//            anchors.horizontalCenter: parent.horizontalCenter
//            anchors.top: showMsgDialogMessage.bottom
//            anchors.topMargin: mainWindow.height*0.057
//            spacing: mainWindow.width*0.011

//            VMButton{
//                id: btnNo
//                height: mainWindow.height*0.072
//                vmText: "NO"
//                vmFont: gothamM.name
//                vmInvertColors: true
//                onClicked: {
//                    showMsgDialog.close()
//                }
//            }

//            VMButton{
//                id: btnOk
//                vmText: "OK";
//                vmFont: gothamM.name
//                onClicked: {
//                    showMsgDialog.close()
//                    checkAndSave();
//                }
//            }
//        }

//    }
