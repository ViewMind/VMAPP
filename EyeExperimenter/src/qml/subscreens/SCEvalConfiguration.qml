import QtQuick
import QtQuick.Controls
import "../"
import "../components"

Rectangle {


    id: evaluationConfiguration

    property var vmSelectedStudies: [];
    property var vmSelectedOptionsForEachStudy: ({});
    property var vmPreSelectedStudySequence: []
    property int vmIgnoreSelectedSignal: 0;

    readonly property int vmNBACK_RT_STD_HOLD_TIME: 250
    readonly property int vmNBACK_RT_SLOW_HOLD_TIME: 400
    readonly property int vmNBACK_RT_TIME_OUT_DEFAULT: 3000
    readonly property int vmNBACK_RT_TIME_OUT_SLOW: 5000
    readonly property int vmNBACK_RT_TRANSITION_DEFAULT: 500
    readonly property int vmNBACK_RT_TRANSITION_SLOW : 1000
    readonly property int vmGNG3D_DEFAULT_SPEED: 100
    readonly property int vmGNG3D_SLOW_SPEED: 60
    readonly property var vmGNG3D_VARIABLE: [50, 80, 120]; // Min, Initial, Max
    readonly property int vmBINDING_SHORT_STUDY_N_TRIALS: 32

    signal goToEvalRun();

    function loadEvaluationSequences(sequence_name){

        if (sequence_name === ""){
            vmPreSelectedStudySequence = [];
        }
        else{
           vmPreSelectedStudySequence = loader.getStudySequence(sequence_name);
           console.log("PreSelected Study Sequence name: " + sequence_name + " its values are")
           console.log(JSON.stringify(vmPreSelectedStudySequence));

        }

    }

    // The purpose of this function is to modify the created item appended to availableEvaluations
    // So that it may properly display the selected options BASED on vmPreSelectedStudySequence

    function configureItemBasedOnPreSelectedSequence(item){

        for (let i = 0; i < vmPreSelectedStudySequence.length; i++){
            let preconfigItem = vmPreSelectedStudySequence[i];

            if (item.vmIndex === preconfigItem[VMGlobals.vmUNIQUE_STUDY_ID]){

                // So now we parse teh value map. The ORDER of the map should be the same as the ORDER of the keys.
                let stringArrays = item.vmOptionValueMap.split("||"); // This will now contain as many arrays (in the form of strings) as there are sets of options.
                let optionNames  = [];
                for (let name in item.vmOptions){
                    optionNames.push(name);
                }

                // This item is part of the preconfiguration. So now we search the options of the item that were selected and get their stored value.
                for (let j = 0; j < optionNames.length; j++){
                    let optionName = optionNames[j];

                    // We now parse the option array.
                    let possibleValues = stringArrays[j].split("|");

                    let option_value = preconfigItem[optionName];
                    // Now we search for the index of the option value in the possible values.
                    // console.log("Searching for option value " + option_value + " on " + optionName + " for study " + item.vmIndex + " on " + JSON.stringify(possibleValues))
                    let index = -1

                    // The search needs to be done by hand as using indexOf provides bad matches when data type changes, i.e. 3 vs "3" will not be found.
                    for (let k = 0; k < possibleValues.length; k++){
                        if (possibleValues[k] == option_value){
                            index = k;
                            break;
                        }
                    }

                    if (index !== -1){
                        // We've found the option and we set it.
                        // console.log("Setting the option: " + optionName + " to index of " + index + " for study " + item.vmIndex);
                        item.vmOptions[optionName][VMGlobals.vmSCO_OPTION_SELECTED] = index
                    }

                }

                break; // We are only ever pre configuring 1 item.

            }

        }

        return item;

    }

    function setStudyOrderAccordingToPreSelectedSequence(){

        //console.log("Setting ignore signals to true");

        // We need to ignore the change selection. Since the signals are emitted AFTER all the vmIsSelected Changes.
        // We need to do this with a counter of how many signals we need to ignore.
        // To add to the complication, some stored sequences are composed of multiple studies that represent only 1 graphical item
        // (i.e Only Binding at the time of writing this) So in order to know the propert number of "clicks" to ignore we need to figure out
        // Which of the graphical items are actually beign clicked.

        vmIgnoreSelectedSignal = 0
        for (let i = 0; i < vmPreSelectedStudySequence.length; i++){
            let preconfigItem = vmPreSelectedStudySequence[i];
            for (let j = 0; j < availableEvaluations.count; j++){
                if (availableEvaluations.get(j).vmIndex == preconfigItem[VMGlobals.vmUNIQUE_STUDY_ID]){
                    vmIgnoreSelectedSignal++;
                }
            }
        }


        // We need to separate marking the orders as selected from actually re ordering the items.
        // The reason for this is that the delay between changing the "selected" property and actually
        // calling reorder which screws up the availableEvaluations order itself.

        for (let i = 0; i < vmPreSelectedStudySequence.length; i++){
            let preconfigItem = vmPreSelectedStudySequence[i];
            for (let j = 0; j < availableEvaluations.count; j++){
                if (availableEvaluations.get(j).vmIndex == preconfigItem[VMGlobals.vmUNIQUE_STUDY_ID]){
                    //console.log("Selecting evaluation " + preconfigItem[VMGlobals.vmUNIQUE_STUDY_ID]);
                    availableEvaluations.setProperty(j,"vmIsSelected",true)
                    break;
                }
            }
        }

        //console.log("Setting ignore signals to false");

        // The final step is to actually call reorder studies so that that the sequence is properly implemented graphically.
        // We need to make sure that we ONLY call it on graphically represented studies.
        for (let i = 0; i < vmPreSelectedStudySequence.length; i++){
            let preconfigItem = vmPreSelectedStudySequence[i];
            for (let j = 0; j < availableEvaluations.count; j++){
                if (availableEvaluations.get(j).vmIndex == preconfigItem[VMGlobals.vmUNIQUE_STUDY_ID]){
                    //console.log("Selecting evaluation " + preconfigItem[VMGlobals.vmUNIQUE_STUDY_ID] + " to order " + i);
                    reorderStudies(preconfigItem[VMGlobals.vmUNIQUE_STUDY_ID],true);
                    break;
                }
            }
        }

    }

    function resetStudySelection(){

        availableEvaluations.clear()
        vmSelectedOptionsForEachStudy = {};
        vmSelectedStudies = [];

        let studiesToHide = loader.getHiddenStudies();
        // We need to turn the strings to int.
        for (let i in studiesToHide){
            studiesToHide[i] = parseInt(studiesToHide[i])
        }

        //console.log("Studies to hide: " + JSON.stringify(studiesToHide));

        ////////////////////////////////////////////////////////////////// New Colors ////////////////////////////////////////////////////////////////////
        let item = {}
        let options = {}

        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS] = {}
        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS][VMGlobals.vmSCO_OPTION_NAME] = loader.getStringForKey("viewevaluation_number_of_targets");
        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS][VMGlobals.vmSCO_OPTION_VAlUES] = [2,3];
        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS][VMGlobals.vmSCO_OPTION_SELECTED] = 0;
        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS][VMGlobals.vmSCO_OPTION_WIDTH] = 40;

        options[VMGlobals.vmSCP_NUMBER_OF_TRIALS] = {}
        options[VMGlobals.vmSCP_NUMBER_OF_TRIALS][VMGlobals.vmSCO_OPTION_NAME] = loader.getStringForKey("viewevaluation_binding_length");
        options[VMGlobals.vmSCP_NUMBER_OF_TRIALS][VMGlobals.vmSCO_OPTION_VAlUES] = [loader.getStringForKey("viewevaluation_binding_normal"),
                                                                                     loader.getStringForKey("viewevaluation_binding_short")];
        options[VMGlobals.vmSCP_NUMBER_OF_TRIALS][VMGlobals.vmSCO_OPTION_SELECTED] = 1;
        options[VMGlobals.vmSCP_NUMBER_OF_TRIALS][VMGlobals.vmSCO_OPTION_WIDTH] = 60;

        item = {
            vmIndex: VMGlobals.vmINDEX_BINDING_UC,
            vmStudyName : loader.getStringForKey("viewevaluation_binding_uc") ,
            vmIsLastSelected: false,
            vmOptions: options,
            vmOrder: VMGlobals.vmSCP_NUMBER_OF_TARGETS + "|" + VMGlobals.vmSCP_NUMBER_OF_TRIALS,
            vmOptionValueMap: "2|3||-1|32",
            vmIsSelected: false
        }
        if (!studiesToHide.includes(item.vmIndex)) availableEvaluations.append(configureItemBasedOnPreSelectedSequence(item))

        ////////////////////////////////////////////////////////////////// Color Combination ////////////////////////////////////////////////////////////////////

        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS] = {}
        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS][VMGlobals.vmSCO_OPTION_NAME] = loader.getStringForKey("viewevaluation_number_of_targets");
        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS][VMGlobals.vmSCO_OPTION_VAlUES] = [2,3];
        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS][VMGlobals.vmSCO_OPTION_SELECTED] = 0;
        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS][VMGlobals.vmSCO_OPTION_WIDTH] = 40;

        options[VMGlobals.vmSCP_NUMBER_OF_TRIALS] = {}
        options[VMGlobals.vmSCP_NUMBER_OF_TRIALS][VMGlobals.vmSCO_OPTION_NAME] = loader.getStringForKey("viewevaluation_binding_length");
        options[VMGlobals.vmSCP_NUMBER_OF_TRIALS][VMGlobals.vmSCO_OPTION_VAlUES] = [loader.getStringForKey("viewevaluation_binding_normal"),
                                                                                     loader.getStringForKey("viewevaluation_binding_short")];
        options[VMGlobals.vmSCP_NUMBER_OF_TRIALS][VMGlobals.vmSCO_OPTION_SELECTED] = 1;
        options[VMGlobals.vmSCP_NUMBER_OF_TRIALS][VMGlobals.vmSCO_OPTION_WIDTH] = 60;

        item = {
            vmIndex: VMGlobals.vmINDEX_BINDING_BC,
            vmStudyName : loader.getStringForKey("viewevaluation_binding_bc") ,
            vmIsLastSelected: false,
            vmOptions: options,
            vmOrder: VMGlobals.vmSCP_NUMBER_OF_TARGETS + "|" + VMGlobals.vmSCP_NUMBER_OF_TRIALS,
            vmOptionValueMap: "2|3||-1|32",
            vmIsSelected: false
        }
        if (!studiesToHide.includes(item.vmIndex)) availableEvaluations.append(configureItemBasedOnPreSelectedSequence(item))

