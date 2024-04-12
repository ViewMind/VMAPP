import QtQuick
import "../"
import "../components"

Rectangle {

    readonly property int vmSTAGE_CALIBRATION:      0
    readonly property int vmSTAGE_PRE_HAND_CALIB:   1
    readonly property int vmSTAGE_HAND_CALIB_H:     2
    readonly property int vmSTAGE_HAND_CALIB_V:     3
    readonly property int vmSTAGE_HAND_CALIB_VERIF: 4
    readonly property int vmSTAGE_EXPLANATION:      5
    readonly property int vmSTAGE_EXAMPLES:         6
    readonly property int vmSTAGE_EVALUATION:       7

    readonly property int vmHAND_CALIB_START_H : 0;
    readonly property int vmHAND_CALIB_START_V : 1;
    readonly property int vmHAND_CALIB_END     : 2;

    property int vmEvaluationStage : vmSTAGE_CALIBRATION

    // The flag is required to indetify the moment between entering the strign and calibration actually staring.
    property bool vmInCalibration: false
    property bool vmIsCalibrated: false;
    property bool vmIsPreviousCalibrationType3D: false;
    property int  vmCurrentTask: 0

    property bool vmSlowCalibrationSelected: false

    property string vmWhichHandToCalibrate: "";

    property var vmTaskList: []

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
                flowControl.renderWaitScreen("");
                // We show the wait screen while we do some background processing.
                flowControl.hideRenderWindow();
                mainWindow.openWait(loader.getStringForKey("viewwait_study_end"));
                //flowControl.finalizeStudyOperations();
                flowControl.requestStudyData();

            }
        }


        function onStudyEndProcessingDone() {
            // We now need to check if all is good or if there was a problem with the data tranfer.
            mainWindow.closeWait();
            flowControl.showRenderWindow();
            viewEvaluations.changeNextButtonTextAndIcon(loader.getStringForKey("viewevaluation_next_button"),"next")
            viewEvaluations.enableNextButton(true)

            // If a study has bad QCI, the application should take them to ongoing evaluations while showing a message of
            // why it's happening.
            if (!flowControl.hasLastTaskPassedQCI()){
               mainWindow.swipeTo(VMGlobals.vmSwipeIndexMainScreen)
               viewMainSetup.goToReportList();
               messageDiag.vmLarge = true;
               messageDiag.loadFromKey("viewevaluation_badqci_middle_eval",true);
               messageDiag.open(true)
               return;
            }

            if (!flowControl.isExperimentEndOk()){
                // We abort everything.
                let popup = loader.getStringForKey("viewevaluation_err_data_transfer")
                mainWindow.swipeTo(VMGlobals.vmSwipeIndexMainScreen)
                mainWindow.popUpNotify(VMGlobals.vmNotificationRed,popup);
            }
        }

        function onCalibrationDone(calibrated) {

            if (calibrated) {
                flowControl.renderWaitScreen("");
            }
            else {
                flowControl.renderWaitScreen("",true);
            }

            if (isCurrentEvaluationA3DStudy()){
                vmIsPreviousCalibrationType3D = true;
            }
            else {
                vmIsPreviousCalibrationType3D = false;
            }

            vmInCalibration = false;
            evaluationsView.enableNextButton(true);
            evaluationRun.vmSlowCalibrationSelected = false; // This is very important. If the slow calibration was selected, it was a once off. It needs to be reselected.
            mainWindow.showCalibrationValidation();

        }

        function onHandCalibrationDone(){
            // When the hand calibration is done, we move on to verification and we give the possibility of starting the hand calibration again.
            showHandCalibrationVerification();
        }

        function onNewExperimentMessages(string_value_map){

            //console.log(JSON.stringify(string_value_map));

            if (vmEvaluationStage == vmSTAGE_EVALUATION){

                // console.log("Getting the string value map of: " + JSON.stringify(string_value_map));

                // It has two parts. stats and extra.
                let stats = string_value_map["stats"];
                let extra = string_value_map["extra"];

                let list = "<ul>"
                for (let key in stats){
                    let message = loader.getStringForKey(key);
                    message = VMGlobals.stringReplaceAll("<<N>>",stats[key],message)
                    list = list + "<li>" + message + "</li>"
                }
                list = list + "<ul>"
                //console.log("DBUG: Setting Study Message: " + list)

                if (extra !== ""){
                    list = list + "<br><br>" + loader.getStringForKey(extra);
                }

                studyMessages.text = list;
            }
            else if ((vmEvaluationStage == vmSTAGE_EXPLANATION) || (vmEvaluationStage == vmSTAGE_EXAMPLES)){
                // This should contains only one key. and it's value is the index value in the screen.
                //console.log("Showing messages where string value map is " + JSON.stringify(string_value_map));
                for (let key in string_value_map){
                    let message_list = loader.getStringListForKey(key,false)
                    let index = string_value_map[key];

                    if (message_list.length < 1) return; // IN this case there is nothing to do.

                    let message_to_display = message_list[index];

                    // The 4 % are used as a code to represent an empty string. This is necessary because I can't code in the text files a 1 item list where the item is just a space.
                    if (message_to_display === "%%%%") message_to_display = "";

                    //console.log("Setting display message to: |" + message_to_display + "|");

                    let current_task = vmTaskList[vmCurrentTask].type;

                    // The arrow use instruction changes on the last explanation slide.
                    if (index === (message_list.length -1)){
                        if (vmEvaluationStage == vmSTAGE_EXAMPLES){
                            arrowUseText.text = "(" + loader.getStringForKey("viewevalution_arrow_use_eval") + ")"
                        }
                        else {
                           arrowUseText.text = "(" + loader.getStringForKey("viewevalution_arrow_use_examples") + ")"
                        }
                    }
                    else {
                        arrowUseText.text = "(" + loader.getStringForKey("viewevalution_arrow_use") + ")"
                    }

                    let ntargets;

                    if ((current_task === VMGlobals.vmTASK_BINDING_BC_2_SHORT) || (current_task === VMGlobals.vmTASK_BINDING_BC_2_SHORT)){
                        ntargets = 2;
                    }
                    else if ((current_task === VMGlobals.vmTASK_BINDING_BC_3_SHORT) ||
                             (current_task === VMGlobals.vmTASK_BINDING_BC_3_SHORT) ||
                             (current_task === VMGlobals.vmTASK_NBACK_3) ){
                        ntargets = 3;
                    }
                    else if (current_task === VMGlobals.vmTASK_NBACK_4){
                        ntargets = 4;
                    }

                    if (ntargets > 0){
                        message_to_display = VMGlobals.stringReplaceAll("<<N>>",ntargets,message_to_display)
                    }

                    if (message_to_display === undefined){
                        console.log("Got an undefined message when getting message of index " + index +  " out of a list of " + message_list.length);
                    }
                    else {
                        studyExplanationText.text = message_to_display;
                    }

                }

            }
        }
    }

    function doesCurrentEvalutionRequiredHandCalibration(){
        var task_code = vmTaskList[vmCurrentTask].type;
        return flowControl.doesTaskRequireHandCalibration(task_code);
    }

    function isCurrentEvaluationA3DStudy() {
        var task_code = vmTaskList[vmCurrentTask].type;
        return flowControl.isTask3D(task_code);
    }

    function prepareNextStudyOrHandCalibration(calibrationSkipped, is_recalibrating_hands){

        if (!calibrationSkipped) {
            // Calibration notification success will be in the middle of the screen as not to obstruct the button.
            popUpNotify(VMGlobals.vmNotificationGreen,loader.getStringForKey("viewevalcalibration_success"),true);
        }

        // The next state depends on whether the hand calibration is necessary or not.
        if (doesCurrentEvalutionRequiredHandCalibration()){
            vmWhichHandToCalibrate = "both"
            prepareForHandCalibration(is_recalibrating_hands)
        }
        else {
            prepareTaskStart();
        }

    }

    function prepareForHandCalibration(is_recalibrating){
        vmEvaluationStage = vmSTAGE_PRE_HAND_CALIB
        if (!is_recalibrating) {
            plineEvaluationStages.indicateNext();
        }
        viewEvaluations.changeNextButtonTextAndIcon(loader.getStringForKey("viewevaluation_action_start_hand_calib"),"");
        studyExplanationText.text = loader.getStringForKey("viewevaluation_turn_on_controllers",false);
    }

    function startHorizontalHandCalibration(){
        vmEvaluationStage = vmSTAGE_HAND_CALIB_H;
        viewEvaluations.changeNextButtonTextAndIcon(loader.getStringForKey("viewevaluation_action_hcalib_v"),"");
        flowControl.handCalibrationControl(vmHAND_CALIB_START_H,vmWhichHandToCalibrate);
        studyExplanationText.text = loader.getStringForKey("viewevaluation_hand_calib_h",false);
    }

    function showHandCalibrationVerification(){
        vmEvaluationStage = vmSTAGE_HAND_CALIB_VERIF
        viewEvaluations.changeNextButtonTextAndIcon(loader.getStringForKey("viewevaluation_action_starteval"),"");
        studyExplanationText.text = loader.getStringForKey("viewevaluation_hand_verif",false);
    }


    function setupEvaluation(){

        // When the system calls this fucntion a subject, and evaluation must be set. This defines everthing else.

        // So first we make sure that the proper patient is loaded.
        viewEvaluations.setPatientForEvaluation();

        // This internally sets the current task list and it's returned.
        vmTaskList = flowControl.setupCurrentTaskList();

        //console.log("Current Task List: " + JSON.stringify(vmTaskList));

        let taskNameMap = loader.getTaskCodeToNameMap();
        let task_names  = [];

        for (let i = 0; i < vmTaskList.length; i++){
            let code = vmTaskList[i].type;
            task_names.push(taskNameMap[code]);
        }


        // The we get the list of tasks that we need to do.
        viewEvaluations.setUpStudyNames(task_names);

        // Index to indicate the current evaluation being performed.
        vmCurrentTask = 0;

        // Making sure the is calibrated flag is set to false.
        vmIsCalibrated = false;

        // This shoudl render the secondary progress line.
        resetEvaluationStages();

        vmEvaluationStage = evaluationRun.vmSTAGE_CALIBRATION
        setCalibrationExplantion() // Setting the calibration explanation message.
        vmInCalibration = false;

        // Finally we make sure that the proper title is shown at the right hand side.
        setStudyAndStage();

        // Set patient changes the button text. So we make sure it shows the proper text.
        viewEvaluations.changeNextButtonTextAndIcon(loader.getStringForKey("viewevaluation_action_calibrate"),"")

    }

    function prepareTaskStart(){

        // Next state in the state machine.
        vmEvaluationStage = vmSTAGE_EXPLANATION;

        // We load the text explanation depending on the study. so we get the study unique number id.
        let task_code = vmTaskList[vmCurrentTask].type;

        // Button text must change to the next action, which is to start the examples.
        viewEvaluations.changeNextButtonTextAndIcon(loader.getStringForKey("viewevaluation_action_examples"),"")
        var stageNames;
        if (doesCurrentEvalutionRequiredHandCalibration()){
            stageNames = loader.getStringListForKey("viewevaluation_evaluation_steps_with_hand_calib")
        }
        else {
            stageNames = loader.getStringListForKey("viewevaluation_evaluation_steps")
        }

        // Advance the left hand side indicator
        // viewEvaluations.advanceStudyIndicator();
        plineEvaluationStages.indicateNext();

        // Change the stage text in the title.
        setStudyAndStage();

        // Start the new task. The only thing that is required is the index.
        if (!flowControl.startTask(vmCurrentTask)){
            let title = loader.getStringForKey("viewevaluation_err_programming");
            mainWindow.popUpNotify(VMGlobals.vmNotificationRed,title);
            return;
        }

    }

    function setStudyAndStage(){
        var texts = progressLine.getCurrentTexts();
        if (texts.length !== 2){
            console.log("Current texts from progress line are not 2 but: " + texts.length);
            return;
        }

        // Text[0] is just evaluations.Text[1] contains the evaluation name.
        let nText = (vmCurrentTask + 1)
        keysRect.setEvalName(nText,texts[0])

    }

    function onNextButtonPressed(){
        //console.log("PRESSED the NEXT Button in Stage" + vmEvaluationStage)
        if (vmEvaluationStage === vmSTAGE_CALIBRATION){
            eyeTrackingCheckDialog.open(eyeTrackingCheckDialog.vmINTENT_CALIBRATE);
        }
        else if (vmEvaluationStage == vmSTAGE_PRE_HAND_CALIB){
            startHorizontalHandCalibration();
        }
        else if (vmEvaluationStage == vmSTAGE_HAND_CALIB_H){
            vmEvaluationStage = vmSTAGE_HAND_CALIB_V;
            studyExplanationText.text = loader.getStringForKey("viewevaluation_hand_calib_v",false);
            flowControl.handCalibrationControl(vmHAND_CALIB_START_V,vmWhichHandToCalibrate);
            viewEvaluations.changeNextButtonTextAndIcon(loader.getStringForKey("viewevaluation_action_hcalib_end"),"");
        }
        else if (vmEvaluationStage == vmSTAGE_HAND_CALIB_V){
            flowControl.handCalibrationControl(vmHAND_CALIB_END,vmWhichHandToCalibrate);
            // When onHandCalibrationDone is called the next stage will be called.
        }
        else if (vmEvaluationStage == vmSTAGE_HAND_CALIB_VERIF){
            prepareTaskStart();
        }
        else if (vmEvaluationStage == vmSTAGE_EXPLANATION){
            vmEvaluationStage = vmSTAGE_EXAMPLES;
            // We need to make sure the right message is display as the arrow instruction.
            arrowUseText.text = "(" + loader.getStringForKey("viewevalution_arrow_use") + ")"
            flowControl.startStudyExamplePhase();
            viewEvaluations.changeNextButtonTextAndIcon(loader.getStringForKey("viewevaluation_action_starteval"),"");
            // viewEvaluations.advanceStudyIndicator();
            plineEvaluationStages.indicateNext();
        }
        else if (vmEvaluationStage == vmSTAGE_EXAMPLES){
            vmEvaluationStage = vmSTAGE_EVALUATION;
            viewEvaluations.enableNextButton(false);
            // viewEvaluations.advanceStudyIndicator();
            plineEvaluationStages.indicateNext();
            // Necessary to avoid confusing message between the end of an evaluation and the start of the next one.
            studyExplanationText.text = "";
            flowControl.startStudyEvaluationPhase();
            // This should guarantee that we get the key presses during evaluation
            mainWindow.requestApplicationActiveFocus();
        }
        else if (vmEvaluationStage == vmSTAGE_EVALUATION){
            advanceStudy();
        }
    }

    function resetEvaluationStages(){

        let eval_steps;
        if (doesCurrentEvalutionRequiredHandCalibration()){
            eval_steps = loader.getStringListForKey("viewevaluation_evaluation_steps_with_hand_calib")
        }
        else {
           eval_steps = loader.getStringListForKey("viewevaluation_evaluation_steps")
        }

        plineEvaluationStages.setup(eval_steps);
        plineEvaluationStages.reset();

    }

    function advanceStudy(){
        vmCurrentTask++;
        viewEvaluations.advanceStudyIndicator();

        if (vmCurrentTask >= vmTaskList.length){
            // We are done.
            allEvalsDone()
        }
        else {

            // We reset the phases.
            resetEvaluationStages();

            // Changing the evaluation name and resetting the stage.
            vmEvaluationStage = vmSTAGE_CALIBRATION

            // If the previous calibration is of a different type than the current we need to set the vmIsCalibrated to false.
            if ((vmIsPreviousCalibrationType3D) && (!isCurrentEvaluationA3DStudy())) vmIsCalibrated = false;
            else if ((!vmIsPreviousCalibrationType3D) && (isCurrentEvaluationA3DStudy())) vmIsCalibrated = false;

            viewEvaluations.changeNextButtonTextAndIcon(loader.getStringForKey("viewevaluation_action_calibrate"),"")
            evaluationRun.setCalibrationExplantion() // Setting the calibration explanation message.

        }
    }

    function setCalibrationExplantion(){
        // Loading the screen with the 4 corner targets to check if all are visible.
        //console.log("Sending the command for rendering the 4 target wait screen");
        flowControl.renderWaitScreen("",true);
        studyExplanationText.text = loader.getStringForKey("viewevaluation_calibration_explanation",false);
    }

    function onMove(){
        let x = this.x + renderWindowOffsetX + hmdView.x
        let y = this.y + renderWindowOffsetY + hmdView.y
        flowControl.setRenderWindowGeometry(x,y,hmdView.width,hmdView.height);
    }

    Rectangle {
        id: hmdView
        width: VMGlobals.adjustWidth(697);
        height: VMGlobals.adjustHeight(310*697/597);
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: VMGlobals.adjustHeight(31)
        anchors.leftMargin: VMGlobals.adjustWidth(31)

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
        visible:  (vmEvaluationStage === vmSTAGE_EXPLANATION) ||
                  (vmEvaluationStage === vmSTAGE_CALIBRATION) ||
                  (vmEvaluationStage == vmSTAGE_EXAMPLES)     ||
                  (vmEvaluationStage == vmSTAGE_HAND_CALIB_H) ||
                  (vmEvaluationStage == vmSTAGE_HAND_CALIB_V) ||
                  (vmEvaluationStage == vmSTAGE_PRE_HAND_CALIB) ||
                  (vmEvaluationStage == vmSTAGE_HAND_CALIB_VERIF)

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

        VMArrowButton {
            id: pressKeyToContinue
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.bottomMargin: VMGlobals.adjustHeight(5);
            anchors.rightMargin: VMGlobals.adjustWidth(5);
            vmArrowPointsForward: true
            visible: (vmEvaluationStage === vmSTAGE_EXPLANATION) || (vmEvaluationStage == vmSTAGE_EXAMPLES)
            onArrowPressed: {
                flowControl.keyboardKeyPressed(Qt.Key_N);
            }
        }

        VMArrowButton {
            id: pressKeyToGoBack
            anchors.bottom: pressKeyToContinue.bottom
            anchors.right: pressKeyToContinue.left
            anchors.rightMargin: VMGlobals.adjustWidth(1);
            vmArrowPointsForward: false
            //visible: (vmEvaluationStage === vmSTAGE_EXPLANATION)
            // It is apparently NEVER necessary to go back. But I leave the code here just in case.
            visible: false;
            onArrowPressed: {
                flowControl.keyboardKeyPressed(Qt.Key_B);
            }
        }

        Text {
            id: arrowUseText
            text: "(" + loader.getStringForKey("viewevalution_arrow_use") + ")"
            color: VMGlobals.vmBlackText
            font.pixelSize: VMGlobals.vmFontBaseSize
            font.weight: 800
            font.italic: true
            anchors.left: studyExplanationText.left
            anchors.top: studyExplanationText.bottom
            visible: pressKeyToContinue.visible
        }

    }

    Rectangle {

        id: keysRect
        color: VMGlobals.vmGrayToggleOff
        height: parent.height
        width: VMGlobals.adjustWidth(211)
        anchors.top: parent.top
        anchors.right: parent.right

        function getEvalName() {
            return plineEvaluationStages.vmEvalTitle;
        }

        function setEvalName(number, name){
            plineEvaluationStages.vmEvalNumber = number;
            plineEvaluationStages.vmEvalTitle = name;
        }

        VMStudyPhaseTracker {
            id: plineEvaluationStages
            y: hmdView.y
            anchors.horizontalCenter: keysRect.horizontalCenter
            width: keysRect.width*0.7
            height: parent.height*0.5
        }

    }



}
