import QtQuick 2.6
import QtQuick.Controls 2.3

VMBase {

    id: viewPresentExperiment
    width: viewPresentExperiment.vmWIDTH
    height: viewPresentExperiment.vmHEIGHT

    readonly property string keysearch: "viewpresentexp_"

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


}
