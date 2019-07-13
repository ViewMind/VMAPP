import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0


VMBase {


    readonly property string keybase: "viewshowreports_"
    readonly property int repTableWidth: width*0.7
    readonly property int repTableHeight: height*0.4
    readonly property int columnWidth: repTableWidth/4

    // The two properties that must be set when checking this.
    property string vmPatientDirectory: ""
    property string vmPatientName: "";

    id: viewReport;
    ListModel{
        id: reportList;
    }

    // Loads the list model with the patient information
    function loadReportsForPatient() {

        // CODE FOR DEBUG ONLY
        vmPatientName = "Some Patient";
        vmPatientDirectory = "C:/Users/Viewmind/Documents/viewmind_projects/EyeExperimenter/exe32/viewmind_etdata/0_0000_P0000"
        // END DEBUG CODE

        // Clearing the current model.
        reportList.clear()
        // console.log("Patient name: " + vmPatientName + " and directory: " + vmPatientDirectory);
        flowControl.prepareForReportListIteration(vmPatientDirectory);

        var done = false;

        while (!done){
            var map = flowControl.nextReportInList();
            done = !("repname" in map);
            if (!done){
                reportList.append(map);
            }
        }
        reportListView.currentIndex = -1;
    }

    // The instruction text
    Text {
        id: diagViewRepTitle
        font.family: viewHome.gothamB.name
        font.pixelSize: 43
        anchors.top: vmBanner.bottom
        anchors.topMargin: 30
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#297fca"
        text:  loader.getStringForKey(keybase+"diagViewRepTitle");
    }

    // The instruction text
    Text {
        id: diagViewRepSubTitle
        font.family: viewHome.robotoR.name
        font.pixelSize: 13
        anchors.top:  diagViewRepTitle.bottom
        anchors.topMargin: 26
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#cfcfcf"
        text: loader.getStringForKey(keybase+"diagViewRepSubTitle");
    }

    // The patient name
    Text {
        id: diagPatientName
        font.family: viewHome.robotoR.name
        font.pixelSize: 15
        anchors.top:  diagViewRepSubTitle.bottom
        anchors.topMargin: 26
        anchors.left: tableHeader.left
        color: "#5d5d5d"
        text: {
            if (uimap.getStructure() === "S")  return loader.getStringForKey(keybase+"diagViewRepSubject") + ": " +  viewReport.vmPatientName;
            else if (uimap.getStructure() === "P") return loader.getStringForKey(keybase+"diagViewRepPatient") + ": " + viewReport.vmPatientName;
        }
    }

    // The table header.
    Row {
        id: tableHeader
        anchors.top: diagPatientName.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        height: 30
        Rectangle {
            id: headerDate
            color: "#adadad"
            border.width: 2
            border.color: "#EDEDEE"
            radius: 4
            width: columnWidth
            height: parent.height
            Text {
                id: patientText
                text: loader.getStringForKey(keybase+"diagTabDate");
                width: parent.width
                font.family: gothamB.name
                font.pixelSize: 15
                horizontalAlignment: Text.AlignHCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle {
            id: headerReading
            color: "#adadad"
            border.width: 2
            border.color: "#EDEDEE"
            radius: 4
            width: columnWidth
            height: parent.height
            Text {
                id: readingText
                text: loader.getStringForKey(keybase+"diagTabReading");
                width: parent.width
                font.family: gothamB.name
                font.pixelSize: 15
                horizontalAlignment: Text.AlignHCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle {
            id: headerColors
            color: "#adadad"
            border.width: 2
            border.color: "#EDEDEE"
            radius: 4
            width: columnWidth
            height: parent.height
            Text {
                id: colorsText
                text: loader.getStringForKey(keybase+"diagTabBinding");
                width: parent.width
                font.family: gothamB.name
                font.pixelSize: 15
                horizontalAlignment: Text.AlignHCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle {
            id: headerStatus
            color: "#adadad"
            border.width: 2
            border.color: "#EDEDEE"
            radius: 4
            width: columnWidth
            height: parent.height
            Text {
                id: statusText
                text: loader.getStringForKey(keybase+"diagTabStatus");
                width: parent.width
                font.family: gothamB.name
                font.pixelSize: 15
                horizontalAlignment: Text.AlignHCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

    Rectangle {
        id: tableBackground
        color: "#ffffff"
        border.width: 2
        border.color: "#EDEDEE"
        radius: 4
        anchors.top: tableHeader.bottom
        anchors.left: tableHeader.left
        width: repTableWidth
        height: repTableHeight

        ScrollView {
            id: tableArea
            anchors.fill: parent
            clip: true
            ListView {
                id: reportListView
                anchors.fill: parent
                model: reportList
                delegate: VMReportEntry {
                    vmReportName: repname
                    vmDate: date
                    vmReading: reading
                    vmBinding: binding
                    vmItemIndex: index
                    vmIsUpToDate: uptodate
                    vmFileList: filelist
                    onReprocessReport: {
                        console.log("Requested reprocessing of report: " + reportName)
                    }
                }
                onCurrentIndexChanged: {
                    for (var i = 0; i < model.count; i++){
                        if (i != currentIndex){
                            reportList.setProperty(i,"vmRepSelected",false);
                        }
                    }
                }
            }
        }
    }


    Row{

        anchors.top: tableBackground.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 30

        VMButton{
            id: diagBtnBack
            height: 50
            vmText: loader.getStringForKey(keybase+"diagBtnBack");
            vmFont: viewHome.gothamM.name
            vmInvertColors: true
            onClicked: {
                swiperControl.currentIndex = swiperControl.vmIndexPatientList;
            }
        }

        VMButton{
            id: diagBtnView
            height: 50
            vmText: loader.getStringForKey(keybase+"diagBtnView");
            vmFont: viewHome.gothamM.name
            enabled: reportListView.currentIndex !== -1
            onClicked: {
                flowControl.setReportIndex(reportListView.currentIndex);
                swiperControl.currentIndex = swiperControl.vmIndexResults;
            }
        }

    }

}
