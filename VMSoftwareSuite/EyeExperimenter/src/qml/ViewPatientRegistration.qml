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

    function loadPatientInformation(){
        var patInfo = loader.getCurrentSubjectInfo();

        labelName.clear();
        labelLastName.clear();
        labelBirthDate.clear();
        labelGender.setSelection(0);
        labelFormativeYears.clear();
        labelInstitutionProvidedID.clear();
        labelAge.clear();

        // Loading the default county.
        var index = loader.getDefaultCountry(false);
        labelCountry.setCurrentIndex(index);

        // Resetting the patient UID to erase any previous one.
        patientUID = "";

        if ("local_id" in patInfo){
            patientUID = patInfo.local_id;
            labelName.setText(patInfo.name);
            labelLastName.setText(patInfo.lastname);
            labelInstitutionProvidedID.setText(patInfo.supplied_institution_id)
            labelAge.setText(patInfo.age);
            // Setting the gender
            if (patInfo.gender === "M") labelGender.setSelection(1);
            else if (patInfo.gender === "F") labelGender.setSelection(2);
            // Setting the country.
            index = loader.getCountryIndexFromCode(patInfo.birthcountry);
            labelCountry.setCurrentIndex(index);
            // The birth date
            labelBirthDate.setISODate(patInfo.birthdate);
            if (patInfo.years_formation >= 0) labelFormativeYears.setText(patInfo.years_formation);
        }

    }

    function clearAllErrors(){
        labelName.vmErrorMsg = "";
        labelLastName.vmErrorMsg = "";
        labelAge.vmErrorMsg = "";
        labelBirthDate.vmErrorMsg = "";
        labelFormativeYears.vmErrorMsg = "";
        labelInstitutionProvidedID.vmErrorMsg = "";
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
        text: loader.getStringForKey(keysearch+"viewTitleSubject");
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


    Column {
        id: mainDataEntryColumn
        spacing: mainWindow.height*0.033
        anchors.top: viewSubTitle.bottom
        anchors.topMargin: mainWindow.height*0.043
        anchors.horizontalCenter: parent.horizontalCenter
        z: 2

        Row {
            id: rowNames
            width: mainWindow.width*0.344
            spacing: mainWindow.width*0.0125

            // Name and last name
            VMTextDataInput{
                id: labelName
                width: parent.width/2 - parent.spacing
                vmPlaceHolder: loader.getStringForKey(keysearch+"labelName");
                Keys.onTabPressed: labelLastName.vmFocus = true;
                onHasFocus: clearAllErrors();
            }

            VMTextDataInput{
                id: labelLastName
                width: labelName.width
                vmPlaceHolder: loader.getStringForKey(keysearch+"labelLastName");
                Keys.onTabPressed: labelAge.vmFocus = true;
                onHasFocus: clearAllErrors();
            }
        }

        Row {

            id: birthdateOrAge
            width: rowNames.width
            spacing: mainWindow.width*0.0165
            anchors.left: rowNames.left
            z: 3

            VMTextDataInput {
                id: labelAge
                width: mainWindow.width*0.113
                vmPlaceHolder: loader.getStringForKey(keysearch+"labelAge");
                anchors.verticalCenter: parent.verticalCenter
                Keys.onTabPressed: labelBirthDate.vmFocus = true;
                onHasFocus: clearAllErrors();
            }

            VMDateInputField{
                id: labelBirthDate
                width: parent.width - labelAge.width - parent.spacing
                //vmCalendarInput: true
                vmPlaceHolder: loader.getStringForKey(keysearch+"labelBirthDate");
                anchors.verticalCenter: parent.verticalCenter
                Keys.onTabPressed: labelFormativeYears.vmFocus = true;
                onHasFocus: clearAllErrors();
            }

        }

        Row {

            id: yearsAndID
            width: rowNames.width
            spacing: mainWindow.width*0.0125
            anchors.left: rowNames.left

            VMTextDataInput {
                id: labelFormativeYears
                width: labelAge.width
                vmPlaceHolder: loader.getStringForKey(keysearch+"labelFormativeYears");
                Keys.onTabPressed: labelInstitutionProvidedID.vmFocus = true;
                onHasFocus: clearAllErrors();
            }

            VMTextDataInput{
                id: labelInstitutionProvidedID
                width: labelBirthDate.width
                vmPlaceHolder: loader.getStringForKey(keysearch+"labelDocument_number");
                Keys.onTabPressed: labelAssignedDoctor.vmFocus = true;
                onHasFocus: clearAllErrors();
            }
        }

        Row {
            id: countryAndYearsRow
            width: rowNames.width
            spacing: mainWindow.width*0.0125
            anchors.left: rowNames.left
            z: 10

            VMAutoCompleteComboBox{
                id: labelCountry
                width: parent.width
                vmNoLabelHeight: mainWindow.height*0.043
                z: 10
                vmLabel: loader.getStringForKey(keysearch+"labelCountry")
                vmList: loader.getCountryList()
                vmValues: loader.getCountryCodeList()
                onVmValuesChanged: labelCountry.setCurrentIndex(loader.getDefaultCountry(false))
                onVmListChanged: labelCountry.setCurrentIndex(loader.getDefaultCountry(false))
                vmEnabled: true
                vmMaxOptions: 5
                Keys.onTabPressed: labelName.vmFocus = true;
            }
        }

        Row {

            id: genderAndPreferredDr
            width: rowNames.width
            spacing: mainWindow.width*0.0125
            anchors.left: rowNames.left
            z: 2

            // Gender and Date of Birth.
            VMComboBox2{
                z: 2
                id: labelGender
                width: mainWindow.width*0.113
                Component.onCompleted: labelGender.setModelList([loader.getStringForKey(keysearch+"labelGender"), "M", "F"])
            }

            VMComboBox2 {
                z: 2
                id: labelPreferredDoctor
                width: parent.width - labelGender.width - parent.spacing
            }

        }

    }

    // Buttons
    Row{
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: mainWindow.height*0.038
        spacing: mainWindow.width*0.023

        VMButton{
            id: btnBack
            height: mainWindow.height*0.072
            vmText: loader.getStringForKey(keysearch+"btnCancel");
            vmFont: viewHome.gothamM.name
            vmInvertColors: true
            onClicked: {
                swiperControl.currentIndex = swiperControl.vmIndexPatientList;
            }
        }

        VMButton{
            id: btnSave
            vmText: loader.getStringForKey(keysearch+"btnContinue");
            vmFont: viewHome.gothamM.name
            onClicked: {

                if (labelFormativeYears.vmEnteredText === ""){
                    labelFormativeYears.vmErrorMsg = loader.getStringForKey(keysearch + "errorEmpty");
                    return
                }
                else{
                    var value = parseInt(labelFormativeYears.vmEnteredText)
                    if (isNaN(value) || (value < 0)){
                        labelFormativeYears.vmErrorMsg = loader.getStringForKey(keysearch + "errorInvalidNum");
                        return
                    }
                }

                if ((labelBirthDate.vmEnteredText === "") && (labelAge.vmEnteredText === "")){
                    labelBirthDate.vmErrorMsg = loader.getStringForKey(keysearch + "errorCannotBothEmpty");
                    labelAge.vmErrorMsg = loader.getStringForKey(keysearch + "errorCannotBothEmpty");
                    return;
                }

                if (labelAge.vmEnteredText != ""){
                    value = parseInt(labelAge.vmEnteredText)
                    if (isNaN(value) || (value < 0)){
                        labelAge.vmErrorMsg = loader.getStringForKey(keysearch + "errorInvalidNum");
                        return
                    }
                }

                var bdate = "";
                if (labelBirthDate.vmEnteredText !== ""){
                    bdate = labelBirthDate.getISODate();
                }

                // The absolute must values are the document, the country, the name and the last name.
                if (labelCountry.vmCurrentText === ""){
                    // THIS SHOULD NEVER Happen. If no country has been selected the first one is selected by default and the field does not allow
                    // an empty box or non existing country.
                    // But I leave this code here just in case, I'm wrong
                    return;
                }

                // If the institution provided ID is empty, then the name and lastname is a must. Otherwise, the can be empty.
                if (labelInstitutionProvidedID.vmEnteredText === ""){
                    if (labelName.vmEnteredText === ""){
                        labelName.vmErrorMsg = loader.getStringForKey(keysearch + "errorCannotBothEmpty");
                        labelInstitutionProvidedID.vmErrorMsg = loader.getStringForKey(keysearch + "errorCannotBothEmpty");
                        return;
                    }
                    else if (labelLastName.vmEnteredText === ""){
                        labelLastName.vmErrorMsg = loader.getStringForKey(keysearch + "errorCannotBothEmpty");
                        labelInstitutionProvidedID.vmErrorMsg = loader.getStringForKey(keysearch + "errorCannotBothEmpty");
                        return
                    }
                }

                loader.addOrModifySubject(patientUID,
                                          labelName.vmEnteredText,labelLastName.vmEnteredText,
                                          labelInstitutionProvidedID.vmEnteredText,labelAge.vmEnteredText,
                                          bdate,labelCountry.vmCurrentText,labelGender.vmCurrentText,
                                          labelFormativeYears.vmEnteredText)
                swiperControl.currentIndex = swiperControl.vmIndexPatientList;

            }
        }
    }


}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//    Dialog {
//        id: showTextDialog;
//        modal: true
//        width: mainWindow.width*0.48
//        height: mainWindow.height*0.87

