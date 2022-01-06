import QtQuick
import "../"
import "../components"
import "../js/ObjectListSorter.js" as OLS

Rectangle {

    id: subScreenEvaluationGeneralSettings

    property int vmSelectedDoctorIndex: -1

    signal goToEvalSetup();

    function prepareSettings(){

        // Settign the Eye Configuration to the Default.
        eye.setSelection(2);

        // Filling the doctor selection combo box.
        var medics = loader.getMedicList();
        let doctorList = [];
        for (var key in medics){
            //console.log(key + " => " + medics[key])
            doctorList.push({ "value" : medics[key], "metadata" : key });
        }

        doctorSelection.setModelList(doctorList);
        doctorSelection.setSelection(-1);

        // Setting the default value for the selected patient.
        var preferred_doctor = loader.getCurrentlySelectedAssignedDoctor();
        //console.log("Preferred doctor for selected patient is: " + preferred_doctor);
        vmSelectedDoctorIndex = -1;
        for (var i in doctorList){
            if (doctorList[i]["metadata"] === preferred_doctor){
                vmSelectedDoctorIndex = i
                doctorSelection.setSelection(i);
                break;
            }
        }

        // Getting the last selected protocol
        var selectedprotocol = loader.getConfigurationString("last_selected_protocol");
        var selectedProtocolIndex = 0;

        // Filling the protocol list while also searching for the last selected protocol
        let protocolMap = loader.getProtocolList();
        let protocolModelList = [];        

        let noprotocol = loader.getStringForKey("viewevaluation_no_protocol");
        let item_np = { "value" : noprotocol, "metadata" : ""  }
        protocolModelList.push(item_np);
        i = 1;

        for (var protocolID in protocolMap){
            let name = protocolMap[protocolID]["protocol_name"];
            let item = { "value" : name + " (" + protocolID + ")", "metadata" : protocolID  }
            protocolModelList.push(item)
            if (protocolID === selectedprotocol) {
                selectedProtocolIndex = i;
            }
            i++;
        }

        protocol.setModelList(protocolModelList);
        protocol.setSelection(selectedProtocolIndex);

    }

    function onNext(){
        if (doctorSelection.vmCurrentIndex === -1){
            doctorSelection.vmErrorMsg = loader.getStringForKey("viewevaluation_must_select_doctor");
            return;
        }

        // Storing the last selected protocol
        let selectedProtocol = protocol.getCurrentlySelectedMetaDataField();        
        loader.setSettingsValue("last_selected_protocol",selectedProtocol);
        viewEvaluations.vmSelectedProtocol = selectedProtocol;

        // Checking if the selected doctor for the patient changed. If it did, then it means that we must updated in the DB.
        if (doctorSelection.vmCurrentIndex !== vmSelectedDoctorIndex){
            // There was a changed and it must be saved.
            let newDoctor = doctorSelection.getCurrentlySelectedMetaDataField()
            loader.modifySubjectSelectedMedic(viewEvaluations.vmSelectedPatientID,newDoctor);
        }

        switch(eye.vmCurrentIndex){
        case 0: viewEvaluations.vmSelectedEye = VMGlobals.vmSCV_EYE_LEFT;
            break;
        case 1: viewEvaluations.vmSelectedEye = VMGlobals.vmSCV_EYE_RIGHT;
            break;
        case 2: viewEvaluations.vmSelectedEye = VMGlobals.vmSCV_EYE_BOTH;
            break;
        }

        viewEvaluations.vmSelectedDoctor = doctorSelection.getCurrentlySelectedMetaDataField();

        goToEvalSetup();
    }

    Text {
        id: title
        text: loader.getStringForKey("viewevaluation_general_settings")
        font.pixelSize: VMGlobals.vmFontExtraExtraLarge
        font.weight: 600
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: VMGlobals.adjustHeight(36)
        anchors.leftMargin: VMGlobals.adjustWidth(30)
    }

    Column {

        id: formColumn
        spacing: VMGlobals.adjustHeight(54)
        anchors.left: title.left
        anchors.top: title.bottom
        anchors.topMargin: VMGlobals.adjustHeight(54)
        width: VMGlobals.adjustWidth(340)
        z: parent.z + 1

        VMComboBox {
            id: autoStudySetup
            width: parent.width
            vmLabel: loader.getStringForKey("viewevaluation_eval_setup")
            z: doctorSelection.z + 1
            Component.onCompleted: {
                var list = [];
                list.push(loader.getStringForKey("viewevaluation_eval_setup_list"))
                setModelList(list);
                setSelection(0);
            }
        }

        VMComboBox {
            id: doctorSelection
            width: parent.width
            vmLabel: loader.getStringForKey("viewevaluation_doctor")
            vmPlaceHolderText: loader.getStringForKey("viewevaluation_doctor_ph")
            z: protocol.z + 1
            //            onVmCurrentIndexChanged: {
            //                console.log("Selected doctor changed to: " + getCurrentlySelectedMetaDataField());
            //            }
        }

        VMComboBox {
            id: protocol
            width: parent.width
            z: eye.z + 1;
            vmLabel: loader.getStringForKey("viewpatlist_protocol")
            vmPlaceHolderText: loader.getStringForKey("viewevaluation_protocol_ph")
        }

        VMComboBox {
            id: eye
            width: parent.width
            vmLabel: loader.getStringForKey("viewevaluation_eye_text")
            z: parent.z + 1
            vmMaxDisplayItems: 2
            Component.onCompleted: {
                var list = loader.getStringListForKey("viewevaluation_eye_selection_list")
                setModelList(list)
                setSelection(2); // Both eyes selected by default
            }
        }

    }


}
