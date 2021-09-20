import QtQuick 2.6
import QtQuick.Controls 2.3

VMBase {

    id: viewPresentExperiment

    readonly property double vmExpTrackerItemWidth: mainWindow.width/5;

    property string vmSlideTitle: "NO TITLE SET";
    property string vmSlideExplanation: "No explanation set";

    property bool experimentInProgress: false;

    function enableContinue(){
        btnContinue.enabled = true;
    }

    Connections{
        target: flowControl
        onExperimentHasFinished:{
            btnContinue.enabled = true;
            if (!flowControl.isExperimentEndOk()){
                var titleMsg
                swiperControl.currentIndex = swiperControl.vmIndexPresentExperiment;
                flowControl.generateWaitScreen("");
                vmErrorDiag.vmErrorCode = vmErrorDiag.vmErrorCodeNotClose;
                titleMsg = viewHome.getErrorTitleAndMessage("error_experiment_end");
                vmErrorDiag.vmErrorMessage = titleMsg[1];
                vmErrorDiag.vmErrorTitle = titleMsg[0];
                vmErrorDiag.open();
                btnContinue.enabled = false;
                return;
            }
            else{
                flowControl.generateWaitScreen(loader.getStringForKey("waitscreenmsg_studyEnd") + "\n" + vmSlideTitle);
                viewVRDisplay.finishedStudySucessfully();
            }
            if (advanceCurrentExperiment()){
                btnContinue.enabled = true;
                swiperControl.currentIndex = swiperControl.vmIndexStudyDone;
            }
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
        }
    }

    function setSlideImages(code,count){
        var slides = [];
        for (var i = 0; i < count; i++){
            slides.push("qrc:/images/" + code + "/" + i + ".gif");
        }
        slideViewer.setImageList(slides);
    }

    function setPropertiesForExperiment(study_config){
        var index = study_config[viewStudyStart.vmUNIQUE_STUDY_ID];
        switch(index){
        case viewStudyStart.vmINDEX_BINDING_BC:
            if (study_config[viewStudyStart.vmSCP_NUMBER_OF_TARGETS] === viewStudyStart.vmSCV_BINDING_TARGETS_3){
                vmSlideTitle = loader.getStringForKey("viewpresentexp_itemBindingBC3");
                viewVRDisplay.vmStudyTitle = vmSlideTitle;
                vmSlideExplanation = loader.getStringForKey("viewpresentexp_bindingBCExp");
                //vmSlideAnimation = "qrc:/images/bound_3.gif"
                slideViewer.imgScale = 1.3;
                setSlideImages("bound3",15);
            }
            else{
                vmSlideTitle = loader.getStringForKey("viewpresentexp_itemBindingBC");
                viewVRDisplay.vmStudyTitle = vmSlideTitle;
                vmSlideExplanation = loader.getStringForKey("viewpresentexp_bindingBCExp");
                //vmSlideAnimation = "qrc:/images/bound.gif"
                slideViewer.imgScale = 1.3;
                setSlideImages("bound",15);
            }
            //slideAnimation.visible = false;
            slideDescription.visible = false;
            break;
        case viewStudyStart.vmINDEX_BINDING_UC:
            if (study_config[viewStudyStart.vmSCP_NUMBER_OF_TARGETS] === viewStudyStart.vmSCV_BINDING_TARGETS_3){
                vmSlideTitle = loader.getStringForKey("viewpresentexp_itemBindingUC3");
                viewVRDisplay.vmStudyTitle = vmSlideTitle;
                vmSlideExplanation = loader.getStringForKey("viewpresentexp_bindingUCExp");
                //vmSlideAnimation = "qrc:/images/unbound_3.gif"
                slideViewer.imgScale = 1.3;
                setSlideImages("unbound3",15);
            }
            else{
                vmSlideTitle = loader.getStringForKey("viewpresentexp_itemBindingUC");
                viewVRDisplay.vmStudyTitle = vmSlideTitle;
                vmSlideExplanation = loader.getStringForKey("viewpresentexp_bindingUCExp");
                //vmSlideAnimation = "qrc:/images/unbound.gif"
                slideViewer.imgScale = 1.3;
                setSlideImages("unbound",15);
            }
            //slideAnimation.visible = false;
            slideDescription.visible = false;
            break;
        case viewStudyStart.vmINDEX_READING:
            vmSlideTitle = loader.getStringForKey("viewpresentexp_itemReading");
            viewVRDisplay.vmStudyTitle = vmSlideTitle;
            vmSlideExplanation = loader.getStringForKey("viewpresentexp_readingExp");
            //vmSlideAnimation = "qrc:/images/reading.gif"
            //slideAnimation.visible = false;
            slideDescription.visible = false;
            slideViewer.imgScale = 1.3;

            var readlang = study_config[viewStudyStart.vmSCP_LANGUAGE];
            if (readlang === viewStudyStart.vmSCV_LANG_DE){
                setSlideImages("reading_de",3);
            }
            else if (readlang === viewStudyStart.vmSCV_LANG_ES){
                setSlideImages("reading_es",3);
            }
            else if (readlang === viewStudyStart.vmSCV_LANG_IS){
                setSlideImages("reading_is",3);
            }
            else if (readlang === viewStudyStart.vmSCV_LANG_FR){
                setSlideImages("reading_fr",3);
            }
            else if (readlang === viewStudyStart.vmSCV_LANG_EN){
                setSlideImages("reading_en",3);
            }

            break;
        case viewStudyStart.vmINDEX_NBACKMS:
            vmSlideTitle = loader.getStringForKey("viewpresentexp_itemFielding");
            viewVRDisplay.vmStudyTitle = vmSlideTitle;
            vmSlideExplanation = loader.getStringForKey("viewpresentexp_fieldingExp");
            slideDescription.visible = false;
            slideViewer.imgScale = 1.3;
            setSlideImages("fielding",1);
            break;
        case viewStudyStart.vmINDEX_NBACKRT:
            vmSlideTitle = loader.getStringForKey("viewpresentexp_itemNBackRT");
            viewVRDisplay.vmStudyTitle = vmSlideTitle;
            vmSlideExplanation = loader.getStringForKey("viewpresentexp_fieldingExp");
            slideDescription.visible = false;
            slideViewer.imgScale = 1.3;
            setSlideImages("fielding",1);
            break;
        case viewStudyStart.vmINDEX_GONOGO:
            vmSlideTitle = loader.getStringForKey("viewpresentexp_itemGoNoGo");
            viewVRDisplay.vmStudyTitle = vmSlideTitle;
            vmSlideExplanation = "";
            slideDescription.visible = false;
            slideViewer.imgScale = 1.3;
            setSlideImages("gonogo",5);
            break;
        case viewStudyStart.vmINDEX_NBACKVS:
            vmSlideTitle = loader.getStringForKey("viewpresentexp_itemNBackVS");
            viewVRDisplay.vmStudyTitle = vmSlideTitle;
            vmSlideExplanation = "";
            slideDescription.visible = false;
            slideViewer.imgScale = 1.3;
            setSlideImages("fielding",1);
            break;
        case viewStudyStart.vmINDEX_PERCEPTION:
            vmSlideTitle = loader.getStringForKey("viewpresentexp_itemPerception");
            viewVRDisplay.vmStudyTitle = vmSlideTitle;
            vmSlideExplanation = "";
            slideDescription.visible = false;
            slideViewer.imgScale = 1.3;
            setSlideImages("perception",3);
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

            switch (list[i][viewStudyStart.vmUNIQUE_STUDY_ID]){
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
            switch (list[i][viewStudyStart.vmUNIQUE_STUDY_ID]){
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
        setStateOfItem(index,itemReading.vmTRACKER_ITEM_STATE_CURRENT)

        // Properties are set to the values of the curent experiment
        setPropertiesForExperiment(viewStudyStart.vmSelectedExperiments[viewStudyStart.vmCurrentExperimentIndex]);

        // Not done with the sequence.
        return false

    }

    function changeBindingTitles(use3){
        experimentTracker.changeBindingTitles(use3);
    }

    function startNextStudy(){

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
    }

    // The description
    Text{
        id: slideDescription
        textFormat: Text.RichText
        font.pixelSize: 16*viewHome.vmScale
        font.family: robotoR.name
        color: "#297fca"
        text: vmSlideExplanation
        anchors.top: slideTitle.bottom
        anchors.topMargin: mainWindow.height*0.099
        anchors.left: parent.left
        anchors.leftMargin: mainWindow.width*0.139
    }

    VMSlideViewer {
        id: slideViewer
        height: parent.height*0.5
        width: parent.width*0.6
        btnSide: mainWindow.width*0.0625
        anchors.top: slideTitle.bottom
        anchors.topMargin: mainWindow.height*0.014
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
                swiperControl.currentIndex = swiperControl.vmIndexStudyStart;
            }
        }

        VMButton{
            id: btnContinue
            vmText: loader.getStringForKey("viewpresentexp_btnContinue")
            vmSize: [mainWindow.width*0.28, mainWindow.height*0.072]
            vmInvertColors: true
            vmFont: viewPresentExperiment.gothamM.name
            onClicked: {
                btnContinue.enabled = false;
                if (!loader.getConfigurationBoolean("use_mouse") && loader.isVREnabled()){
                    swiperControl.currentIndex = swiperControl.vmIndexVRDisplay;
                }
                else {
                    startNextStudy();
                }
            }
        }

    }


}
