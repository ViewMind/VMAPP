import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

VMBase {

    id: viewDoctorInformation
    width: viewDoctorInformation.vmWIDTH
    height: viewDoctorInformation.vmHEIGHT

    readonly property string keybase: "viewdrinfo_"

    function clearAllFields(){
        labelAddress.clear();
        labelCity.clear();
        docTypes.currentIndex = 0;
        labelLastName.clear();
        labelDocument_number.clear();
        labelMail.clear();
        labelName.clear();
        labelPhone.clear();
        labelPassword.clear();
        labelVerifyPassword.clear();
        labelProvince.clear();
        labelCountry.enabled = true;
        labelDocument_number.enabled = true;
        cboxDisable.visible = false;
        cboxDisable.checked = false;
        labelCountry.vmEnabled = true;
        setMenuVisibility(false);
    }

    function loadDoctorInformation(){
        var drInfo = loader.getCurrentDoctorInformation();
        labelAddress.setText(drInfo.address);
        labelCity.setText(drInfo.city);
        labelMail.setText(drInfo.email);
        labelName.setText(drInfo.firstname);
        labelLastName.setText(drInfo.lastname);
        labelProvince.setText(drInfo.state);
        labelPhone.setText(drInfo.telephone);

        // Clearing error just in case
        labelAddress.vmErrorMsg = "";
        labelCity.vmErrorMsg = "";
        labelMail.vmErrorMsg = "";
        labelName.vmErrorMsg = "";
        labelLastName.vmErrorMsg = "";
        labelProvince.vmErrorMsg = "";
        labelPhone.vmErrorMsg = "";
        labelDocument_number.vmErrorMsg = "";

        // Substr is used as the first two letters are the country code.
        labelDocument_number.setText(drInfo.uid.substr(2));

        // Setting the id type.
        var idType = drInfo.idtype;
        var model = docTypes.model;
        for (var i = 0; i < model.length; i++){
            if (model[i] === idType){
                docTypes.currentIndex = i;
                break;
            }
        }

        // Setting the country.
        var index = loader.getCountryIndexFromCode(drInfo.countryid);
        labelCountry.setCurrentIndex(index);

        // The country and ID are unique. They can't be modified.
        labelCountry.vmEnabled = false;
        labelDocument_number.enabled = false;

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
            uid: labelDocument_number.vmEnteredText,
            idtype: docTypes.currentText,
            firstname: labelName.vmEnteredText,
            lastname: labelLastName.vmEnteredText,
            countryid: labelCountry.vmCurrentText,
            state: labelProvince.vmEnteredText,
            city: labelCity.vmEnteredText,
            telephone: labelPhone.vmEnteredText,
            email: labelMail.vmEnteredText,
            address: labelAddress.vmEnteredText
        };

        // The absolute must values are the document, the country, the name and the last name.
        if (labelCountry.currentIndex === 0){
            labelCountry.vmErrorMsg = loader.getStringForKey(keybase + "errorEmpty");
            return;
        }

        if (dbData.uid === ""){
            labelDocument_number.vmErrorMsg = loader.getStringForKey(keybase + "errorEmpty");
            return;
        }

        if (dbData.firstname === ""){
            labelName.vmErrorMsg = loader.getStringForKey(keybase + "errorEmpty");
            return;
        }

        if (dbData.lastname === ""){
            labelLastName.vmErrorMsg = loader.getStringForKey(keybase + "errorEmpty");
            return;
        }

        if (loader.addNewDoctorToDB(dbData, labelPassword.getText(), cboxDisable.checked, !cboxDisable.visible)){
            viewDrSelection.updateDrProfile();
            if (cboxDisable.visible) swiperControl.currentIndex = swiperControl.vmIndexPatientList;
            else swiperControl.currentIndex = swiperControl.vmIndexHome;
        }
        else{
            labelDocument_number.vmErrorMsg = loader.getStringForKey(keybase + "drexists");
        }

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


//    VMAutoCompleteComboBox{
//        id: labelCountry2
//        width: 260
//        height: 30
//        vmLabel: loader.getStringForKey(keybase+"labelCountry")
//        vmList: loader.getCountryList()
//        vmValues: loader.getCountryCodeList()
//        onVmValuesChanged: labelCountry.setCurrentIndex(loader.getDefaultCountry(false))
//        onVmListChanged: labelCountry.setCurrentIndex(loader.getDefaultCountry(false))
//        anchors.top: diagSubTitle.bottom
//                anchors.horizontalCenter: parent.horizontalCenter
//                z: 10
//                anchors.topMargin: 100
//    }

    Column {

        id: mainForm
        width: 436
        anchors.top: diagSubTitle.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 23

        // The form fields

        Row {
            id: rowNameAndLastName
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 16
            width: parent.width

            // Name and institution
            VMTextDataInput{
                id: labelName
                width: (rowNameAndLastName.width - rowNameAndLastName.spacing)/2
                vmPlaceHolder: loader.getStringForKey(keybase+"labelName");
                Keys.onTabPressed: labelLastName.vmFocus = true;
            }

            VMTextDataInput{
                id: labelLastName
                width: (rowNameAndLastName.width - rowNameAndLastName.spacing)/2
                vmPlaceHolder: loader.getStringForKey(keybase+"labelLastName");
                Keys.onTabPressed: labelDocument_number.vmFocus = true;
            }

        }

        Row {
            id: rowDocument
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 16
            width: parent.width

            // Document type and number.
            VMComboBox{
                id: docTypes
                width: 162
                vmModel:  loader.getStringListForKey(keybase+"docTypes")
                font.family: robotoR.name
                anchors.bottom: labelDocument_number.bottom
            }

            VMTextDataInput{
                id: labelDocument_number
                width: (rowDocument.width - rowDocument.spacing - docTypes.width)
                vmPlaceHolder: loader.getStringForKey(keybase+"labelDocument_number");
                Keys.onTabPressed: labelCountry.vmFocus = true;
            }

        }

        Row {
            id: rowCountryAndProvince
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 16
            width: parent.width
            z: 10

            VMAutoCompleteComboBox{
                id: labelCountry
                width: 260
                height: 30
                vmMaxOptions: 7
                vmLabel: loader.getStringForKey(keybase+"labelCountry")
                vmList: loader.getCountryList()
                vmValues: loader.getCountryCodeList()
                onVmValuesChanged: labelCountry.setCurrentIndex(loader.getDefaultCountry(false))
                onVmListChanged: labelCountry.setCurrentIndex(loader.getDefaultCountry(false))
                anchors.bottom: labelProvince.bottom
                Keys.onTabPressed: labelProvince.vmFocus = true;
            }

            VMTextDataInput{
                id: labelProvince
                width: (rowCountryAndProvince.width - rowCountryAndProvince.spacing - labelCountry.width)
                vmPlaceHolder: loader.getStringForKey(keybase+"labelProvince");
                Keys.onTabPressed: labelCity.vmFocus = true;
            }

        }

        Row {
            id: rowCityAndAddress
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 16
            width: parent.width

            VMTextDataInput{
                id: labelCity
                width: 162
                vmPlaceHolder: loader.getStringForKey(keybase+"labelCity");
                 Keys.onTabPressed: labelAddress.vmFocus = true;
            }

            VMTextDataInput{
                id: labelAddress
                width: (rowCityAndAddress.width - rowCityAndAddress.spacing - labelCity.width)
                vmPlaceHolder: loader.getStringForKey(keybase+"labelAddress");
                Keys.onTabPressed: labelPhone.vmFocus = true;
            }

        }

        Row {
            id: phoneAndEmail
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 16
            width: parent.width

            // Phone, address and email
            VMTextDataInput{
                id: labelPhone
                width: 162
                vmPlaceHolder: loader.getStringForKey(keybase+"labelPhone");
                Keys.onTabPressed: labelMail.vmFocus = true;
            }

            VMTextDataInput{
                id: labelMail
                width: (phoneAndEmail.width - phoneAndEmail.spacing - labelPhone.width)
                vmPlaceHolder: loader.getStringForKey(keybase+"labelMail");
                Keys.onTabPressed: labelPassword.vmFocus = true;
            }

        }

        Row {
            id: rowPasswords
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 16
            width: parent.width

            VMPasswordField{
                id: labelPassword;
                width: 162
                vmLabelText: loader.getStringForKey(keybase+"password");
                Keys.onTabPressed: labelVerifyPassword.vmFocus = true;
            }

            VMPasswordField{
                id: labelVerifyPassword
                width: (rowPasswords.width - rowPasswords.spacing - labelPassword.width)
                vmLabelText: loader.getStringForKey(keybase+"verify_password");
                Keys.onTabPressed: labelName.vmFocus = true;
            }
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
