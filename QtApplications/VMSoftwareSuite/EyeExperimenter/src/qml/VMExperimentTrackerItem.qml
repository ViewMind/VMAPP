import QtQuick

Rectangle {

    id: trackerItem
    height: mainWindow.height*0.103
    color: "#EDEDEE"

    readonly property int vmTRACKER_ITEM_STATE_DONE:    0
    readonly property int vmTRACKER_ITEM_STATE_NOTDONE: 1    
    readonly property int vmTRACKER_ITEM_STATE_CURRENT: 2


    readonly property double vmSpaceBetweenNumberAndTitle: 0.005

    property int vmOrderInTracker: 1
    property int vmTrackerItemState: vmTRACKER_ITEM_STATE_NOTDONE
    property bool vmIsLast: false
    property string vmFont: "Mono"
    property string vmText: "Text"

    function getWidth(){
        var scaling = (vmTrackerItemState == vmTRACKER_ITEM_STATE_DONE)? viewHome.vmScale*0.8 : viewHome.vmScale*0.46
        var ans = number.width*scaling + experiment.width + mainWindow.width**vmSpaceBetweenNumberAndTitle
        if (!vmIsLast) ans = ans + separator.width;
        return ans;
    }

    onHeightChanged: resizeAndReposition();
    onWidthChanged: resizeAndReposition();

    function getImageSrc(){
        var src = "qrc:/images/0"
        switch (vmTrackerItemState){
        case vmTRACKER_ITEM_STATE_CURRENT:
            src = src + parseInt(vmOrderInTracker);
            break;
        case vmTRACKER_ITEM_STATE_DONE:
            src = "qrc:/images/CHECK";
            break;
        case vmTRACKER_ITEM_STATE_NOTDONE:
            src = src + parseInt(vmOrderInTracker) + "d";
            break;
        }
        src = src + ".png"
        return src;
    }


    Image{
       id: number
       source: getImageSrc()
       scale: (vmTrackerItemState == vmTRACKER_ITEM_STATE_DONE)? viewHome.vmScale*0.8 : viewHome.vmScale*0.46
       transformOrigin: Item.TopLeft
       onScaleChanged: trackerItem.resizeAndReposition();
    }

    function resizeAndReposition(){
        var scaling = (vmTrackerItemState == vmTRACKER_ITEM_STATE_DONE)? viewHome.vmScale*0.8 : viewHome.vmScale*0.46
        number.y = (trackerItem.height - number.height*scaling)/2
        number.x = 0
        experiment.x = number.width*scaling + mainWindow.width*vmSpaceBetweenNumberAndTitle;
        //console.log(vmText)
        //console.log("Tracker W" + trackerItem.width  + " End of Experimet at: " + (experiment.x + experiment.width));
        separator.width = (trackerItem.width - (experiment.x + experiment.width))*0.9
        separator.x = experiment.x + experiment.width + trackerItem.width*0.01
    }

    Text{
        id: experiment
        text: vmText
        font.family: vmFont
        font.pixelSize: 14*viewHome.vmScale
        color: (vmTrackerItemState == vmTRACKER_ITEM_STATE_NOTDONE)? "#bcbec0" : "#000000"
        anchors.verticalCenter: parent.verticalCenter
    }

    // The separator
    Rectangle{
        id: separator
        width: mainWindow.width*0.07
        height: mainWindow.height*0.004
        color: "#d5d6d7"
        visible: !vmIsLast
        anchors.verticalCenter: parent.verticalCenter
    }

}
