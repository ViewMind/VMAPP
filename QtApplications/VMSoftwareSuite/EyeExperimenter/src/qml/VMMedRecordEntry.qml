import QtQuick 2.0

Item {

    id: vmMedicationEntry

    readonly property int vmHeight: mainWindow.height*0.043
    readonly property int vmFontSize: 12*viewHome.vmScale

    height: vmHeight
    width: headerMain.width

    Rectangle {
        id: medicationRect
        color: vmIsSelected? "#4984b3" : "#ffffff"
        border.color: "#EDEDEE"
        border.width: mainWindow.width*0.002
        height: vmHeight
        width: headerMain.width
        anchors.left: parent.left
        anchors.top: parent.top
        MouseArea {
            id: selectArea
            anchors.fill: parent
            onClicked: {
                vmIsSelected = true;
                medRecListView.currentIndex = vmItemIndex;
            }
        }
        Text {
            id: dateText
            font.family: viewHome.gothamR.name
            font.pixelSize: vmFontSize
            text: vmDate
            color: vmIsSelected? "#ffffff" : "#000000"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: mainWindow.width*0.016
        }
    }

}
