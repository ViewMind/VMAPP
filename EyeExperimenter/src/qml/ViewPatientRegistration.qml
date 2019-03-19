import QtQuick 2.6
import QtQuick.Controls 2.3
//import QtQuick.Controls 1.4
import QtGraphicalEffects 1.0

VMBase {

    id: viewPatientRegistration
    width: viewPatientRegistration.vmWIDTH
    height: viewPatientRegistration.vmHEIGHT

    readonly property string keysearch: "viewpatientreg_"
    property bool vmIsNew: true;

    function clearAll(){
        labelBirthDate.clear();
        labelName.clear();
        labelLastName.clear();
        labelGender.currentIndex = 0;
        docTypes.currentIndex = 0;
        labelDocument_number.clear();
        labelProvince.clear();
        labelCity.clear();
        cbConsent.checked = false;
        labelCountry.vmEnabled = true;
        labelDocument_number.enabled = true;
        vmIsNew = true;
    }

    function loadPatientInformation(){
        var patInfo = loader.getCurrentPatientInformation();
        labelCity.setText(patInfo.city);
        labelName.setText(patInfo.firstname);
        labelLastName.setText(patInfo.lastname);
        labelProvince.setText(patInfo.state);

        //        for (var keyputa in patInfo){
        //            console.log(keyputa + ": " + patInfo[keyputa]);
        //        }

        // Substr is used as the first two letters are the country code.
        if ("puid" in patInfo){
            labelDocument_number.setText(patInfo.puid.substr(2));
        }
        else if ("uid" in patInfo){
            labelDocument_number.setText(patInfo.uid.substr(2));
        }

        // Setting the document type.
        var idType = patInfo.idtype;
        var model = docTypes.model;
        for (var i = 0; i < model.length; i++){
            if (model[i] === idType){
                docTypes.currentIndex = i;
                break;
            }
        }

        // Setting the gender
        if (patInfo.sex === "M") labelGender.currentIndex = 1;
        else labelGender.currentIndex = 2;

        // Setting the country.
        var index = loader.getCountryIndexFromCode(patInfo.birthcountry);
        labelCountry.setCurrentIndex(index);
        labelCountry.vmEnabled = false;

        // The country and ID are unique. They can't be modified.
        //labelCountry.enabled = false;
        labelDocument_number.enabled = false;

        labelBirthDate.setISODate(patInfo.birthdate);

        cbConsent.checked = true;
        vmIsNew = false;
    }


    Dialog {
        id: showTextDialog;
        modal: true
        width: 614
        height: 600

        property string vmContent: ""
        property string vmTitle: ""

        y: (parent.height - height)/2
        x: (parent.width - width)/2
        closePolicy: Popup.NoAutoClose

        contentItem: Rectangle {
            id: rectDialog
            anchors.fill: parent
            layer.enabled: true
            layer.effect: DropShadow{
                radius: 5
            }
        }

        Text {
            id: diagTitle
            font.family: viewHome.gothamB.name
            font.pixelSize: 43
            anchors.top: parent.top
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#297fca"
            text: showTextDialog.vmTitle

        }
        ScrollView {
            id: idScrollView
            width: showTextDialog.width*0.9;
            contentWidth: width
            height: showTextDialog.height*0.62;
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top : diagTitle.bottom
            anchors.topMargin: 40
            clip: true
            //horizontalScrollBarPolicy : Qt.ScrollBarAlwaysOff
            TextEdit {
                id: idContent
                width: parent.width;
                height: parent.height;
                font.family: robotoR.name
                font.pixelSize: 13
                readOnly: true
                text: showTextDialog.vmContent
                wrapMode: Text.Wrap
            }
        }

        VMButton{
            id: btnClose
            height: 50
            vmText: "OK";
            vmFont: viewHome.gothamM.name
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 20;
            onClicked: {
                showTextDialog.close();
            }
        }

    }

    // Title and subtitle
    Text {
        id: viewTitle
        font.family: gothamB.name
        font.pixelSize: 43
        anchors.top:  vmBanner.bottom
        anchors.topMargin: 30
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#3fa2f7"
        text: loader.getStringForKey(keysearch+"viewTitle");
    }

    Text {
        id: viewSubTitle
        font.family: robotoR.name
        font.pixelSize: 13
        anchors.top:  viewTitle.bottom
        anchors.topMargin: 13
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#5499d5"
        text: loader.getStringForKey(keysearch+"viewSubTitle");
    }

    Text {
        id: noAcceptError
        font.family: robotoR.name
        font.pixelSize: 13
        anchors.top:  viewSubTitle.bottom
        anchors.topMargin: 15
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#ca2026"
        visible: false
        text: loader.getStringForKey(keysearch+"errorNoAccept");
    }


    Column {

        id: mainForm
        spacing: 30
        width: 440
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: viewSubTitle.bottom
        anchors.topMargin: 35
        z: 10
        Row {
            id: rowNames
            width: parent.width
            spacing: 16

            // Name and last name
            VMTextDataInput{
                id: labelName
                width: parent.width/2 - parent.spacing
                vmPlaceHolder: loader.getStringForKey(keysearch+"labelName");
                Keys.onTabPressed: labelLastName.vmFocus = true;
            }

            VMTextDataInput{
                id: labelLastName
                width: labelName.width
                vmPlaceHolder: loader.getStringForKey(keysearch+"labelLastName");
                Keys.onTabPressed: labelCountry.vmFocus = true;
            }
        }

        Rectangle{
            id: spacer
            border.width: 0;
            color: "#FFFFFF";
            width: parent.width
            height: 1
        }

        VMAutoCompleteComboBox{
            id: labelCountry
            width: parent.width
            height: 30
            z: 10
            vmLabel: loader.getStringForKey(keysearch+"labelCountry")
            vmList: loader.getCountryList()
            vmValues: loader.getCountryCodeList()
            onVmValuesChanged: labelCountry.setCurrentIndex(loader.getDefaultCountry(false))
            onVmListChanged: labelCountry.setCurrentIndex(loader.getDefaultCountry(false))
            vmEnabled: true
            Keys.onTabPressed: labelBirthDate.vmFocus = true;
        }

        Row {

            id: genderAndBDateRow
            width: parent.width
            spacing: 16

            // Gender and Date of Birth.
            VMComboBox{
                id: labelGender
                width: 144
                vmModel: [loader.getStringForKey(keysearch+"labelGender"), "M", "F"]
                font.family: viewHome.robotoR.name
                anchors.bottom: parent.bottom
            }

            VMDateInputField{
                id: labelBirthDate
                width: parent.width - labelGender.width - parent.spacing
                //vmCalendarInput: true
                vmPlaceHolder: loader.getStringForKey(keysearch+"labelBirthDate");
                Keys.onTabPressed: labelDocument_number.vmFocus = true;
                anchors.verticalCenter: parent.verticalCenter
            }

        }

        VMTextDataInput{
            id: labelDocument_number
            width: parent.width
            vmPlaceHolder: loader.getStringForKey(keysearch+"labelDocument_number");
            Keys.onTabPressed: labelProvince.vmFocus = true;
        }

        VMComboBox{
            id: assignedDoctor
            width: parent.width
            //vmModel: [loader.getStringForKey(keysearch+"labelGender"), "M", "F"]
            font.family: viewHome.robotoR.name
        }
    }

    // Message and Buttons

    Row{
        id: labelConsent
        spacing: 5
        anchors.top:  mainForm.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        VMCheckBox{
            id: cbConsent
        }
        Text {
            id: consentText
            font.family: robotoR.name
            font.pixelSize: 13
            color: "#58595b"
            text: loader.getStringForKey(keysearch+"labelConsent");
            anchors.verticalCenter: cbConsent.verticalCenter
            onLinkActivated: {
                showTextDialog.vmContent = loader.loadTextFile(":/agreements/" + link + ".txt");
                showTextDialog.vmTitle = loader.getStringForKey(keysearch + link);
                showTextDialog.open();
            }
        }
    }

    // Buttons
    Row{
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: labelConsent.bottom
        anchors.topMargin: 26
        spacing: 29

        VMButton{
            id: btnBack
            height: 50
            vmText: loader.getStringForKey(keysearch+"btnCancel");
            vmFont: viewHome.gothamM.name
            vmInvertColors: true
            onClicked: {
                clearAll();
                swiperControl.currentIndex = swiperControl.vmIndexPatientList;
            }
        }

        VMButton{
            id: btnSave
            vmText: loader.getStringForKey(keysearch+"btnContinue");
            vmFont: viewHome.gothamM.name
            onClicked: {

                if (!cbConsent.checked){
                    noAcceptError.visible = true;
                    return;
                }
                else noAcceptError.visible = false;

                // THIS IS THE TABLE DATA.
                var dbDataReq = {
                    puid: labelDocument_number.vmEnteredText,
                    doctorid: loader.getConfigurationString(vmDefines.vmCONFIG_DOCTOR_UID),
                    birthdate: labelBirthDate.vmEnteredText,
                    firstname: labelName.vmEnteredText,
                    lastname: labelLastName.vmEnteredText,
                    idtype: docTypes.currentText,
                    sex: labelGender.currentText,
                    birthcountry: labelCountry.vmCurrentText
                };

                var dbDataOpt = {
                    state: labelProvince.vmEnteredText,
                    city: labelCity.vmEnteredText
                };


                if (dbDataReq.birthdate === ""){
                    labelBirthDate.vmErrorMsg = loader.getStringForKey(keysearch + "errorEmpty");
                    return;
                }

                // The absolute must values are the document, the country, the name and the last name.
                if (labelCountry.vmCurrentText === ""){
                    // THIS SHOULD NEVER Happen. If no country has been selected the first one is selected by default and the field does not allow
                    // an empty box or non existing country.
                    // But I leave this code here just in case, I'm wrong as the country code is FUNDAMENTAL in creating the ID.
                    // labelCountry.vmErrorMsg = loader.getStringForKey(keysearch + "errorEmpty");
                    return;
                }

                if (labelGender.currentIndex === 0){
                    labelGender.vmErrorMsg = loader.getStringForKey(keysearch + "errorEmpty");
                    return;
                }

                if (dbDataReq.uid === ""){
                    labelDocument_number.vmErrorMsg = loader.getStringForKey(keysearch + "errorEmpty");
                    return;
                }

                if (dbDataReq.firstname === ""){
                    labelName.vmErrorMsg = loader.getStringForKey(keysearch + "errorEmpty");
                    return;
                }

                if (dbDataReq.lastname === ""){
                    labelLastName.vmErrorMsg = loader.getStringForKey(keysearch + "errorEmpty");
                    return;
                }

                //console.log("Country Index: " + dbData.countryid + ". Text: " + labelCountry.currentText);

                if (loader.addNewPatientToDB(dbDataReq,dbDataOpt,vmIsNew)){
                    swiperControl.currentIndex = swiperControl.vmIndexPatientList;
                }
                else{
                    labelDocument_number.vmErrorMsg = loader.getStringForKey(keysearch + "errorExists");
                }
            }
        }
    }


}
