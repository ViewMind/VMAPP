import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"

ViewBase {

    id: loginView

    function updateProfileList(){
        var ans = loader.getLoginEmails();
        evaluatorSelection.setModelList(ans)
        evaluatorSelection.setSelection(-1)
        passwordInput.clear()
    }

    function logInAttempt(){

        var user = evaluatorSelection.vmCurrentText;
        var password = passwordInput.vmCurrentText
        var correntSwiperIndexToLoad = VMGlobals.vmSwipeIndexMainScreen;

        ////////////////////////////////////// START DEBUG SETUP ////////////////////////////////////

        // Getting the Debug Options to see if any of this will be over ridden.
        var dbug_user = loader.getDebugOption("login_user");
        var dbug_password = loader.getDebugOption("login_password");
        var dbug_index_to_load = loader.getDebugOption("view_index_to_show");
        var dbug_selected_patient = loader.getDebugOption("selected_patient");
        var dbug_selected_doctor = loader.getDebugOption("selected_doctor");
        var dbug_qc_file_to_set = loader.getDebugOption("qc_file_path");
        var dbug_setup_studies = loader.getDebugOption("study_configutation_map")


        if (dbug_user !== ""){
            user = dbug_user;
            password = dbug_password;
            console.log("DBUG: Logging in as " + user + ":" + password);
        }
        if (dbug_index_to_load !== ""){
            correntSwiperIndexToLoad = dbug_index_to_load
            console.log("DBUG: Loading View: " + dbug_index_to_load);
        }

        if (dbug_selected_doctor !== ""){
            // WARNING: Actually selecting and patient and doctor normally will overwrite this.
            console.log("DBUG: Setting selected doctor to " + dbug_selected_doctor);
            //viewStudyStart.vmSelectedMedic = dbug_selected_doctor;
        }

        if (dbug_selected_patient !== ""){
            // WARNING: Actually selecting and patient and doctor normally will overwrite this.
            console.log("DBUG: Setting selected patient to " + dbug_selected_patient);
            loader.setSelectedSubject(dbug_selected_patient);
            viewEvaluations.setPatientForEvaluation()
        }

        if (dbug_qc_file_to_set !== ""){
            // WARNING: Actually selecting a file to QC in the report view will over write this.
            console.log("DBUG: Setting File To QC to: " + dbug_qc_file_to_set);
            loader.setCurrentStudyFileForQC(dbug_qc_file_to_set);
        }

        ////////////////////////////////////// END DEBUG SETUP ////////////////////////////////////

        if (loader.evaluatorLogIn(user,password)){
            // Updating the text of the doctor menu.
            passwordInput.vmErrorMsg = "";
            mainWindow.swipeTo(correntSwiperIndexToLoad);

            if (dbug_setup_studies !== ""){
                console.log("DBUG: Study string found. To parse: " + dbug_setup_studies);

                // --- Indicates se paration between different studies.
                // --  Indicates separation between different fields of the same study
                // -   Indicates separation between key and value.

                console.log("TODO Setup Studies")
//                var all_studies = dbug_setup_studies.split("---");
//                viewStudyStart.vmSelectedExperiments = [];
//                for (var s = 0; s < all_studies.length; s++){
//                    var study_fields = all_studies[s].split("--");
//                    var config = {};
//                    for (var f = 0; f < study_fields.length; f++){
//                        var key_and_value = study_fields[f].split("-");
//                        config[key_and_value[0]] = key_and_value[1]
//                    }
//                    viewStudyStart.vmSelectedExperiments.push(config);
//                }
//                viewStudyStart.startStudies();

            }
        }
        else{
            passwordInput.vmErrorMsg = loader.getStringForKey("viewlogin_wrong_pass");;
        }
    }


    Text {
        id: title
        text: loader.getStringForKey("viewlogin_title")
        font.pixelSize: VMGlobals.vmFontExtraExtraLarge
        font.weight: 600
        color: VMGlobals.vmBlackText
        height: VMGlobals.adjustHeight(43)
        verticalAlignment: Text.AlignVCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(607)
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
            mainWindow.swipeTo(VMGlobals.vmSwipeIndexHome)
        }
    }

    Text {
        id: subtTitle
        text: loader.getStringForKey("viewlogin_subtitle")
        font.pixelSize: VMGlobals.vmFontLarge
        font.weight: 400
        color: VMGlobals.vmGrayPlaceholderText
        verticalAlignment: Text.AlignVCenter
        height: VMGlobals.adjustHeight(18)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: title.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)
    }

    VMComboBox {
        id: evaluatorSelection
        vmLabel: loader.getStringForKey("viewlogin_label_evaluator")
        vmPlaceHolderText: loader.getStringForKey("viewlogin_placeholder_evaluator")
        width: VMGlobals.adjustWidth(320)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: subtTitle.bottom
        anchors.topMargin: VMGlobals.adjustHeight(54)
        z: 1
    }

    VMPasswordInput {
        id: passwordInput
        width: evaluatorSelection.width
        vmLabel: loader.getStringForKey("viewlogin_label_pass")
        vmPlaceHolderText: loader.getStringForKey("viewlogin_placeholder_password")
        vmShowText: loader.getStringForKey("viewlogin_show")
        vmHideText: loader.getStringForKey("viewlogin_hide")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: evaluatorSelection.bottom
        anchors.topMargin: VMGlobals.adjustHeight(54)
    }


    VMButton {
        id: loginButton
        vmText: loader.getStringForKey("viewstart_login")
        vmCustomWidth: evaluatorSelection.width
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: passwordInput.bottom
        anchors.topMargin: VMGlobals.adjustHeight(30)
        onClickSignal: {
            logInAttempt();
        }
    }

    Text {
        id: orlinetext
        text: loader.getStringForKey("viewlogin_orline")
        font.pixelSize: VMGlobals.vmFontLarger
        font.weight: 400
        verticalAlignment: Text.AlignVCenter
        color: VMGlobals.vmGrayLightGrayText
        anchors.horizontalCenter: parent.horizontalCenter
        height: VMGlobals.adjustHeight(21)
        anchors.top: loginButton.bottom
        anchors.topMargin: VMGlobals.adjustHeight(20)
    }

    Rectangle {
        id: leftDivider
        width: VMGlobals.adjustWidth(140)
        height: VMGlobals.adjustHeight(1)
        border.width: 0
        color: orlinetext.color
        anchors.verticalCenter: orlinetext.verticalCenter
        anchors.left: loginButton.left
    }

    Rectangle {
        id: rightDivider
        width: leftDivider.width
        height: leftDivider.height
        border.width: 0
        color: orlinetext.color
        anchors.verticalCenter: orlinetext.verticalCenter
        anchors.right: loginButton.right
    }


    VMButton {
        id: addNewEval
        vmButtonType: addNewEval.vmTypeSecondary
        vmText: loader.getStringForKey("viewstart_addneweval")
        vmCustomWidth: evaluatorSelection.width
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: loginButton.bottom
        anchors.topMargin: VMGlobals.adjustHeight(61)
        onClickSignal: {
            mainWindow.swipeTo(VMGlobals.vmSwipeIndexAddEval)
        }
    }

}
