import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"

ViewBase {

    id: addPatientView

    property string vmCurrentlyLoadedPatient: ""
    property var vmEducationYearsList: []

    property var vmDaysInMonth : [31,29,31,30,31,30,31,31,30,31,30,31]

    function clear(){
        fname.clear()
        lname.clear()
        day.clear()
        year.clear()
        personalID.clear()
        yearsOfEducation.clear()
        educationSelection.vmErrorMsg = "";
        email.clear()
        acceptTerms.vmIsOn = false;
        acceptTerms.visible = true;

        // Years of education is a even numbered list. Even values are display texts. Odd values are the actual number of years.
        let years_ed_list = loader.getStringListForKey("viewpatform_years_map");
        vmEducationYearsList = [];
        let displayList = [];
        let lastValue = 0;
        for (let i = 0; i < years_ed_list.length; i++){
            let val = years_ed_list[i]
            if ((i % 2) === 0){
                lastValue = val;
                vmEducationYearsList.push(val);
            }
            else {
                val = val.replace("<<N>>",lastValue) // We need to do this so the actual number of years appears in the display text.
                displayList.push(val);
            }
        }
        educationSelection.setModelList(displayList)

        month.setSelection(-1)
        sex.setSelection(-1)
        educationSelection.setSelection(-1);
        vmCurrentlyLoadedPatient = ""
        yearsOfEducation.visible = false;

    }

    function loadPatient(patientData){

        clear()

        vmCurrentlyLoadedPatient = patientData["local_id"];

        fname.setText(patientData["name"])
        lname.setText(patientData["lastname"])
        personalID.setText(patientData["supplied_institution_id"])

        let selected_years = patientData["years_formation"] + ""; // We add an empty string to make sure the number is treated as a strng. Otherwise when seaching the range it belongs to, it won't work.

        if ("email" in patientData){
            email.setText(patientData["email"]);
        }

        var sex_array = ["M","F","O"];
        sex.setSelection(sex_array.indexOf(patientData["gender"]));

        var bdate = patientData["birthdate"];
        var bdate_parts = bdate.split("-");
        if (bdate_parts.length === 3){
            day.setText(bdate_parts[2])
            year.setText(bdate_parts[0])
            var m = parseInt(bdate_parts[1])
            m = m - 1;
            month.setSelection(m);
        }


        let index = vmEducationYearsList.indexOf(selected_years);
        //console.log("Loading patient with " + selected_years + " of education. Index for education is: " + index)
        if (index === -1){
            // Other must be selected. That is the one where -1 is the value..
            index = vmEducationYearsList.indexOf("-1");
            //console.log("While searching for index of others we get " + index)
            yearsOfEducation.visible = true;
        }

        educationSelection.setSelection(index)
        yearsOfEducation.setText(selected_years)

        // The accept term conditions should be checked by default
        acceptTerms.vmIsOn = true;
        //acceptTerms.visible = false;

    }

    function numberCheck(text,range){
        let isnum = /^\d+$/.test(text);
        if (!isnum) return -1;
        var number = parseInt(text)
        if (isNaN(number)) return -1;
        if (number < range[0]) return 0;
        if (number > range[1]) return 2;
        return 1;
    }

    function checkAndSave(checkForDupes){

        if (educationSelection.vmCurrentIndex === -1){
            educationSelection.vmErrorMsg = loader.getStringForKey("viewpatform_must_select");
            return;
        }

        if (yearsOfEducation.vmCurrentText === ""){
            yearsOfEducation.vmErrorMsg = loader.getStringForKey("viewsendsupport_email_error")
            return;
        }

        // The check on years of education is ONLY necessary if it's visible.
        if (yearsOfEducation.visible){
            var check = numberCheck(yearsOfEducation.vmCurrentText,[0, 99])
            if (check !== 1){
                if (check === -1) yearsOfEducation.vmErrorMsg = loader.getStringForKey("viewpatform_isnotanumber")
                else yearsOfEducation.vmErrorMsg = loader.getStringForKey("viewpatform_years_too_large")
                return;
            }
        }


        // The logic is that either the patient ID or the last name bust be filled.
        if ((lname.vmCurrentText === "") && (personalID.vmCurrentText === "")){
            lname.vmErrorMsg = loader.getStringForKey("viewpatform_last_or_id")
            personalID.vmErrorMsg = loader.getStringForKey("viewpatform_last_or_id")
            return;
        }

        if (lname.vmCurrentText !== ""){
            // We only force the first name to be filled if the last name was.
            if (fname.vmCurrentText === ""){
                fname.vmErrorMsg = loader.getStringForKey("viewsendsupport_email_error")
                return;
            }
        }

        if (day.vmCurrentText === ""){
            day.vmErrorMsg = loader.getStringForKey("viewsendsupport_email_error")
            return;
        }

        if (month.vmCurrentIndex == -1){
            month.vmErrorMsg = loader.getStringForKey("viewsendsupport_email_error")
            return;
        }

        if (year.vmCurrentText === ""){
            year.vmErrorMsg = loader.getStringForKey("viewsendsupport_email_error")
            return;
        }

        if (sex.vmCurrentIndex == -1){
            sex.vmErrorMsg = loader.getStringForKey("viewsendsupport_email_error")
            return;
        }


        check = numberCheck(day.vmCurrentText,[1, 31])
        if (check !== 1){
            if (check === -1) day.vmErrorMsg = loader.getStringForKey("viewpatform_isnotanumber")
            else day.vmErrorMsg = loader.getStringForKey("viewpatform_invalid_range")
            return;
        }

        // We check the coherence of the day/month pair.
        let maxDay = vmDaysInMonth[month.vmCurrentIndex];
        if (day.vmCurrentText > maxDay){
            day.vmErrorMsg = loader.getStringForKey("viewpatform_invalid_range")
            return;
        }

        let currentYear = new Date().getFullYear();
        let minYear = currentYear - 120
        check = numberCheck(year.vmCurrentText,[minYear, currentYear])
        if (check !== 1){
            if (check === -1) year.vmErrorMsg = loader.getStringForKey("viewpatform_isnotanumber")
            else year.vmErrorMsg = loader.getStringForKey("viewpatform_invalid_range")
            return;
        }

        //mainWindow.popUpNotify(VMGlobals.vmNotificationBlue,"All Good");

        let sex_index = sex.vmCurrentIndex
        let selected_sex = ["M","F","O"][sex_index]

        //console.log("Selected sex index: " + selected_sex)

        let mm = (month.vmCurrentIndex + 1)
        if (mm < 10) mm = "0" + mm // WE need to compare with 10 because mm is a number up to this point.
        let dd = day.vmCurrentText // We need to check string length instead of number value cause dd is a text.
        if (dd.length < 2) dd = "0" + dd;
        let bdate = year.vmCurrentText + "-" + mm + "-" + dd;

        if (checkForDupes){
            let list = loader.findPossibleDupes(fname.vmCurrentText,lname.vmCurrentText,personalID.vmCurrentText,bdate);
            if (list.length > 0){
                // We only show the dialog if there are matching dupes.
                possibleDupeDialog.setNameList(list);
                possibleDupeDialog.open();
                return;
            }
        }

        loader.addOrModifySubject(vmCurrentlyLoadedPatient,
                                  fname.vmCurrentText,lname.vmCurrentText,
                                  personalID.vmCurrentText,
                                  bdate,selected_sex, // Country is always empty.
                                  yearsOfEducation.vmCurrentText,email.vmCurrentText)

        var message = "";
        if (vmCurrentlyLoadedPatient == "")  message = loader.getStringForKey("viewpatform_success_add");
        else message = loader.getStringForKey("viewpatform_success_edit");

        let notify_id = ""
        if (lname.vmCurrentText == ""){
            notify_id = personalID.vmCurrentText;
        }
        else {
            notify_id = fname.vmCurrentText + " " + lname.vmCurrentText
        }

        message = message.replace("<b></b>","<b>" + notify_id + "</b>")
        //console.log("Should be popping up a message");
        mainWindow.popUpNotify(VMGlobals.vmNotificationGreen,message);
        mainWindow.swipeTo(VMGlobals.vmSwipeIndexMainScreen)
    }

    VMPossibleDupeDialog {
        id: possibleDupeDialog
        onSaveNewPatient: {
            close();
            checkAndSave(false);
        }
        onGoBackToPatientList: {
            close();
            mainWindow.swipeTo(VMGlobals.vmSwipeIndexMainScreen)
        }
    }


    VMButton {
        id: backButton
        vmText: loader.getStringForKey("viewlogin_back")
        vmIconSource: "arrow"
        vmButtonType: backButton.vmTypeTertiary
        anchors.bottom: parent.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(592)
        anchors.left: parent.left
        anchors.leftMargin: VMGlobals.adjustWidth(5)
        onClickSignal: {
            mainWindow.swipeTo(VMGlobals.vmSwipeIndexMainScreen)
        }
    }

    Rectangle {
        property double vmBorderPadding: VMGlobals.adjustWidth(1)
        id: mainRect
        clip: true
        radius: VMGlobals.adjustHeight(10)
        width: VMGlobals.adjustWidth(969);
        height: VMGlobals.adjustHeight(670+10);
        anchors.right: parent.right
        anchors.rightMargin: VMGlobals.adjustWidth(15)
        anchors.bottom: parent.bottom
        anchors.bottomMargin: -radius
        border.width: vmBorderPadding
        border.color: VMGlobals.vmGrayUnselectedBorder

        Text {
            id: title
            text: (vmCurrentlyLoadedPatient == "") ? loader.getStringForKey("viewpatform_title") : loader.getStringForKey("viewpatform_title_edit")
            color: VMGlobals.vmBlackText
            height: VMGlobals.adjustHeight(42)
            font.pixelSize: VMGlobals.vmFontExtraExtraLarge
            font.weight: 600
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: VMGlobals.adjustWidth(30)
            anchors.topMargin: VMGlobals.adjustHeight(31)
        }

        Text {
            id: subtitle
            text: loader.getStringForKey("viewpatform_subtitle")
            font.pixelSize: VMGlobals.vmFontLarge
            font.weight: 400
            color: VMGlobals.vmGrayPlaceholderText
            height: VMGlobals.adjustHeight(21)
            anchors.left: title.left
            anchors.top: title.bottom
            anchors.topMargin: VMGlobals.adjustHeight(10)
        }

        Row {
            id: formRow
            anchors.top: subtitle.bottom
            anchors.topMargin: VMGlobals.adjustHeight(44)
            anchors.left: title.left
            spacing: VMGlobals.adjustWidth(20)
            z: 10

            Column {
                id: leftColumn
                spacing: VMGlobals.adjustHeight(54)
                width: VMGlobals.adjustWidth(320)

                VMTextInput {
                    id: fname
                    width: parent.width
                    vmLabel: loader.getStringForKey("viewaddeval_fname")
                    vmPlaceHolderText: loader.getStringForKey("viewaddeval_fname_ph")
                    Keys.onTabPressed: lname.vmFocus = true
                }

                Row {
                    id: bdateRow
                    spacing: VMGlobals.adjustWidth(5)
                    width: parent.width
                    //z: addPatientView.z + 2
                    z: educationSelection.z + 1

                    VMTextInput {
                        id: day
                        width: VMGlobals.adjustWidth(75)
                        vmLabel: loader.getStringForKey("viewpatlist_bdate")
                        vmPlaceHolderText: loader.getStringForKey("viewpatform_day_ph")
                        Keys.onTabPressed: year.vmFocus = true
                    }
                    VMComboBox {
                        id: month
                        width: VMGlobals.adjustWidth(320 - 75 - 80 - 2*5)
                        vmPlaceHolderText: loader.getStringForKey("viewpatform_month_ph")
                        //z: addPatientView.z + 2
                        z: educationSelection.z + 1
                        Component.onCompleted: {
                            setModelList(loader.getStringListForKey("viewpatform_months"))
                        }
                    }
                    VMTextInput {
                        id: year
                        width: VMGlobals.adjustWidth(80)
                        vmPlaceHolderText: loader.getStringForKey("viewpatform_year_ph")
                        //Keys.onTabPressed: yearsOfEducation.vmFocus = true
                    }
                }

                VMComboBox {
                    id: educationSelection
                    width: parent.width
                    vmLabel: loader.getStringForKey("viewpatform_max_education_level")
                    vmPlaceHolderText: loader.getStringForKey("viewpatform_max_education_select")
                    z: formRow.z + 1
                    onVmCurrentIndexChanged: {
                        let nyears = vmEducationYearsList[vmCurrentIndex];
                        if (nyears == "-1"){
                            yearsOfEducation.visible = true;
                            yearsOfEducation.setText("");
                        }
                        else {
                            yearsOfEducation.setText(nyears)
                            yearsOfEducation.visible = false;
                        }
                    }
                }

                VMTextInput {
                    id: yearsOfEducation
                    width: parent.width
                    vmLabel: loader.getStringForKey("viewpatform_years_of_education")
                    vmPlaceHolderText: ""//loader.getStringForKey("viewpatform_years_of_education_ph")
                    //Keys.onTabPressed: personalID.vmFocus = true
                }


            }

            Column {
                id: rightColumn
                spacing: VMGlobals.adjustHeight(54)
                width: leftColumn.width

                VMTextInput{
                    id: lname
                    width: parent.width
                    vmLabel: loader.getStringForKey("viewaddeval_lname")
                    vmPlaceHolderText: loader.getStringForKey("viewaddeval_lname_ph")
                    Keys.onTabPressed: day.vmFocus = true
                    onVmCurrentTextChanged: {
                        personalID.vmErrorMsg = "";
                    }
                }

                VMTextInput {
                    id: personalID
                    width: parent.width
                    vmLabel: loader.getStringForKey("viewpatlist_id")
                    vmPlaceHolderText: loader.getStringForKey("viewpatform_personal_id_ph")
                    //vmClarification: "(" + loader.getStringForKey("viewpatform_id_warning") + ")"
                    vmToolTip: loader.getStringForKey("viewpatlist_id_explanation")
                    Keys.onTabPressed: email.vmFocus = true
                    //z: sex.z + 1 // I need this so that the tooltip will appear OVER the sex dropdown.
                    onVmCurrentTextChanged: {
                        lname.vmErrorMsg = "";
                    }
                }

                VMComboBox {
                    id: sex
                    width: parent.width
                    vmLabel: loader.getStringForKey("viewpatform_sex")
                    vmPlaceHolderText: loader.getStringForKey("viewpatform_sex_ph")
                    z: addPatientView.z + 1
                    Component.onCompleted: {
                        setModelList(loader.getStringListForKey("viewpatform_sex_options"))
                    }

                }

                VMTextInput {
                    id: email
                    width: parent.width
                    vmLabel: loader.getStringForKey("viewaddeval_email")
                    vmPlaceHolderText: loader.getStringForKey("viewaddeval_email_ph")
                    vmClarification: "(" + loader.getStringForKey("viewpatform_optional") + ")"
                    Keys.onTabPressed: fname.vmFocus = true
                }

            }
        }

        VMCheckBox {
            id: acceptTerms
            anchors.top: formRow.bottom
            anchors.left: formRow.left
            anchors.topMargin: VMGlobals.adjustHeight(35)
            vmText: loader.getStringForKey("viewaddeval_accept")
            onLinkClicked: function (url){
                loader.openURLInBrowser(url);
            }
            z: formRow.z - 1
        }

        Rectangle {
            id: horizontalDivider
            height: VMGlobals.adjustHeight(1)
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.bottomMargin: VMGlobals.adjustHeight(84)
            width: parent.width
            border.width: 0
            color: VMGlobals.vmGrayUnselectedBorder
            z: formRow.z - 1
        }

        VMButton {
            id: saveButton
            vmText: loader.getStringForKey("viewaccount_save")
            vmCustomWidth: VMGlobals.adjustWidth(120)
            anchors.top: horizontalDivider.top
            anchors.topMargin: VMGlobals.adjustHeight(20)
            anchors.right: parent.right
            anchors.rightMargin: VMGlobals.adjustWidth(29)
            vmEnabled: acceptTerms.vmIsOn
            onClickSignal: {
                // Dupe checking should only happen when CREATING subjects.
                let checkForDupes = (vmCurrentlyLoadedPatient === "");
                checkAndSave(checkForDupes)
            }
        }

        VMButton {
            id: cancelButton
            vmText: loader.getStringForKey("viewsettings_cancel")
            vmButtonType: cancelButton.vmTypeSecondary
            vmCustomWidth: VMGlobals.adjustWidth(120)
            anchors.top: saveButton.top
            anchors.right: saveButton.left
            anchors.rightMargin: VMGlobals.adjustWidth(20)
            onClickSignal: {
                mainWindow.swipeTo(VMGlobals.vmSwipeIndexMainScreen)
            }
        }

    }

}
