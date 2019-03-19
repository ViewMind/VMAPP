import QtQuick 2.0
import QtQuick.Controls 2.2

Rectangle {

    // Requires:
    // vmDisplaText
    // vmIndexInList

    property bool isBeingHovered: false

    border.width: 1;
    border.color: "#eeeeee"
    color: isBeingHovered? "#6298c5" : "#ffffff"

    signal selectionMade(int indexInList, string displayText);
    signal archiveItem(int indexInList);

    MouseArea {
        id: selectArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            selectionMade(vmIndexInList,vmDisplayText)
        }
        onEntered:  {
            isBeingHovered = true;
        }
        onExited: {
            isBeingHovered = false;
        }
    }

    Text {
        id: datInfo
        font.family: viewHome.robotoR.name
        font.pixelSize: 11
        text: vmDisplayText
        color: isBeingHovered? "#ffffff" : "#000000"
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 10
    }

    Button{
        id: archiveButton
        height: 40;
        width:  20;
        scale: archiveButton.pressed? 0.8:1
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 20
        hoverEnabled: true;

        onHoveredChanged: {
           if (hovered){
               isBeingHovered = true;
           }
        }

        onClicked: {
            archiveItem(vmIndexInList);
        }

        Behavior on scale{
            NumberAnimation {
                duration: 25
            }
        }

        background: Rectangle {
            id: rectArea
            border.width: 0
            color: "#00ffffff"
            anchors.fill: parent
        }
        contentItem: Item{
            id: archiveImg
            anchors.fill: parent
            Image {
                id: archivePic
                scale: archiveButton.width/archivePic.width
                source: "qrc:/images/black_folder.png"
                anchors.centerIn: parent
            }
        }
    }
}