//        /////////////////////////////////////////////////////////////////// NBack RT (Depracated) /////////////////////////////////////////////////////////////////////
//        item = {}
//        options = {}
//        item = {
//            vmIndex: VMGlobals.vmINDEX_NBACKRT,
//            vmStudyName : loader.getStringForKey("viewevaluation_eval_nbackrt") ,
//            vmIsLastSelected: false,
//            vmOptions: options,
//            vmOrder: "",
//            vmOptionValueMap: "",
//            vmIsSelected: false
//        }
//        availableEvaluations.append(item)


        //////////////////////////////////////////////////////////////// NBack (New Version. Slow)  /////////////////////////////////////////////////////////////////
        item = {}
        options = {}

        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS] = {}
        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS][VMGlobals.vmSCO_OPTION_NAME] = loader.getStringForKey("viewevaluation_number_of_targets");
        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS][VMGlobals.vmSCO_OPTION_VAlUES] = [4,3];
        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS][VMGlobals.vmSCO_OPTION_SELECTED] = 1;
        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS][VMGlobals.vmSCO_OPTION_WIDTH] = 100;

        item = {
            vmIndex: VMGlobals.vmINDEX_NBACK,
            vmStudyName : loader.getStringForKey("viewevaluation_eval_nbackslow") ,
            vmIsLastSelected: false,
            vmOptions: options,
            vmOrder: VMGlobals.vmSCP_NUMBER_OF_TARGETS,
            vmOptionValueMap: "4|3", // Default NBack number of targets should be 4.
            vmIsSelected: false
        }
        if (!studiesToHide.includes(item.vmIndex)) availableEvaluations.append(configureItemBasedOnPreSelectedSequence(item))

        //////////////////////////////////////////////////////////////// Go No Go //////////////////////////////////////////////////////////////////////////
        item = {}
        options = {}
        item = {
            vmIndex: VMGlobals.vmINDEX_GONOGO,
            vmStudyName : loader.getStringForKey("viewevaluation_eval_gonogo") ,
            vmIsLastSelected: false,
            vmOptions: options,
            vmOrder: "",
            vmOptionValueMap: "",
            vmIsSelected: false
        }
        if (!studiesToHide.includes(item.vmIndex)) availableEvaluations.append(configureItemBasedOnPreSelectedSequence(item))


        //////////////////////////////////////////////////////////////// Dot Follower //////////////////////////////////////////////////////////////////////////
        item = {}
        options = {}
        item = {
            vmIndex: VMGlobals.vmINDEX_DOT_FOLLOW,
            vmStudyName : loader.getStringForKey("viewevaluation_eval_dotfollow") ,
            vmIsLastSelected: false,
            vmOptions: options,
            vmOrder: "",
            vmOptionValueMap: "",
            vmIsSelected: false
        }
        if (!studiesToHide.includes(item.vmIndex)) availableEvaluations.append(configureItemBasedOnPreSelectedSequence(item))

