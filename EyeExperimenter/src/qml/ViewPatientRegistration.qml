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
    property string patientUID: "";

    function clearAll(){
        labelBirthDate.clear();
        labelName.clear();
        labelLastName.clear();
        labelGender.currentIndex = 0;
        labelDocument_number.clear();
        cbConsent.checked = false;
        labelCountry.vmEnabled = true;
        labelDocument_number.enabled = true;
        loader.loadDoctorSelectionInformation();
        assignedDoctor.vmModel = loader.getDoctorNameList();
        var ind = loader.getIndexOfDoctor("");
        if (ind < 0) ind = 0;
        assignedDoctor.currentIndex = ind;
        patientUID = "";
        var plist = loader.getProtocolList(false);
        plist.unshift(loader.getStringForKey(keysearch+"labelProtocol"));
        labelProtocol.vmModel = plist;
    }

    function loadPatientInformation(){
        var patInfo = loader.getCurrentPatientInformation();
        labelName.setText(patInfo.firstname);
        labelLastName.setText(patInfo.lastname);

        //        for (var k in patInfo){
        //            console.log(k + "=" + patInfo[k]);
        //        }

        patientUID = patInfo.puid;
        labelDocument_number.setText(patInfo.displayID)
        labelDocument_number.enabled = false;
        labelProtocol.vmModel = [patInfo.patient_protocol];
        labelProtocol.enabled = false;


        // Setting the gender
        if (patInfo.sex === "M") labelGender.currentIndex = 1;
        else labelGender.currentIndex = 2;

        // Setting the country.
        var index = loader.getCountryIndexFromCode(patInfo.birthcountry);
        labelCountry.setCurrentIndex(index);

        // The birth date
        labelBirthDate.setISODate(patInfo.birthdate);

        // The assigned doctor.
        loader.loadDoctorSelectionInformation();
        assignedDoctor.vmModel = loader.getDoctorNameList();
        assignedDoctor.currentIndex = loader.getIndexOfDoctor(patInfo.doctorid);

        cbConsent.checked = true;
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

    Row {
        id: rowNames
        width: 440
        spacing: 16
        anchors.top: viewSubTitle.bottom
        anchors.topMargin: 30
        anchors.horizontalCenter: parent.horizontalCenter
        // visible: false
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

    VMAutoCompleteComboBox{
        id: labelCountry
        width: rowNames.width
        height: 30
        z: 10
        vmLabel: loader.getStringForKey(keysearch+"labelCountry")
        vmList: loader.getCountryList()
        vmValues: loader.getCountryCodeList()
        onVmValuesChanged: labelCountry.setCurrentIndex(loader.getDefaultCountry(false))
        onVmListChanged: labelCountry.setCurrentIndex(loader.getDefaultCountry(false))
        vmEnabled: true
        Keys.onTabPressed: labelBirthDate.vmFocus = true;
        anchors.top: rowNames.bottom
        anchors.topMargin: 35
        anchors.left: rowNames.left
    }

    Row {

        id: genderAndBDateRow
        width: rowNames.width
        spacing: 16
        anchors.top: labelCountry.bottom
        anchors.topMargin: 23
        anchors.left: rowNames.left

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

    Row {

        id: genderIDAndProtocol
        width: rowNames.width
        spacing: 16
        anchors.top: genderAndBDateRow.bottom
        anchors.topMargin: 18
        anchors.left: rowNames.left

        // Gender and Date of Birth.
        VMComboBox{
            id: labelProtocol
            width: labelGender.width
            font.family: viewHome.robotoR.name
            anchors.bottom: parent.bottom
        }

        VMTextDataInput{
            id: labelDocument_number
            width: labelBirthDate.width
            vmPlaceHolder: loader.getStringForKey(keysearch+"labelDocument_number");
            Keys.onTabPressed: labelAssignedDoctor.vmFocus = true;
        }

    }

    Text{
        id: labelAssignedDoctor
        text: loader.getStringForKey(keysearch+"labelAssignedDoctor");
        color:  "#297fca"
        font.family: "Mono"
        font.pixelSize: 11
        anchors.left: rowNames.left
        anchors.bottom: assignedDoctor.top
        anchors.bottomMargin: 5
        Keys.onTabPressed: labelName.vmFocus = true;
    }

    VMComboBox{
        id: assignedDoctor
        width: rowNames.width
        //vmModel:
        font.family: viewHome.robotoR.name
        anchors.top: genderIDAndProtocol.bottom
        anchors.topMargin: 50
        anchors.left: rowNames.left
    }

    // Message and Buttons

    Row{
        id: labelConsent
        spacing: 5
        anchors.top:  assignedDoctor.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        VMCheckBox{
            id: cbConsent
            visible: false
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
            visible: false
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

                // Consent does not need to be verified
                //                if (!cbConsent.checked){
                //                    noAcceptError.visible = true;
                //                    return;
                //                }
                //                else noAcceptError.visible = false;

                // THIS IS THE TABLE DATA.
                var dbDataReq = {
                    displayID: labelDocument_number.vmEnteredText,
                    doctorid: loader.getDoctorUIDByIndex(assignedDoctor.currentIndex),
                    birthdate: labelBirthDate.vmEnteredText,
                    firstname: labelName.vmEnteredText,
                    lastname: labelLastName.vmEnteredText,
                    sex: labelGender.currentText,
                    birthcountry: labelCountry.vmCurrentText,
                    puid: patientUID,
                    patient_protocol: labelProtocol.currentText
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


//                if ((labelProtocol.currentIndex === 0) && (dbDataReq.puid === "")){
//                    labelProtocol.vmErrorMsg = loader.getStringForKey(keysearch + "errorEmpty");
//                    return;
//                }

                // This version of the software allows leaving the protocol name empty as to generate the old DISPLAY ID.
                if (labelProtocol.currentIndex === 0){
                    dbDataReq.patient_protocol = ""
                }

                // Since the last name will not be entered, it will always be empty.

                if (dbDataReq.lastname === ""){
                    labelLastName.vmErrorMsg = loader.getStringForKey(keysearch + "errorEmpty");
                    return;
                }

                loader.addNewPatientToDB(dbDataReq)
                swiperControl.currentIndex = swiperControl.vmIndexPatientList;

            }
        }
    }


}
