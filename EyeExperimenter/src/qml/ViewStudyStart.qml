import QtQuick 2.6
import QtQuick.Controls 2.3

VMBase {

    id: viewStudyStart
    width: viewStudyStart.vmWIDTH
    height: viewStudyStart.vmHEIGHT

    readonly property string keysearch: "viewstudystart_"
    property var vmSelectedExperiments: []
    property int vmCurrentExperimentIndex: 0

    Connections{
        target: loader
        onUpdatedDoctorPatientList:{
            var ans = loader.getPatientList();
            ans.unshift(loader.getStringForKey(keysearch+"cbPatientSelection"));
            cbPatientSelection.vmModel = ans;
        }
    }

    // Title and subtitle
    Text {
        id: viewTitle
        font.family: gothamB.name
        font.pixelSize: 43
        anchors.top:  vmBanner.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#3fa2f7"
        text: loader.getStringForKey(keysearch+"labelTitle");
    }

    Text {
        id: viewSubTitle
        font.family: robotoR.name
        font.pixelSize: 13
        anchors.top:  viewTitle.bottom
        anchors.topMargin: 23
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#5499d5"
        text: loader.getStringForKey(keysearch+"labelSubTitle");
    }


    // Patient selection
    VMComboBox{
        id: cbPatientSelection
        width: 440
        vmModel: [];
        font.family: viewHome.robotoR.name
        anchors.left: parent.left
        anchors.leftMargin: 420
        anchors.top: viewSubTitle.bottom
        anchors.topMargin: 39
    }

    VMButton{
        id: btnAddPatient
        height: 50
        width: cbPatientSelection.width
        vmText: loader.getStringForKey(keysearch+"btnAddPatient");
        vmFont: viewHome.gothamM.name
        anchors.left: cbPatientSelection.left
        anchors.top: cbPatientSelection.bottom
        anchors.topMargin: 30
        vmInvertColors: true
        onClicked: {
            viewPatientReg.clearAll();
            swiperControl.currentIndex = swiperControl.vmIndexPatientReg;
        }
    }

    // Message for study selection
    Text {
        id: labelInstruction
        font.family: robotoB.name
        font.pixelSize: 15
        anchors.top: btnAddPatient.bottom
        anchors.topMargin: 22
        anchors.left: btnAddPatient.left
        color: "#5499d5"
        text: loader.getStringForKey(keysearch+"labelInstruction");
    }

    // The possible studies
    VMCheckBox{
        id: cboxReading
        text: loader.getStringForKey(keysearch+"cboxReading");
        font.family: robotoR.name
        font.pixelSize: 13
        anchors.top: labelInstruction.bottom
        anchors.topMargin: 14
        anchors.left: labelInstruction.left
        onCheckedChanged: {
            labelNoInstructionSetError.visible = false;
        }
    }

    VMCheckBox{
        id: cboxBindingUC
        text: loader.getStringForKey(keysearch+"cboxBindingUC");
        font.family: robotoR.name
        font.pixelSize: 13
        anchors.top: labelInstruction.bottom
        anchors.topMargin: 14
        anchors.left: cboxReading.right
        anchors.leftMargin: 20
        onCheckedChanged: {
            labelNoInstructionSetError.visible = false;
        }
    }

    VMCheckBox{
        id: cboxBindingBC
        text: loader.getStringForKey(keysearch+"cboxBindingBC");
        font.family: robotoR.name
        font.pixelSize: 13
        anchors.top: labelInstruction.bottom
        anchors.topMargin: 14
        anchors.left: cboxBindingUC.right
        anchors.leftMargin: 20
        onCheckedChanged: {
            labelNoInstructionSetError.visible = false;
        }

    }

    VMCheckBox{
        id: cboxFielding
        text: loader.getStringForKey(keysearch+"cboxFielding");
        font.family: robotoR.name
        font.pixelSize: 13
        anchors.top: labelInstruction.bottom
        anchors.topMargin: 14
        anchors.left: cboxBindingBC.right
        anchors.leftMargin: 20
        onCheckedChanged: {
            labelNoInstructionSetError.visible = false;
        }

    }

    // Message for configuring experiment options
    Text {
        id: labelInstruction2
        font.family: robotoB.name
        font.pixelSize: 15
        anchors.top: cboxReading.bottom
        anchors.topMargin: 22
        anchors.left: btnAddPatient.left
        color: "#5499d5"
        text: loader.getStringForKey(keysearch+"labelInstruction2");
    }


    Text {
        id: labelEyeMsg
        font.family: robotoB.name
        font.pixelSize: 13
        width: cbEyeMsg.width
        anchors.top: labelInstruction2.bottom
        anchors.topMargin: 22
        anchors.left: btnAddPatient.left
        color: "#d5d5d5"
        text: loader.getStringForKey(keysearch+"labelEyeMsg");
    }

    VMComboBox{
        id: cbEyeMsg
        width: 212
        vmModel:  loader.getStringListForKey(keysearch+"labelEyeType");
        font.family: viewHome.robotoR.name
        font.pixelSize: 13
        anchors.top: labelEyeMsg.bottom
        anchors.topMargin: 5
        anchors.left: labelInstruction2.left
        currentIndex: 2;
    }

    Text {
        id: labelNTargets
        font.family: robotoB.name
        font.pixelSize: 13
        width: cbNumberOfTargets.width
        anchors.top: labelInstruction2.bottom
        anchors.topMargin: 22
        anchors.left: cbNumberOfTargets.left
        color: "#d5d5d5"
        text: loader.getStringForKey(keysearch+"labelNTargets");
    }

    VMComboBox{
        id: cbNumberOfTargets
        width: 212
        vmModel: ["2","3"];
        font.family: viewHome.robotoR.name
        font.pixelSize: 13
        anchors.top: labelNTargets.bottom
        anchors.topMargin: 5
        anchors.left: cbEyeMsg.right
        anchors.leftMargin: 16
    }

    // An error message.
    Text{
        id: labelNoInstructionSetError
        text: loader.getStringForKey(keysearch+"labelNoInstructionSetError");
        color:  "#ca2026"
        font.family: viewHome.robotoM.name
        font.pixelSize: 12
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: buttonRow.top
        anchors.bottomMargin: 10
        visible: false
    }

    // Buttons
    Row{
        id: buttonRow
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: cbNumberOfTargets.bottom
        anchors.topMargin: 36
        spacing: 29

        VMButton{
            id: btnBack
            height: 50
            vmText: loader.getStringForKey(keysearch+"btnBack");
            vmFont: viewHome.gothamM.name
            vmInvertColors: true
            onClicked: {
                swiperControl.currentIndex = swiperControl.vmIndexHome;
            }
        }

        VMButton{
            id: btnStart
            height: 50
            vmText: loader.getStringForKey(keysearch+"btnStart");
            vmFont: viewHome.gothamM.name
            onClicked: {
                var patient_name = cbPatientSelection.currentText;
                var parts = patient_name.split("(");
                var uid = parts[1];
                // The last character is a ) so it is removed.
                uid = uid.slice(0,-1);

                if (cbPatientSelection.currentIndex === 0){
                    cbPatientSelection.vmErrorMsg = loader.getStringForKey(keysearch+"labelNoPatientError");
                    return;
                }
                if (!loader.createPatientDirectory(uid)){
                    vmErrorDiag.vmErrorCode = vmErrorDiag.vmERROR_CREATING_PDIR;
                    var titleMsg = viewHome.getErrorTitleAndMessage("error_patient_dir");
                    vmErrorDiag.vmErrorMessage = titleMsg[1];
                    vmErrorDiag.vmErrorTitle = titleMsg[0];
                    vmErrorDiag.open();
                    return;
                }

                vmSelectedExperiments = [];
                if (cboxReading.checked){
                    vmSelectedExperiments.push(viewPatientReg.vmExpIndexReading);
                }
                if (cboxBindingBC.checked){
                    vmSelectedExperiments.push(viewPatientReg.vmExpIndexBindingBC);
                }
                if (cboxBindingUC.checked){
                    vmSelectedExperiments.push(viewPatientReg.vmExpIndexBindingUC);
                }
                if (cboxFielding.checked){
                    vmSelectedExperiments.push(viewPatientReg.vmExpIndexFielding);
                }
                if (vmSelectedExperiments.length > 0){

                    // All is good, so the parameters are set.
                    loader.setConfigurationInt(vmDefines.vmCONFIG_VALID_EYE,cbEyeMsg.currentIndex);
                    flowControl.setUse3BindingTargets(cbNumberOfTargets.currentIndex === 1);

                    viewPresentExperimet.setTracker(vmSelectedExperiments);
                    swiperControl.currentIndex = swiperControl.vmIndexCalibrationStart
                    vmCurrentExperimentIndex = -1;
                    viewPresentExperimet.advanceCurrentExperiment()
                }
                else{
                    labelNoInstructionSetError.visible = true;
                }

            }
        }
    }


}