//        /////////////////////////////////////////////////////////// NBack Variable Speed /////////////////////////////////////////////////////////////////
//        item = {}
//        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS] = {}
//        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS][VMGlobals.vmSCO_OPTION_NAME] = loader.getStringForKey("viewevaluation_number_of_targets");
//        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS][VMGlobals.vmSCO_OPTION_VAlUES] = [3,4,5,6];
//        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS][VMGlobals.vmSCO_OPTION_SELECTED] = 0;
//        options[VMGlobals.vmSCP_NUMBER_OF_TARGETS][VMGlobals.vmSCO_OPTION_WIDTH] = 35;

//        options[VMGlobals.vmSCP_NBACK_LIGHTUP] = {}
//        options[VMGlobals.vmSCP_NBACK_LIGHTUP][VMGlobals.vmSCO_OPTION_NAME] = loader.getStringForKey("viewevaluation_nback_light_up_title");
//        options[VMGlobals.vmSCP_NBACK_LIGHTUP][VMGlobals.vmSCO_OPTION_VAlUES] = [loader.getStringForKey("viewevaluation_nback_light_up_correct"),
//                                                                                 loader.getStringForKey("viewevaluation_nback_light_up_all")];
//        options[VMGlobals.vmSCP_NBACK_LIGHTUP][VMGlobals.vmSCO_OPTION_SELECTED] = 0;
//        options[VMGlobals.vmSCP_NBACK_LIGHTUP][VMGlobals.vmSCO_OPTION_WIDTH] = 30;

