import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects

VMDialogBase {

    id: viewDoctorSelection
    width: mainWindow.width*0.511
    height: mainWindow.height*0.942

    function logInAttempt(){

        var user = labelDrProfile.vmCurrentText;
        var password = drPassword.getText();
        var correntSwiperIndexToLoad = swiperControl.vmIndexPatientList;

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
            viewStudyStart.vmSelectedMedic = dbug_selected_doctor;
        }

        if (dbug_selected_patient !== ""){
            // WARNING: Actually selecting and patient and doctor normally will overwrite this.
            console.log("DBUG: Setting selected patient to " + dbug_selected_patient);
            loader.setSelectedSubject(dbug_selected_patient);
        }

        if (dbug_qc_file_to_set !== ""){
            // WARNING: Actually selecting a file to QC in the report view will over write this.
            console.log("DBUG: Setting File To QC to: " + dbug_qc_file_to_set);
            loader.setCurrentStudyFileForQC(dbug_qc_file_to_set);
        }

        ////////////////////////////////////// END DEBUG SETUP ////////////////////////////////////

        if (loader.evaluatorLogIn(user,password)){
            // Updating the text of the doctor menu.
            viewHome.updateDrMenuText();
            viewDoctorSelection.close();
            drPassword.vmErrorMsg = "";
            swiperControl.currentIndex = correntSwiperIndexToLoad
            if (dbug_setup_studies !== ""){
                console.log("DBUG: Study string found. To parse: " + dbug_setup_studies);

                // --- Indicates se paration between different studies.
                // --  Indicates separation between different fields of the same study
                // -   Indicates separation between key and value.

                var all_studies = dbug_setup_studies.split("---");
                for (var s = 0; s < all_studies.length; s++){
                    var study_fields = all_studies[s].split("--");
                    var config = {};
                    for (var f = 0; f < study_fields.length; f++){
                        var key_and_value = study_fields[f].split("-");
                        config[key_and_value[0]] = key_and_value[1]
                    }
                    viewStudyStart.vmSelectedExperiments = [];
                    viewStudyStart.vmSelectedExperiments.push(config);
                    viewStudyStart.startStudies();
                }

            }
        }
        else{
            drPassword.vmErrorMsg = loader.getStringForKey("viewdrsel_drwrongpass");;
        }
    }

    function updateDrProfile(){
        var ans = loader.getLoginEmails();
        //console.log(ans);
        ans.unshift(loader.getStringForKey("viewdrsel_labelDrProfile"));
        labelDrProfile.setModelList(ans)
        labelDrProfile.vmCurrentIndex = 0;
    }


    onOpened: {
        //labelDrProfile.clear();
        drPassword.clear();
        updateDrProfile();
    }

    Image {
        id: drPic
        source: "qrc:/images/dr_icon.png"
        width: mainWindow.width*0.084
        height: mainWindow.height*0.157
        anchors.top: parent.top
        anchors.topMargin: mainWindow.height*0.101
        anchors.horizontalCenter: parent.horizontalCenter
    }

    // The Doctor Information Title and subtitle
    Text {
        id: diagTitle
        font.pixelSize: 40*viewHome.vmScale
        font.family: viewHome.gothamB.name
        color: "#297FCA"
        text: loader.getStringForKey("viewdrsel_viewTitle");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: drPic.bottom
        anchors.topMargin: mainWindow.height*0.022
    }

    Text {
        id: diagSubTitle
        font.pixelSize: 11*viewHome.vmScale
        font.family: viewHome.gothamR.name
        color: "#969696"
        text: loader.getStringForKey("viewdrsel_viewSubTitle");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: diagTitle.bottom
        anchors.topMargin: mainWindow.height*0.014
    }

    Row {

        id: rowProfileAndAdd
        spacing: mainWindow.width*0.008
        anchors.top: diagSubTitle.bottom
        anchors.topMargin: mainWindow.height*0.057
        anchors.horizontalCenter: parent.horizontalCenter
        z: 1

        //        VMTextDataInput {
        //            id: labelDrProfile
        //            width: mainWindow.width*0.273
        //            height: mainWindow.height*0.043
        //            vmFont: viewHome.robotoR.name
        //            vmPlaceHolder: "Email"
        //        }

        // The selection box and the add button
        VMComboBox2{
            id: labelDrProfile
            width: mainWindow.width*0.273
            onVmCurrentIndexChanged: {
                vmErrorMsg = "";
                drPassword.vmErrorMsg = "";
            }
        }

        VMPlusButton{
            id: btnAddProfile
            height: labelDrProfile.height
            anchors.bottom: labelDrProfile.bottom
            onClicked: {
                viewDoctorSelection.close();
                viewDrInfo.clearAllFields();
                loader.logOut();
                viewDrInfo.newEvaluator();
                swiperControl.currentIndex = swiperControl.vmIndexDrProfile;
            }
        }

    }

    VMPasswordField{
        id: drPassword
        vmLabelText: loader.getStringForKey("viewdrsel_drpassword");
        width: labelDrProfile.width
        anchors.left: rowProfileAndAdd.left
        anchors.top: rowProfileAndAdd.bottom
        anchors.topMargin: mainWindow.height*0.072
        onEnterPressed: logInAttempt();
    }


    VMButton{
        id: btnOk
        height: mainWindow.height*0.072
        vmText: loader.getStringForKey("viewdrsel_btnOk");
        vmFont: viewHome.gothamM.name
        anchors.bottom: parent.bottom
        anchors.bottomMargin: mainWindow.height*0.043
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: {
            logInAttempt();
        }
    }

}
