import QtQuick 2.6
import QtQuick.Controls 2.3

VMBase {

    id: viewPatientRegistration
    width: viewPatientRegistration.vmWIDTH
    height: viewPatientRegistration.vmHEIGHT

    readonly property string keysearch: "viewpatientreg_"

    Connections{
        target: loader
        onNewPatientAdded:{
            clearAll();
            loader.getPatientListFromDB();
            swiperControl.currentIndex = swiperControl.vmIndexStudyStart;
        }
    }

    function clearAll(){
        labelBirthDate.clear();
        labelName.clear();
        labelLastName.clear();
        labelGender.currentIndex = 0;
        docTypes.currentIndex = 0;
        labelDocument_number.clear();
        labelProvince.clear();
        labelCity.clear();
    }

    // Title and subtitle
    Text {
        id: viewTitle
        font.family: gothamB.name
        font.pixelSize: 43
        anchors.top:  vmBanner.bottom
        anchors.topMargin: 46
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#3fa2f7"
        text: loader.getStringForKey(keysearch+"viewTitle");
    }

    Text {
        id: viewSubTitle
        font.family: robotoR.name
        font.pixelSize: 13
        anchors.top:  viewTitle.bottom
        anchors.topMargin: 23
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#5499d5"
        text: loader.getStringForKey(keysearch+"viewSubTitle");
    }

    // Name and last name
    VMTextDataInput{
        id: labelName
        width: 212
        anchors.left: parent.left
        anchors.leftMargin: 420
        anchors.top: viewSubTitle.bottom
        anchors.topMargin: 39
        vmPlaceHolder: loader.getStringForKey(keysearch+"labelName");
    }

    VMTextDataInput{
        id: labelLastName
        width: 212
        anchors.left: labelName.right
        anchors.leftMargin: 16
        anchors.top: viewSubTitle.bottom
        anchors.topMargin: 39
        vmPlaceHolder: loader.getStringForKey(keysearch+"labelLastName");
    }

    // Country, province and city
    VMComboBox{
        id: labelCountry
        width: 440
        vmModel: {
            var data = loader.getCountryList();
            data.unshift(loader.getStringForKey(keysearch+"labelCountry"));
            return data;
        }
        currentIndex: loader.getDefaultCountry();
        font.family: viewHome.robotoR.name
        font.pixelSize: 13
        anchors.top: labelLastName.bottom
        anchors.topMargin: 22
        anchors.left: labelName.left
    }

    // Gender and Date of Birth.
    VMComboBox{
        id: labelGender
        width: 144
        vmModel: [loader.getStringForKey(keysearch+"labelGender"), "M", "F"]
        font.family: viewHome.robotoR.name
        anchors.top: labelCountry.bottom
        anchors.topMargin: 23
        anchors.left: labelName.left
    }

    VMDateInputField{
        id: labelBirthDate
        width: 268
        //vmCalendarInput: true
        anchors.left: labelGender.right
        anchors.leftMargin: 28
        anchors.bottom: labelGender.bottom
        vmPlaceHolder: loader.getStringForKey(keysearch+"labelBirthDate");
    }

    // Document type and number.
    VMComboBox{
        id: docTypes
        width: 144
        vmModel:  loader.getStringListForKey(keysearch+"docTypes")
        font.family: viewHome.robotoR.name
        anchors.top: labelGender.bottom
        anchors.topMargin: 30
        anchors.left: labelName.left
    }

    VMTextDataInput{
        id: labelDocument_number
        width: 268
        anchors.left: docTypes.right
        anchors.leftMargin: 28
        anchors.bottom: docTypes.bottom
        vmPlaceHolder: loader.getStringForKey(keysearch+"labelDocument_number");
    }


    VMTextDataInput{
        id: labelProvince
        width: 163
        anchors.left: labelName.left
        anchors.top: docTypes.bottom
        anchors.topMargin: 22
        vmPlaceHolder: loader.getStringForKey(keysearch+"labelProvince");
    }

    VMTextDataInput{
        id: labelCity
        width: 261
        anchors.left: labelProvince.right
        anchors.leftMargin: 16
        anchors.top: docTypes.bottom
        anchors.topMargin: 22
        vmPlaceHolder: loader.getStringForKey(keysearch+"labelCity");
    }

    // Message and Buttons
    Text {
        id: labelConsent
        font.family: robotoR.name
        font.pixelSize: 13
        anchors.top:  labelCity.bottom
        anchors.topMargin: 41
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#58595b"
        text: loader.getStringForKey(keysearch+"labelConsent");
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
                swiperControl.currentIndex = swiperControl.vmIndexStudyStart;
            }
        }

        VMButton{
            id: btnSave
            vmText: loader.getStringForKey(keysearch+"btnContinue");
            vmFont: viewHome.gothamM.name
            onClicked: {
                // THIS IS THE TABLE DATA.
                var dbDataReq = {
                   uid: labelDocument_number.vmEnteredText,
                   doctorid: loader.getConfigurationString(vmDefines.vmCONFIG_DOCTOR_UID),
                   birthdate: labelBirthDate.vmEnteredText,
                   firstname: labelName.vmEnteredText,
                   lastname: labelLastName.vmEnteredText,
                   idtype: docTypes.currentText,
                   sex: labelGender.currentText,
                   birthcountry: labelCountry.currentText
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
                if (labelCountry.currentIndex === 0){
                    labelCountry.vmErrorMsg = loader.getStringForKey(keysearch + "errorEmpty");
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

                loader.addNewPatientToDB(dbDataReq,dbDataOpt);
            }
        }
    }


}
