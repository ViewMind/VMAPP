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
        labelGender.setSelection(0);
        labelFormativeYears.clear();
        labelDocument_number.clear();
        cbConsent.checked = false;
        labelCountry.vmEnabled = true;
        labelDocument_number.enabled = true;
        loader.loadDoctorSelectionInformation();
        assignedDoctor.setModelList(loader.getDoctorNameList());
        var ind = loader.getIndexOfDoctor("");
        if (ind < 0) ind = 0;
        assignedDoctor.setSelection(ind);
        patientUID = "";
        var plist = loader.getProtocolList(false);
        plist.unshift(loader.getStringForKey(keysearch+"labelProtocol"));
        labelProtocol.setModelList(plist);
        labelProtocol.vmEnabled = true;
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
        labelProtocol.setModelList([patInfo.patient_protocol]);
        labelProtocol.vmEnabled = false;


        // Setting the gender
        if (patInfo.sex === "M") labelGender.setSelection(1);
        else if (patInfo.sex === "F") labelGender.setSelection(2);

        // Setting the country.
        var index = loader.getCountryIndexFromCode(patInfo.birthcountry);
        labelCountry.setCurrentIndex(index);

        // The birth date
        labelBirthDate.setISODate(patInfo.birthdate);

        // The assigned doctor.
        loader.loadDoctorSelectionInformation();
        assignedDoctor.setModelList(loader.getDoctorNameList());
        assignedDoctor.setSelection(loader.getIndexOfDoctor(patInfo.doctorid))

        // The formative years.
        if ("formative_years" in patInfo){
            if (patInfo.formative_years >= 0)
                labelFormativeYears.setText(patInfo.formative_years);
        }

        cbConsent.checked = true;
    }

    Dialog {
        id: showTextDialog;
        modal: true
        width: mainWindow.width*0.48
        height: mainWindow.height*0.87

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
            font.pixelSize: 43*viewHome.vmScale
            anchors.top: parent.top
            anchors.topMargin: mainWindow.height*0.029
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
            anchors.topMargin: mainWindow.height*0.058
            clip: true
            //horizontalScrollBarPolicy : Qt.ScrollBarAlwaysOff
            TextEdit {
                id: idContent
                width: parent.width;
                height: parent.height;
                font.family: robotoR.name
                font.pixelSize: 13*viewHome.vmScale
                readOnly: true
                text: showTextDialog.vmContent
                wrapMode: Text.Wrap
            }
        }

        VMButton{
            id: btnClose
            height: mainWindow.height*0.072
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
        font.pixelSize: 43*viewHome.vmScale
        anchors.top:  vmBanner.bottom
        anchors.topMargin: mainWindow.height*0.043
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#3fa2f7"
        text:{
            if (uimap.getStructure() === "P") return loader.getStringForKey(keysearch+"viewTitle");
            if (uimap.getStructure() === "S") return loader.getStringForKey(keysearch+"viewTitleSubject");
         }
    }

    Text {
        id: viewSubTitle
        font.family: robotoR.name
        font.pixelSize: 13*viewHome.vmScale
        anchors.top:  viewTitle.bottom
        anchors.topMargin: mainWindow.height*0.019
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#5499d5"
        text: loader.getStringForKey(keysearch+"viewSubTitle");
    }

    Text {
        id: noAcceptError
        font.family: robotoR.name
        font.pixelSize: 13*viewHome.vmScale
        anchors.top:  viewSubTitle.bottom
        anchors.topMargin: mainWindow.height*0.022
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#ca2026"
        visible: false
        text: loader.getStringForKey(keysearch+"errorNoAccept");
    }

    Row {
        id: rowNames
        width: mainWindow.width*0.344
        spacing: mainWindow.width*0.0125
        anchors.top: viewSubTitle.bottom
        anchors.topMargin: mainWindow.height*0.043
        anchors.horizontalCenter: parent.horizontalCenter
        visible: {
            if (uimap.getStructure() === "P") return true;
            else if (uimap.getStructure() === "S") return false;
        }
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

    Row {

        id: countryAndYearsRow
        width: rowNames.width
        spacing: mainWindow.width*0.0125
        anchors.top: rowNames.bottom
        anchors.topMargin: rowNames.visible? 35 : 0
        anchors.left: rowNames.left
        z: 10

        VMAutoCompleteComboBox{
            id: labelCountry
            width: countryAndYearsRow.width*0.6
            height: mainWindow.height*0.043
            z: 10
            vmLabel: loader.getStringForKey(keysearch+"labelCountry")
            vmList: loader.getCountryList()
            vmValues: loader.getCountryCodeList()
            onVmValuesChanged: labelCountry.setCurrentIndex(loader.getDefaultCountry(false))
            onVmListChanged: labelCountry.setCurrentIndex(loader.getDefaultCountry(false))
            vmEnabled: true
            Keys.onTabPressed: labelFormativeYears.vmFocus = true;
        }

        VMTextDataInput {
            id: labelFormativeYears
            width: countryAndYearsRow.width - countryAndYearsRow.spacing - labelCountry.width
            vmPlaceHolder: loader.getStringForKey(keysearch+"labelFormativeYears");
            Keys.onTabPressed: labelBirthDate.vmFocus = true;
            anchors.bottom: labelCountry.bottom
        }
    }

    Row {

        id: genderAndBDateRow
        width: rowNames.width
        spacing: mainWindow.width*0.0165
        anchors.top: countryAndYearsRow.bottom
        anchors.topMargin: mainWindow.height*0.033
        anchors.left: rowNames.left
        z: 3

        // Gender and Date of Birth.
        VMComboBox2{
            id: labelGender
            width: mainWindow.width*0.113
            anchors.bottom: parent.bottom
            Component.onCompleted: labelGender.setModelList([loader.getStringForKey(keysearch+"labelGender"), "M", "F"])
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
        spacing: mainWindow.width*0.0125
        anchors.top: genderAndBDateRow.bottom
        anchors.topMargin: mainWindow.height*0.026
        anchors.left: rowNames.left
        z: 2

        // Gender and Date of Birth.
        VMComboBox2{
            id: labelProtocol
            width: labelGender.width
            anchors.bottom: parent.bottom
            visible: {
               if (uimap.getStructure() === "P") return false;
               else if (uimap.getStructure() === "S") return true;
            }            
        }

        VMTextDataInput{
            id: labelDocument_number
            width: {
                labelBirthDate.width
                if (uimap.getStructure() === "P") return  rowNames.width;
                else if (uimap.getStructure() === "S") return labelBirthDate.width;
            }
            vmPlaceHolder: loader.getStringForKey(keysearch+"labelDocument_number");
            Keys.onTabPressed: labelAssignedDoctor.vmFocus = true;
        }

    }

    Text{
        id: labelAssignedDoctor
        text: loader.getStringForKey(keysearch+"labelAssignedDoctor");
        color:  "#297fca"
        font.family: "Mono"
        font.pixelSize: 11*viewHome.vmScale
        anchors.left: rowNames.left
        anchors.bottom: assignedDoctor.top
        anchors.bottomMargin: mainWindow.height*0.007
        Keys.onTabPressed: labelName.vmFocus = true;
    }

    VMComboBox2{
        id: assignedDoctor
        width: rowNames.width
        anchors.top: genderIDAndProtocol.bottom
        anchors.topMargin: mainWindow.height*0.072
        anchors.left: rowNames.left
        z: 1
    }

    // Message and Buttons

    Row{
        id: labelConsent
        spacing: mainWindow.width*0.004
        anchors.top:  assignedDoctor.bottom
        anchors.topMargin: mainWindow.height*0.029
        anchors.horizontalCenter: parent.horizontalCenter
        VMCheckBox{
            id: cbConsent
            visible: false
        }
        Text {
            id: consentText
            font.family: robotoR.name
            font.pixelSize: 13*viewHome.vmScale
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
        anchors.topMargin: mainWindow.height*0.038
        spacing: mainWindow.width*0.023

        VMButton{
            id: btnBack
            height: mainWindow.height*0.072
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
                    doctorid: loader.getDoctorUIDByIndex(assignedDoctor.vmCurrentIndex),
                    birthdate: labelBirthDate.vmEnteredText,
                    firstname: labelName.vmEnteredText,
                    lastname: labelLastName.vmEnteredText,
                    sex: labelGender.vmCurrentText,
                    birthcountry: labelCountry.vmCurrentText,
                    puid: patientUID,
                    patient_protocol: labelProtocol.vmCurrentText,
                    formative_years: labelFormativeYears.vmEnteredText
                };

                if (labelFormativeYears.vmEnteredText === "") dbDataReq.formative_years = -1;
                else{
                    var value = parseInt(labelFormativeYears.vmEnteredText)
                    if (isNaN(value) || (value < 0)){
                        labelFormativeYears.vmErrorMsg = loader.getStringForKey(keysearch + "errorInvalidNum");
                        return
                    }
                    else dbDataReq.formative_years = value;
                }

//                if (dbDataReq.birthdate === ""){
//                    labelBirthDate.vmErrorMsg = loader.getStringForKey(keysearch + "errorEmpty");
//                    return;
//                }

                // The absolute must values are the document, the country, the name and the last name.
                if (labelCountry.vmCurrentText === ""){
                    // THIS SHOULD NEVER Happen. If no country has been selected the first one is selected by default and the field does not allow
                    // an empty box or non existing country.
                    // But I leave this code here just in case, I'm wrong as the country code is FUNDAMENTAL in creating the ID.
                    // labelCountry.vmErrorMsg = loader.getStringForKey(keysearch + "errorEmpty");
                    return;
                }

//                if (labelGender.currentIndex === 0){
//                    labelGender.vmErrorMsg = loader.getStringForKey(keysearch + "errorEmpty");
//                    return;
//                }

                if (dbDataReq.uid === ""){
                    labelDocument_number.vmErrorMsg = loader.getStringForKey(keysearch + "errorEmpty");
                    return;
                }

                if (uimap.getStructure() === "S"){
                    if ((labelProtocol.vmCurrentIndex === 0) && (dbDataReq.puid === "")){
                        labelProtocol.vmErrorMsg = loader.getStringForKey(keysearch + "errorEmpty");
                        return;
                    }
                }

                if (uimap.getStructure() === "P"){
                    // This version of the software allows leaving the protocol name empty as to generate the old DISPLAY ID.
                    if (labelProtocol.vmCurrentIndex === 0){
                        dbDataReq.patient_protocol = ""
                    }
                    // Only check for empty last name in this version.
                    if (dbDataReq.lastname === ""){
                        labelLastName.vmErrorMsg = loader.getStringForKey(keysearch + "errorEmpty");
                        return;
                    }
                    // Only check for empty fist name in this version.
                    if (dbDataReq.firstname === ""){
                        labelName.vmErrorMsg = loader.getStringForKey(keysearch + "errorEmpty");
                        return;
                    }
                }

                loader.addNewPatientToDB(dbDataReq)
                swiperControl.currentIndex = swiperControl.vmIndexPatientList;

            }
        }
    }


}
