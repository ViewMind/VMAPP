import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0


Dialog {

    property string vmPatientName: "Eustaquio Señor Paciente";
    readonly property string keybase: "viewshowreports_"
    readonly property int dialogHeight: 650
    readonly property int dialogWidth: 800
    readonly property int repTableWidth: dialogWidth*0.8
    readonly property int repTableHeight: dialogHeight*0.4
    readonly property int columnWidth: repTableWidth/3

    id: viewReport;
    modal: true
    width: dialogWidth
    height: dialogHeight
    y: (parent.height - height)/2
    x: (parent.width - width)/2
    closePolicy: Popup.NoAutoClose

    contentItem: Rectangle {
        id: rectReportDialog
        anchors.fill: parent
        layer.enabled: true
        layer.effect: DropShadow{
            radius: 5
        }
    }

    ListModel{
        id: reportList;
    }

    onOpened: {
        loadReportsForPatient();
    }


    // Loads the list model with the patient information
    function loadReportsForPatient() {

        // Clearing the current model.
        reportList.clear()
        flowControl.prepareForReportListIteration();

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

    // Creating the close button
    VMDialogCloseButton {
        id: btnClose
        anchors.top: parent.top
        anchors.topMargin: 22
        anchors.right: parent.right
        anchors.rightMargin: 25
        onClicked: {
            viewReport.close();
        }
    }


    // The instruction text
    Text {
        id: diagViewRepTitle
        font.family: viewHome.gothamB.name
        font.pixelSize: 43
        anchors.top: parent.top
        anchors.topMargin: 60
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
        text: viewReport.vmPatientName;
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


    VMButton{
        id: diagBtnView
        height: 50
        vmText: loader.getStringForKey(keybase+"diagBtnView");
        vmFont: viewHome.gothamM.name
        enabled: reportListView.currentIndex !== -1
        anchors.top: tableBackground.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: {
            viewReport.close();
            flowControl.setReportIndex(reportListView.currentIndex);
            swiperControl.currentIndex = swiperControl.vmIndexResults;
        }
    }


}
