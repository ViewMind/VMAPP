import QtQuick 2.0

Item {

    id: vmPatientEntry

    property int vmPatientColWidth: headerPatient.width
    property int vmPatientCreationDateWidth: headerCreationDate.width
    property int vmPatientIDWidth: headerID.width

    readonly property int vmHeight: mainWindow.height*0.043
    readonly property int vmFontSize: 12*viewHome.vmScale

    height: vmHeight
    width: vmPatientColWidth + vmPatientCreationDateWidth + vmPatientIDWidth

    MouseArea {
        id: selectArea
        anchors.fill: parent
        onClicked: {
            vmIsSelected = true;
            patientListView.currentIndex = vmItemIndex
        }
    }

    Rectangle {

        id: patientRect
        color: vmIsSelected? "#4984b3" : "#ffffff"
        border.color: "#EDEDEE"
        border.width: mainWindow.width*0.002
        height: vmHeight
        width: vmPatientColWidth
        anchors.left: parent.left
        anchors.top: parent.top
//        MouseArea {
//            id: selectArea
//            anchors.fill: parent
//            onClicked: {
//                vmIsSelected = true;
//                patientListView.currentIndex = vmItemIndex
//            }
//        }
        Text {
            id: patientText
            font.family: viewHome.gothamR.name
            font.pixelSize: vmFontSize
            text: name + " " + lastname
            color: vmIsSelected? "#ffffff" : "#000000"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: mainWindow.width*0.016
        }
    }


    Rectangle {
        id: patientIDRect
        color: vmIsSelected? "#4984b3" : "#ffffff"
        border.color: "#EDEDEE"
        border.width: mainWindow.width*0.002
        height: vmHeight
        width: vmPatientIDWidth
        anchors.left: patientRect.right
        anchors.top: parent.top
        visible: true
        Text {
            id: idText
            color: vmIsSelected? "#ffffff" : "#000000"
            font.family: viewHome.gothamR.name
            font.pixelSize: vmFontSize
            text: supplied_institution_id
            anchors.centerIn: parent
        }
    }

    Rectangle {
        id: creationDateRect
        color: vmIsSelected? "#4984b3" : "#ffffff"
        border.color: "#EDEDEE"
        border.width: mainWindow.width*0.002
        height: vmHeight
        width: vmPatientCreationDateWidth
        anchors.left: patientIDRect.right;
        anchors.top: parent.top
        visible: true
        Text {
            id: creationDateText
            color: vmIsSelected? "#ffffff" : "#000000"
            font.family: viewHome.gothamR.name
            font.pixelSize: vmFontSize
            text: creation_date
            anchors.centerIn: parent
        }
    }

}
