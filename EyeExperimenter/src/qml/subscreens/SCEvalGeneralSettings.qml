import QtQuick
import "../"
import "../components"
import "../js/ObjectListSorter.js" as OLS

Rectangle {

    id: subScreenEvaluationGeneralSettings

    z:10

    property int vmSelectedDoctorIndex: -1
    property var vmExpLangCodeList: [];

    signal goToEvalRun();

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

        // Every time we go in here we clear the dr error just in case.
        doctorSelection.vmErrorMsg = "";

        // Getting the last selected protocol
        var selectedprotocol = loader.getSettingsString("last_selected_protocol");
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

        // The code in this section is only run when creating a NEW Evaluation.

        if (doctorSelection.vmCurrentIndex === -1){
            doctorSelection.vmErrorMsg = loader.getStringForKey("viewevaluation_must_select_doctor");
            return;
        }

        // Storing the last selected protocol
        let selectedProtocol = protocol.getCurrentlySelectedMetaDataField();
        loader.setSettingsValue("last_selected_protocol",selectedProtocol);

        // Checking if the selected doctor for the patient changed. If it did, then it means that we must updated in the DB.
        if (doctorSelection.vmCurrentIndex !== vmSelectedDoctorIndex){
            // There was a changed and it must be saved.
            let newDoctor = doctorSelection.getCurrentlySelectedMetaDataField()
            loader.modifySubjectSelectedMedic(newDoctor);
        }

        let doctorID = doctorSelection.getCurrentlySelectedMetaDataField();

        // Create an evaluation and setup the sequence.
        let avalialbleEvaluations = loader.getAvailableEvaluations();
        let selectedEvaluationIndex = avalialbleEvaluations[evaluationSelection.vmCurrentIndex].id;

        // Once the evaluations are setup we KNOW that this is the start of an evaluation run. This is when the calibration history needs to be reset.
        flowControl.resetCalibrationHistory();

        // Since this is the start of a new evaluation we need to create a new entry in the subject database.
        flowControl.newEvaluation(selectedEvaluationIndex,doctorID,selectedProtocol);

        goToEvalRun();

    }

    function reloadEvalSequenceList(){

        let avalialbleEvaluations = loader.getAvailableEvaluations();
        var list = [];
        for (let i = 0; i < avalialbleEvaluations.length; i++){
            list.push(avalialbleEvaluations[i].name);
        }
        evaluationSelection.setModelList(list);
        evaluationSelection.setSelection(0);

    }

    function updateTaskList(){

        let available_evaluations = loader.getAvailableEvaluations();

        // This line is mostly here to avoid pointless warning in code.
        if ((evaluationSelection.vmCurrentIndex < 0) || (evaluationSelection.vmCurrentIndex >= available_evaluations.length)) return;

        let task_list_codes = available_evaluations[evaluationSelection.vmCurrentIndex].tasks;
        let taskNameMap     = loader.getTaskCodeToNameMap();

        // So now we use the name map to transform code to names.
        let names = [];
        for (let i = 0; i < task_list_codes.length; i++){
            let code = task_list_codes[i];
            if (code in taskNameMap){
                names.push(taskNameMap[code]);
            }
            else {
                names.push(code)
            }
        }


        //bkgTaskList.setTaskList(names)
        taskList.configure(loader.getStringForKey("viewevaluation_task_list"),names)

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
            id: evaluationSelection
            width: parent.width
            vmLabel: loader.getStringForKey("viewevaluation_eval_setup")
            z: doctorSelection.z + 1

            onVmCurrentIndexChanged: {
                //console.log("Changed selection now points to object: " + JSON.stringify(vmEvaluationTaskComposition[vmCurrentIndex]))
                updateTaskList();
            }
        }

        VMFilteringComboBox {
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

    VMEvaluationTaskDisplayCard {
        id: taskList
        width:   parent.width*0.36;
        anchors.top: formColumn.top
        anchors.left: formColumn.right
        anchors.leftMargin: VMGlobals.adjustWidth(40)
    }


}
