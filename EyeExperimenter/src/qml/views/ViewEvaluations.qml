import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"
import "../subscreens"

ViewBase {

    id: evaluationsView

    property string vmSelectedPatientName: ""

    readonly property int vmSC_INDEX_GENERAL_SETTINGS:     0   
    readonly property int vmSC_INDEX_EVALUATION_SCREEN:    1
    readonly property int vmSC_INDEX_EVALUATION_FINISHED:  2


    function setPatientForEvaluation(){

        var patientData = loader.getCurrentSubjectInfo();

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

        let patientDisplayID = "";
        let patientFname = patientData["name"];
        let patientLname = patientData["lastname"]

        if ((patientFname == null) && (patientLname == null)){
            patientDisplayID = patientData["supplied_institution_id"];
        }
        else {
            if (patientFname == null) {
                patientDisplayID = patientLname;
            }
            else if (patientLname == null){
                patientDisplayID = patientFname;
            }
            else {
                patientDisplayID = patientLname + ", " + patientFname;
            }
        }

        var patient = loader.getStringForKey("viewevaluation_patient") + ",<br>";
        vmSelectedPatientName = "<b>" + patientDisplayID + "</b>";
        patient = patient + vmSelectedPatientName
        patientText.text = patient;

        dateValue.text = day + " " + month + " " +  year + ", " + hour + ":" + minutes;
        //console.log(JSON.stringify(patientData));
        personalIDValue.text = patientData["supplied_institution_id"]

        let selectedPatient = patientData["local_id"]

        if (!loader.setSelectedSubject(selectedPatient)){
            mainWindow.showErrorMessage("viewevaluation_error_patient_dir")
            mainWindow.swipeTo(VMGlobals.vmSwipeIndexMainScreen)
        }

        // Resetting the button state.
        // nextButton.vmIconSource = "next";
        nextButton.vmText = loader.getStringForKey("viewevaluation_action_starteval");
        nextButton.vmEnabled = true;

        // Ensuring we start with normal calibration speed
        setCalibrationSpeedToSlow(false)

        // We need to make sure that settings are prepared.
        generalSettings.prepareSettings();

    }

    function forceSettingsView(){
        viewer.currentIndex = vmSC_INDEX_GENERAL_SETTINGS;
    }

    function continueEvaluation(evaluationID){
        loader.setEvaluationID(evaluationID);
        mainWindow.swipeTo(VMGlobals.vmSwipeIndexEvalView)
        viewer.currentIndex = vmSC_INDEX_EVALUATION_SCREEN;
        evaluationRun.setupEvaluation();
    }

    function setCurrentEvaluationName(){
        let name = loader.getSelectedEvaluationName();
        console.log("The current evaluation name is " + name);
        currentEvaluationValue.text = loader.getSelectedEvaluationName();
    }

    function setUpStudyNames(study_names) {

        let plineSetup = {};

        for (let i in study_names){
            plineSetup[study_names[i]] = [];
        }

        plineSetup[loader.getStringForKey("viewevaluation_finish")] = []

        // Adding the "finish" step. No substeps.
        study_names.push(loader.getStringForKey("viewevaluation_finish"))

        // progressLine.vmHideNumberInMainNodes = false
        progressLine.vmOnlyColorCurrent = true;

        progressLine.vmHideNumberInMainNodes = false;
        progressLine.vmOnlyColorCurrent = true;
        progressLine.setup(plineSetup);
        progressLine.reset();        
    }

    function getTaskAndStage(){
        return progressLine.getCurrentTexts();
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

    function calibrationValidated(){
        evaluationRun.vmIsCalibrated = true;
        evaluationRun.prepareNextStudyOrHandCalibration(false,false);
    }

    function setCalibrationSpeedToSlow(slow){
        evaluationRun.vmSlowCalibrationSelected = slow;
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

    VMEyeTrackingCheckDialog {
        id: eyeTrackingCheckDialog
        onDismissed: function (allgood) {
            if (allgood) {
                let mode3d = evaluationRun.isCurrentEvaluationA3DStudy();
                //console.log("Starting the eye tracking calibration with mode 3d equal to ");
                //console.log(mode3d);
                evaluationRun.vmIsCalibrated = false;
                evaluationRun.vmInCalibration = true;
                evaluationsView.enableNextButton(false);
                flowControl.calibrateEyeTracker(evaluationRun.vmSlowCalibrationSelected, mode3d);
            }
            else {
                // Need to go back to the patient list.
                mainWindow.swipeTo(VMGlobals.vmSwipeIndexMainScreen)
                mainWindow.popUpNotify(VMGlobals.vmNotificationRed,loader.getStringForKey("viewevaluation_err_badend_study"))
            }
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
        visible: {
            // The button should not be shown during calibration.
            if (evaluationRun.vmInCalibration) return false;
            // The calibration validation button should not be shown while showning the calibration results.
            if (calibrationValidation.visible) return false;
            // If we are totally done, this shouldn't be shown. This happens only when the btnBackToPatient is visible.
            if (btnBackToPatientScreen.visible) return false;
            // If the study is done and that is left to do is to press the next button, then the back button should not be shown.
            if ((evaluationRun.vmEvaluationStage == evaluationRun.vmSTAGE_EVALUATION) && (nextButton.vmEnabled)) return false;
            // If we are doing hand calibration the abort button is not shown.
            if ((evaluationRun.vmEvaluationStage == evaluationRun.vmSTAGE_HAND_CALIB_H) || (evaluationRun.vmEvaluationStage == evaluationRun.vmSTAGE_HAND_CALIB_H)) return false;

            // In any other case the button should be visible.
            return true;
        }
        onClickSignal: {

            // If we are on general settings, we just go back, nothing to check as nothing has been set.
            if (viewer.currentIndex === vmSC_INDEX_GENERAL_SETTINGS) {
                mainWindow.swipeTo(VMGlobals.vmSwipeIndexMainScreen)
                return;
            }

            // There is essentially only two different messages. If the user aborts during a task, they need to warned that their progress will be lost
            // Otherwise no other text is presented. In both cases they are told how to continue by going into ongoing evaluations.

            if (evaluationRun.vmEvaluationStage == evaluationRun.vmSTAGE_EVALUATION){
                confirmStudyAbort.askForConfirmation(loader.getStringForKey("viewevaluation_comfirm_abort_title"),
                                                     loader.getStringForKey("viewevaluation_comfirm_abort_msg_stop_task"))
            }
            else {
                confirmStudyAbort.askForConfirmation(loader.getStringForKey("viewevaluation_comfirm_abort_title"),
                                                     loader.getStringForKey("viewevaluation_comfirm_abort_msg_stop_eval"))
            }

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
        id: currentEvaluationRow
        height: VMGlobals.adjustHeight(20)
        anchors.top: patientText.bottom
        anchors.left: patientText.left
        anchors.topMargin: VMGlobals.adjustHeight(14)
        spacing: VMGlobals.adjustWidth(5)

        Text {
            id: currentEvaluationName
            height: parent.height
            verticalAlignment: Text.AlignVCenter
            text: loader.getStringForKey("viewongoing_eval") + ":"
            color: VMGlobals.vmGrayPlaceholderText
            font.pixelSize: VMGlobals.vmFontBaseSize
            font.weight: 400
        }
        Text {
            id: currentEvaluationValue
            height: parent.height
            verticalAlignment: Text.AlignVCenter
            color: VMGlobals.vmBlackText
            font.pixelSize: VMGlobals.vmFontBaseSize
            font.weight: 400
        }
    }

    Row {
        id: personalIDRow
        height: VMGlobals.adjustHeight(20)
        anchors.top: currentEvaluationRow.bottom
        anchors.left: currentEvaluationRow.left
        anchors.topMargin: VMGlobals.adjustHeight(10)
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
                    onGoToEvalRun: {
                        viewer.currentIndex = vmSC_INDEX_EVALUATION_SCREEN
                    }
                    onEvaluationsReloaded: {
                        nextButton.vmEnabled = !generalSettings.vmNoEvalsAvailable;
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

                case vmSC_INDEX_EVALUATION_SCREEN:
                    //console.log("Should be setting the calibration button")

                    flowControl.setRenderWindowState(false);
                    flowControl.showRenderWindow();

                    evaluationRun.setupEvaluation();
                    break;

                case vmSC_INDEX_EVALUATION_FINISHED:
                    enableNextButton(true);
                    nextButton.vmText = loader.getStringForKey("viewongoing_revevals");
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


        VMButton {
            id: skipCalibrationButton
            vmText: loader.getStringForKey("viewevaluation_skip_calibration")
            vmButtonType: skipCalibrationButton.vmTypeSecondary
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: VMGlobals.adjustWidth(29)
            visible: {
                if (viewer.currentIndex !== vmSC_INDEX_EVALUATION_SCREEN) return false;
                if (evaluationRun.vmCurrentTask === 0) return false;
                if (!evaluationRun.vmIsCalibrated) return false; // This takes precedence over the stage.
                if (evaluationRun.vmEvaluationStage === evaluationRun.vmSTAGE_CALIBRATION)  return true;
                else return false;
            }
            onClickSignal: {
                evaluationRun.prepareNextStudyOrHandCalibration(true,false)
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
                evaluationRun.prepareNextStudyOrHandCalibration(true,true)
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
        vmHideNumberInMainNodes: true;
        vmUseLargerFontSize: true
        visible: (viewer.currentIndex === vmSC_INDEX_EVALUATION_SCREEN)
        onProgressLineUpdated: {
            if (viewer.currentIndex === vmSC_INDEX_EVALUATION_SCREEN){
                evaluationRun.setStudyAndStage();
            }
        }
    }

    Keys.onPressed: function (event) {

        //console.log("KEY PRESSED: " + event.key);

        if (viewer.currentIndex !== vmSC_INDEX_EVALUATION_SCREEN) return;

        // If this dialog is visible all key strokes should be ignored.
        if (eyeTrackingCheckDialog.visible) return;

        // If the calibration validation is open, all key strokes should be ignored.
        if (calibrationValidation.visible) return;

        var allowed_keys = [];

        switch(evaluationRun.vmEvaluationStage){
        case evaluationRun.vmSTAGE_EVALUATION:
            allowed_keys = [Qt.Key_S, Qt.Key_D]
            break;
        default:
            return;
        }

        //console.log("Allowed keys in the current stage of " + evaluationRun.vmEvaluationStage + " are " + JSON.stringify(allowed_keys) + ". Pressed key " + event.key)

        for (var i = 0; i < allowed_keys.length; i++){
            if (event.key === allowed_keys[i]){
                flowControl.keyboardKeyPressed(event.key);
                return;
            }
        }

    }

}

