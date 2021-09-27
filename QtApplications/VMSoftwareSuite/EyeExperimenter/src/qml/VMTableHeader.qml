import QtQuick

Rectangle {
    id: header
    color: "#ffffff"
    border.width: mainWindow.width*0.002
    border.color: "#EDEDEE"
    radius: 4

    property string vmText: ""
    property bool vmIndicatorVisible: false;

    signal headerClicked();

    function indicatorStateIsUp(){
        console.log(indicator.vmIsUp);
        return indicator.vmIsUp;
    }

    function setIndicator(isUp){
        indicator.vmIsUp = isUp;
        indicator.requestPaint()
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: {
            indicator.toggle();
            headerClicked()
        }
    }

    VMUpDownIndicator {
        id: indicator
        anchors.right: parent.right;
        anchors.rightMargin: mainWindow.width*0.005
        anchors.verticalCenter: parent.verticalCenter
        height: mainText.height/2
        width: parent.width*0.03
        visible: vmIndicatorVisible
    }


    Text {
        id: mainText
        text: vmText
        width: parent.width
        font.family: gothamB.name
        font.pixelSize: 15*viewHome.vmScale
        horizontalAlignment: Text.AlignHCenter
        anchors.verticalCenter: parent.verticalCenter
    }

}
