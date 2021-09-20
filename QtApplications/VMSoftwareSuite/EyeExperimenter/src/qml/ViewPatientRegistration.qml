import QtQuick 2.6
import QtQuick.Controls 2.3
//import QtQuick.Controls 1.4
import QtGraphicalEffects 1.0

VMBase {

    id: viewPatientRegistration
    width: viewPatientRegistration.vmWIDTH
    height: viewPatientRegistration.vmHEIGHT

    property bool vmIsNew: true;
    property string patientUID: "";
    property var vmMedicsModelList: []

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

        // Filling the dr list.
        var medics = loader.getMedicList();
        vmMedicsModelList = [];

        var medic_instruction = loader.getStringForKey("viewpatientreg_labelAssignedDoctor")
        vmMedicsModelList.push({ "value" : medic_instruction, "metadata" : "-1" });
        for (var key in medics){
            vmMedicsModelList.push({ "value" : medics[key], "metadata" : key });
        }
        labelPreferredDoctor.setModelList(vmMedicsModelList);

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

            // Setting the preffered doctor selection.
            for (var i in vmMedicsModelList){
                // console.log("Comparing " + vmMedicsModelList[i]["metadata"] + " to " + patInfo.assigned_medic);
                if (vmMedicsModelList[i]["metadata"] === String(patInfo.assigned_medic)){
                    labelPreferredDoctor.setSelection(i);
                    break;
                }
            }
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
        text: loader.getStringForKey("viewpatientreg_viewTitleSubject");
    }

    Text {
        id: viewSubTitle
        font.family: robotoR.name
        font.pixelSize: 13*viewHome.vmScale
        anchors.top:  viewTitle.bottom
        anchors.topMargin: mainWindow.height*0.019
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#5499d5"
        text: loader.getStringForKey("viewpatientreg_viewSubTitle");
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
        text: loader.getStringForKey("viewpatientreg_errorNoAccept");
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
                vmPlaceHolder: loader.getStringForKey("viewpatientreg_labelName");
                Keys.onTabPressed: labelLastName.vmFocus = true;
                onHasFocus: clearAllErrors();
            }

            VMTextDataInput{
                id: labelLastName
                width: labelName.width
                vmPlaceHolder: loader.getStringForKey("viewpatientreg_labelLastName");
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
                vmPlaceHolder: loader.getStringForKey("viewpatientreg_labelAge");
                anchors.verticalCenter: parent.verticalCenter
                Keys.onTabPressed: labelBirthDate.vmFocus = true;
                onHasFocus: clearAllErrors();
            }

            VMDateInputField{
                id: labelBirthDate
                width: parent.width - labelAge.width - parent.spacing
                //vmCalendarInput: true
                vmPlaceHolder: loader.getStringForKey("viewpatientreg_labelBirthDate");
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
                vmPlaceHolder: loader.getStringForKey("viewpatientreg_labelFormativeYears");
                Keys.onTabPressed: labelInstitutionProvidedID.vmFocus = true;
                onHasFocus: clearAllErrors();
            }

            VMTextDataInput{
                id: labelInstitutionProvidedID
                width: labelBirthDate.width
                vmPlaceHolder: loader.getStringForKey("viewpatientreg_labelDocument_number");
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
                vmLabel: loader.getStringForKey("viewpatientreg_labelCountry")
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
                Component.onCompleted: labelGender.setModelList([loader.getStringForKey("viewpatientreg_labelGender"), "M", "F"])
            }

            VMComboBox2 {
                z: 2
                id: labelPreferredDoctor
                width: parent.width - labelGender.width - parent.spacing
                onVmCurrentIndexChanged: {
                    console.log(width)
                    console.log(height)
                }
//                onVmCurrentIndexChanged: {
//                    console.log("Current index: " + vmCurrentIndex);
//                    console.log("Selected keyid: " + vmMedicsModelList[vmCurrentIndex]["metadata"]);
//                }
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
            vmText: loader.getStringForKey("viewpatientreg_btnCancel");
            vmFont: viewHome.gothamM.name
            vmInvertColors: true
            onClicked: {
                swiperControl.currentIndex = swiperControl.vmIndexPatientList;
            }
        }

        VMButton{
            id: btnSave
            vmText: loader.getStringForKey("viewpatientreg_btnContinue");
            vmFont: viewHome.gothamM.name
            onClicked: {

                if (labelFormativeYears.vmEnteredText === ""){
                    labelFormativeYears.vmErrorMsg = loader.getStringForKey("viewpatientreg_errorEmpty");
                    return
                }
                else{
                    var value = parseInt(labelFormativeYears.vmEnteredText)
                    if (isNaN(value) || (value < 0)){
                        labelFormativeYears.vmErrorMsg = loader.getStringForKey("viewpatientreg_errorInvalidNum");
                        return
                    }
                }

                if ((labelBirthDate.vmEnteredText === "") && (labelAge.vmEnteredText === "")){
                    labelBirthDate.vmErrorMsg = loader.getStringForKey("viewpatientreg_errorCannotBothEmpty");
                    labelAge.vmErrorMsg = loader.getStringForKey("viewpatientreg_errorCannotBothEmpty");
                    return;
                }

                if (labelAge.vmEnteredText != ""){
                    value = parseInt(labelAge.vmEnteredText)
                    if (isNaN(value) || (value < 0)){
                        labelAge.vmErrorMsg = loader.getStringForKey("viewpatientreg_errorInvalidNum");
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

                var gender = "";
                if (labelGender.vmCurrentIndex > 0) gender = labelGender.vmCurrentText;

                // If the institution provided ID is empty, then the name and lastname is a must. Otherwise, the can be empty.
                if (labelInstitutionProvidedID.vmEnteredText === ""){
                    if (labelName.vmEnteredText === ""){
                        labelName.vmErrorMsg = loader.getStringForKey("viewpatientreg_errorCannotBothEmpty");
                        labelInstitutionProvidedID.vmErrorMsg = loader.getStringForKey("viewpatientreg_errorCannotBothEmpty");
                        return;
                    }
                    else if (labelLastName.vmEnteredText === ""){
                        labelLastName.vmErrorMsg = loader.getStringForKey("viewpatientreg_errorCannotBothEmpty");
                        labelInstitutionProvidedID.vmErrorMsg = loader.getStringForKey("viewpatientreg_errorCannotBothEmpty");
                        return
                    }
                }

                loader.addOrModifySubject(patientUID,
                                          labelName.vmEnteredText,labelLastName.vmEnteredText,
                                          labelInstitutionProvidedID.vmEnteredText,labelAge.vmEnteredText,
                                          bdate,labelCountry.vmCurrentText,gender,
                                          labelFormativeYears.vmEnteredText,vmMedicsModelList[labelPreferredDoctor.vmCurrentIndex]["metadata"])
                swiperControl.currentIndex = swiperControl.vmIndexPatientList;

            }
        }
    }


}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


