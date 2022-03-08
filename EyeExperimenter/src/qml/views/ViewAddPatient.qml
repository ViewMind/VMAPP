import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"

ViewBase {

    id: addPatientView

    property string vmCurrentlyLoadedPatient: ""

    function clear(){
        fname.clear()
        lname.clear()
        day.clear()
        year.clear()
        personalID.clear()
        yearsOfEducation.clear()
        email.clear()

        month.setSelection(-1)
        sex.setSelection(-1)
        country.setSelection(loader.getDefaultCountry(false));
        vmCurrentlyLoadedPatient = ""
    }

    function loadPatient(patientData){

        clear()

        vmCurrentlyLoadedPatient = patientData["local_id"];

        fname.setText(patientData["name"])
        lname.setText(patientData["lastname"])
        personalID.setText(patientData["supplied_institution_id"])
        yearsOfEducation.setText(patientData["years_formation"])
        if ("email" in patientData){
            email.setText(patientData["email"]);
        }

        country.setSelection(loader.getCountryIndexFromCode(patientData["birthcountry"]))

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

    }

    function numberCheck(text,range){
        var number = parseInt(text)
        if (isNaN(number)) return -1;
        if (number < range[0]) return 0;
        if (number > range[1]) return 2;
        return 1;
    }

    function checkAndSave(){

        if (fname.vmCurrentText === ""){
            fname.vmErrorMsg = loader.getStringForKey("viewpatform_cannotbeempty")
            return;
        }

        if (lname.vmCurrentText === ""){
            lname.vmErrorMsg = loader.getStringForKey("viewpatform_cannotbeempty")
            return;
        }

        if (day.vmCurrentText === ""){
            day.vmErrorMsg = loader.getStringForKey("viewpatform_cannotbeempty")
            return;
        }

        if (month.vmCurrentIndex == -1){
            month.vmErrorMsg = loader.getStringForKey("viewpatform_cannotbeempty")
            return;
        }

        if (year.vmCurrentText === ""){
            year.vmErrorMsg = loader.getStringForKey("viewpatform_cannotbeempty")
            return;
        }

        if (yearsOfEducation.vmCurrentText === ""){
            yearsOfEducation.vmErrorMsg = loader.getStringForKey("viewpatform_cannotbeempty")
            return;
        }

        if (sex.vmCurrentIndex == -1){
            sex.vmErrorMsg = loader.getStringForKey("viewpatform_cannotbeempty")
            return;
        }

        if (country.vmCurrentIndex == -1){
            country.vmErrorMsg = loader.getStringForKey("viewpatform_cannotbeempty")
            return;
        }


        var check = numberCheck(yearsOfEducation.vmCurrentText,[0, 100])
        if (check !== 1){
            if (check === -1) yearsOfEducation.vmErrorMsg = loader.getStringForKey("viewpatform_isnotanumber")
            else yearsOfEducation.vmErrorMsg = loader.getStringForKey("viewpatform_invalid_range")
            return;
        }

        check = numberCheck(day.vmCurrentText,[1, 31])
        if (check !== 1){
            if (check === -1) day.vmErrorMsg = loader.getStringForKey("viewpatform_isnotanumber")
            else day.vmErrorMsg = loader.getStringForKey("viewpatform_invalid_range")
            return;
        }

        let currentYear = new Date().getFullYear();
        check = numberCheck(year.vmCurrentText,[1900, currentYear])
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
        if (mm.length < 2) mm = "0" + mm
        let bdate = year.vmCurrentText + "-" + mm + "-" + day.vmCurrentText;

        loader.addOrModifySubject(vmCurrentlyLoadedPatient,
                                  fname.vmCurrentText,lname.vmCurrentText,
                                  personalID.vmCurrentText,
                                  bdate,country.vmCurrentText,selected_sex,
                                  yearsOfEducation.vmCurrentText,email.vmCurrentText)

        var message = "";
        if (vmCurrentlyLoadedPatient == "")  message = loader.getStringForKey("viewpatform_success_add");
        else message = loader.getStringForKey("viewpatform_success_edit");
        message = message.replace("<b></b>","<b>" + fname.vmCurrentText + " " + lname.vmCurrentText + "</b>")
        //console.log("Should be popping up a message");
        mainWindow.popUpNotify(VMGlobals.vmNotificationGreen,message);
        mainWindow.swipeTo(VMGlobals.vmSwipeIndexMainScreen)
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
                    z: addPatientView.z + 2

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
                        z: addPatientView.z + 2
                        Component.onCompleted: {
                            setModelList(loader.getStringListForKey("viewpatform_months"))
                        }
                    }
                    VMTextInput {
                        id: year
                        width: VMGlobals.adjustWidth(80)
                        vmPlaceHolderText: loader.getStringForKey("viewpatform_year_ph")
                        Keys.onTabPressed: personalID.vmFocus = true
                    }
                }

                VMComboBox {
                    id: country
                    width: parent.width
                    vmLabel: loader.getStringForKey("viewpatform_country")
                    vmPlaceHolderText: loader.getStringForKey("viewpatform_country_ph")
                    z: addPatientView.z + 1
                    Component.onCompleted: {
                        setModelList(loader.getCountryList())
                    }
                }

                VMTextInput {
                    id: yearsOfEducation
                    width: parent.width
                    vmLabel: loader.getStringForKey("viewpatform_years_of_education")
                    vmPlaceHolderText: loader.getStringForKey("viewpatform_years_of_education_ph")
                    Keys.onTabPressed: email.vmFocus = true
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
                }

                VMComboBox {
                    id: sex
                    width: parent.width
                    vmLabel: loader.getStringForKey("viewpatlist_sex")
                    vmPlaceHolderText: loader.getStringForKey("viewpatform_sex_ph")
                    z: addPatientView.z + 1
                    Component.onCompleted: {
                        setModelList(loader.getStringListForKey("viewpatform_sex_options"))
                    }

                }

                VMTextInput {
                    id: personalID
                    width: parent.width
                    vmLabel: loader.getStringForKey("viewpatlist_id")
                    vmPlaceHolderText: loader.getStringForKey("viewpatform_personal_id_ph")
                    Keys.onTabPressed: yearsOfEducation.vmFocus = true
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


        Rectangle {
            id: horizontalDivider
            height: VMGlobals.adjustHeight(1)
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.bottomMargin: VMGlobals.adjustHeight(84)
            width: parent.width
            border.width: 0
            color: VMGlobals.vmGrayUnselectedBorder
        }

        VMButton {
            id: saveButton
            vmText: loader.getStringForKey("viewaccount_save")
            vmCustomWidth: VMGlobals.adjustWidth(120)
            anchors.top: horizontalDivider.top
            anchors.topMargin: VMGlobals.adjustHeight(20)
            anchors.right: parent.right
            anchors.rightMargin: VMGlobals.adjustWidth(29)
            onClickSignal: {
                checkAndSave()
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
