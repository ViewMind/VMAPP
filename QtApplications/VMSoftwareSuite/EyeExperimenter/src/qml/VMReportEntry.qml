import QtQuick 2.0

Item {

    id: vmReportEntry
    property string vmDate: ""
    property string vmTime: ""
    property string vmReportName: ""
    property bool vmIsUpToDate: true
    property string vmFileList: ""
    property int vmItemIndex: 0


    readonly property int vmHeight: mainWindow.height*0.043
    readonly property int vmFontSize: 12*viewHome.vmScale

    height: vmHeight
    width: viewReport.columnWidth*2

    signal reprocessReport(var reportName)

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
        border.width: mainWindow.width*0.002
        height: vmHeight
        width: viewReport.columnWidth
        anchors.left: parent.left
        anchors.top: parent.top
        Text {
            font.family: viewHome.gothamR.name
            font.pixelSize: vmFontSize
            text: {
                if (vmDate === viewResults.vmDEMO_DATE) return "DEMO";
                else return vmDate
            }
            color: vmRepSelected? "#ffffff" : "#000000"
            anchors.centerIn: parent
        }
    }


    Rectangle {
        id: statusRectText
        color: vmRepSelected? "#4984b3" : "#ffffff"
        border.color: "#EDEDEE"
        border.width: mainWindow.width*0.002
        height: vmHeight
        width: viewReport.columnWidth
        anchors.left: dateRect.right
        anchors.top: parent.top
        visible: (vmIsUpToDate || (vmDate === viewResults.vmDEMO_DATE))
        Text {
            font.family: viewHome.gothamR.name
            font.pixelSize: vmFontSize
            text: "OK"
            color: vmRepSelected? "#ffffff" : "#000000"
            anchors.centerIn: parent
        }
    }


    VMButton {
        id: btnFetchReport
        visible: !statusRectText.visible
        vmText: loader.getStringForKey("viewshowreports_diagReprocess");
        height: vmHeight
        width: viewReport.columnWidth
        vmFont: viewHome.gothamM.name
        anchors.left: dateRect.right
        anchors.top: parent.top
        onClicked: {
            vmReportEntry.reprocessReport(vmReportName)
        }
    }


}