//        item = {
//            vmIndex: VMGlobals.vmINDEX_NBACKVS,
//            vmStudyName : loader.getStringForKey("viewevaluation_eval_nbackvs") ,
//            vmIsLastSelected: false,
//            vmOrder: VMGlobals.vmSCP_NUMBER_OF_TARGETS + "|" + VMGlobals.vmSCP_NBACK_LIGHTUP,
//            vmOptions: options,
//            vmOptionValueMap: "3|4|5|6||false|true",
//            vmIsSelected: false
//        }
//        //availableEvaluations.append(item)
//        // For now variable speed is not an option.
//        //availableEvaluations.append(configureItemBasedOnPreSelectedSequence(item))

        /////////////////////////////////////////////////////////// GNG Spheres /////////////////////////////////////////////////////////////////////////
        item = {}
        options = {};
        options[VMGlobals.vmSCP_HAND_TO_USE] = {}
        options[VMGlobals.vmSCP_HAND_TO_USE][VMGlobals.vmSCO_OPTION_NAME] = loader.getStringForKey("viewevaluation_gng3D_hand_sel");
        options[VMGlobals.vmSCP_HAND_TO_USE][VMGlobals.vmSCO_OPTION_VAlUES] = [loader.getStringForKey("viewevaluation_gng3D_hand_right"),
                                                                               loader.getStringForKey("viewevaluation_gng3D_hand_left"),
                                                                               loader.getStringForKey("viewevaluation_gng3D_hand_both")];
        options[VMGlobals.vmSCP_HAND_TO_USE][VMGlobals.vmSCO_OPTION_SELECTED] = 2;
        options[VMGlobals.vmSCP_HAND_TO_USE][VMGlobals.vmSCO_OPTION_WIDTH] = 40;

        options[VMGlobals.vmSCP_GNG3DSPEED] = {}
        options[VMGlobals.vmSCP_GNG3DSPEED][VMGlobals.vmSCO_OPTION_NAME] = loader.getStringForKey("viewevaluation_gng3d_speed");
        options[VMGlobals.vmSCP_GNG3DSPEED][VMGlobals.vmSCO_OPTION_VAlUES] = [loader.getStringForKey("viewevaluation_gng3d_fast"),
                                                                               loader.getStringForKey("viewevaluation_gng3d_slow"),
                                                                               loader.getStringForKey("viewevaluation_gng3d_variable")];
        options[VMGlobals.vmSCP_GNG3DSPEED][VMGlobals.vmSCO_OPTION_SELECTED] = 0;
        options[VMGlobals.vmSCP_GNG3DSPEED][VMGlobals.vmSCO_OPTION_WIDTH] = 30;

        item = {
            vmIndex: VMGlobals.vmINDEX_GONOGO3D,
            vmStudyName : loader.getStringForKey("viewevaluation_eval_gonogo3D") ,
            vmIsLastSelected: false,
            vmOptions: options,
            vmOrder: VMGlobals.vmSCP_HAND_TO_USE + "|" + VMGlobals.vmSCP_GNG3DSPEED,
            vmOptionValueMap: "right|left|both||0|1|2", // These are the values inside the study configuration map corresponding to each of the option values.
            vmIsSelected: false
        }
        if (!studiesToHide.includes(item.vmIndex)) availableEvaluations.append(configureItemBasedOnPreSelectedSequence(item))

