import QtQuick

Item {

    id: vmPatientEntry

    property int vmPatientColWidth: headerPatient.width
    property int vmDoctorColWidth: headerDoctor.width
    property int vmItemIndex: 0

    readonly property int vmHeight: mainWindow.height*0.043
    readonly property int vmFontSize: 12*viewHome.vmScale

    height: vmHeight
    width: vmPatientColWidth + vmDoctorColWidth

    Rectangle {

        id: patientRect
        color: vmIsSelected? "#4984b3" : "#ffffff"
        border.color: "#EDEDEE"
        border.width: mainWindow.width*0.002
        height: vmHeight
        width: vmPatientColWidth
        anchors.left: parent.left
        anchors.top: parent.top
        MouseArea {
            id: selectArea
            anchors.fill: parent
            onClicked: {
                vmIsSelected = true;
                allPatientListView.currentIndex = vmItemIndex
            }
        }
        Text {
            id: patientText
            font.family: viewHome.gothamR.name
            font.pixelSize: vmFontSize
            text: vmPatientName + " (" + vmPatientUID  + ")"
            color: vmIsSelected? "#ffffff" : "#000000"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: mainWindow.width*0.016
        }
    }


    Rectangle {

        id: doctorRect
        color: vmIsSelected? "#4984b3" : "#ffffff"
        border.color: "#EDEDEE"
        border.width: mainWindow.width*0.002
        height: vmHeight
        width: vmDoctorColWidth
        anchors.left: patientRect.right
        anchors.top: parent.top
        MouseArea {
            id: selectAreaDoc
            anchors.fill: parent
            onClicked: {
                vmIsSelected = true;
                allPatientListView.currentIndex = vmItemIndex
            }
        }
        Text {
            id: doctorText
            font.family: viewHome.gothamR.name
            font.pixelSize: vmFontSize
            text: vmDoctorName + " (" + vmDoctorUID  + ")"
            color: vmIsSelected? "#ffffff" : "#000000"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: mainWindow.width*0.016
        }
    }


}
