import QtQuick
import "../"
import "../components"
import "../js/ObjectListSorter.js" as OLS

Rectangle {

    id: subScreenEvaluationGeneralSettings

    z:10

    property int vmSelectedDoctorIndex: -1
    property var vmExpLangCodeList: [];

    signal goToEvalSetup();

    function prepareSettings(){

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

        reloadEvalSequenceList()

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

        viewEvaluations.vmSelectedEye = VMGlobals.vmSCV_EYE_BOTH;
        viewEvaluations.vmSelectedDoctor = doctorSelection.getCurrentlySelectedMetaDataField();

        goToEvalSetup();
    }

    function clearSequenceSelection(){
        autoStudySetup.setSelection = 0;
    }

    function reloadEvalSequenceList(){
        var list = [];
        list.push(loader.getStringForKey("viewevaluation_eval_setup_list"))

        var map = loader.getStudySequenceListAndCurrentlySelected();
        var options = map.list;
        var selected = map.current;
        let selectedIndex = 0;

        for (let i = 0; i < options.length; i++){
            if (options[i] === selected){
                selectedIndex = i+1 // 0 is selecting nothing.
            }
            list.push(options[i]);
        }

        autoStudySetup.setModelList(list);
        autoStudySetup.setSelection(selectedIndex);
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
            vmShowRemoveButton: true
            z: doctorSelection.z + 1
            Component.onCompleted: {
                reloadEvalSequenceList()
            }

            onRemoveClicked: function (index, text) {
                loader.deleteStudySequence(text);
                reloadEvalSequenceList();
            }
            onVmCurrentIndexChanged: {
                // console.log("Auto study setup with vmIndex of " + vmCurrentIndex)
                if (vmCurrentIndex == -1) return;
                if (vmCurrentIndex == 0){
                    viewEvaluations.setCurrentStudySequence("");
                }
                else {
                    viewEvaluations.setCurrentStudySequence(vmCurrentText);
                }
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
            z: explanationLanguage.z + 1;
            vmLabel: loader.getStringForKey("viewpatlist_protocol")
            vmPlaceHolderText: loader.getStringForKey("viewevaluation_protocol_ph")
        }

        VMComboBox {
            id: explanationLanguage
            width: parent.width
            vmLabel: loader.getStringForKey("viewevaluation_exp_lang")
            z: parent.z + 1
            vmMaxDisplayItems: 2
            Component.onCompleted: {

                var map = loader.getExplanationLangMap();
                let currenSelection = map["--"];
                let lang_list = []
                vmExpLangCodeList = [];
                let selection = 0;
                for (let key in map){
                    if (key === "--") continue;
                    let value = map[key];
                    if (value === currenSelection) selection = lang_list.length;
                    lang_list.push(key);
                    vmExpLangCodeList.push(value);
                }
                setModelList(lang_list)
                // console.log("Setting selection to index " + selection + " from lang code " + currenSelection)
                setSelection(selection);

            }
            onVmCurrentIndexChanged: {
                let lang_code = vmExpLangCodeList[vmCurrentIndex]
                loader.setSettingsValue("explanation_language",lang_code);
                loader.setExplanationLanguage();
            }
        }

    }


}
