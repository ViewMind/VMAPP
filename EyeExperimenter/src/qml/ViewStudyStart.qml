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
        anchors.topMargin: 80
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#297fca"
        text: loader.getStringForKey(keysearch+"labelTitle");
    }

    Text {
        id: viewSubTitle
        font.family: robotoR.name
        font.pixelSize: 13
        anchors.top:  viewTitle.bottom
        anchors.topMargin: 11
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#bcbec0"
        text: loader.getStringForKey(keysearch+"labelSubTitle");
    }

    Row{

        id: rowPatientAndAdd
        spacing: 10
        anchors.top: viewSubTitle.bottom
        anchors.topMargin: 43
        anchors.horizontalCenter: parent.horizontalCenter

        // Patient selection
        VMComboBox{
            id: cbPatientSelection
            width: 470
            vmModel: [];
            font.family: viewHome.robotoR.name
        }

        VMPlusButton{
            id: btnAddPatient
            height: cbPatientSelection.height
            onClicked: {
                viewPatientReg.clearAll();
                swiperControl.currentIndex = swiperControl.vmIndexPatientReg;
            }
        }

    }

    Row{
        id: rowSelectStudiesInstruction
        anchors.left: rowPatientAndAdd.left
        anchors.top: rowPatientAndAdd.bottom
        anchors.topMargin: 45
        spacing: 5

        // Message for study selection
        Text {
            id: labelSelectStudy
            font.family: robotoB.name
            font.pixelSize: 15
            color: "#000000"
            text: loader.getStringForKey(keysearch+"labelSelectStudy");
        }

        Text {
            id: labelInstruction
            font.family: robotoR.name
            font.pixelSize: 9
            color: "#58595b"
            anchors.verticalCenter: labelSelectStudy.verticalCenter
            text: "(" + loader.getStringForKey(keysearch+"labelInstruction") + ")";
        }

    }


    Row{
        id: rowSelectStudies
        anchors.left: rowSelectStudiesInstruction.left
        spacing: 10
        anchors.top: rowSelectStudiesInstruction.bottom
        anchors.topMargin: 13

        // The possible studies
        VMToggleButton{
            id: cboxReading
            vmText: loader.getStringForKey(keysearch+"cboxReading");
            vmFont: robotoR.name
            onVmOnChanged: {
                labelNoInstructionSetError.visible = false;
            }
        }

        VMToggleButton{
            id: cboxBindingUC
            vmText: loader.getStringForKey(keysearch+"cboxBindingUC");
            vmFont: robotoR.name
            onVmOnChanged: {
                labelNoInstructionSetError.visible = false;
            }
        }

        VMToggleButton{
            id: cboxBindingBC
            vmText: loader.getStringForKey(keysearch+"cboxBindingBC");
            vmFont: robotoR.name
            onVmOnChanged: {
                labelNoInstructionSetError.visible = false;
            }

        }

        VMToggleButton{
            id: cboxFielding
            vmText: loader.getStringForKey(keysearch+"cboxFielding");
            vmFont: robotoR.name
            onVmOnChanged: {
                labelNoInstructionSetError.visible = false;
            }
        }
    }

    // Message for configuring experiment options
    Text {
        id: labelInstruction2
        font.family: robotoB.name
        font.pixelSize: 15
        anchors.top: rowSelectStudies.bottom
        anchors.topMargin: 44
        anchors.left: rowPatientAndAdd.left
        color: "#000000"
        text: loader.getStringForKey(keysearch+"labelInstruction2");
    }


    Text {
        id: labelEyeMsg
        font.family: robotoB.name
        font.pixelSize: 13
        width: cbEyeMsg.width
        anchors.top: labelInstruction2.bottom
        anchors.topMargin: 13
        anchors.left: rowPatientAndAdd.left
        color: "#d5d5d5"
        text: loader.getStringForKey(keysearch+"labelEyeMsg");
    }

    VMComboBox{
        id: cbEyeMsg
        width: (rowPatientAndAdd.width - 16)/3
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
        anchors.top: labelEyeMsg.top
        anchors.left: cbNumberOfTargets.left
        color: "#d5d5d5"
        text: loader.getStringForKey(keysearch+"labelNTargets");
    }

    VMComboBox{
        id: cbNumberOfTargets
        width: cbEyeMsg.width
        vmModel: ["2","3"];
        font.family: viewHome.robotoR.name
        font.pixelSize: 13
        anchors.top: labelNTargets.bottom
        anchors.topMargin: 5
        anchors.left: cbEyeMsg.right
        anchors.leftMargin: 16
    }

    Text {
        id: labelTargetSize
        font.family: robotoB.name
        font.pixelSize: 13
        width: cbNumberOfTargets.width
        anchors.top: labelEyeMsg.top
        anchors.left: cbTargetSize.left
        color: "#d5d5d5"
        text: loader.getStringForKey(keysearch+"labelTargetSize");
    }

    VMComboBox{
        id: cbTargetSize
        width: cbEyeMsg.width
        vmModel: loader.getStringListForKey(keysearch+"targetOptions");
        font.family: viewHome.robotoR.name
        font.pixelSize: 13
        anchors.top: labelNTargets.bottom
        anchors.topMargin: 5
        anchors.left: cbNumberOfTargets.right
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
        spacing: 30

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

                if (cbPatientSelection.currentIndex === 0){
                    cbPatientSelection.vmErrorMsg = loader.getStringForKey(keysearch+"labelNoPatientError");
                    return;
                }

                var patient_name = cbPatientSelection.currentText;
                var parts = patient_name.split("(");
                var uid = parts[1];
                // The last character is a ) so it is removed.
                uid = uid.slice(0,-1);

                if (!loader.createPatientDirectory(uid)){
                    vmErrorDiag.vmErrorCode = vmErrorDiag.vmERROR_CREATING_PDIR;
                    var titleMsg = viewHome.getErrorTitleAndMessage("error_patient_dir");
                    vmErrorDiag.vmErrorMessage = titleMsg[1];
                    vmErrorDiag.vmErrorTitle = titleMsg[0];
                    vmErrorDiag.open();
                    return;
                }

                vmSelectedExperiments = [];
                if (cboxReading.vmOn){
                    vmSelectedExperiments.push(viewPatientReg.vmExpIndexReading);
                }
                if (cboxBindingBC.vmOn){
                    vmSelectedExperiments.push(viewPatientReg.vmExpIndexBindingBC);
                }
                if (cboxBindingUC.vmOn){
                    vmSelectedExperiments.push(viewPatientReg.vmExpIndexBindingUC);
                }
                if (cboxFielding.vmOn){
                    vmSelectedExperiments.push(viewPatientReg.vmExpIndexFielding);
                }
                if (vmSelectedExperiments.length > 0){

                    // All is good, so the parameters are set.
                    loader.setConfigurationInt(vmDefines.vmCONFIG_VALID_EYE,cbEyeMsg.currentIndex,false);
                    loader.setConfigurationInt(vmDefines.vmCONFIG_BINDING_NUMBER_OF_TARGETS,cbNumberOfTargets.currentText,false);
                    loader.setConfigurationBoolean(vmDefines.vmCONFIG_BINDING_TARGET_SMALL,(cbTargetSize.currentIndex == 1),false);

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
