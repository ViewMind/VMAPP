import QtQuick 2.6
import QtQuick.Controls 2.3

VMBase {

    id: viewPatientReg
    width: viewPatientReg.vmWIDTH
    height: viewPatientReg.vmHEIGHT

    readonly property string keysearch: "viewpatientreg_"
    property var vmSelectedExperiments: []
    property int vmCurrentExperimentIndex: 0


    // Title and subtitle
    Text {
        id: viewTitle
        font.family: gothamB.name
        font.pixelSize: 43
        anchors.top:  vmBanner.bottom
        anchors.topMargin: 129
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#3fa2f7"
        text: loader.getStringForKey(keysearch+"viewTitle");
    }

    Text {
        id: viewSubTitle
        font.family: robotoR.name
        font.pixelSize: 13
        anchors.top:  viewTitle.bottom
        anchors.topMargin: 31
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#5499d5"
        text: loader.getStringForKey(keysearch+"viewSubTitle");
    }

    // Input fields
    VMRegistrationTextInput {
        id: liPatientPlaceholder
        vmEnteredText: "a";
        vmFont: viewHome.robotoM.name
        vmPlaceHolder: loader.getStringForKey(keysearch+"liPatientPlaceholder");
        width: 410
        height: 50
        anchors.top: viewSubTitle.bottom
        anchors.topMargin: 55
        anchors.left: parent.left
        anchors.leftMargin: 323
    }

    VMRegistrationTextInput {
        id: liAgePlaceholder
        vmFont: viewHome.robotoM.name
        vmEnteredText: "5";
        vmNumbersOnly: true
        vmPlaceHolder: loader.getStringForKey(keysearch+"liAgePlaceholder");
        width: 192
        height: 50
        anchors.top: viewSubTitle.bottom
        anchors.topMargin: 55
        anchors.left: liPatientPlaceholder.right
        anchors.leftMargin: 17
    }

    VMRegistrationTextInput {
        id: liEmailPlaceholder
        vmFont: viewHome.robotoM.name
        vmPlaceHolder: loader.getStringForKey(keysearch+"liEmailPlaceholder");
        width: (liAgePlaceholder.width + liAgePlaceholder.x - liPatientPlaceholder.x)
        height: 50
        anchors.top: liPatientPlaceholder.bottom
        anchors.topMargin: 23
        anchors.left: liPatientPlaceholder.left
    }

    // The experiment checkboxes
    Text {
        id: labelExperimentTitle
        font.family: robotoB.name
        font.pixelSize: 13
        font.bold: true
        anchors.top:  liEmailPlaceholder.bottom
        anchors.topMargin: 24
        anchors.left: liEmailPlaceholder.left
        color: "#5499d5"
        text: loader.getStringForKey(keysearch+"labelExperimentTitle");
    }

    VMCheckBox{
        id: cboxReading
        text: loader.getStringForKey(keysearch+"cboxReading");
        font.family: robotoR.name
        font.pixelSize: 13
        anchors.top: labelExperimentTitle.bottom
        anchors.topMargin: 14
        anchors.left: labelExperimentTitle.left
        checked: true;
        onCheckedChanged: {
            labelNoInstructionSetError.visible = false;
        }
    }

    VMCheckBox{
        id: cboxBindingUC
        text: loader.getStringForKey(keysearch+"cboxBindingUC");
        font.family: robotoR.name
        font.pixelSize: 13
        anchors.top: labelExperimentTitle.bottom
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
        anchors.top: labelExperimentTitle.bottom
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
        anchors.top: labelExperimentTitle.bottom
        anchors.topMargin: 14
        anchors.left: cboxBindingBC.right
        anchors.leftMargin: 20
        // TODO: Fix Fielding and make this visible again
        visible: false
        onCheckedChanged: {
            labelNoInstructionSetError.visible = false;
        }

    }

    // The instruction text
    Text {
        id: labelInstruction
        font.family: robotoR.name
        font.pixelSize: 12
        anchors.top:  cboxReading.bottom
        anchors.topMargin: 14
        anchors.left: cboxReading.left
        color: "#cfcfcf"
        text: loader.getStringForKey(keysearch+"labelInstruction");
    }

    // The error
    Text {
        id: labelNoInstructionSetError
        font.family: robotoR.name
        font.pixelSize: 12
        anchors.top:  cboxReading.bottom
        anchors.topMargin: 14
        anchors.left: labelInstruction.right
        anchors.leftMargin: 15
        color: "#ca2026"
        text: loader.getStringForKey(keysearch+"labelNoInstructionSetError");
        visible: false
    }


    // The accept and cancel buttons
    VMButton{
        id: btnCancel
        vmInvertColors: true
        vmSize: [124, 50]
        vmText: loader.getStringForKey(keysearch+"btnCancel");
        vmFont: gothamM.name
        anchors.top: labelInstruction.bottom
        anchors.topMargin: 35
        anchors.left: parent.left
        anchors.leftMargin: 500
        onClicked: {
            swiperControl.currentIndex = swiperControl.vmIndexHome;
        }
    }

    VMButton{
        id: btnOK
        vmSize: [124, 50]
        vmText: loader.getStringForKey(keysearch+"btnOK");
        vmFont: gothamM.name
        anchors.top: labelInstruction.bottom
        anchors.topMargin: 35
        anchors.left: btnCancel.right
        anchors.leftMargin: 30
        onClicked: {

            var patient_name = liPatientPlaceholder.vmEnteredText;
            var patient_age = liAgePlaceholder.vmEnteredText;
            var patient_email = liEmailPlaceholder.vmEnteredText;

            if (patient_name === ""){
                liPatientPlaceholder.vmErrorMsg = loader.getStringForKey(keysearch+"error_empty_patient_name");
                return;
            }

            if (patient_age === ""){
                liAgePlaceholder.vmErrorMsg = loader.getStringForKey(keysearch+"error_empty_patient_age");
                return;
            }

            if (!loader.createPatientDirectory(patient_name,patient_age,patient_email)){
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
