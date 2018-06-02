import QtQuick 2.6
import QtQuick.Controls 2.3

VMBase {

    id: viewPresentExperiment
    width: viewPresentExperiment.vmWIDTH
    height: viewPresentExperiment.vmHEIGHT

    readonly property string keysearch: "viewpresentexp_"

    property string vmSlideTitle: "NO TITLE SET";
    property string vmSlideExplanation: "No explanation set";
    property string vmSlideAnimation: "";

    Connections{
        target: flowControl
        onExperimentHasFinished:{
            if (!flowControl.isExperimentEndOk()){
                vmErrorDiag.vmErrorCode = vmErrorDiag.vmERROR_EXP_END_ERROR;
                var titleMsg = viewHome.getErrorTitleAndMessage("error_experiment_end");
                vmErrorDiag.vmErrorMessage = titleMsg[1];
                vmErrorDiag.vmErrorTitle = titleMsg[0];
                vmErrorDiag.open();
                return;
            }
            if (advanceCurrentExperiment()){
                swiperControl.currentIndex = swiperControl.vmIndexResults;
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
        }
    }

    function setPropertiesForExperiment(index){
        switch(index){
        case vmExpIndexBindingBC:
            vmSlideTitle = loader.getStringForKey(keysearch+"itemBindingBC");
            vmSlideExplanation = loader.getStringForKey(keysearch+"bindingBCExp");
            vmSlideAnimation = "qrc:/images/bound.gif"
            break;
        case vmExpIndexBindingUC:
            vmSlideTitle = loader.getStringForKey(keysearch+"itemBindingUC");
            vmSlideExplanation = loader.getStringForKey(keysearch+"bindingUCExp");
            vmSlideAnimation = "qrc:/images/unbound.gif"
            break;
        case vmExpIndexReading:
            console.log("Setting reading");
            vmSlideTitle = loader.getStringForKey(keysearch+"itemReading");
            vmSlideExplanation = loader.getStringForKey(keysearch+"readingExp");            
            vmSlideAnimation = "qrc:/images/reading.gif"
            break;
        case vmExpIndexFielding:
            vmSlideTitle = loader.getStringForKey(keysearch+"itemFielding");
            vmSlideExplanation = loader.getStringForKey(keysearch+"fieldingExp");
            vmSlideAnimation = "qrc:/images/fielding.gif"
            break;
        }
    }

    function setTracker(list){

        // Disabling all items
        itemBindingBC.visible = false
        itemBindingUC.visible = false
        itemReading.visible = false
        itemFielding.visible = false

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
            }
        }

        // Once the list was passed once the offset is calculated:
        //console.log("Accum w " + accWidth + "L " + L + " and spacign " + spacing + " p.w " + viewPresentExperiment.viewPresentExperiment.vmWIDTH)
        var x = accWidth + (L-1)*spacing
        x = (viewPresentExperiment.vmWIDTH - x)/2;
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
            }
        }
    }

    function advanceCurrentExperiment(){
        var index = viewPatientReg.vmCurrentExperimentIndex;

        // Can only advance if this is not the last one.
        if (index === viewPatientReg.vmSelectedExperiments.length-1){
            return true;
        }

        viewPatientReg.vmCurrentExperimentIndex++;
        // If this is not the first experiment set the current experiment as done.
        if (index !== -1){
            setStateOfItem(viewPatientReg.vmSelectedExperiments[index],itemReading.vmTRACKER_ITEM_STATE_DONE)
        }
        // The new experiment is selected as done.
        index = viewPatientReg.vmSelectedExperiments[viewPatientReg.vmCurrentExperimentIndex];
        setStateOfItem(index,itemReading.vmTRACKER_ITEM_STATE_CURRENT)

        // Properties are set to the values of the curent experiment
        setPropertiesForExperiment(index);

        // Not done with the sequence.
        return false

    }

    // The experiment tracker
    Rectangle{

        id: experimentTracker
        width: parent.width
        height: 71
        anchors.top: vmBanner.bottom
        anchors.left: parent.left
        color: "#EDEDEE"

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

    }

    // The title of this slide
    Text{
        id: slideTitle
        text: vmSlideTitle
        font.family: viewPresentExperiment.gothamB.name
        font.pixelSize: 43
        color: "#297fca"
        anchors.top: experimentTracker.bottom
        anchors.topMargin: 60
        anchors.horizontalCenter: parent.horizontalCenter
    }

    // The description
    Text{
        id: slideDescription
        textFormat: Text.RichText
        font.pixelSize: 16
        font.family: robotoR.name
        color: "#297fca"
        text: vmSlideExplanation
        anchors.top: slideTitle.bottom
        anchors.topMargin: 68
        anchors.left: parent.left
        anchors.leftMargin: 178
    }

    AnimatedImage {
        id: slideAnimation
        source: vmSlideAnimation
        anchors.top: slideTitle.bottom
        anchors.topMargin: 68
        anchors.right: parent.right
        anchors.rightMargin: 180
    }

    VMButton{
        id: btnContinue
        vmText: loader.getStringForKey(keysearch + "btnContinue")
        vmSize: [180, 50]
        vmInvertColors: true
        vmFont: viewPresentExperiment.gothamM.name
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 82
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: {
            var index = viewPatientReg.vmCurrentExperimentIndex;
            if (!flowControl.startNewExperiment(viewPatientReg.vmSelectedExperiments[index])){
                vmErrorDiag.vmErrorCode = vmErrorDiag.vmERROR_PROG_ERROR;
                titleMsg = viewHome.getErrorTitleAndMessage("error_programming");
                vmErrorDiag.vmErrorMessage = titleMsg[1];
                vmErrorDiag.vmErrorTitle = titleMsg[0];
                vmErrorDiag.open();
                return;
            }
        }
    }


}
