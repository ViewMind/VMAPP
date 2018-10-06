import QtQuick 2.0

Item {

    id: vmPatientEntry
    property string vmDate: ""
    property string vmTime: ""
    property string vmReading: ""
    property string vmBinding: ""
    property string vmReportName: ""
    property int vmItemIndex: 0

    readonly property int vmHeight: 30
    readonly property int vmFontSize: 12

    height: vmHeight
    width: viewReport.columnWidth*3

    MouseArea {
        anchors.fill: parent
        onClicked: {
            vmRepSelected = true;
            reportListView.currentIndex = vmItemIndex
        }
    }

    Rectangle {
        id: dateRect
        color: vmRepSelected? "#4984b3" : "#ffffff"
        border.color: "#EDEDEE"
        border.width: 2
        height: vmHeight
        width: viewReport.columnWidth
        anchors.left: parent.left
        anchors.top: parent.top
        Text {
            font.family: viewHome.gothamR.name
            font.pixelSize: vmFontSize
            text: {
                if (vmDate === "03/06/2010") return "DEMO";
                else return vmDate
            }
            color: vmRepSelected? "#ffffff" : "#000000"
            anchors.centerIn: parent
        }
    }

    Rectangle {
        id: readingRect
        color: vmRepSelected? "#4984b3" : "#ffffff"
        border.color: "#EDEDEE"
        border.width: 2
        height: vmHeight
        width: viewReport.columnWidth
        anchors.left: dateRect.right
        anchors.top: parent.top
        Text {
            font.family: viewHome.gothamR.name
            font.pixelSize: vmFontSize
            text: vmReading
            color: vmRepSelected? "#ffffff" : "#000000"
            anchors.centerIn: parent
        }
    }

    Rectangle {
        id: bindingRect
        color: vmRepSelected? "#4984b3" : "#ffffff"
        border.color: "#EDEDEE"
        border.width: 2
        height: vmHeight
        width: viewReport.columnWidth
        anchors.left: readingRect.right
        anchors.top: parent.top
        Text {
            font.family: viewHome.gothamR.name
            font.pixelSize: vmFontSize
            text: vmBinding
            color: vmRepSelected? "#ffffff" : "#000000"
            anchors.centerIn: parent
        }
    }

}
