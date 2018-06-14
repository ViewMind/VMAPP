import QtQuick 2.0

Rectangle {

    id: trackerItem
    height: 71
    color: "#EDEDEE"

    readonly property int vmTRACKER_ITEM_STATE_DONE:    0
    readonly property int vmTRACKER_ITEM_STATE_NOTDONE: 1
    readonly property int vmTRACKER_ITEM_STATE_CURRENT: 2

    property int vmOrderInTracker: 1
    property int vmTrackerItemState: vmTRACKER_ITEM_STATE_NOTDONE
    property bool vmIsLast: false
    property string vmFont: "Mono"
    property string vmText: "Text"

    function getWidth(){
        var ans = number.width + experiment.width
        if (!vmIsLast) ans = ans + separator.width;
        return ans;
    }

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
       scale: (vmTrackerItemState == vmTRACKER_ITEM_STATE_DONE)? 0.8 : 0.46
       anchors.verticalCenter: parent.verticalCenter
    }

    Text{
        id: experiment
        text: vmText
        font.family: vmFont
        font.pixelSize: 14
        color: (vmTrackerItemState == vmTRACKER_ITEM_STATE_NOTDONE)? "#bcbec0" : "#000000"
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: number.right
        anchors.leftMargin: (vmTrackerItemState == vmTRACKER_ITEM_STATE_DONE)? 3 : -5
    }

    // The separator
    Rectangle{
        id: separator
        width: 90
        height: 3
        color: "#d5d6d7"
        visible: !vmIsLast
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: experiment.right
        anchors.leftMargin: 18
    }

}
