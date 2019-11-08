import QtQuick 2.0

Item {

    id: vmEvaluationEntry

    readonly property int vmHeight: mainWindow.height*0.043
    readonly property int vmFontSize: 12*viewHome.vmScale

    height: vmHeight
    width: headerEvaluation.width + headerResults.width + btnRemove.width

    signal removeItem(var itemIndex)

    Rectangle {

        id: evaluationRect
        color: vmIsSelected? "#4984b3" : "#ffffff"
        border.color: "#EDEDEE"
        border.width: mainWindow.width*0.002
        height: vmHeight
        width: headerMedication.width
        anchors.left: parent.left
        anchors.top: parent.top
        MouseArea {
            id: selectArea
            anchors.fill: parent
            onClicked: {
                vmIsSelected = true;
                evaluationListView.currentIndex = vmItemIndex;
            }
        }
        Text {
            id: evaluationText
            font.family: viewHome.gothamR.name
            font.pixelSize: vmFontSize
            text: vmEvaluation
            color: vmIsSelected? "#ffffff" : "#000000"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: mainWindow.width*0.016
        }
    }


    Rectangle {

        id: resultRect
        color: vmIsSelected? "#4984b3" : "#ffffff"
        border.color: "#EDEDEE"
        border.width: mainWindow.width*0.002
        height: vmHeight
        width: headerResults.width
        anchors.left: evaluationRect.right
        anchors.top: parent.top
        MouseArea {
            id: selectAreaDoc
            anchors.fill: parent
            onClicked: {
                vmIsSelected = true;
                evaluationListView.currentIndex = vmItemIndex;
            }
        }
        Text {
            id: resultText
            font.family: viewHome.gothamR.name
            font.pixelSize: vmFontSize
            text: vmResult
            color: vmIsSelected? "#ffffff" : "#000000"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: mainWindow.width*0.016
        }
    }

    VMPlusButton{
        id: btnRemove
        height: vmHeight;
        vmMakeDeleteButton: true
        anchors.left: resultRect.right
        onClicked: {
            removeItem(vmItemIndex)
        }
    }


}
