import QtQuick
import "../"
import "../components"
import com.qml 1.0

Rectangle {

    readonly property int vmSTAGE_CALIBRATION:      0
    readonly property int vmSTAGE_HAND_CALIB_H:     1
    readonly property int vmSTAGE_HAND_CALIB_V:     2
    readonly property int vmSTAGE_EXPLANATION:      3
    readonly property int vmSTAGE_EXAMPLES:         4
    readonly property int vmSTAGE_EVALUATION:       5

    readonly property int vmHAND_CALIB_START_H : 0;
    readonly property int vmHAND_CALIB_START_V : 1;
    readonly property int vmHAND_CALIB_END     : 2;

    readonly property int vmCALIB_HAND_LEFT: 0
    readonly property int vmCALIB_HAND_RIGHT: 1
    readonly property int vmCALIB_HAND_BOTH: 2

    readonly property string vmONGOING_STUDY_FIELD: "ongoing_study_file"

    property int vmEvaluationStage : vmSTAGE_CALIBRATION
    // The flag is required to indetify the moment between entering the strign and calibration actually staring.
    property bool vmInCalibration: false
    property bool vmIsCalibrated: false;
    property int  vmCurrentEvaluation: 0

    property bool vmBindingStudyStarted: false;

    property bool vmSlowCalibrationSelected: false

    property int vmWhichHandToCalibrate: vmCALIB_HAND_BOTH;

    property int renderWindowOffsetX: 0;
    property int renderWindowOffsetY: 0;

    signal allEvalsDone();

    Connections {
        target: flowControl

        function onRequestWindowGeometry () {
            onMove();
        }

        function onExperimentHasFinished (){

            if (!flowControl.isExperimentEndOk()){
                flowControl.renderWaitScreen("");
                mainWindow.popUpNotify(VMGlobals.vmNotificationRed,loader.getStringForKey("viewevaluation_err_badend_study"))
                mainWindow.swipeTo(VMGlobals.vmSwipeIndexMainScreen)
                return;
            }
            else{
                flowControl.renderWaitScreen(loader.getStringForKey("waitscreenmsg_studyEnd") + "\n" + evalTitle.text);
                viewEvaluations.changeNextButtonTextAndIcon(loader.getStringForKey("viewevaluation_next_button"),"next")
                viewEvaluations.enableNextButton(true)
            }
        }

        function onConnectedToEyeTracker () {
            var titleMsg;
            if (!flowControl.isConnected()){
                mainWindow.popUpNotify(VMGlobals.vmNotificationRed,loader.getStringForKey("viewevaluation_err_noconnect"))
                return;
            }
            // All is good so the calibration is requested.

            let mode3d = isCurrentEvaluationA3DStudy();

            //console.log("Starting calibration. 3D Mode is: " + mode3d);

            flowControl.calibrateEyeTracker(vmSlowCalibrationSelected, mode3d);
            vmInCalibration = true;
        }

        function onCalibrationDone(calibrated) {

            //console.log("Rendering Wait Screen of On Calibration Done");
            if (calibrated) {
                flowControl.renderWaitScreen(loader.getStringForKey("waitscreenmsg_calibrationEnd"));
            }
            else {
                flowControl.renderWaitScreen("");
            }

            vmInCalibration = false;
            mainWindow.showCalibrationValidation();

        }

        function onNewExperimentMessages(string_value_map){

            if (vmEvaluationStage == vmSTAGE_EVALUATION){
                let list = "<ul>"
                for (let key in string_value_map){
                    let message = loader.getStringForKey(key);
                    //message = message.replace("<<N>>","<b>" + string_value_map[key] + "</b>");
                    message = message.replace("<<N>>",string_value_map[key]);
                    list = list + "<li>" + message + "</li>"
                }
                list = list + "<ul>"
                //console.log("DBUG: Setting Study Message: " + list)
                studyMessages.text = list;
            }
            else if (vmEvaluationStage == vmSTAGE_EXPLANATION){
                // This should contains only one key. and it's value is the index value in the screen.
                for (let key in string_value_map){
                    let message_list = loader.getStringListForKey(key)
                    let index = string_value_map[key];

//                    console.log("Showing explanation text " + index + " in a list of " + message_list.length);
//                    for (var i = 0; i < message_list.length; i++){
//                        console.log("  Message in index " + i + " is " + message_list[i]);
//                    }

                    let message_to_display = message_list[index];

                    let current_config = viewEvaluations.vmSelectedEvaluationConfigurations[vmCurrentEvaluation];
                    if (VMGlobals.vmSCP_NUMBER_OF_TARGETS in current_config){
                        let ntargets = current_config[VMGlobals.vmSCP_NUMBER_OF_TARGETS]
                        message_to_display = message_to_display.replace("<<N>>",ntargets);
                    }

                    studyExplanationText.text = message_to_display;
                }

            }
        }
    }

    function doesCurrentEvalutionRequiredHandCalibration(){
        var current_config = evaluationsView.vmSelectedEvaluationConfigurations[vmCurrentEvaluation];
        if (VMGlobals.vmSCP_STUDY_REQ_H_CALIB in current_config){
           return current_config[VMGlobals.vmSCP_STUDY_REQ_H_CALIB];
        }
        else return false;
    }

    function isCurrentEvaluationA3DStudy() {
        var current_config = evaluationsView.vmSelectedEvaluationConfigurations[vmCurrentEvaluation];
        if (VMGlobals.vmSCP_IS_STUDY_3D in current_config){
           return current_config[VMGlobals.vmSCP_IS_STUDY_3D];
        }
        else return false;
    }

    function setOngoingFileNameForStudyConfiguration(){
        let study_config = viewEvaluations.vmSelectedEvaluationConfigurations[vmCurrentEvaluation];
        study_config[vmONGOING_STUDY_FIELD] = loader.getCurrentSubjectStudyFile();
        viewEvaluations.vmSelectedEvaluationConfigurations[vmCurrentEvaluation] = study_config;
    }

    function prepareNextStudyOrHandCalibration(calibrationSkipped){

        if (!flowControl.isCalibrated()){
            let leftEyeOk = flowControl.isLeftEyeCalibrated();
            let rightEyeOk = flowControl.isRightEyeCalibrated();
            mainWindow.showCalibrationError(leftEyeOk,rightEyeOk)
            return;
        }        

        if (!calibrationSkipped) {
            // Calibration notification success will be in the middle of the screen as not to obstruct the button.
            popUpNotify(VMGlobals.vmNotificationGreen,loader.getStringForKey("viewevalcalibration_success"),true);
        }

        // The next state depends on whether the hand calibration is necessary or not.
        let current_config = viewEvaluations.vmSelectedEvaluationConfigurations[vmCurrentEvaluation];
        if (current_config[VMGlobals.vmSCP_STUDY_REQ_H_CALIB]){
            //console.log("Preparing for Hand Calibration");
            prepareForHandCalibration()
        }
        else {
            prepareStudyStart();
        }

    }

    function prepareForHandCalibration(){

        vmEvaluationStage = vmSTAGE_HAND_CALIB_H;
        viewEvaluations.advanceStudyIndicator();
        viewEvaluations.changeNextButtonTextAndIcon(loader.getStringForKey("viewevaluation_action_hcalib_v"),"");
        flowControl.handCalibrationControl(vmHAND_CALIB_START_H,vmWhichHandToCalibrate);
        /// TODO: Show text explanation.

    }

    function prepareStudyStart(){

        // Next state in the state machine.
        vmEvaluationStage = vmSTAGE_EXPLANATION;

        // Loading the explanation text for the study.

        // We load the text explanation depending on the study.
        let unique_study_id = parseInt(viewEvaluations.vmSelectedEvaluationConfigurations[vmCurrentEvaluation][VMGlobals.vmUNIQUE_STUDY_ID]);

        if (vmCurrentEvaluation == 0){
            vmBindingStudyStarted = false;
        }

        if (unique_study_id === VMGlobals.vmINDEX_BINDING_BC){
            if (vmBindingStudyStarted){
                setOngoingFileNameForStudyConfiguration();
            }
            else {
                vmBindingStudyStarted = true;
            }
        }
        else if (unique_study_id === VMGlobals.vmINDEX_BINDING_UC){
            if (vmBindingStudyStarted){
                setOngoingFileNameForStudyConfiguration();
            }
            else {
                vmBindingStudyStarted = true;
            }
        }
        else if (unique_study_id === VMGlobals.vmINDEX_NBACKVS){
            let ntargets = viewEvaluations.vmSelectedEvaluationConfigurations[vmCurrentEvaluation][VMGlobals.vmSCP_NUMBER_OF_TARGETS]
        }

        // Button text must change to the next action, which is to start the examples.
        viewEvaluations.changeNextButtonTextAndIcon(loader.getStringForKey("viewevaluation_action_examples"),"")
        var stageNames = loader.getStringListForKey("viewevaluation_evaluation_steps")

        // Advance the left hand side indicator
        viewEvaluations.advanceStudyIndicator();

        // Change the stage text in the title.
        let current_study = evalTitle.text
        setStudyAndStage(current_study,stageNames[vmSTAGE_EXPLANATION])

        //viewEvaluations.vmSelectedEvaluationConfigurations[vmCurrentEvaluation]

        // Now we need to activate explanation mode. First step is creating the study file.

        if (!loader.createSubjectStudyFile(viewEvaluations.vmSelectedEvaluationConfigurations[vmCurrentEvaluation],
                                           viewEvaluations.vmSelectedDoctor,viewEvaluations.vmSelectedProtocol)){
            let title = loader.getStringForKey("viewevaluation_err_programming");
            mainWindow.popUpNotify(VMGlobals.vmNotificationRed,title);
            return;
        }

        // Actually starting the experiment.
        if (!flowControl.startNewExperiment(viewEvaluations.vmSelectedEvaluationConfigurations[vmCurrentEvaluation])){
            let title = loader.getStringForKey("viewevaluation_err_programming");
            mainWindow.popUpNotify(VMGlobals.vmNotificationRed,title);
            return;
        }

    }

    function setStudyAndStage(study,stage){
        evalTitle.text = study;
        evalStage.text = "/ " + stage;
    }

    function onNextButtonPressed(){
        //console.log("PRESSED the NEXT Button in Stage" + vmEvaluationStage)
        if (vmEvaluationStage === vmSTAGE_CALIBRATION){

            if (!flowControl.isConnected()) flowControl.connectToEyeTracker();
            else {
                let mode3d = isCurrentEvaluationA3DStudy();
                console.log("Starting EyeTracker Calibratration Slowly. Is Mode3D: " + mode3d);
                flowControl.calibrateEyeTracker(vmSlowCalibrationSelected,mode3d);
                vmIsCalibrated = false;
                vmInCalibration = true;
            }
        }
        else if (vmEvaluationStage == vmSTAGE_HAND_CALIB_H){
            vmEvaluationStage = vmSTAGE_HAND_CALIB_V;
            flowControl.handCalibrationControl(vmHAND_CALIB_START_V,vmWhichHandToCalibrate);
            viewEvaluations.changeNextButtonTextAndIcon(loader.getStringForKey("viewevaluation_action_hcalib_end"),"");
            viewEvaluations.advanceStudyIndicator();
        }
        else if (vmEvaluationStage == vmSTAGE_HAND_CALIB_V){
            vmEvaluationStage = vmSTAGE_EXPLANATION;
            flowControl.handCalibrationControl(vmHAND_CALIB_END,vmWhichHandToCalibrate);
            prepareStudyStart();
        }
        else if (vmEvaluationStage == vmSTAGE_EXPLANATION){
            vmEvaluationStage = vmSTAGE_EXAMPLES;
            flowControl.startStudyExamplePhase();
            viewEvaluations.changeNextButtonTextAndIcon(loader.getStringForKey("viewevaluation_action_starteval"),"");
            viewEvaluations.advanceStudyIndicator();
            studyExplanationText.text = "\n";
        }
        else if (vmEvaluationStage == vmSTAGE_EXAMPLES){
            vmEvaluationStage = vmSTAGE_EVALUATION;
            viewEvaluations.enableNextButton(false);
            viewEvaluations.advanceStudyIndicator();
            // Necessary to avoid confusing message between the end of an evaluation and the start of the next one.
            studyExplanationText.text = "";
            flowControl.startStudyEvaluationPhase();

        }
        else if (vmEvaluationStage == vmSTAGE_EVALUATION){
            advanceStudy();
        }
    }

    function advanceStudy(){
        vmCurrentEvaluation++;
        viewEvaluations.advanceStudyIndicator();

        if (vmCurrentEvaluation >= viewEvaluations.vmSelectedEvaluationConfigurations.length){
            // We are done.
            allEvalsDone()
        }
        else {
            // Changing the evaluation name and resetting the stage.
            vmEvaluationStage = vmSTAGE_CALIBRATION
            let studyAndStage = progressLine.getCurrentTexts();
            evaluationRun.setStudyAndStage(studyAndStage[0],studyAndStage[1])
            viewEvaluations.changeNextButtonTextAndIcon(loader.getStringForKey("viewevaluation_action_calibrate"),"")
        }
    }

    function setCalibrationExplantion(){
        studyExplanationText.text = loader.getStringForKey("viewevaluation_calibration_explanation");
    }

    function onMove(){
        let x = this.x + renderWindowOffsetX + hmdView.x
        let y = this.y + renderWindowOffsetY + hmdView.y
        flowControl.setRenderWindowGeometry(x,y,hmdView.width,hmdView.height);
    }

    Text {
        id: evalTitle
        color: VMGlobals.vmBlackText
        font.pixelSize: VMGlobals.vmFontExtraExtraLarge
        font.weight: 600
        height: VMGlobals.adjustHeight(32)
        verticalAlignment: Text.AlignVCenter
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: VMGlobals.adjustHeight(31)
        anchors.leftMargin: VMGlobals.adjustWidth(31)
    }

    Text {
        id: evalStage
        color: VMGlobals.vmGrayAccented
        font.pixelSize: VMGlobals.vmFontExtraExtraLarge
        font.weight: 400
        height: VMGlobals.adjustHeight(32)
        verticalAlignment: Text.AlignVCenter
        anchors.top: evalTitle.top
        anchors.left: evalTitle.right
        anchors.leftMargin: VMGlobals.adjustWidth(10)
    }

    Rectangle {
        id: hmdView
        width: VMGlobals.adjustWidth(597);
        height: VMGlobals.adjustHeight(310);
        //anchors.top: slideTitle.bottom
        anchors.top: evalTitle.bottom
        anchors.topMargin: VMGlobals.adjustHeight(34);
        anchors.left: evalTitle.left
        border.width: 0
        color: "#ffffff"
        onWidthChanged: {
            onMove()
        }

        onYChanged: {
            onMove()
        }

        onXChanged: {
            onMove()
        }

        onHeightChanged: {
            onMove()
        }
    }

    // Message to display study progress messages.
    Rectangle {
        id: messageDisplay
        width: hmdView.width
        color: VMGlobals.vmGreenBKGStudyMessages
        height: VMGlobals.adjustHeight(117)
        anchors.left: hmdView.left
        anchors.top: hmdView.bottom
        anchors.topMargin: VMGlobals.adjustHeight(20)
        radius: VMGlobals.adjustHeight(8)
        visible: (vmEvaluationStage === vmSTAGE_EVALUATION)

        Image {
            id: info_icon
            source: "qrc:/images/info-green.png"
            height: VMGlobals.adjustHeight(21.5)
            fillMode: Image.PreserveAspectFit
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.topMargin: VMGlobals.adjustHeight(21.25)
            anchors.leftMargin: VMGlobals.adjustWidth(21.25)
        }

        Text {
            id: studyMessages
            color: VMGlobals.vmBlackText
            font.pixelSize: VMGlobals.vmFontLarger
            font.weight: 400
            anchors.top: info_icon.top
            anchors.left: info_icon.right
            anchors.leftMargin: VMGlobals.adjustWidth(12.25)
        }

    }

    Rectangle {
        id: studyExplantionDisplay
        width: hmdView.width
        color: VMGlobals.vmBlueVeryLightBkg
        height: VMGlobals.adjustHeight(155)
        anchors.left: hmdView.left
        anchors.top: hmdView.bottom
        anchors.topMargin: VMGlobals.adjustHeight(20)
        radius: VMGlobals.adjustHeight(8)
        // Uncomment line below to enable feature.
        // visible: false;
        visible:  (vmEvaluationStage === vmSTAGE_EXPLANATION) || (vmEvaluationStage === vmSTAGE_CALIBRATION) || (vmEvaluationStage == vmSTAGE_EXAMPLES)

        Image {
            id: studyExplanationInfoIcon
            source: "qrc:/images/info_blue.png"
            height: VMGlobals.adjustHeight(21.5)
            fillMode: Image.PreserveAspectFit
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.topMargin: VMGlobals.adjustHeight(10)
            anchors.leftMargin: VMGlobals.adjustWidth(21.25)
        }

        Text {
            id: studyExplanationText
            color: VMGlobals.vmBlackText
            font.pixelSize: VMGlobals.vmFontBaseSize
            font.weight: 400
            anchors.top: studyExplanationInfoIcon.top
            anchors.left: studyExplanationInfoIcon.right
            anchors.leftMargin: VMGlobals.adjustWidth(12.25)
        }

        Text {
            id: pressKeyToContinue;
            color: VMGlobals.vmBlueSelected
            text: loader.getStringForKey("explanation_key_to_continue");
            font.pixelSize: VMGlobals.vmFontBaseSize
            font.weight: 600
            anchors.top: studyExplanationText.bottom
            anchors.left: studyExplanationText.left
            visible: (vmEvaluationStage === vmSTAGE_EXPLANATION) || (vmEvaluationStage == vmSTAGE_EXAMPLES)
        }

        Text {
            id: pressKeyToGoBack;
            color: VMGlobals.vmBlueSelected
            text: loader.getStringForKey("explanation_key_to_goback");
            font.pixelSize: VMGlobals.vmFontBaseSize
            font.weight: 600
            anchors.top: pressKeyToContinue.bottom
            anchors.left: pressKeyToContinue.left
            visible: (vmEvaluationStage === vmSTAGE_EXPLANATION)
        }


    }

    Rectangle {
        id: keysRect
        color: VMGlobals.vmGrayToggleOff
        height: parent.height
        width: VMGlobals.adjustWidth(311)
        anchors.top: parent.top
        anchors.right: parent.right

        Text {
            id: shorcutTitle
            text: loader.getStringForKey("viewevaluation_shortcuts")
            color: VMGlobals.vmBlackText
            font.pixelSize: VMGlobals.vmFontVeryLarge
            font.weight: 600
            height: VMGlobals.adjustHeight(32)
            verticalAlignment: Text.AlignVCenter
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: VMGlobals.adjustHeight(31)
            anchors.leftMargin: VMGlobals.adjustWidth(30)
        }

        ListModel {
            id: keysModel;
        }

        Row {
            id: rowForKeys
            anchors.left: shorcutTitle.left
            anchors.top: shorcutTitle.bottom
            anchors.topMargin: VMGlobals.adjustHeight(26)

            Column {
                id: columnsForKeys
                width: VMGlobals.adjustWidth(46);
                spacing: VMGlobals.adjustHeight(24)

                Repeater {
                    model: keysModel

                    Rectangle {
                        height: VMGlobals.adjustHeight(23)
                        width:  (keyText.text.length === 3) ? VMGlobals.adjustWidth(36) : VMGlobals.adjustWidth(23)
                        color: "transparent"
                        border.width: VMGlobals.adjustHeight(1)
                        radius: VMGlobals.adjustHeight(4)
                        border.color: VMGlobals.vmGrayStudyDivisor

                        Text {
                            id: keyText
                            text: keysModel.get(index).key
                            color: VMGlobals.vmGrayStudyDivisor
                            font.pixelSize: VMGlobals.vmFontBaseSize
                            font.weight: 600
                            height: VMGlobals.adjustHeight(18.62)
                            verticalAlignment: Text.AlignVCenter
                            anchors.centerIn: parent
                        }

                    }

                }

            }

            Column {
                id: columnForExplanations
                spacing: columnsForKeys.spacing

                Repeater {
                    model: keysModel
                    Text {
                        id: explanationText
                        text: keysModel.get(index).explanation
                        color: VMGlobals.vmBlackText
                        font.pixelSize: VMGlobals.vmFontBaseSize
                        font.weight: 400
                        height: VMGlobals.adjustHeight(23)
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }

        }

    }

    Component.onCompleted: {

        var explanations = loader.getStringListForKey("viewevaluation_keyboard_explanations");
        var keys = ["ESC","G","N","B","D","S"]

        if (keys.length !== explanations.length){
            //console.log("Key Array is " + keys.length + " and explanation array is " + explanations.length)
            return;
        }

        for (var i = 0; i < keys.length; i++){
            let item = { "key" : keys[i], "explanation": explanations[i] };
            keysModel.append(item);
        }

    }


}
