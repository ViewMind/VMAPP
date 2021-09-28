import QtQuick
import QtQuick.Controls
import com.qml 1.0

VMBase {

    id: viewPresentExperiment

    readonly property double vmExpTrackerItemWidth: mainWindow.width/5;

    property string vmSlideTitle: "NO TITLE SET";
    property int vmSelectedEye;
    property int vmState: 0
    property int vmCurrentUniqueStudyIndexID: -1

    ///////////////////// State Values for State Machine
    readonly property int vmSTATE_START: 0
    readonly property int vmSTATE_CALIBRATION: 1
    readonly property int vmSTATE_CALIBRATION_DONE: 2
    readonly property int vmSTATE_DEMO: 3
    readonly property int vmSTATE_EVALUATION: 4
    readonly property int vmSTATE_GOBACK: 5


    ///////////////////// INDEXES FOR BULLETS FOR CONVENIENCE
    readonly property int vmBULLET_IDX_CALIBRATION: 0
    readonly property int vmBULLET_IDX_EXPLANATION: 1
    readonly property int vmBULLET_IDX_EVALUATION:  2

    readonly property int vmBULLET_IDX_PRESS_N:     0
    readonly property int vmBULLET_IDX_PRESS_ESC:   1
    readonly property int vmBULLET_IDX_PRESS_S:     2
    readonly property int vmBULLET_IDX_PRESS_D:     3
    readonly property int vmBULLET_IDX_NTORESUME:   4

    VMCalibrationFailedDialog{
        id: calibrationFailedDialog
        x: (mainWindow.width-width)/2
        y: (mainWindow.height-height)/2
    }

    Connections{
        target: flowControl
        function onExperimentHasFinished (){
            if (!flowControl.isExperimentEndOk()){
                var titleMsg
                swiperControl.currentIndex = swiperControl.vmIndexPresentExperiment;
                flowControl.generateWaitScreen("");
                vmErrorDiag.vmErrorCode = vmErrorDiag.vmErrorCodeNotClose;
                titleMsg = viewHome.getErrorTitleAndMessage("error_experiment_end");
                vmErrorDiag.vmErrorMessage = titleMsg[1];
                vmErrorDiag.vmErrorTitle = titleMsg[0];
                setActionsForState(vmSTATE_GOBACK);
                vmErrorDiag.open();
                return;
            }
            else{
                flowControl.generateWaitScreen(loader.getStringForKey("waitscreenmsg_studyEnd") + "\n" + vmSlideTitle);
                setActionsForState(vmSTATE_CALIBRATION_DONE);
            }
            if (advanceCurrentExperiment()){
                swiperControl.currentIndex = swiperControl.vmIndexStudyDone;
            }
        }
        function onNewImageAvailable () {
            hmdView.image = flowControl.image;
        }
        function onConnectedToEyeTracker () {
            var titleMsg;
            if (!flowControl.isConnected()){
                vmErrorDiag.vmErrorCode = vmErrorDiag.vmErrorCodeNotClose;
                titleMsg = viewHome.getErrorTitleAndMessage("error_et_connect");
                vmErrorDiag.vmErrorMessage = titleMsg[1];
                vmErrorDiag.vmErrorTitle = titleMsg[0];
                vmErrorDiag.open();
                setActionsForState(vmSTATE_START);
                return;
            }
            // All is good so the calibration is requested.
            flowControl.calibrateEyeTracker(vmSelectedEye);
        }
        function onCalibrationDone() {
            var titleMsg;
            if (!flowControl.isCalibrated()){
                calibrationFailedDialog.vmLeftEyePassed = flowControl.isLeftEyeCalibrated();
                calibrationFailedDialog.vmRightEyePassed = flowControl.isRightEyeCalibrated();
                calibrationFailedDialog.open();
                setActionsForState(vmSTATE_START);
                return;
            }
            flowControl.generateWaitScreen(loader.getStringForKey("waitscreenmsg_calibrationEnd"));
            setActionsForState(vmSTATE_CALIBRATION_DONE);
        }

    }

    // The experiment tracker logic and math functions
    function enableTrackItem(item, place, accWidth, listLength){
        item.vmOrderInTracker = place+1;
        item.visible = true
        var ll = listLength - 1;
        if (place === 0) item.vmTrackerItemState = item.vmTRACKER_ITEM_STATE_CURRENT;
        if (place === listLength-1) item.vmIsLast = true;
        else item.vmIsLast = false
        accWidth = accWidth + item.width
        return accWidth
    }

    function setEnabledItemX(item,x,spacing){
        item.x = x;
        x = item.width + x + spacing
        return x
    }

    function setStateOfItem(index,state){
        index = parseInt(index);
        switch (index){
        case viewStudyStart.vmINDEX_BINDING_BC:
            itemBindingBC.vmTrackerItemState = state;
            break;
        case viewStudyStart.vmINDEX_BINDING_UC:
            itemBindingUC.vmTrackerItemState = state;
            break;
        case viewStudyStart.vmINDEX_READING:
            itemReading.vmTrackerItemState = state;
            break;
        case viewStudyStart.vmINDEX_NBACKMS:
            itemFielding.vmTrackerItemState = state;
            break;
        case viewStudyStart.vmINDEX_NBACKRT:
            itemNBackRT.vmTrackerItemState = state;
            break;
        case viewStudyStart.vmINDEX_GONOGO:
            itemGoNoGo.vmTrackerItemState = state;
            break;
        case viewStudyStart.vmINDEX_NBACKVS:
            itemNBackVS.vmTrackerItemState = state;
            break;
        case viewStudyStart.vmINDEX_PERCEPTION:
            itemPerception.vmTrackerItemState = state;
            break;
        default:
            console.log("Unkown index of type: " + index + " in setStateOfItem in ViewPresentExperiment");
            break;
        }
    }

    function setPropertiesForExperiment(study_config){
        var index = parseInt(study_config[viewStudyStart.vmUNIQUE_STUDY_ID]);
        switch(index){
        case viewStudyStart.vmINDEX_BINDING_BC:
            if (study_config[viewStudyStart.vmSCP_NUMBER_OF_TARGETS] === viewStudyStart.vmSCV_BINDING_TARGETS_3){
                vmSlideTitle = loader.getStringForKey("viewpresentexp_itemBindingBC3");
            }
            else{
                vmSlideTitle = loader.getStringForKey("viewpresentexp_itemBindingBC");
            }
            break;
        case viewStudyStart.vmINDEX_BINDING_UC:
            if (study_config[viewStudyStart.vmSCP_NUMBER_OF_TARGETS] === viewStudyStart.vmSCV_BINDING_TARGETS_3){
                vmSlideTitle = loader.getStringForKey("viewpresentexp_itemBindingUC3");
            }
            else{
                vmSlideTitle = loader.getStringForKey("viewpresentexp_itemBindingUC");
            }
            break;
        case viewStudyStart.vmINDEX_READING:
            vmSlideTitle = loader.getStringForKey("viewpresentexp_itemReading");
            break;
        case viewStudyStart.vmINDEX_NBACKMS:
            vmSlideTitle = loader.getStringForKey("viewpresentexp_itemFielding");
            break;
        case viewStudyStart.vmINDEX_NBACKRT:
            vmSlideTitle = loader.getStringForKey("viewpresentexp_itemNBackRT");
            break;
        case viewStudyStart.vmINDEX_GONOGO:
            vmSlideTitle = loader.getStringForKey("viewpresentexp_itemGoNoGo");
            break;
        case viewStudyStart.vmINDEX_NBACKVS:
            vmSlideTitle = loader.getStringForKey("viewpresentexp_itemNBackVS");
            break;
        case viewStudyStart.vmINDEX_PERCEPTION:
            vmSlideTitle = loader.getStringForKey("viewpresentexp_itemPerception");
            break;
        }

    }

    function setTracker(list){

        // Disabling all items
        itemBindingBC.visible = false
        itemBindingUC.visible = false
        itemReading.visible = false
        itemFielding.visible = false
        itemNBackRT.visible = false
        itemParkinson.visible = false
        itemGoNoGo.visible = false;
        itemNBackVS.visible = false;
        itemPerception.visible = false;

        // Calculated the widths of the enabled items
        var accWidth = 0

        // Spacing defined by the designer
        var spacing = mainWindow.width*0.01;

        // The number of items in the list.
        var L = list.length;

        for (var i = 0; i < L; i++){

            let switchableIndex = parseInt(list[i][viewStudyStart.vmUNIQUE_STUDY_ID])

            switch (switchableIndex){
            case viewStudyStart.vmINDEX_BINDING_BC: //viewStudyStart.vmINDEX_BINDING_BC:
                accWidth = enableTrackItem(itemBindingBC,i,accWidth,L);
                break;
            case viewStudyStart.vmINDEX_BINDING_UC: //viewStudyStart.vmINDEX_BINDING_UC:
                accWidth = enableTrackItem(itemBindingUC,i,accWidth,L);
                break;
            case viewStudyStart.vmINDEX_READING: //viewStudyStart.vmINDEX_READING:
                accWidth = enableTrackItem(itemReading,i,accWidth,L);
                break;
            case viewStudyStart.vmINDEX_NBACKMS: //viewStudyStart.vmINDEX_NBACKMS:
                accWidth = enableTrackItem(itemFielding,i,accWidth,L);
                break;
            case viewStudyStart.vmINDEX_NBACKRT: //viewStudyStart.vmINDEX_NBACKRT:
                accWidth = enableTrackItem(itemNBackRT,i,accWidth,L)
                break;
            case viewStudyStart.vmINDEX_GONOGO: //viewStudyStart.vmINDEX_GONOGO:
                accWidth = enableTrackItem(itemGoNoGo,i,accWidth,L);
                break;
            case viewStudyStart.vmINDEX_NBACKVS: //viewStudyStart.vmINDEX_NBACKVS:
                accWidth = enableTrackItem(itemNBackVS,accWidth,L);
                break;
            case viewStudyStart.vmINDEX_PERCEPTION: //viewStudyStart.vmINDEX_PERCEPTION:
                accWidth = enableTrackItem(itemPerception,accWidth,L);
                break;
            }
        }

        // Once the list was passed once the offset is calculated:
        //console.log("Accum w " + accWidth + "L " + L + " and spacign " + spacing + " p.w " + mainWindow.width)
        var x = vmExpTrackerItemWidth*L + (L-1)*spacing
        x = (mainWindow.width - x)/2;
        //console.log("Start x is " + x)
        for (i = 0; i < L; i++){
            let switchableIndex = parseInt(list[i][viewStudyStart.vmUNIQUE_STUDY_ID]);
            switch (switchableIndex){
            case viewStudyStart.vmINDEX_BINDING_BC:
                x = setEnabledItemX(itemBindingBC,x,spacing);
                break;
            case viewStudyStart.vmINDEX_BINDING_UC:
                x = setEnabledItemX(itemBindingUC,x,spacing);
                break;
            case viewStudyStart.vmINDEX_READING:
                x = setEnabledItemX(itemReading,x,spacing);
                break;
            case viewStudyStart.vmINDEX_NBACKMS:
                x = setEnabledItemX(itemFielding,x,spacing);
                break;
            case viewStudyStart.vmINDEX_NBACKRT:
                x = setEnabledItemX(itemNBackRT,x,spacing);
                break;
            case viewStudyStart.vmINDEX_GONOGO:
                x = setEnabledItemX(itemGoNoGo,x,spacing);
                break;
            case viewStudyStart.vmINDEX_NBACKVS:
                x = setEnabledItemX(itemNBackVS,x,spacing);
                break;
            case viewStudyStart.vmINDEX_PERCEPTION:
                x = setEnabledItemX(itemPerception,x,spacing);
                break;
            }
        }
    }

    function advanceCurrentExperiment(){
        var index = viewStudyStart.vmCurrentExperimentIndex;

        // Can only advance if this is not the last one.
        if (index === viewStudyStart.vmSelectedExperiments.length-1){
            return true;
        }

        viewStudyStart.vmCurrentExperimentIndex++;
        // If this is not the first experiment set the current experiment as done.
        if (index !== -1){
            setStateOfItem(viewStudyStart.vmSelectedExperiments[index][viewStudyStart.vmUNIQUE_STUDY_ID],itemReading.vmTRACKER_ITEM_STATE_DONE)
        }
        // The new experiment is selected as done.
        index = viewStudyStart.vmSelectedExperiments[viewStudyStart.vmCurrentExperimentIndex][viewStudyStart.vmUNIQUE_STUDY_ID];
        vmCurrentUniqueStudyIndexID = index;
        //console.log("Current Study Index is: " + vmCurrentUniqueStudyIndexID);
        setStateOfItem(index,itemReading.vmTRACKER_ITEM_STATE_CURRENT)

        // Properties are set to the values of the curent experiment
        setPropertiesForExperiment(viewStudyStart.vmSelectedExperiments[viewStudyStart.vmCurrentExperimentIndex]);

        // Not done with the sequence.
        return false

    }

    function changeBindingTitles(use3){
        experimentTracker.changeBindingTitles(use3);
    }

    function startNextStudyManualMode(){

        // Setting up the second monitor, if necessary.
        //console.log("Setting up Second Monitor");
        flowControl.setupSecondMonitor();

        // Starting the experiment.
        var index = viewStudyStart.vmCurrentExperimentIndex;

        // Creating and or selecting the patient study file
        var titleAndMsg;
        //console.log(JSON.stringify(viewStudyStart.vmSelectedExperiments));
        if (!loader.createSubjectStudyFile(viewStudyStart.vmSelectedExperiments[index],viewStudyStart.vmSelectedMedic,viewStudyStart.vmSelectedProtocol)){
            vmErrorDiag.vmErrorCode = vmErrorDiag.vmErrorCodeClose;
            titleAndMsg = viewHome.getErrorTitleAndMessage("error_programming");
            vmErrorDiag.vmErrorMessage = titleAndMsg[1];
            vmErrorDiag.vmErrorTitle = titleAndMsg[0];
            vmErrorDiag.open();
            return;
        }

        // Actually starting the experiment.
        //console.log("Starting New Experiment");
        if (!flowControl.startNewExperiment(viewStudyStart.vmSelectedExperiments[index])){
            vmErrorDiag.vmErrorCode = vmErrorDiag.vmERROR_PROG_ERROR;
            titleAndMsg = viewHome.getErrorTitleAndMessage("error_programming");
            vmErrorDiag.vmErrorMessage = titleAndMsg[1];
            vmErrorDiag.vmErrorTitle = titleAndMsg[0];
            vmErrorDiag.open();
            return;
        }

    }

    function startStudy(){
        flowControl.startStudy();
    }

    function resetStateMachine(){

        // Setting the subject name.
        var displayName = "";
        var subject_info = loader.getCurrentSubjectInfo();
        if (subject_info.name === "") displayName = subject_info.supplied_institution_id
        else displayName = subject_info.name + " " + subject_info.lastname;
        subjectCard.setList([displayName]);

        // Setting the state.
        setActionsForState(vmSTATE_START);

    }

    function setActionsForState(state_to_set){

        vmState = state_to_set;

        //console.log("Setting actions for state: " + vmState);

        switch (vmState){
        case vmSTATE_CALIBRATION:
            btnBack.visible = false;
            btnStartDemoMode.visible = false;
            btnStudyStart.visible = false;
            btnCalibrate.visible = false;

            statusCard.setItemState(vmBULLET_IDX_CALIBRATION,true);
            statusCard.setItemState(vmBULLET_IDX_EXPLANATION,false);
            statusCard.setItemState(vmBULLET_IDX_EVALUATION,false);

            actionCard.setItemState(vmBULLET_IDX_PRESS_D,false);
            actionCard.setItemState(vmBULLET_IDX_PRESS_S,false);
            actionCard.setItemState(vmBULLET_IDX_PRESS_ESC,false);
            actionCard.setItemState(vmBULLET_IDX_PRESS_N,false);
            actionCard.setItemState(vmBULLET_IDX_NTORESUME,false);

            break;
        case vmSTATE_START:
            btnBack.visible = true;
            btnStartDemoMode.visible = false;
            btnStudyStart.visible = false;
            btnCalibrate.visible = true;

            statusCard.setItemState(vmBULLET_IDX_CALIBRATION,false);
            statusCard.setItemState(vmBULLET_IDX_EXPLANATION,false);
            statusCard.setItemState(vmBULLET_IDX_EVALUATION,false);

            actionCard.setItemState(vmBULLET_IDX_PRESS_D,false);
            actionCard.setItemState(vmBULLET_IDX_PRESS_S,false);
            actionCard.setItemState(vmBULLET_IDX_PRESS_ESC,false);
            actionCard.setItemState(vmBULLET_IDX_PRESS_N,false);
            actionCard.setItemState(vmBULLET_IDX_NTORESUME,false);

            break;
        case vmSTATE_CALIBRATION_DONE:

            btnBack.visible = true;
            btnStartDemoMode.visible = true;
            btnStudyStart.visible = false;
            btnCalibrate.visible = true;

            statusCard.setItemState(vmBULLET_IDX_CALIBRATION,false);
            statusCard.setItemState(vmBULLET_IDX_EXPLANATION,false);
            statusCard.setItemState(vmBULLET_IDX_EVALUATION,false);

            actionCard.setItemState(vmBULLET_IDX_PRESS_D,false);
            actionCard.setItemState(vmBULLET_IDX_PRESS_S,false);
            actionCard.setItemState(vmBULLET_IDX_PRESS_ESC,false);
            actionCard.setItemState(vmBULLET_IDX_PRESS_N,false);
            actionCard.setItemState(vmBULLET_IDX_NTORESUME,false);

            break;
        case vmSTATE_DEMO:
            btnBack.visible = false;
            btnStartDemoMode.visible = false;
            btnStudyStart.visible = true;
            btnCalibrate.visible = false;

            statusCard.setItemState(vmBULLET_IDX_CALIBRATION,false);
            statusCard.setItemState(vmBULLET_IDX_EXPLANATION,true);
            statusCard.setItemState(vmBULLET_IDX_EVALUATION,false);

            actionCard.setItemState(vmBULLET_IDX_PRESS_D,false);
            actionCard.setItemState(vmBULLET_IDX_PRESS_S,false);
            actionCard.setItemState(vmBULLET_IDX_PRESS_ESC,true);
            actionCard.setItemState(vmBULLET_IDX_PRESS_N,true);
            actionCard.setItemState(vmBULLET_IDX_NTORESUME,false);

            break;
        case vmSTATE_EVALUATION:
            btnBack.visible = false;
            btnStartDemoMode.visible = false;
            btnStudyStart.visible = false;
            btnCalibrate.visible = false;

            statusCard.setItemState(vmBULLET_IDX_CALIBRATION,false);
            statusCard.setItemState(vmBULLET_IDX_EXPLANATION,false);
            statusCard.setItemState(vmBULLET_IDX_EVALUATION,true);

            if ( (vmCurrentUniqueStudyIndexID == viewStudyStart.vmINDEX_BINDING_BC) ||
                 (vmCurrentUniqueStudyIndexID == viewStudyStart.vmINDEX_BINDING_UC) ){
                actionCard.setItemState(vmBULLET_IDX_PRESS_D,true);
                actionCard.setItemState(vmBULLET_IDX_PRESS_S,true);
            }
            else if ( (vmCurrentUniqueStudyIndexID === viewStudyStart.vmINDEX_NBACKRT) ||
                      (vmCurrentUniqueStudyIndexID === viewStudyStart.vmINDEX_NBACKVS) ||
                      (vmCurrentUniqueStudyIndexID === viewStudyStart.vmINDEX_NBACKMS)) {
                actionCard.setItemState(vmBULLET_IDX_NTORESUME,true);
            }

            actionCard.setItemState(vmBULLET_IDX_PRESS_ESC,true);
            actionCard.setItemState(vmBULLET_IDX_PRESS_N,false);
            break;

        case vmSTATE_GOBACK:
            btnBack.visible = true;
            btnStartDemoMode.visible = false;
            btnStudyStart.visible = false;
            btnCalibrate.visible = false;

            statusCard.setItemState(vmBULLET_IDX_CALIBRATION,false);
            statusCard.setItemState(vmBULLET_IDX_EXPLANATION,false);
            statusCard.setItemState(vmBULLET_IDX_EVALUATION,false);

            actionCard.setItemState(vmBULLET_IDX_PRESS_D,false);
            actionCard.setItemState(vmBULLET_IDX_PRESS_S,false);
            actionCard.setItemState(vmBULLET_IDX_PRESS_ESC,false);
            actionCard.setItemState(vmBULLET_IDX_PRESS_N,false);
            actionCard.setItemState(vmBULLET_IDX_NTORESUME,false);
            break;
        }
    }

    // The experiment tracker
    Rectangle{

        id: experimentTracker
        width: parent.width
        height: mainWindow.height*0.103
        anchors.top: vmBanner.bottom
        anchors.left: parent.left
        color: "#EDEDEE"

        function changeBindingTitles(use3){
            if (use3){
                itemBindingUC.vmText = loader.getStringForKey("viewpresentexp_itemBindingUC3");
                itemBindingBC.vmText = loader.getStringForKey("viewpresentexp_itemBindingBC3");
            }
            else{
                itemBindingUC.vmText = loader.getStringForKey("viewpresentexp_itemBindingUC");
                itemBindingBC.vmText = loader.getStringForKey("viewpresentexp_itemBindingBC");
            }
        }

        VMExperimentTrackerItem {
            id: itemReading
            vmText: loader.getStringForKey("viewpresentexp_itemReading")
            vmFont: viewPresentExperiment.robotoM.name
            width: vmExpTrackerItemWidth
        }

        VMExperimentTrackerItem {
            id: itemBindingUC
            vmText: loader.getStringForKey("viewpresentexp_itemBindingUC")
            vmFont: viewPresentExperiment.robotoM.name
            width: vmExpTrackerItemWidth
        }

        VMExperimentTrackerItem {
            id: itemBindingBC
            vmText: loader.getStringForKey("viewpresentexp_itemBindingBC")
            vmFont: viewPresentExperiment.robotoM.name
            vmIsLast: false
            width: vmExpTrackerItemWidth
        }

        VMExperimentTrackerItem {
            id: itemFielding
            vmText: loader.getStringForKey("viewpresentexp_itemFielding")
            vmFont: viewPresentExperiment.robotoM.name
            width: vmExpTrackerItemWidth
        }

        VMExperimentTrackerItem {
            id: itemNBackRT
            vmText: loader.getStringForKey("viewpresentexp_itemNBackRT");
            vmFont: viewPresentExperiment.robotoM.name
            width: vmExpTrackerItemWidth
        }

        VMExperimentTrackerItem {
            id: itemParkinson
            vmText: loader.getStringForKey("viewpresentexp_itemParkinson");
            vmFont: viewPresentExperiment.robotoM.name
            width: vmExpTrackerItemWidth
        }

        VMExperimentTrackerItem {
            id: itemGoNoGo
            vmText: loader.getStringForKey("viewpresentexp_itemGoNoGo")
            width: vmExpTrackerItemWidth
        }

        VMExperimentTrackerItem {
            id: itemNBackVS
            vmText: loader.getStringForKey("viewpresentexp_itemNBackVS")
            width: vmExpTrackerItemWidth
        }

        VMExperimentTrackerItem {
            id: itemPerception
            vmText: loader.getStringForKey("viewpresentexp_itemPerception");
        }

    }


    // The title of this slide
    Text{
        id: slideTitle
        text: vmSlideTitle
        font.family: viewPresentExperiment.gothamM.name
        font.pixelSize: 43*viewHome.vmScale
        color: "#297fca"
        anchors.top: experimentTracker.bottom
        anchors.topMargin: mainWindow.height*0.043
        anchors.horizontalCenter: parent.horizontalCenter
        visible: false
    }


    VMBulletedCard {
        id: subjectCard
        width: mainWindow.width*0.16
        height: mainWindow.height*0.1
        anchors.left: parent.left
        anchors.leftMargin: mainWindow.width*0.02
        anchors.top: hmdView.top
        anchors.topMargin: mainWindow.height*0.01
        vmTitleText: loader.getStringForKey("viewpresentexp_msgTitleSubject");
    }

    VMBulletedCard {
        id: statusCard
        width: subjectCard.width
        height: mainWindow.height*0.2
        anchors.left: subjectCard.left
        anchors.top: subjectCard.bottom
        anchors.topMargin: mainWindow.height*0.01
        vmTitleText: loader.getStringForKey("viewpresentexp_msgTitleState");
        Component.onCompleted: {
            statusCard.setList(loader.getStringListForKey("viewpresentexp_states"));
        }
    }

    VMBulletedCard {
        id: actionCard
        width: subjectCard.width
        height: mainWindow.height*0.35
        anchors.right: parent.right
        anchors.rightMargin: mainWindow.width*0.02
        anchors.top: hmdView.top
        anchors.topMargin: mainWindow.height*0.01
        vmTitleText: loader.getStringForKey("viewpresentexp_msgTitleAction");
        Component.onCompleted: {
            actionCard.setList(loader.getStringListForKey("viewpresentexp_actions"));
        }
    }

    QImageDisplay {
        id: hmdView
        width: parent.width*0.6;
        height: parent.height*0.6;
        //anchors.top: slideTitle.bottom
        anchors.top: experimentTracker.bottom
        anchors.topMargin: parent.height*0.002
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Row {
        id: buttonRow
        anchors.bottom: parent.bottom
        anchors.bottomMargin: mainWindow.height*0.058
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: mainWindow.width*0.04


        VMButton{
            id: btnBack
            vmText: loader.getStringForKey("viewpresentexp_btnBack")
            vmSize: [mainWindow.width*0.28, mainWindow.height*0.072]
            vmInvertColors: true
            vmFont: viewPresentExperiment.gothamM.name
            onClicked: {
                swiperControl.currentIndex = swiperControl.vmIndexStudyStart
            }
        }

        VMButton{
            id: btnCalibrate
            vmText: loader.getStringForKey("viewpresentexp_btnCalibrate")
            vmSize: [mainWindow.width*0.28, mainWindow.height*0.072]
            vmFont: viewPresentExperiment.gothamM.name
            onClicked: {
                setActionsForState(vmSTATE_CALIBRATION);
                if (!flowControl.isConnected()) flowControl.connectToEyeTracker();
                else flowControl.calibrateEyeTracker(vmSelectedEye);
            }
        }

        VMButton{
            id: btnStudyStart
            vmText: loader.getStringForKey("viewpresentexp_btnStartStudy")
            vmSize: [mainWindow.width*0.28, mainWindow.height*0.072]
            vmFont: viewPresentExperiment.gothamM.name
            visible: !btnStartDemoMode.visible // Only visible when study start is NOT visible.
            onClicked: {
                setActionsForState(vmSTATE_EVALUATION);
                startStudy();
            }
        }

        VMButton{
            id: btnStartDemoMode
            vmText: loader.getStringForKey("viewpresentexp_btnStartDemoMode")
            vmSize: [mainWindow.width*0.28, mainWindow.height*0.072]
            vmInvertColors: true
            vmFont: viewPresentExperiment.gothamM.name
            onClicked: {
                setActionsForState(vmSTATE_DEMO);
                startNextStudyManualMode();
            }
        }

    }

    Keys.onPressed: function (event) {

        var allowed_keys = [];

        switch(vmState){
        case vmSTATE_EVALUATION:
            allowed_keys = [Qt.Key_Escape, Qt.Key_S, Qt.Key_D, Qt.Key_G]
            break;
        case vmSTATE_DEMO:
            allowed_keys = [Qt.Key_N, Qt.Key_Escape, Qt.Key_S, Qt.Key_D]
            break;
        default:
            return;
        }

        for (var i = 0; i < allowed_keys.length; i++){
            if (event.key === allowed_keys[i]){
                flowControl.keyboardKeyPressed(event.key);
                return;
            }
        }

    }


}