//        ////////////////////////////////////////////////////////// Pass Ball /////////////////////////////////////////////////////////////////////////
//        item = {}
//        options = {};
//        options[VMGlobals.vmSCP_HAND_TO_USE] = {}
//        options[VMGlobals.vmSCP_HAND_TO_USE][VMGlobals.vmSCO_OPTION_NAME] = loader.getStringForKey("viewevaluation_gng3D_hand_sel");
//        options[VMGlobals.vmSCP_HAND_TO_USE][VMGlobals.vmSCO_OPTION_VAlUES] = [loader.getStringForKey("viewevaluation_gng3D_hand_right"),
//                                                                               loader.getStringForKey("viewevaluation_gng3D_hand_left"),
//                                                                               loader.getStringForKey("viewevaluation_gng3D_hand_both")];
//        options[VMGlobals.vmSCP_HAND_TO_USE][VMGlobals.vmSCO_OPTION_SELECTED] = 2;
//        options[VMGlobals.vmSCP_HAND_TO_USE][VMGlobals.vmSCO_OPTION_WIDTH] = 30;

//        item = {
//            vmIndex: VMGlobals.vmINDEX_PASSBALL,
//            vmStudyName : loader.getStringForKey("viewevaluation_eval_passball") ,
//            vmIsLastSelected: false,
//            vmOptions: options,
//            vmOrder: VMGlobals.vmSCP_HAND_TO_USE,
//            vmOptionValueMap: "right|left|both", // These are the values inside the study configuration map corresponding to each of the option values.
//            vmIsSelected: false
//        }
//        //Disabling ONLY the option for selecting passball.
//        ///availableEvaluations.append(item)
//        ///availableEvaluations.append(configureItemBasedOnPreSelectedSequence(item))


        // Function that sets the propert study sequece, if one was selected.
        setStudyOrderAccordingToPreSelectedSequence();

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
                    // console.log("Would move " + j + " to " + i);
                    availableEvaluations.move(j,i,1)
                    break;
                }
            }
        }

    }

    function setupEvaluations(nameToStore, forceLoadPreSet){

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
                    //study_names.push(loader.getStringForKey("viewevaluation_binding_bc"));

                    requires_hand_calibration.push(false);
                    //requires_hand_calibration.push(false);

                    // Even though the option is no longer selectable, in the backend still exists. It needs to be set anyways
                    configuration[VMGlobals.vmSCP_TARGET_SIZE] = VMGlobals.vmSCV_BINDING_TARGETS_LARGE;

                    // Standard 2D study.
                    configuration[VMGlobals.vmSCP_STUDY_REQ_H_CALIB] = "";
                    configuration[VMGlobals.vmSCP_IS_STUDY_3D] = false;

                    // This actually represents two studies, so we need to select both, with the same configuration.
                    //let config2 = JSON.parse(JSON.stringify(configuration)) // Deep copying configuration.
                    //config2[VMGlobals.vmUNIQUE_STUDY_ID] = VMGlobals.vmINDEX_BINDING_BC

                    viewEvaluations.vmSelectedEvaluationConfigurations.push(configuration); // UC goes first.
                    //viewEvaluations.vmSelectedEvaluationConfigurations.push(config2) // BC goes second.
                    break;


                case VMGlobals.vmINDEX_BINDING_BC:
                    study_names.push(loader.getStringForKey("viewevaluation_binding_bc"));
                    requires_hand_calibration.push(false);

                    // Even though the option is no longer selectable, in the backend still exists. It needs to be set anyways
                    configuration[VMGlobals.vmSCP_TARGET_SIZE] = VMGlobals.vmSCV_BINDING_TARGETS_LARGE;

                    // Standard 2D study.
                    configuration[VMGlobals.vmSCP_STUDY_REQ_H_CALIB] = "";
                    configuration[VMGlobals.vmSCP_IS_STUDY_3D] = false;

                    viewEvaluations.vmSelectedEvaluationConfigurations.push(configuration); // UC goes first.
                    break;

                case VMGlobals.vmINDEX_GONOGO3D:
                    study_names.push(study_name);

                    // Standard 3D study.
                    configuration[VMGlobals.vmSCP_STUDY_REQ_H_CALIB] = configuration[VMGlobals.vmSCP_HAND_TO_USE];
                    configuration[VMGlobals.vmSCP_IS_STUDY_3D] = true;

                    let selected_config = configuration[VMGlobals.vmSCP_GNG3DSPEED];
                    if (selected_config === "0"){
                        // Default fast speed.
                        configuration[VMGlobals.vmSCP_MIN_SPEED]     = vmGNG3D_DEFAULT_SPEED;
                        configuration[VMGlobals.vmSCP_MAX_SPEED]     = vmGNG3D_DEFAULT_SPEED;
                        configuration[VMGlobals.vmSCP_INITIAL_SPEED] = vmGNG3D_DEFAULT_SPEED;
                    }
                    else if (selected_config === "1"){
                        // Slow speed.
                        configuration[VMGlobals.vmSCP_MIN_SPEED]     = vmGNG3D_SLOW_SPEED;
                        configuration[VMGlobals.vmSCP_MAX_SPEED]     = vmGNG3D_SLOW_SPEED;
                        configuration[VMGlobals.vmSCP_INITIAL_SPEED] = vmGNG3D_SLOW_SPEED;
                    }
                    else {
                        // Variable speed.
                        configuration[VMGlobals.vmSCP_MIN_SPEED]     = vmGNG3D_VARIABLE[0];
                        configuration[VMGlobals.vmSCP_MAX_SPEED]     = vmGNG3D_VARIABLE[2];
                        configuration[VMGlobals.vmSCP_INITIAL_SPEED] = vmGNG3D_VARIABLE[1];
                    }

                    requires_hand_calibration.push(true);

                    viewEvaluations.vmSelectedEvaluationConfigurations.push(configuration);
                    break;


                case VMGlobals.vmINDEX_PASSBALL:

                    study_names.push(study_name);

                    // Standard 3D study.
                    configuration[VMGlobals.vmSCP_STUDY_REQ_H_CALIB] = configuration[VMGlobals.vmSCP_HAND_TO_USE];
                    configuration[VMGlobals.vmSCP_IS_STUDY_3D] = true;

                    requires_hand_calibration.push(true);

                    viewEvaluations.vmSelectedEvaluationConfigurations.push(configuration);

                    break;

                case VMGlobals.vmINDEX_NBACKRT:
                    study_names.push(study_name)
                    // Standard 2D study.
                    configuration[VMGlobals.vmSCP_STUDY_REQ_H_CALIB] = "";
                    configuration[VMGlobals.vmSCP_IS_STUDY_3D] = false;
                    configuration[VMGlobals.vmSCP_NUMBER_OF_TARGETS] = 3;
                    configuration[VMGlobals.vmSCP_NBACKRT_HOLD_TIME] = vmNBACK_RT_STD_HOLD_TIME
                    configuration[VMGlobals.vmSCP_NBACKRT_TIMEOUT] = vmNBACK_RT_TIME_OUT_DEFAULT
                    configuration[VMGlobals.vmSCP_NBACKRT_TRANSITION] = vmNBACK_RT_TRANSITION_DEFAULT

                    // Adding it to the configuration list.
                    viewEvaluations.vmSelectedEvaluationConfigurations.push(configuration);

                    requires_hand_calibration.push(false);

                    break;

                case VMGlobals.vmINDEX_NBACK:

                    study_names.push(study_name)

                    // Standard 2D study.
                    configuration[VMGlobals.vmSCP_STUDY_REQ_H_CALIB] = "";
                    configuration[VMGlobals.vmSCP_IS_STUDY_3D] = false;
                    configuration[VMGlobals.vmSCP_NBACKRT_HOLD_TIME] = vmNBACK_RT_SLOW_HOLD_TIME
                    configuration[VMGlobals.vmSCP_NBACKRT_TIMEOUT] = vmNBACK_RT_TIME_OUT_SLOW
                    configuration[VMGlobals.vmSCP_NBACKRT_TRANSITION] = vmNBACK_RT_TRANSITION_SLOW

                    // Number of targets should be part of the configuration so we don't need to set it.
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

        // If the input parameter was not empty, it means that we need to store, under that name, the complete configuration.
        if (nameToStore !== ""){
            //console.log("Storing config as " + nameToStore);
            loader.storeNewStudySequence(nameToStore,viewEvaluations.vmSelectedEvaluationConfigurations)

            // Showing a popup that we've saved the sequence.
            let popup = loader.getStringForKey("viewevaluation_popup_save")
            popup = popup.replace("<b></b>","<b>" + nameToStore + "</b>")
            mainWindow.popUpNotify(VMGlobals.vmNotificationGreen,popup)

        }

        // We NOT passing through the UI of this subscreen (on loading a pre made sequence) some configuration options are NOT properly created.
        // But they were properly stored. Which is why we force the vmSelectedEvaluationConfigurations to the preselected list of configs.

        if (forceLoadPreSet){
            viewEvaluations.vmSelectedEvaluationConfigurations = vmPreSelectedStudySequence;
        }


        // Once the evaluations are setup we KNOW that this is the start of an evaluation run. This is when the calibration history needs to be reset.
        flowControl.resetCalibrationHistory();

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
                if (vmIgnoreSelectedSignal > 0){
                    vmIgnoreSelectedSignal--;
                    //console.log("Ignoring signal Number of signals to ignore is now: " + vmIgnoreSelectedSignal)
                    return;
                }
                // console.log("Calling selection changed: " + studyCode +  " is selected " + isSelected);
                reorderStudies(studyCode,isSelected)
            }
            onUpdateSelectedOptions: function (vmIndex, options){
                vmSelectedOptionsForEachStudy[vmIndex] = options
                //console.log("Updating selected options for index " + vmIndex + ". The options are");
                //console.log(JSON.stringify(vmSelectedOptionsForEachStudy))
            }
        }
    }

    Component.onCompleted: {
        resetStudySelection()
    }



}