//        property string vmContent: ""
//        property string vmTitle: ""

//        y: (parent.height - height)/2
//        x: (parent.width - width)/2
//        closePolicy: Popup.NoAutoClose

//        contentItem: Rectangle {
//            id: rectDialog
//            anchors.fill: parent
//            layer.enabled: true
//            layer.effect: DropShadow{
//                radius: 5
//            }
//        }

//        Text {
//            id: diagTitle
//            font.family: viewHome.gothamB.name
//            font.pixelSize: 43*viewHome.vmScale
//            anchors.top: parent.top
//            anchors.topMargin: mainWindow.height*0.029
//            anchors.horizontalCenter: parent.horizontalCenter
//            color: "#297fca"
//            text: showTextDialog.vmTitle

//        }
//        ScrollView {
//            id: idScrollView
//            width: showTextDialog.width*0.9;
//            contentWidth: width
//            height: showTextDialog.height*0.62;
//            anchors.horizontalCenter: parent.horizontalCenter
//            anchors.top : diagTitle.bottom
//            anchors.topMargin: mainWindow.height*0.058
//            clip: true
//            //horizontalScrollBarPolicy : Qt.ScrollBarAlwaysOff
//            TextEdit {
//                id: idContent
//                width: parent.width;
//                height: parent.height;
//                font.family: robotoR.name
//                font.pixelSize: 13*viewHome.vmScale
//                readOnly: true
//                text: showTextDialog.vmContent
//                wrapMode: Text.Wrap
//            }
//        }

//        VMButton{
//            id: btnClose
//            height: mainWindow.height*0.072
//            vmText: "OK";
//            vmFont: viewHome.gothamM.name
//            anchors.horizontalCenter: parent.horizontalCenter
//            anchors.bottom: parent.bottom
//            anchors.bottomMargin: 20;
//            onClicked: {
//                showTextDialog.close();
//            }
//        }

//    }
