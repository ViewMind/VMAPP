import QtQuick 2.6
import QtQuick.Controls 2.3

VMBase {

    id: viewStudyStart
    width: viewStudyStart.vmWIDTH
    height: viewStudyStart.vmHEIGHT

    readonly property string keysearch: "viewstudystart_"
    property var vmSelectedExperiments: []
    property int vmCurrentExperimentIndex: 0

    function setPatientName(){
        labelPatientName.text = loader.getConfigurationString(vmDefines.vmCONFIG_PATIENT_NAME);
    }

    // Title and subtitle
    Text {
        id: viewTitle
        font.family: gothamB.name
        font.pixelSize: 43
        anchors.top:  vmBanner.bottom
        anchors.topMargin: 50
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


    // Message for study selection
    Text {
        id: labelSelPatient
        font.family: robotoB.name
        font.pixelSize: 15
        color: "#000000"
        text: loader.getStringForKey(keysearch+"labelSelPatient");
        anchors.top: viewSubTitle.bottom
        anchors.topMargin: 43
        anchors.left: backgroundPatientName.left
    }

    Rectangle{
        id: backgroundPatientName
        width: 533
        height: 40
        radius: 5
        color: "#ebf3fa"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: labelSelPatient.bottom
        anchors.topMargin: 13

        Text {
            id: labelPatientName
            font.family: robotoR.name
            font.pixelSize: 15
            color: "#58595b"
            anchors.verticalCenter: backgroundPatientName.verticalCenter
            text: "";
            anchors.left: backgroundPatientName.left
            anchors.leftMargin: 10
        }

    }

    Row{
        id: rowSelectStudiesInstruction
        anchors.left: backgroundPatientName.left
        anchors.top: backgroundPatientName.bottom
        anchors.topMargin: 30
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
        anchors.topMargin: 5

        // The possible studies
        VMToggleButton{
            id: cboxReading
            vmText: loader.getStringForKey(keysearch+"cboxReading");
            vmFont: robotoR.name
            onVmOnChanged: {
                labelNoInstructionSetError.visible = false;
                if (cboxReading.vmOn) multipleSelectionShow.addItem(vmText,vmDefines.vmCONFIG_P_EXP_READING);
                else multipleSelectionShow.removeItemByID(vmDefines.vmCONFIG_P_EXP_READING)
            }
        }

        VMToggleButton{
            id: cboxBindingUC
            vmText: loader.getStringForKey(keysearch+"cboxBindingUC");
            vmFont: robotoR.name
            onVmOnChanged: {
                labelNoInstructionSetError.visible = false;
                if (cboxBindingUC.vmOn) multipleSelectionShow.addItem(vmText,vmDefines.vmCONFIG_P_EXP_BIDING_UC);
                else multipleSelectionShow.removeItemByID(vmDefines.vmCONFIG_P_EXP_BIDING_UC)
            }
        }

        VMToggleButton{
            id: cboxBindingBC
            vmText: loader.getStringForKey(keysearch+"cboxBindingBC");
            vmFont: robotoR.name
            onVmOnChanged: {
                labelNoInstructionSetError.visible = false;
                if (cboxBindingBC.vmOn) multipleSelectionShow.addItem(vmText,vmDefines.vmCONFIG_P_EXP_BIDING_BC);
                else multipleSelectionShow.removeItemByID(vmDefines.vmCONFIG_P_EXP_BIDING_BC)
            }

        }

        VMToggleButton{
            id: cboxFielding
            vmText: loader.getStringForKey(keysearch+"cboxFielding");
            vmFont: robotoR.name
            visible: false
            onVmOnChanged: {
                labelNoInstructionSetError.visible = false;
                if (cboxFielding.vmOn) multipleSelectionShow.addItem(vmText,vmDefines.vmCONFIG_P_EXP_FIELDING);
                else multipleSelectionShow.removeItemByID(vmDefines.vmCONFIG_P_EXP_FIELDING)
            }
        }
    }


    VMMultipleSelectionShow{
        id: multipleSelectionShow
        width: backgroundPatientName.width
        height: 40
        anchors.top: rowSelectStudies.bottom
        anchors.topMargin: 15
        anchors.horizontalCenter: parent.horizontalCenter
    }


    // Message for configuring experiment options
    Text {
        id: labelInstruction2
        font.family: robotoB.name
        font.pixelSize: 15
        anchors.top: multipleSelectionShow.bottom
        anchors.topMargin: 30
        anchors.left: backgroundPatientName.left
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
        anchors.left: backgroundPatientName.left
        color: "#d5d5d5"
        text: loader.getStringForKey(keysearch+"labelEyeMsg");
    }

    VMComboBox{
        id: cbEyeMsg
        width: (backgroundPatientName.width - 16)/4
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

    Text {
        id: labelReadingLanguge
        font.family: robotoB.name
        font.pixelSize: 13
        width: cbNumberOfTargets.width
        anchors.top: labelEyeMsg.top
        anchors.left: cbReadingLang.left
        color: "#d5d5d5"
        text: loader.getStringForKey(keysearch+"labelReadingLanguge");
    }

    VMComboBox{
        id: cbReadingLang
        width: cbEyeMsg.width
        vmModel: ["English", "Español", "Deutsche"];
        font.family: viewHome.robotoR.name
        font.pixelSize: 13
        anchors.top: labelNTargets.bottom
        anchors.topMargin: 5
        anchors.left: cbTargetSize.right
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
                swiperControl.currentIndex = swiperControl.vmIndexPatientList;
            }
        }

        VMButton{
            id: btnStart
            height: 50
            vmText: loader.getStringForKey(keysearch+"btnStart");
            vmFont: viewHome.gothamM.name
            onClicked: {

                vmSelectedExperiments = [];

                var idList = multipleSelectionShow.getListID();
                for (var i = 0; i < idList.length; i++){
                    if (idList[i] === vmDefines.vmCONFIG_P_EXP_BIDING_BC){
                        vmSelectedExperiments.push(viewPatientReg.vmExpIndexBindingBC);
                    }
                    else if (idList[i] === vmDefines.vmCONFIG_P_EXP_READING){
                        vmSelectedExperiments.push(viewPatientReg.vmExpIndexReading);
                    }
                    else if (idList[i] === vmDefines.vmCONFIG_P_EXP_BIDING_UC){
                        vmSelectedExperiments.push(viewPatientReg.vmExpIndexBindingUC);
                    }
                    else if (idList[i] === vmDefines.vmCONFIG_P_EXP_FIELDING){
                        vmSelectedExperiments.push(viewPatientReg.vmExpIndexFielding);
                    }
                }

                if (vmSelectedExperiments.length > 0){

                    // All is good, so the parameters are set.
                    loader.setValueForConfiguration(vmDefines.vmCONFIG_VALID_EYE,cbEyeMsg.currentIndex,false);
                    loader.setValueForConfiguration(vmDefines.vmCONFIG_BINDING_NUMBER_OF_TARGETS,cbNumberOfTargets.currentText,false);
                    loader.setValueForConfiguration(vmDefines.vmCONFIG_BINDING_TARGET_SMALL,(cbTargetSize.currentIndex == 1),false);

                    var readlang;
                    switch(cbReadingLang.currentIndex){
                    case 0:
                        readlang = "English";
                        break;
                    case 1:
                        readlang = "Spanish";
                        break;
                    case 2:
                        readlang = "Deutsche"
                    }
                    loader.setValueForConfiguration(vmDefines.vmCONFIG_READING_EXP_LANG,readlang,false);

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
