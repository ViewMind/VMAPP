import QtQuick
import QtQuick.Controls
import "../"
import "../components"

Rectangle {


    id: evaluationConfiguration

    property var vmSelectedStudies: [];
    property var vmSelectedOptionsForEachStudy: ({});

    readonly property int vmNBACK_RT_STD_HOLD_TIME: 250
    readonly property int vmNBACK_RT_SLOW_HOLD_TIME: 400
    readonly property int vmNBACK_RT_TIME_OUT_DEFAULT: 3000
    readonly property int vmNBACK_RT_TIME_OUT_SLOW: 5000

    signal goToEvalRun();

    function resetStudySelection(){

        availableEvaluations.clear()
        vmSelectedOptionsForEachStudy = {};
        vmSelectedStudies = [];

        let item = {}
        let options = {}

        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS] = {}
        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS][VMGlobals.vmSCO_OPTION_NAME] = loader.getStringForKey("viewevaluation_number_of_targets");
        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS][VMGlobals.vmSCO_OPTION_VAlUES] = [2,3];
        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS][VMGlobals.vmSCO_OPTION_SELECTED] = 0;
        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS][VMGlobals.vmSCO_OPTION_WIDTH] = 100;
        item = {
            vmIndex: VMGlobals.vmINDEX_BINDING_UC,
            vmStudyName : loader.getStringForKey("viewevaluation_eval_binding") ,
            vmIsLastSelected: false,
            vmOptions: options,
            vmOrder: VMGlobals.vmSCP_NUMBER_OF_TARGETS,
            vmOptionValueMap: "2|3"
        }
        availableEvaluations.append(item)

        item = {}
        options = {}
        options[VMGlobals.vmSCP_NBACKRT_HOLD_TIME] = {}
        options[VMGlobals.vmSCP_NBACKRT_HOLD_TIME][VMGlobals.vmSCO_OPTION_NAME]   = loader.getStringForKey("viewevaluation_nbackrt_hold_time");
        options[VMGlobals.vmSCP_NBACKRT_HOLD_TIME][VMGlobals.vmSCO_OPTION_VAlUES] = [loader.getStringForKey("viewevaluation_nbackrt_hold_time_default"),
                                                                                     loader.getStringForKey("viewevaluation_nbackrt_hold_time_slow")];
        options[VMGlobals.vmSCP_NBACKRT_HOLD_TIME][VMGlobals.vmSCO_OPTION_SELECTED] = 0;
        options[VMGlobals.vmSCP_NBACKRT_HOLD_TIME][VMGlobals.vmSCO_OPTION_WIDTH]    = 30;

        item = {
            vmIndex: VMGlobals.vmINDEX_NBACKRT,
            vmStudyName : loader.getStringForKey("viewevaluation_eval_nbackrt") ,
            vmIsLastSelected: false,
            vmOptions: options,
            vmOrder: VMGlobals.vmSCP_NBACKRT_HOLD_TIME,
            vmOptionValueMap: vmNBACK_RT_STD_HOLD_TIME + "|" + vmNBACK_RT_SLOW_HOLD_TIME // Default speed is a hold time of 250 ms. While slow speed is a hold time of 400 ms.
        }
        availableEvaluations.append(item)

        item = {}
        options = {}
        item = {
            vmIndex: VMGlobals.vmINDEX_GONOGO,
            vmStudyName : loader.getStringForKey("viewevaluation_eval_gonogo") ,
            vmIsLastSelected: false,
            vmOptions: options,
            vmOrder: "",
            vmOptionValueMap: ""
        }
        availableEvaluations.append(item)

        item = {}

        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS] = {}
        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS][VMGlobals.vmSCO_OPTION_NAME] = loader.getStringForKey("viewevaluation_number_of_targets");
        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS][VMGlobals.vmSCO_OPTION_VAlUES] = [3,4,5,6];
        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS][VMGlobals.vmSCO_OPTION_SELECTED] = 0;
        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS][VMGlobals.vmSCO_OPTION_WIDTH] = 35;

        options[VMGlobals.vmSCP_NBACK_LIGHTUP] = {}
        options[VMGlobals.vmSCP_NBACK_LIGHTUP][VMGlobals.vmSCO_OPTION_NAME] = loader.getStringForKey("viewevaluation_nback_light_up_title");
        options[VMGlobals.vmSCP_NBACK_LIGHTUP][VMGlobals.vmSCO_OPTION_VAlUES] = [loader.getStringForKey("viewevaluation_nback_light_up_correct"),
                                                                                 loader.getStringForKey("viewevaluation_nback_light_up_all")];
        options[VMGlobals.vmSCP_NBACK_LIGHTUP][VMGlobals.vmSCO_OPTION_SELECTED] = 0;
        options[VMGlobals.vmSCP_NBACK_LIGHTUP][VMGlobals.vmSCO_OPTION_WIDTH] = 30;

        item = {
            vmIndex: VMGlobals.vmINDEX_NBACKVS,
            vmStudyName : loader.getStringForKey("viewevaluation_eval_nbackvs") ,
            vmIsLastSelected: false,
            vmOrder: VMGlobals.vmSCP_NUMBER_OF_TARGETS + "|" + VMGlobals.vmSCP_NBACK_LIGHTUP,
            vmOptions: options,
            vmOptionValueMap: "3|4|5|6||false|true"
        }
        availableEvaluations.append(item)

        item = {}
        options = {};
        options[VMGlobals.vmSCP_HAND_TO_USE] = {}
        options[VMGlobals.vmSCP_HAND_TO_USE][VMGlobals.vmSCO_OPTION_NAME] = loader.getStringForKey("viewevaluation_gng3D_hand_sel");
        options[VMGlobals.vmSCP_HAND_TO_USE][VMGlobals.vmSCO_OPTION_VAlUES] = [loader.getStringForKey("viewevaluation_gng3D_hand_right"),
                                                                               loader.getStringForKey("viewevaluation_gng3D_hand_left"),
                                                                               loader.getStringForKey("viewevaluation_gng3D_hand_both")];
        options[VMGlobals.vmSCP_HAND_TO_USE][VMGlobals.vmSCO_OPTION_SELECTED] = 2;
        options[VMGlobals.vmSCP_HAND_TO_USE][VMGlobals.vmSCO_OPTION_WIDTH] = 30;

        item = {
            vmIndex: VMGlobals.vmINDEX_GONOGO3D,
            vmStudyName : loader.getStringForKey("viewevaluation_eval_gonogo3D") ,
            vmIsLastSelected: false,
            vmOptions: options,
            vmOrder: VMGlobals.vmSCP_HAND_TO_USE,
            vmOptionValueMap: "right|left|both" // These are the values inside the study configuration map corresponding to each of the option values.
        }
        availableEvaluations.append(item)

    }

    function reorderStudies(studyCode,isSelected){

        for (var j = 0; j < availableEvaluations.count; j++){
            availableEvaluations.get(j).vmIsLastSelected = false;
        }


        if (isSelected){
            // Needs to be added to the study list.
            vmSelectedStudies.push(studyCode)
        }
        else{
            // Needs to be removed from the study lis.
            let indexToRemove = vmSelectedStudies.indexOf(studyCode)
            if (indexToRemove > -1){
                vmSelectedStudies.splice(indexToRemove,1)
            }
        }

        for (var i = 0; i < vmSelectedStudies.length; i++){
            let study = vmSelectedStudies[i];
            // First we find the study in the list.
            for (j = 0; j < availableEvaluations.count; j++){
                if (availableEvaluations.get(j).vmIndex === study){
                    if (i == (vmSelectedStudies.length-1)){
                        //console.log("Last in selected list is " + study)
                        availableEvaluations.setProperty(j,"vmIsLastSelected",true)
                    }
                    // Found the study. We now do the move.
                    availableEvaluations.move(j,i,1)
                    break;
                }
            }
        }

    }

    function setupEvaluations(){

        // Clearing the previous selection.
        // console.log("Setting up evaluations");
        viewEvaluations.vmSelectedEvaluationConfigurations = []

        // Names used for setting up the progress line.
        let study_names = [];

        // The progress line changes if the evaluation requires hand calibration. (Horizontal First and Then Vertical)
        let requires_hand_calibration = [];

        if (vmSelectedStudies.length == 0){
            popUpNotify(VMGlobals.vmNotificationRed,loader.getStringForKey("viewevaluation_err_at_least_1_study"))
            return;
        }

        for (let i = 0; i < availableEvaluations.count; i++){

            let study_unique_id = availableEvaluations.get(i).vmIndex;
            let study_name = availableEvaluations.get(i).vmStudyName;

            if (vmSelectedStudies.indexOf(study_unique_id) != -1){
                let configuration = {};
                configuration[VMGlobals.vmSCP_EYES] = viewEvaluations.vmSelectedEye;
                configuration[VMGlobals.vmUNIQUE_STUDY_ID] = study_unique_id;

                let options = {}
                if (study_unique_id in vmSelectedOptionsForEachStudy){
                    options = vmSelectedOptionsForEachStudy[study_unique_id]
                    //console.log("Selected options are")
                    //console.log(JSON.stringify(options));
                }

                // Storing all options.
                for (let key in options){
                    // A MAP Between option name and value.
                    configuration[key] = options[key]
                }

                switch (study_unique_id){
                case VMGlobals.vmINDEX_BINDING_UC:

                    study_names.push(loader.getStringForKey("viewevaluation_binding_uc"));
                    study_names.push(loader.getStringForKey("viewevaluation_binding_bc"));
                    requires_hand_calibration.push(false);
                    requires_hand_calibration.push(false);

                    // Even though the option is no longer selectable, in the backend still exists. It needs to be set anyways
                    configuration[VMGlobals.vmSCP_TARGET_SIZE] = VMGlobals.vmSCV_BINDING_TARGETS_LARGE;

                    // Standard 2D study.
                    configuration[VMGlobals.vmSCP_STUDY_REQ_H_CALIB] = "";
                    configuration[VMGlobals.vmSCP_IS_STUDY_3D] = false;

                    // This actually represents two studies, so we need to select both, with the same configuration.
                    let config2 = JSON.parse(JSON.stringify(configuration)) // Deep copying configuration.
                    config2[VMGlobals.vmUNIQUE_STUDY_ID] = VMGlobals.vmINDEX_BINDING_BC

                    viewEvaluations.vmSelectedEvaluationConfigurations.push(configuration); // UC goes first.
                    viewEvaluations.vmSelectedEvaluationConfigurations.push(config2) // BC goes second.
                    break;
                case VMGlobals.vmINDEX_GONOGO3D:
                    study_names.push(study_name);

                    // Standard 3D study.
                    configuration[VMGlobals.vmSCP_STUDY_REQ_H_CALIB] = configuration[VMGlobals.vmSCP_HAND_TO_USE];
                    configuration[VMGlobals.vmSCP_IS_STUDY_3D] = true;
                    //configuration[VMGlobals.vmSCP_HAND_TO_USE] = "both";
                    configuration[VMGlobals.vmSCP_MIN_SPEED] = 10;
                    configuration[VMGlobals.vmSCP_MAX_SPEED] = 100;
                    configuration[VMGlobals.vmSCP_INITIAL_SPEED] = 30;

                    requires_hand_calibration.push(true);

                    viewEvaluations.vmSelectedEvaluationConfigurations.push(configuration);
                    break;

                case VMGlobals.vmINDEX_NBACKRT:
                    study_names.push(study_name)
                    // Standard 2D study.
                    configuration[VMGlobals.vmSCP_STUDY_REQ_H_CALIB] = "";
                    configuration[VMGlobals.vmSCP_IS_STUDY_3D] = false;

                    // If the slow version of the study was selected the number of targets is 4.
                    if (configuration[VMGlobals.vmSCP_NBACKRT_HOLD_TIME] == vmNBACK_RT_STD_HOLD_TIME){ // Will assume
                        configuration[VMGlobals.vmSCP_NUMBER_OF_TARGETS] = 3;
                        configuration[VMGlobals.vmSCP_NBACKRT_TIMEOUT] = vmNBACK_RT_TIME_OUT_DEFAULT
                    }
                    else {
                        configuration[VMGlobals.vmSCP_NUMBER_OF_TARGETS] = 4;
                        configuration[VMGlobals.vmSCP_NBACKRT_TIMEOUT] = vmNBACK_RT_TIME_OUT_SLOW
                    }

                    // Adding it to the configuration list.
                    viewEvaluations.vmSelectedEvaluationConfigurations.push(configuration);

                    requires_hand_calibration.push(false);

                    break;
                default:
                    study_names.push(study_name)

                    // Standard 2D study.
                    configuration[VMGlobals.vmSCP_STUDY_REQ_H_CALIB] = "";
                    configuration[VMGlobals.vmSCP_IS_STUDY_3D] = false;

                    // Adding it to the configuration list.
                    viewEvaluations.vmSelectedEvaluationConfigurations.push(configuration);

                    requires_hand_calibration.push(false);
                    break;
                }

            }
        }

        // Setting up the progress line
        evaluationsView.setUpStudyNames(study_names, requires_hand_calibration);        

        console.log("Printing Selected Evaluation Configuration")
        console.log(JSON.stringify(viewEvaluations.vmSelectedEvaluationConfigurations));
        //console.log("ONLY PRINTING");

        goToEvalRun();
    }

    Text {
        id: title
        text: loader.getStringForKey("viewevaluation_eval_settings")
        height: VMGlobals.adjustHeight(42)
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: VMGlobals.vmFontExtraExtraLarge
        font.weight: 600
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: VMGlobals.adjustHeight(31)
        anchors.leftMargin: VMGlobals.adjustWidth(30)
    }

    Text {
        id: chooseEval
        text: loader.getStringForKey("viewevaluation_choose")
        font.pixelSize: VMGlobals.vmFontLarger
        font.weight: 600
        height: VMGlobals.adjustHeight(24)
        verticalAlignment: Text.AlignVCenter
        anchors.top: title.bottom
        anchors.left: title.left
        anchors.topMargin: VMGlobals.adjustHeight(35)
    }

    ListModel {
        id: availableEvaluations
    }

    ListView {
        id: evaluationListView
        clip: true
        height: VMGlobals.adjustHeight(453-14)
        width: evaluationConfiguration.width//parent.width - VMGlobals.adjustHeight(1)*2
        anchors.top: chooseEval.bottom
        anchors.topMargin: VMGlobals.adjustHeight(14)
        anchors.horizontalCenter: parent.horizontalCenter
        model: availableEvaluations

        //        ScrollBar.vertical: ScrollBar {
        //            active: true
        //        }

        delegate: VMCofigurableStudyItem {
            width: evaluationConfiguration.width
            //vmOptionStruct: vmOptions
            onSelectionChanged: function (studyCode, isSelected) {
                reorderStudies(studyCode,isSelected)
            }
            onUpdateSelectedOptions: function (vmIndex, options){
                vmSelectedOptionsForEachStudy[vmIndex] = options
                //console.log(JSON.stringify(vmSelectedOptionsForEachStudy))
            }
        }
    }

    Component.onCompleted: {
        resetStudySelection()
    }



}
