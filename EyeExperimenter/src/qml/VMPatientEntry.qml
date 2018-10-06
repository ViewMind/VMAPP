import QtQuick 2.0

Item {

    id: vmPatientEntry

    property string vmPatientName: "Placeholder Text"
    property string vmPatientUID: ""
    property int vmPatientColWidth: headerPatient.width
    property int vmStatusColWidth: headerStatus.width
    property int vmItemIndex: 0

    readonly property int vmHeight: 30
    readonly property int vmFontSize: 12

    height: vmHeight
    width: vmPatientColWidth + vmStatusColWidth

    signal fetchReport(int index)

    Rectangle {

        id: patientRect
        color: vmIsSelected? "#4984b3" : "#ffffff"
        border.color: "#EDEDEE"
        border.width: 2
        height: vmHeight
        width: vmPatientColWidth
        anchors.left: parent.left
        anchors.top: parent.top
        MouseArea {
            id: selectArea
            anchors.fill: parent
            onClicked: {
                vmIsSelected = true;
                patientListView.currentIndex = vmItemIndex
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
            anchors.leftMargin: 20
        }
    }

    Rectangle {
        id: statusRect
        color: "#ffffff"
        border.color: "#EDEDEE"
        border.width: 2
        height: vmHeight
        width: vmStatusColWidth
        anchors.left: patientRect.right
        anchors.top: parent.top
        visible: vmIsOk
        Text {
            id: statusText
            font.family: viewHome.gothamR.name
            font.pixelSize: vmFontSize
            text: loader.getStringForKey("viewpatientlist_statusOK")
            anchors.centerIn: parent
        }
    }

    VMButton {
        id: btnFetchReport
        visible: !vmIsOk
        vmText: loader.getStringForKey("viewpatientlist_statusRepPending")
        height: vmHeight
        width: vmStatusColWidth
        vmFont: viewHome.gothamM.name
        anchors.left: patientRect.right
        anchors.top: parent.top
        onClicked: {
            vmPatientEntry.fetchReport(vmItemIndex);
        }
    }
}
