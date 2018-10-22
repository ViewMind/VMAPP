import QtQuick 2.6
import QtQuick.Controls 2.3

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
        labelCountry.currentIndex = index+1; // 1  is added because 0 is no selection.

        // The country and ID are unique. They can't be modified.
        labelCountry.enabled = false;
        labelDocument_number.enabled = false;
    }

    Button {
        id: btnConfSettings
        scale: btnConfSettings.pressed? 0.9:1
        Behavior on scale{
            NumberAnimation {
                duration: 25
            }
        }
        background: Rectangle {
            id: btnConfSettingsRect
            radius: 3
            color: btnConfSettings.pressed? "#e8f2f8" :"#ffffff"
            width: 178
            height: 43
        }
        contentItem: Text{
            anchors.centerIn: btnConfSettingsRect
            font.family: gothamM.name
            font.pixelSize: 13
            text: loader.getStringForKey("viewhome_btnConfSettings");
            color: "#88b2d0"
        }
        anchors{
            right: parent.right
            rightMargin: 63
            top: parent.top
            topMargin: 19
        }
        onClicked: {
            swiperControl.currentIndex = swiperControl.vmIndexHome
            viewSettings.open()
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
            }

            VMTextDataInput{
                id: labelLastName
                width: (rowNameAndLastName.width - rowNameAndLastName.spacing)/2
                vmPlaceHolder: loader.getStringForKey(keybase+"labelLastName");
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
            }

        }

        Row {
            id: rowCountryAndProvince
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 16
            width: parent.width

            // Country, province and city
            VMComboBox{
                id: labelCountry
                vmModel: {
                    var data = loader.getCountryList();
                    data.unshift(loader.getStringForKey(keybase+"labelCountry"));
                    return data;
                }
                currentIndex:  loader.getDefaultCountry();
                width: 260
                font.family: robotoR.name
                font.pixelSize: 13
                anchors.bottom: labelProvince.bottom
            }

            VMTextDataInput{
                id: labelProvince
                width: (rowCountryAndProvince.width - rowCountryAndProvince.spacing - labelCountry.width)
                vmPlaceHolder: loader.getStringForKey(keybase+"labelProvince");
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
            }

            VMTextDataInput{
                id: labelAddress
                width: (rowCityAndAddress.width - rowCityAndAddress.spacing - labelCity.width)
                vmPlaceHolder: loader.getStringForKey(keybase+"labelAddress");
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
            }

            VMTextDataInput{
                id: labelMail
                width: (phoneAndEmail.width - phoneAndEmail.spacing - labelPhone.width)
                vmPlaceHolder: loader.getStringForKey(keybase+"labelMail");
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
            }

            VMPasswordField{
                id: labelVerifyPassword
                width: (rowPasswords.width - rowPasswords.spacing - labelPassword.width)
                vmLabelText: loader.getStringForKey(keybase+"verify_password");
            }
        }

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
                swiperControl.currentIndex = swiperControl.vmIndexHome;
            }
        }

        VMButton{
            id: btnSave
            vmText: loader.getStringForKey(keybase+"btnSave");
            vmFont: gothamM.name
            onClicked: {

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
                    countryid: labelCountry.currentText,
                    state: labelProvince.vmEnteredText,
                    city: labelCity.vmEnteredText,
                    medicalinstitution: labelInstitution.vmEnteredText,
                    telephone: labelPhone.vmEnteredText,
                    email: labelMail.vmEnteredText,
                    address: labelAddress.vmEnteredText,
                    password: labelPassword.getText()
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

                loader.addNewDoctorToDB(dbData);
                viewDrSelection.updateDrProfile();
                swiperControl.currentIndex = swiperControl.vmIndexHome;

            }
        }
    }



}
