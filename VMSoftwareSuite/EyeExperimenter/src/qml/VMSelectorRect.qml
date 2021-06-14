import QtQuick 2.0

Item {

    id: vmSelectorRect

    readonly property int vmHeight: mainWindow.height*0.043
    readonly property int vmFontSize: 12*viewHome.vmScale

    height: vmHeight
    width: selectedRect.width

    MouseArea {
        id: selectArea
        anchors.fill: parent
        onClicked: {
            vmIsSelected = true;
            if (vmChangeToTrigger === 0) qcGraphsView.currentIndex = vmItemIndex
            else qcStudyView.currentIndex = vmItemIndex
        }
    }

    Rectangle {

        id: selectedRect
        color: vmIsSelected? "#4984b3" : "#ffffff"
        border.color: "#EDEDEE"
        border.width: mainWindow.width*0.001
        height: vmHeight
        width: headerStudy.width
        anchors.left: parent.left
        anchors.top: parent.top
        Text {
            id: selectedRectText
            font.family: viewHome.gothamR.name
            font.pixelSize: vmFontSize
            text: vmText
            color: vmIsSelected? "#ffffff" : "#000000"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: mainWindow.width*0.016
        }
    }


}
