import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"
import "../subscreens"

ViewBase {

    id: evaluationsView

    property string vmSelectedPatientID: ""
    property string vmSelectedProtocol: ""
    property string vmSelectedEye: ""
    property string vmSelectedDoctor: ""
    property string vmSelectedPatientName: ""
    property var vmSelectedEvaluationConfigurations: []
    property string vmCurrentStudySequence: ""


    // Used for debugging ONLY
    property int vmDebugSubScreen: vmSC_INDEX_GENERAL_SETTINGS

    readonly property int vmSC_INDEX_GENERAL_SETTINGS:     0
    readonly property int vmSC_INDEX_EVAL_SETTINGS:        1
    readonly property int vmSC_INDEX_EVALUATION_SCREEN:    2
    readonly property int vmSC_INDEX_EVALUATION_FINISHED:  3

    function setPatientForEvaluation(){

        var patientData = loader.getCurrentSubjectInfo();

        //console.log("Loading patient for Evaluation")
        //console.log(JSON.stringify(patientData))

        var date = new Date();
        var year = date.getFullYear()
        var day  = date.getDate()
        var month = date.getMonth();
        var hour = date.getHours()
        var minutes =  date.getMinutes();
        if (minutes < 10) minutes = "0" + minutes;
        var monthlist = loader.getStringListForKey("viewpatlist_months")
        //console.log("Current month as index is " + month)
        //console.log("Month List " + JSON.stringify(monthlist))
        month = monthlist[month]

        var patient = loader.getStringForKey("viewevaluation_patient") + ",<br>";
        vmSelectedPatientName = "<b>" + patientData["name"] + " " + patientData["lastname"] + "</b>";
        patient = patient + vmSelectedPatientName
        //console.log("Setting patient to " + patient)
        patientText.text = patient;

        dateValue.text = day + " " + month + " " +  year + ", " + hour + ":" + minutes;
        //console.log(JSON.stringify(patientData));
        personalIDValue.text = patientData["supplied_institution_id"]

        vmSelectedPatientID = patientData["local_id"]

        if (!loader.setSelectedSubject(vmSelectedPatientID)){
            mainWindow.showErrorMessage("viewevaluation_error_patient_dir")
            mainWindow.swipeTo(VMGlobals.vmSwipeIndexMainScreen)
        }

        // Setting up the progress line.
        let plineSetup = {};
        plineSetup[loader.getStringForKey("viewevaluation_general_settings")] = [];
        plineSetup[loader.getStringForKey("viewevaluation_eval_settings")] = [];
        progressLine.vmOnlyColorCurrent = true;
        progressLine.setup(plineSetup);
        progressLine.reset();

        // Resetting the button state.
        nextButton.vmIconSource = "next";
        nextButton.vmText = loader.getStringForKey("viewevaluation_next_button");
        nextButton.vmEnabled = true;

        // Ensuring the back button is re enabled.
        backButton.vmEnabled = true;

        // Ensuring the proper screen is shown.
        viewer.currentIndex = vmDebugSubScreen;
        generalSettings.prepareSettings();

        // Ensuring we start with normal calibration speed
        setCalibrationSpeedToSlow(false)

    }

    function setUpStudyNames(study_names, uses_h_calib){

        let eval_steps = loader.getStringListForKey("viewevaluation_evaluation_steps")
        let eval_steps_with_hcalib = loader.getStringListForKey("viewevaluation_evaluation_steps_with_hand_calib")

        let plineSetup = {};


        for (let i = 0; i < study_names.length; i++){
            if (uses_h_calib[i]){
                plineSetup[study_names[i]] = eval_steps_with_hcalib;
            }
            else {
                plineSetup[study_names[i]] = eval_steps;
            }
        }

        // Adding the "finish" step. No substeps.
        plineSetup[loader.getStringForKey("viewevaluation_finish")] = []

        progressLine.vmOnlyColorCurrent = true;
        progressLine.setup(plineSetup);
        progressLine.reset();

        //console.log("Set up study names");
        //console.log(JSON.stringify());
        let studyAndStage = progressLine.getCurrentTexts();
        evaluationRun.setStudyAndStage(studyAndStage[0],studyAndStage[1])

        // Index to indicate the current evaluation being performed.
        evaluationRun.vmCurrentEvaluation = 0;

        // Making sure the is calibrated flag is set to false.
        evaluationRun.vmIsCalibrated = false;

        // The back button is disabled past this point.
        backButton.vmEnabled = false;

    }

    function advanceStudyIndicator(){
        progressLine.indicateNext();
    }

    function changeNextButtonTextAndIcon(text,icon){
        nextButton.vmText = text;
        nextButton.vmIconSource = icon;
    }

    function enableNextButton(enable){
        nextButton.vmEnabled = enable;
    }

    function enableBackButton(enable){
        backButton.vmEnabled = enable;
    }

    function calibrationValidated(){
        evaluationRun.vmIsCalibrated = true;
        evaluationRun.prepareNextStudyOrHandCalibration(false);
    }

    function setCalibrationSpeedToSlow(slow){
        evaluationRun.vmSlowCalibrationSelected = slow;
    }

    function setCurrentStudySequence(name){
        vmCurrentStudySequence = name;
        sequenceName.clear();
        loader.setCurrentStudySequence(name);
    }

    function finalizeSequenceAction(goToReports){

        let popup = loader.getStringForKey("viewevaluation_finish_message")
        popup = popup.replace("<b></b>",vmSelectedPatientName)

        mainWindow.swipeTo(VMGlobals.vmSwipeIndexMainScreen)

        mainWindow.popUpNotify(VMGlobals.vmNotificationGreen,popup)

        if (goToReports){
            viewMainSetup.goToReportList();
        }

    }

    VMConfirmDialog {
        id: confirmStudyAbort
        onConfirm: {
            flowControl.keyboardKeyPressed(Qt.Key_Escape);
        }
    }


    VMButton {
        id: backButton
        vmText: loader.getStringForKey("viewlogin_back")
        vmIconSource: "arrow"
        vmButtonType: backButton.vmTypeTertiary
        anchors.bottom: parent.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(592)
        anchors.left: parent.left
        anchors.leftMargin: VMGlobals.adjustWidth(5)
        visible: vmEnabled
        onClickSignal: {
            mainWindow.swipeTo(VMGlobals.vmSwipeIndexMainScreen)
        }
    }


    Text {
        id: patientText
        font.pixelSize: VMGlobals.vmFontVeryLarge
        font.weight: 400
        /// VERY VERY IMPORTANT. Qt BUG: So here is what happens. When I developed this, rich text formatting got disabled if
        /// before I set the text string, a context from a Canvas object set a font. The work around I found is to simply 'have'
        /// HTML text, BEFORE the Canvas does it's operations and then when over writing the text, it shows up correctly.
        /// I managed to build a MWE (which I have since deleted). Steps to reproduce:
        ///    Create a Text QML object with "<b>Bold</b><br>Not Bold" (call it id: text)
        ///    Create a Canvas and on the onPaint method, get the canvas (ctx variable) and do cxt.font = "10px sans-serif"
        ///    Create a button. On the onClicked Method of the button set the text.text = "<b>More Bold</b><br>More Not Bold"
        ///    This will work.
        ///    Now simply comment the text property in the id: text object and re run.
        ///    When pressing the button, the text will appear but it won't be in bold.
        text: "Patient,<br><b>patientname</b>"
        textFormat: Text.RichText
        color: VMGlobals.vmBlackText
        height: VMGlobals.adjustHeight(64)
        verticalAlignment: Text.AlignVCenter
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(511)
        anchors.leftMargin: VMGlobals.adjustWidth(15)
    }

    Row {
        id: personalIDRow
        height: VMGlobals.adjustHeight(20)
        anchors.top: patientText.bottom
        anchors.left: patientText.left
        anchors.topMargin: VMGlobals.adjustHeight(14)
        spacing: VMGlobals.adjustWidth(5)

        Text {
            id: personalIDName
            height: parent.height
            verticalAlignment: Text.AlignVCenter
            text: loader.getStringForKey("viewpatlist_id") + ":"
            color: VMGlobals.vmGrayPlaceholderText
            font.pixelSize: VMGlobals.vmFontBaseSize
            font.weight: 400
        }
        Text {
            id: personalIDValue
            height: parent.height
            verticalAlignment: Text.AlignVCenter
            color: VMGlobals.vmBlackText
            font.pixelSize: VMGlobals.vmFontBaseSize
            font.weight: 400
        }
    }

    Row {
        id: dateRow
        height: VMGlobals.adjustHeight(20)
        anchors.top: personalIDRow.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)
        anchors.left: patientText.left
        spacing: VMGlobals.adjustWidth(5)

        Text {
            id: dateName
            text: loader.getStringForKey("viewevaluation_date") + ":"
            height: parent.height
            verticalAlignment: Text.AlignVCenter
            color: VMGlobals.vmGrayPlaceholderText
            font.pixelSize: VMGlobals.vmFontBaseSize
            font.weight: 400
        }
        Text {
            id: dateValue
            height: parent.height
            verticalAlignment: Text.AlignVCenter
            color: VMGlobals.vmBlackText
            font.pixelSize: VMGlobals.vmFontBaseSize
            font.weight: 400
        }
    }

    Rectangle {
        property double vmBorderPadding: VMGlobals.adjustWidth(1)
        id: mainRect
        clip: true
        radius: VMGlobals.adjustHeight(10)
        width: VMGlobals.adjustWidth(969);
        height: VMGlobals.adjustHeight(670+10-84);
        anchors.right: parent.right
        anchors.rightMargin: VMGlobals.adjustWidth(15)
        anchors.bottom: horizontalDivider.top
        anchors.bottomMargin: -radius
        border.width: vmBorderPadding
        border.color: VMGlobals.vmGrayUnselectedBorder

        SwipeView {
            id: viewer
            interactive: false
            currentIndex: 0
            width: mainRect.width - mainRect.vmBorderPadding*2
            height: mainRect.height - mainRect.radius - mainRect.vmBorderPadding
            x: mainRect.vmBorderPadding
            y: mainRect.vmBorderPadding

            Item {
                SCEvalGeneralSettings {
                    id: generalSettings
                    radius: mainRect.radius
                    border.width:  mainRect.border.width
                    border.color: mainRect.border.color
                    width: parent.width
                    height: parent.height + radius
                    onGoToEvalSetup: {
                        progressLine.indicateNext()
                        // If an evaluation sequece was selected then we set that sequence at this point.
                        evaluationSetup.loadEvaluationSequences(vmCurrentStudySequence);
                        viewer.currentIndex = vmSC_INDEX_EVAL_SETTINGS
                    }
                }
            }

            Item {
                SCEvalConfiguration {
                    id: evaluationSetup
                    radius: mainRect.radius
                    border.width:  mainRect.border.width
                    border.color: mainRect.border.color
                    width: parent.width
                    height: parent.height + radius
                    onGoToEvalRun: {
                        viewer.currentIndex = vmSC_INDEX_EVALUATION_SCREEN
                    }
                }
            }

            Item {
                SCEvalRunEvaluation {
                    id: evaluationRun
                    radius: mainRect.radius
                    border.width:  0
                    width: parent.width
                    height: parent.height + radius
                    renderWindowOffsetX: mainRect.x + viewer.x
                    renderWindowOffsetY: mainRect.y + viewer.y
                    onAllEvalsDone: {
                        viewer.currentIndex = vmSC_INDEX_EVALUATION_FINISHED
                    }
                }
            }

            Item {
                SCEValsFinished {
                    id: evaluationFinished
                    radius: mainRect.radius
                    border.width:  0
                    width: parent.width
                    height: parent.height + radius
                }
            }

            onCurrentIndexChanged: {

                flowControl.hideRenderWindow();

                switch (currentIndex){
                case vmSC_INDEX_GENERAL_SETTINGS:
                    nextButton.vmIconSource = "next";
                    nextButton.vmText = loader.getStringForKey("viewevaluation_next_button");
                    generalSettings.prepareSettings();
                    break;
                case vmSC_INDEX_EVAL_SETTINGS:
                    nextButton.vmIconSource = "";
                    nextButton.vmText = loader.getStringForKey("viewevaluation_start");
                    evaluationSetup.resetStudySelection();
                    break;
                case vmSC_INDEX_EVALUATION_SCREEN:
                    //console.log("Should be setting the calibration button")

                    flowControl.setRenderWindowState(false);
                    flowControl.showRenderWindow();

                    nextButton.vmText = loader.getStringForKey("viewevaluation_action_calibrate")
                    nextButton.vmIconSource = ""
                    evaluationRun.vmEvaluationStage = evaluationRun.vmSTAGE_CALIBRATION
                    evaluationRun.setCalibrationExplantion() // Setting the calibration explanation message.
                    evaluationRun.vmInCalibration = false;
                    evaluationRun.setStudyAndStage("",evaluationRun.vmSTAGE_CALIBRATION);
                    // console.log(JSON.stringify(vmSelectedEvaluationConfigurations))
                    break;
                case vmSC_INDEX_EVALUATION_FINISHED:
                    enableNextButton(true);
                    nextButton.vmText = loader.getStringForKey("view_qc_send_report").toUpperCase();
                    nextButton.vmIconSource = "";
                    break;
                }
            }

        }

    }

    Rectangle {
        id: horizontalDivider
        height: VMGlobals.adjustHeight(84-1)
        anchors.right: mainRect.right
        anchors.bottom: parent.bottom
        width: mainRect.width
        border.width: VMGlobals.adjustHeight(1)
        border.color: VMGlobals.vmGrayUnselectedBorder

        VMButton {
            id: nextButton
            vmText: loader.getStringForKey("viewevaluation_next_button")
            vmIconSource: "next"
            vmIconToTheRight: true
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: VMGlobals.adjustWidth(29)
            onClickSignal: {
                switch (viewer.currentIndex){
                case vmSC_INDEX_GENERAL_SETTINGS:
                    generalSettings.onNext()
                    break;
                case vmSC_INDEX_EVAL_SETTINGS:
                    evaluationSetup.setupEvaluations(sequenceName.vmCurrentText,false);
                    break;
                case vmSC_INDEX_EVALUATION_SCREEN:
                    evaluationRun.onNextButtonPressed();
                    break;
                case vmSC_INDEX_EVALUATION_FINISHED:
                    finalizeSequenceAction(true)
                    break;
                }
            }
        }

        VMButton {
            id: btnBackToPatientScreen
            vmText: loader.getStringForKey("viewevaluation_goback");
            vmButtonType: btnBackToPatientScreen.vmTypeSecondary
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: nextButton.left
            anchors.rightMargin: VMGlobals.adjustWidth(29)
            visible: (viewer.currentIndex === vmSC_INDEX_EVALUATION_FINISHED)
            onClickSignal: {
                finalizeSequenceAction(false)
            }


        }

        Row {
            id: sequenceNameInputRow
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: VMGlobals.adjustWidth(20)
            visible: viewer.currentIndex === vmSC_INDEX_EVAL_SETTINGS
            spacing: VMGlobals.adjustWidth(10)

            Text {
                id: sequenceNameLabel
                text: loader.getStringForKey("viewevaluation_seq_name")
                color: VMGlobals.vmBlackText
                font.pixelSize: VMGlobals.vmFontLarge
                font.weight: 600
                anchors.verticalCenter: sequenceName.verticalCenter
            }

            VMTextInput {
                id: sequenceName
                vmPlaceHolderText: loader.getStringForKey("viewevaluation_seq_name_ph")
                vmLabel: ""
                width: VMGlobals.adjustWidth(300)
                vmMaxLength: 41
                onVmCurrentTextChanged: {
                    if (vmCurrentText == ""){
                        nextButton.vmText = loader.getStringForKey("viewevaluation_start");
                    }
                    else {
                        nextButton.vmText = loader.getStringForKey("viewevaluation_start_and_save");
                    }
                }
            }

        }


/////////////// It is unlikely that the button will ever be activated again.
/////////////// But it took me a long time to figure out the right sequence fo calls in order to start the sequence with no errors. So I'm leaving it here.
//        VMButton {
//            id: btnStartStudySequence
//            vmButtonType: btnStartStudySequence.vmTypeSecondary
//            vmText: loader.getStringForKey("viewevaluation_start_sequence")
//            anchors.verticalCenter: parent.verticalCenter
//            anchors.right: nextButton.left
//            anchors.rightMargin: VMGlobals.adjustWidth(10)
//            visible: (viewer.currentIndex === vmSC_INDEX_GENERAL_SETTINGS) && (vmCurrentStudySequence != "")
//            onClickSignal: {
//                generalSettings.onNext()
//                evaluationSetup.loadEvaluationSequences(vmCurrentStudySequence)
//                evaluationSetup.resetStudySelection()
//                evaluationSetup.setupEvaluations("",true);
//            }
//        }

        VMButton {
            id: skipCalibrationButton
            vmText: loader.getStringForKey("viewevaluation_skip_calibration")
            vmButtonType: skipCalibrationButton.vmTypeSecondary
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: VMGlobals.adjustWidth(29)
            visible: {
                if (evaluationRun.vmCurrentEvaluation === 0) return false;
                if (!evaluationRun.vmIsCalibrated) return false; // This takes precedence over the stage.
                if (evaluationRun.vmEvaluationStage === evaluationRun.vmSTAGE_CALIBRATION)  return true;
                else return false;
            }
            onClickSignal: {
                evaluationRun.prepareNextStudyOrHandCalibration(true)
            }
        }

        VMButton {
            id: restartHandCalibrationButton
            vmText: loader.getStringForKey("viewevaluation_restart_hand_calib")
            vmButtonType: skipCalibrationButton.vmTypeSecondary
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: VMGlobals.adjustWidth(29)
            visible: (evaluationRun.vmEvaluationStage == evaluationRun.vmSTAGE_HAND_CALIB_VERIF)
            onClickSignal: {
                // This should reset the hand calibration.
                evaluationRun.prepareNextStudyOrHandCalibration(true)
            }
        }

    }

    VMProgressLine {
        id: progressLine
        anchors.top: dateRow.bottom
        anchors.topMargin: VMGlobals.adjustHeight(20)
        anchors.left: parent.left
        anchors.leftMargin: VMGlobals.adjustWidth(15)
        vmSuggestedWidth: VMGlobals.mainWidth - mainRect.width - mainRect.anchors.rightMargin
        onProgressLineUpdated: {
            if (viewer.currentIndex === vmSC_INDEX_EVALUATION_SCREEN){
                evaluationRun.setStudyAndStage();
            }
        }
    }

    Keys.onPressed: function (event) {

        if (viewer.currentIndex !== vmSC_INDEX_EVALUATION_SCREEN) return;

        //console.log("Inside the evaluation screen")

        var allowed_keys = [];

        switch(evaluationRun.vmEvaluationStage){
        case evaluationRun.vmSTAGE_CALIBRATION:
            if (event.key === Qt.Key_Escape){
                if (!evaluationRun.vmInCalibration){
                    flowControl.renderWaitScreen("");
                    // Store calibration history right here.
                    flowControl.storeCalibrationHistoryAsFailedCalibration();
                    mainWindow.swipeTo(VMGlobals.vmSwipeIndexMainScreen)
                }
            }
            break;
        case evaluationRun.vmSTAGE_EVALUATION:
            allowed_keys = [Qt.Key_Escape, Qt.Key_S, Qt.Key_D, Qt.Key_G]
            break;
        case evaluationRun.vmSTAGE_EXAMPLES:
            allowed_keys = [Qt.Key_N, Qt.Key_Escape, Qt.Key_S, Qt.Key_D]
            break;
        case evaluationRun.vmSTAGE_EXPLANATION:
            allowed_keys = [Qt.Key_N, Qt.Key_B, Qt.Key_Escape];
            break;
        default:
            return;
        }

        //console.log("Allowed keys in the current stage of " + evaluationRun.vmEvaluationStage + " are " + JSON.stringify(allowed_keys) + ". Pressed key " + event.key)

        for (var i = 0; i < allowed_keys.length; i++){
            if (event.key === allowed_keys[i]){
                //console.log("Passing on event key " + event.key)

                if (event.key === Qt.Key_Escape){
                    confirmStudyAbort.askForConfirmation(loader.getStringForKey("viewevaluation_comfirm_abort_title"),
                                                         loader.getStringForKey("viewevaluation_comfirm_abort_msg"))
                }
                else {
                    flowControl.keyboardKeyPressed(event.key);
                }

                return;
            }
        }

    }

}

