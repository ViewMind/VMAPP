import QtQuick 2.0

Item {

    id: vmMedicationEntry

    readonly property int vmHeight: 30
    readonly property int vmFontSize: 12*viewHome.vmScale

    height: vmHeight
    width: headerMedication.width + headerMedication.width + btnRemove.width

    signal removeItem(var itemIndex)

    Rectangle {

        id: medicationRect
        color: vmIsSelected? "#4984b3" : "#ffffff"
        border.color: "#EDEDEE"
        border.width: 2
        height: vmHeight
        width: headerMedication.width
        anchors.left: parent.left
        anchors.top: parent.top
        MouseArea {
            id: selectArea
            anchors.fill: parent
            onClicked: {
                vmIsSelected = true;
                medicationListView.currentIndex = vmItemIndex;
            }
        }
        Text {
            id: medicationText
            font.family: viewHome.gothamR.name
            font.pixelSize: vmFontSize
            text: vmMedication
            color: vmIsSelected? "#ffffff" : "#000000"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 20
        }
    }


    Rectangle {

        id: dosisRect
        color: vmIsSelected? "#4984b3" : "#ffffff"
        border.color: "#EDEDEE"
        border.width: 2
        height: vmHeight
        width: headerDose.width
        anchors.left: medicationRect.right
        anchors.top: parent.top
        MouseArea {
            id: selectAreaDoc
            anchors.fill: parent
            onClicked: {
                vmIsSelected = true;
                medicationListView.currentIndex = vmItemIndex;
            }
        }
        Text {
            id: dosisText
            font.family: viewHome.gothamR.name
            font.pixelSize: vmFontSize
            text: vmDose
            color: vmIsSelected? "#ffffff" : "#000000"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 20
        }
    }

    VMPlusButton{
        id: btnRemove
        height: vmHeight;
        vmMakeDeleteButton: true
        anchors.left: dosisRect.right
        onClicked: {
            removeItem(vmItemIndex)
        }
    }


}
