import QtQuick
import "../"
import "../components"
import com.qml 1.0

Rectangle {

    readonly property int vmSTAGE_CALIBRATION: 0
    readonly property int vmSTAGE_EXPLANATION: 1
    readonly property int vmSTAGE_EVALUATION:  2

    property int vmEvaluationStage : vmSTAGE_CALIBRATION
    // The flag is required to indetify the moment between entering the strign and calibration actually staring.
    property bool vmInCalibration: false
    property int vmCurrentEvaluation: 0

    property var vmExplanationTextArray: []
    property int vmExplanationTextIndex: -1

    signal allEvalsDone();

    Connections {
        target: flowControl
        function onExperimentHasFinished (){

            if (!flowControl.isExperimentEndOk()){
                flowControl.generateWaitScreen("");
                mainWindow.popUpNotify(VMGlobals.vmNotificationRed,loader.getStringForKey("viewevaluation_err_badend_study"))
                mainWindow.swipeTo(VMGlobals.vmSwipeIndexMainScreen)
                return;
            }
            else{
                flowControl.generateWaitScreen(loader.getStringForKey("waitscreenmsg_studyEnd") + "\n" + evalTitle.text);
                viewEvaluations.changeNextButtonTextAndIcon(loader.getStringForKey("viewevaluation_next_button"),"next")
                viewEvaluations.enableNextButton(true)
            }
        }

        function onNewImageAvailable () {
            hmdView.image = flowControl.image;
        }

        function onConnectedToEyeTracker () {
            var titleMsg;
            if (!flowControl.isConnected()){
                mainWindow.popUpNotify(VMGlobals.vmNotificationRed,loader.getStringForKey("viewevaluation_err_noconnect"))
                return;
            }
            // All is good so the calibration is requested.
            flowControl.calibrateEyeTracker();
            vmInCalibration = true;
        }

        function onCalibrationDone() {

            vmInCalibration = false;

            mainWindow.showCalibrationValidation();

            //prepareNextStudy(false);

            //flowControl.setupSecondMonitor(); // LEGACY CODE: This is where the old UI had it. Leaving it here for reference.

        }

        function onNewExperimentMessages(string_value_map){

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
    }

    function prepareNextStudy(calibrationSkipped){

        if (!flowControl.isCalibrated()){
            let leftEyeOk = flowControl.isLeftEyeCalibrated();
            let rightEyeOk = flowControl.isRightEyeCalibrated();
            mainWindow.showCalibrationError(leftEyeOk,rightEyeOk)
            return;
        }
        flowControl.generateWaitScreen(loader.getStringForKey("waitscreenmsg_calibrationEnd"));

        // Next state in the state machine.
        vmEvaluationStage = vmSTAGE_EXPLANATION;

        // Loading the explanation text for the study.

        // We load the text explanation depending on the study.
        let unique_study_id = parseInt(viewEvaluations.vmSelectedEvaluationConfigurations[vmCurrentEvaluation][VMGlobals.vmUNIQUE_STUDY_ID]);
        if (unique_study_id === VMGlobals.vmINDEX_GONOGO){
            vmExplanationTextArray = loader.getStringListForKey("explanation_gonogo");            
        }
        else if (unique_study_id === VMGlobals.vmINDEX_BINDING_BC){
            vmExplanationTextArray = loader.getStringListForKey("explanation_binding_bc");
        }
        else if (unique_study_id === VMGlobals.vmINDEX_BINDING_UC){
            vmExplanationTextArray = loader.getStringListForKey("explanation_binding_uc");
        }
        else if (unique_study_id === VMGlobals.vmINDEX_NBACKRT){
            vmExplanationTextArray = loader.getStringListForKey("explanation_nbackrt");
        }
        else if (unique_study_id === VMGlobals.vmINDEX_NBACKVS){
            let ntargets = viewEvaluations.vmSelectedEvaluationConfigurations[vmCurrentEvaluation][VMGlobals.vmSCP_NUMBER_OF_TARGETS]
            vmExplanationTextArray = loader.getStringListForKey("explanation_nbackvs_" + ntargets);
            //console.log(JSON.stringify(viewEvaluations.vmSelectedEvaluationConfigurations[vmCurrentEvaluation]));
            //vmExplanationTextArray = loader.getStringListForKey("explanation_nbackvs_3");
        }
        else if (unique_study_id === VMGlobals.vmINDEX_NBACKMS){
            vmExplanationTextArray = loader.getStringListForKey("explanation_nbackms");
        }
        else {
            vmExplanationTextArray = [""];
        }

        vmExplanationTextIndex = -1;
        nextExplanationPhrase();

        // Button text must change to the next action, which is to start evaluation.
        viewEvaluations.changeNextButtonTextAndIcon(loader.getStringForKey("viewevaluation_action_starteval"),"")
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
        // console.log("Starting New Experiment");
        if (!flowControl.startNewExperiment(viewEvaluations.vmSelectedEvaluationConfigurations[vmCurrentEvaluation])){
            let title = loader.getStringForKey("viewevaluation_err_programming");
            mainWindow.popUpNotify(VMGlobals.vmNotificationRed,title);
            return;
        }

        if (calibrationSkipped) return;
        popUpNotify(VMGlobals.vmNotificationGreen,loader.getStringForKey("viewevalcalibration_success"))
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
                flowControl.calibrateEyeTracker();
                vmInCalibration = true;
            }
        }
        else if (vmEvaluationStage == vmSTAGE_EXPLANATION){
            vmEvaluationStage = vmSTAGE_EVALUATION;
            viewEvaluations.enableNextButton(false);
            viewEvaluations.advanceStudyIndicator();
            flowControl.startStudy();
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

    function nextExplanationPhrase(){
        vmExplanationTextIndex++;
        // Modulus operation in order to ensure circularity.
        vmExplanationTextIndex = (vmExplanationTextIndex % vmExplanationTextArray.length)
        let phrase = vmExplanationTextArray[vmExplanationTextIndex];
        studyExplanationText.text = phrase;
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

    QImageDisplay {
        id: hmdView
        width: VMGlobals.adjustWidth(597);
        height: VMGlobals.adjustHeight(310);
        //anchors.top: slideTitle.bottom
        anchors.top: evalTitle.bottom
        anchors.topMargin: VMGlobals.adjustHeight(34);
        anchors.left: evalTitle.left
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
//        onVisibleChanged: {
//            if (visible){
//              console.log("Message DISPLAY is now Visible");
//            }
//            else {
//              console.log("Message DISPLAY is now HIDDEN");
//            }
//        }

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
            //height: VMGlobals.adjustHeight(32)
            //verticalAlignment: Text.AlignVCenter
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
        visible:  (vmEvaluationStage === vmSTAGE_EXPLANATION)

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
        var keys = ["ESC","G","N","D","S"]

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
