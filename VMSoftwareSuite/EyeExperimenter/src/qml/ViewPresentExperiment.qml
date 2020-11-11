import QtQuick 2.6
import QtQuick.Controls 2.3

VMBase {

    id: viewPresentExperiment

    readonly property string keysearch: "viewpresentexp_"

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
                if (flowControl.areThereFrequencyErrorsPresent()){
                    vmErrorDiag.vmErrorCode = vmErrorDiag.vmERROR_EXP_END_ERROR;
                    titleMsg = viewHome.getErrorTitleAndMessage("error_freq_check");
                    vmErrorDiag.vmErrorMessage = titleMsg[1];
                    vmErrorDiag.vmErrorTitle = titleMsg[0];
                    vmErrorDiag.open();
                    btnContinue.enabled = false;
                    return;
                }
                else{
                    vmErrorDiag.vmErrorCode = vmErrorDiag.vmERROR_EXP_END_ERROR;
                    titleMsg = viewHome.getErrorTitleAndMessage("error_experiment_end");
                    vmErrorDiag.vmErrorMessage = titleMsg[1];
                    vmErrorDiag.vmErrorTitle = titleMsg[0];
                    vmErrorDiag.open();
                    btnContinue.enabled = false;
                    return;
                }
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
        accWidth = accWidth + item.getWidth()
        return accWidth
    }

    function setEnabledItemX(item,x,spacing){
        item.x = x;
        x = item.getWidth() + x + spacing
        return x
    }

    function setStateOfItem(index,state){
        switch (index){
        case vmExpIndexBindingBC:
            itemBindingBC.vmTrackerItemState = state;
            break;
        case vmExpIndexBindingUC:
            itemBindingUC.vmTrackerItemState = state;
            break;
        case vmExpIndexReading:
            itemReading.vmTrackerItemState = state;
            break;
        case vmExpIndexFielding:
            itemFielding.vmTrackerItemState = state;
            break;
        case vmExpIndexNBackRT:
            itemNBackRT.vmTrackerItemState = state;
            break;
        case vmExpIndexParkinson:
            itemParkinson.vmTrackerItemState = state;
            break;
        case vmExpIndexGoNoGo:
            itemGoNoGo.vmTrackerItemState = state;
            break;
        case vmExpIndexNBackVS:
            itemNBackVS.vmTrackerItemState = state;
            break;
        case vmExpIndexPerception:
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

    function setPropertiesForExperiment(index){
        switch(index){
        case vmExpIndexBindingBC:
            if (loader.getConfigurationString(vmDefines.vmCONFIG_BINDING_NUMBER_OF_TARGETS) === "3"){
                vmSlideTitle = loader.getStringForKey(keysearch+"itemBindingBC3");
                viewVRDisplay.vmStudyTitle = vmSlideTitle;
                vmSlideExplanation = loader.getStringForKey(keysearch+"bindingBCExp");
                //vmSlideAnimation = "qrc:/images/bound_3.gif"
                slideViewer.imgScale = 1.3;
                setSlideImages("bound3",15);
            }
            else{
                vmSlideTitle = loader.getStringForKey(keysearch+"itemBindingBC");
                viewVRDisplay.vmStudyTitle = vmSlideTitle;
                vmSlideExplanation = loader.getStringForKey(keysearch+"bindingBCExp");
                //vmSlideAnimation = "qrc:/images/bound.gif"
                slideViewer.imgScale = 1.3;
                setSlideImages("bound",15);
            }
            //slideAnimation.visible = false;
            slideDescription.visible = false;
            break;
        case vmExpIndexBindingUC:
            if (loader.getConfigurationString(vmDefines.vmCONFIG_BINDING_NUMBER_OF_TARGETS) === "3"){
                vmSlideTitle = loader.getStringForKey(keysearch+"itemBindingUC3");
                viewVRDisplay.vmStudyTitle = vmSlideTitle;
                vmSlideExplanation = loader.getStringForKey(keysearch+"bindingUCExp");
                //vmSlideAnimation = "qrc:/images/unbound_3.gif"
                slideViewer.imgScale = 1.3;
                setSlideImages("unbound3",15);
            }
            else{
                vmSlideTitle = loader.getStringForKey(keysearch+"itemBindingUC");
                viewVRDisplay.vmStudyTitle = vmSlideTitle;
                vmSlideExplanation = loader.getStringForKey(keysearch+"bindingUCExp");
                //vmSlideAnimation = "qrc:/images/unbound.gif"
                slideViewer.imgScale = 1.3;
                setSlideImages("unbound",15);
            }
            //slideAnimation.visible = false;
            slideDescription.visible = false;
            break;
        case vmExpIndexReading:
            vmSlideTitle = loader.getStringForKey(keysearch+"itemReading");
            viewVRDisplay.vmStudyTitle = vmSlideTitle;
            vmSlideExplanation = loader.getStringForKey(keysearch+"readingExp");
            //vmSlideAnimation = "qrc:/images/reading.gif"
            //slideAnimation.visible = false;
            slideDescription.visible = false;
            slideViewer.imgScale = 1.3;
            setSlideImages("reading_" + loader.getConfigurationString(vmDefines.vmCONFIG_READING_EXP_LANG),3);
            break;
        case vmExpIndexFielding:
            vmSlideTitle = loader.getStringForKey(keysearch+"itemFielding");
            viewVRDisplay.vmStudyTitle = vmSlideTitle;
            vmSlideExplanation = loader.getStringForKey(keysearch+"fieldingExp");
            slideDescription.visible = false;
            slideViewer.imgScale = 1.3;
            setSlideImages("fielding",1);
            break;
        case vmExpIndexNBackRT:
            vmSlideTitle = loader.getStringForKey(keysearch+"itemNBackRT");
            viewVRDisplay.vmStudyTitle = vmSlideTitle;
            vmSlideExplanation = loader.getStringForKey(keysearch+"fieldingExp");
            slideDescription.visible = false;
            slideViewer.imgScale = 1.3;
            setSlideImages("fielding",1);
            break;
        case vmExpIndexParkinson:
            vmSlideTitle = loader.getStringForKey(keysearch+"itemParkinson");
            viewVRDisplay.vmStudyTitle = vmSlideTitle;
            vmSlideExplanation = "";
            slideDescription.visible = false;
            slideViewer.imgScale = 1.3;
            setSlideImages("fielding",1);
            break;
        case vmExpIndexGoNoGo:
            vmSlideTitle = loader.getStringForKey(keysearch+"itemGoNoGo");
            viewVRDisplay.vmStudyTitle = vmSlideTitle;
            vmSlideExplanation = "";
            slideDescription.visible = false;
            slideViewer.imgScale = 1.3;
            setSlideImages("gonogo",5);
            break;
        case vmExpIndexNBackVS:
            vmSlideTitle = loader.getStringForKey(keysearch+"itemNBackVS");
            viewVRDisplay.vmStudyTitle = vmSlideTitle;
            vmSlideExplanation = "";
            slideDescription.visible = false;
            slideViewer.imgScale = 1.3;
            setSlideImages("fielding",1);
            break;
        case vmExpIndexPerception:
            vmSlideTitle = loader.getStringForKey(keysearch+"itemPerception");
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
        var spacing = 18;

        // The number of items in the list.
        var L = list.length;

        for (var i = 0; i < L; i++){
            switch (list[i]){
            case vmExpIndexBindingBC:
                accWidth = enableTrackItem(itemBindingBC,i,accWidth,L);
                break;
            case vmExpIndexBindingUC:
                accWidth = enableTrackItem(itemBindingUC,i,accWidth,L);
                break;
            case vmExpIndexReading:
                accWidth = enableTrackItem(itemReading,i,accWidth,L);
                break;
            case vmExpIndexFielding:
                accWidth = enableTrackItem(itemFielding,i,accWidth,L);
                break;
            case vmExpIndexNBackRT:
                accWidth = enableTrackItem(itemNBackRT,i,accWidth,L)
                break;
            case vmExpIndexParkinson:
                accWidth = enableTrackItem(itemParkinson,i,accWidth,L);
                break;
            case vmExpIndexGoNoGo:
                accWidth = enableTrackItem(itemGoNoGo,i,accWidth,L);
                break;
            case vmExpIndexNBackVS:
                accWidth = enableTrackItem(itemNBackVS,accWidth,L);
                break;
            case vmExpIndexPerception:
                accWidth = enableTrackItem(itemPerception,accWidth,L);
                break;
            }
        }

        // Once the list was passed once the offset is calculated:
        //console.log("Accum w " + accWidth + "L " + L + " and spacign " + spacing + " p.w " + mainWindow.width)
        var x = accWidth + (L-1)*spacing
        x = (mainWindow.width - x)/2;
        //console.log("Start x is " + x)
        for (i = 0; i < L; i++){
            switch (list[i]){
            case vmExpIndexBindingBC:
                x = setEnabledItemX(itemBindingBC,x,spacing);
                break;
            case vmExpIndexBindingUC:
                x = setEnabledItemX(itemBindingUC,x,spacing);
                break;
            case vmExpIndexReading:
                x = setEnabledItemX(itemReading,x,spacing);
                break;
            case vmExpIndexFielding:
                x = setEnabledItemX(itemFielding,x,spacing);
                break;
            case vmExpIndexNBackRT:
                x = setEnabledItemX(itemNBackRT,x,spacing);
                break;
            case vmExpIndexParkinson:
                x = setEnabledItemX(itemParkinson,x,spacing);
                break;
            case vmExpIndexGoNoGo:
                x = setEnabledItemX(itemGoNoGo,x,spacing);
                break;
            case vmExpIndexNBackVS:
                x = setEnabledItemX(itemNBackVS,x,spacing);
                break;
            case vmExpIndexPerception:
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
            setStateOfItem(viewStudyStart.vmSelectedExperiments[index],itemReading.vmTRACKER_ITEM_STATE_DONE)
        }
        // The new experiment is selected as done.
        index = viewStudyStart.vmSelectedExperiments[viewStudyStart.vmCurrentExperimentIndex];
        setStateOfItem(index,itemReading.vmTRACKER_ITEM_STATE_CURRENT)

        // Properties are set to the values of the curent experiment
        setPropertiesForExperiment(index);

        // Not done with the sequence.
        return false

    }

    function changeBindingTitles(use3){
        experimentTracker.changeBindingTitles(use3);
    }

    function startNextStudy(){

        // Setting up the second monitor, if necessary.
        flowControl.setupSecondMonitor();

        // Starting the experiment.
        var index = viewStudyStart.vmCurrentExperimentIndex;
        if (!flowControl.startNewExperiment(viewStudyStart.vmSelectedExperiments[index])){
            vmErrorDiag.vmErrorCode = vmErrorDiag.vmERROR_PROG_ERROR;
            var titleAndMsg = viewHome.getErrorTitleAndMessage("error_programming");
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
                itemBindingUC.vmText = loader.getStringForKey(keysearch + "itemBindingUC3");
                itemBindingBC.vmText = loader.getStringForKey(keysearch + "itemBindingBC3");
            }
            else{
                itemBindingUC.vmText = loader.getStringForKey(keysearch + "itemBindingUC");
                itemBindingBC.vmText = loader.getStringForKey(keysearch + "itemBindingBC");
            }
        }

        VMExperimentTrackerItem {
            id: itemReading
            vmText: loader.getStringForKey(keysearch + "itemReading")
            vmFont: viewPresentExperiment.robotoM.name
        }

        VMExperimentTrackerItem {
            id: itemBindingUC
            vmText: loader.getStringForKey(keysearch + "itemBindingUC")
            vmFont: viewPresentExperiment.robotoM.name
        }

        VMExperimentTrackerItem {
            id: itemBindingBC
            vmText: loader.getStringForKey(keysearch + "itemBindingBC")
            vmFont: viewPresentExperiment.robotoM.name
            vmIsLast: false
        }

        VMExperimentTrackerItem {
            id: itemFielding
            vmText: loader.getStringForKey(keysearch + "itemFielding")
            vmFont: viewPresentExperiment.robotoM.name
        }

        VMExperimentTrackerItem {
            id: itemNBackRT
            vmText: loader.getStringForKey(keysearch + "itemNBackRT");
            vmFont: viewPresentExperiment.robotoM.name
        }

        VMExperimentTrackerItem {
            id: itemParkinson
            vmText: loader.getStringForKey(keysearch + "itemParkinson");
            vmFont: viewPresentExperiment.robotoM.name
        }

        VMExperimentTrackerItem {
            id: itemGoNoGo
            vmText: loader.getStringForKey(keysearch + "itemGoNoGo")
        }

        VMExperimentTrackerItem {
            id: itemNBackVS
            vmText: loader.getStringForKey(keysearch + "itemNBackVS")
        }

        VMExperimentTrackerItem {
            id: itemPerception
            vmText: loader.getStringForKey(keysearch + "itemPerception");
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
            vmText: loader.getStringForKey(keysearch + "btnBack")
            vmSize: [mainWindow.width*0.28, mainWindow.height*0.072]
            vmInvertColors: true
            vmFont: viewPresentExperiment.gothamM.name
            onClicked: {
                swiperControl.currentIndex = swiperControl.vmIndexStudyStart;
            }
        }

        VMButton{
            id: btnContinue
            vmText: loader.getStringForKey(keysearch + "btnContinue")
            vmSize: [mainWindow.width*0.28, mainWindow.height*0.072]
            vmInvertColors: true
            vmFont: viewPresentExperiment.gothamM.name
            onClicked: {
                btnContinue.enabled = false;
                if (loader.getConfigurationString(vmDefines.vmCONFIG_SELECTED_ET) === vmDefines.vmCONFIG_P_ET_HTCVIVEEYEPRO){
                    swiperControl.currentIndex = swiperControl.vmIndexVRDisplay;
                }
                else {
                    startNextStudy();
                }
            }
        }

    }


}
