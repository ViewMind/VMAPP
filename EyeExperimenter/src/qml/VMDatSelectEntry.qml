import QtQuick 2.0

Item {

    // Need to fill:
    // vmIsSelected
    // vmDisplayText
    // vmIndexInList

    id: vmDatSelectEntry

    readonly property int vmHeight: 30
    readonly property int vmFontSize: 12

    height: vmHeight
    width: parent.width

    signal selected(int indexInList);

    Rectangle {

        id: patientRect
        color: vmIsSelected? "#4984b3" : "#ffffff"
        border.color: "#EDEDEE"
        border.width: 2
        height: vmHeight
        width: vmDatSelectEntry.width
        anchors.left: parent.left
        anchors.top: parent.top
        MouseArea {
            id: selectArea
            anchors.fill: parent
            onClicked: {
                vmIsSelected = true;
                vmDatSelectEntry.selected(vmIndexInList);
            }
        }
        Text {
            id: patientText
            font.family: viewHome.gothamR.name
            font.pixelSize: vmFontSize
            text: vmDisplayText
            color: vmIsSelected? "#ffffff" : "#000000"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 20
        }
    }

}
