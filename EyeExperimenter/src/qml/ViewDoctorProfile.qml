import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

VMBase {

    id: viewDoctorInformation
    width: viewDoctorInformation.vmWIDTH
    height: viewDoctorInformation.vmHEIGHT

    readonly property string keybase: "viewdrinfo_"
    property string drUID: "";

    function clearAllFields(){
        labelLastName.clear();
        labelMail.clear();
        labelName.clear();
        labelPassword.clear();
        labelVerifyPassword.clear();
        cboxDisable.visible = false;
        cboxDisable.checked = false;
        drUID = "";
        setMenuVisibility(false);
    }

    function loadDoctorInformation(){
        var drInfo = loader.getCurrentDoctorInformation();
        labelMail.setText(drInfo.email);
        labelName.setText(drInfo.firstname);
        labelLastName.setText(drInfo.lastname);
        drUID = drInfo.uid;

        // Clearing error just in case
        labelMail.vmErrorMsg = "";
        labelName.vmErrorMsg = "";
        labelLastName.vmErrorMsg = "";

        cboxDisable.visible = true;
        setMenuVisibility(true);
    }

    function checkAndSave(){
        if (labelPassword.getText() !== labelVerifyPassword.getText()){
            labelPassword.vmErrorMsg = loader.getStringForKey(keybase + "password_match");
            return;
        }

        // THIS IS THE TABLE DATA.
        var dbData = {
            firstname: labelName.vmEnteredText,
            lastname: labelLastName.vmEnteredText,
            email: labelMail.vmEnteredText,
            uid: drUID
        };

        if (dbData.firstname === ""){
            labelName.vmErrorMsg = loader.getStringForKey(keybase + "errorEmpty");
            return;
        }

        if (dbData.lastname === ""){
            labelLastName.vmErrorMsg = loader.getStringForKey(keybase + "errorEmpty");
            return;
        }

        loader.addNewDoctorToDB(dbData, labelPassword.getText(), cboxDisable.checked)
        viewDrSelection.updateDrProfile();
        if (cboxDisable.visible) swiperControl.currentIndex = swiperControl.vmIndexPatientList;
        else swiperControl.currentIndex = swiperControl.vmIndexHome;
    }

    Dialog {
        id: showMsgDialog;
        modal: true
        width: 614
        height: 280
        y: (parent.height - height)/2
        x: (parent.width - width)/2
        closePolicy: Popup.NoAutoClose

        contentItem: Rectangle {
            id: rectShowMsgDialog
            anchors.fill: parent
            layer.enabled: true
            layer.effect: DropShadow{
                radius: 5
            }
        }

        VMDialogCloseButton {
            id: btnClose
            anchors.top: parent.top
            anchors.topMargin: 22
            anchors.right: parent.right
            anchors.rightMargin: 25
            onClicked: {
                showMsgDialog.close();
            }
        }

        // The instruction text
        Text {
            id: showMsgDialogTitle
            font.family: viewHome.gothamB.name
            font.pixelSize: 43
            anchors.top: parent.top
            anchors.topMargin: 50
            anchors.left: parent.left
            anchors.leftMargin: 20
            color: "#297fca"
            text: loader.getStringListForKey(keybase + "disable_warning")[0];
        }

        // The instruction text
        Text {
            id: showMsgDialogMessage
            font.family: viewHome.robotoR.name
            font.pixelSize: 13
            textFormat: Text.RichText
            anchors.top:  showMsgDialogTitle.bottom
            anchors.topMargin: 20
            anchors.left: showMsgDialogTitle.left
            text: loader.getStringListForKey(keybase + "disable_warning")[1];
        }

        // Buttons
        Row{
            id: rowButtonsDiag
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: showMsgDialogMessage.bottom
            anchors.topMargin: 40
            spacing: 15

            VMButton{
                id: btnNo
                height: 50
                vmText: "NO"
                vmFont: gothamM.name
                vmInvertColors: true
                onClicked: {
                    showMsgDialog.close()
                }
            }

            VMButton{
                id: btnOk
                vmText: "OK";
                vmFont: gothamM.name
                onClicked: {
                    showMsgDialog.close()
                    checkAndSave();
                }
            }
        }

    }

    // The Doctor Information Title and subtitle
    Text {
        id: diagTitle
        font.pixelSize: 43
        font.family: gothamB.name
        color: "#297FCA"
        text: loader.getStringForKey(keybase+"title");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: vmBanner.bottom
        anchors.topMargin: 30
    }

    Text {
        id: diagSubTitle
        font.pixelSize: 11
        font.family: gothamR.name
        color: "#cfcfcf"
        text: loader.getStringForKey(keybase+"subtitle");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: diagTitle.bottom
        anchors.topMargin: 11
    }

    Column {

        id: mainForm
        width: 436
        anchors.top: diagSubTitle.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 40

        // The form fields

        // Name and institution
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

    // The disable button
    VMCheckBox{
        id: cboxDisable
        text: loader.getStringForKey(keybase + "disable");
        anchors.top: mainForm.bottom
        anchors.topMargin: 20
        anchors.left: mainForm.left
        visible: false;
    }

    // Buttons
    Row{
        id: rowButtons
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: mainForm.bottom
        anchors.topMargin: 50
        spacing: 15

        VMButton{
            id: btnBack
            height: 50
            vmText: loader.getStringForKey(keybase+"btnBack");
            vmFont: gothamM.name
            vmInvertColors: true
            onClicked: {
                if (cboxDisable.visible) swiperControl.currentIndex = swiperControl.vmIndexPatientList;
                else swiperControl.currentIndex = swiperControl.vmIndexHome;
            }
        }

        VMButton{
            id: btnSave
            vmText: loader.getStringForKey(keybase+"btnSave");
            vmFont: gothamM.name
            onClicked: {
                if (cboxDisable.checked){
                    showMsgDialog.open();
                }
                else{
                    checkAndSave();
                }
            }
        }
    }



}
