import QtQuick

Rectangle {

    id: bulletedText
    color: "#ffffff"
    border.width: 0

    Rectangle {
        id: bullet
        height: parent.height*0.5
        width: height
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        radius: width/2
        visible: !vmTextOnly
        color: vmEnabled? "#297fca" : "#d5e7f6"
    }

    Text {
        id: text
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: vmTextOnly? parent.left : bullet.right
        anchors.leftMargin: parent.width*0.05
        font.family: robotoM.name
        font.pixelSize: 10*viewHome.vmScale
        text: vmText
        color: vmEnabled ? "#4a4d4f" : "#bcbec0"
    }

}
