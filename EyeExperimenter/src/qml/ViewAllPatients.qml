import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0


Dialog {

    property string vmPatientName: "Eustaquio Señor Paciente";
    readonly property string keybase: "viewallreports_"
    readonly property int dialogHeight: 650
    readonly property int dialogWidth: 800
    readonly property int repTableWidth: dialogWidth*0.8
    readonly property int repTableHeight: dialogHeight*0.5
    readonly property int columnWidth: repTableWidth/2

    id: viewAllPatients;
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
        id: allPatientList;
    }

    onOpened: {
        loadPatients();
    }


    // Loads the list model with the patient information
    function loadPatients() {

        allPatientList.clear();
        loader.prepareAllPatientIteration();

        while (true){
            var list = loader.nextInAllPatientIteration();
            if (list.length === 0) break;
            allPatientList.append({   "vmPatientName": list[2],
                                      "vmPatientUID": list[3],
                                      "vmDoctorName": list[0],
                                      "vmDoctorUID": list[1],
                                      "vmIsSelected": false
                                  });
        }
        allPatientListView.currentIndex = -1;
    }

    // Creating the close button
    VMDialogCloseButton {
        id: btnClose
        anchors.top: parent.top
        anchors.topMargin: 22
        anchors.right: parent.right
        anchors.rightMargin: 25
        onClicked: {
            viewAllPatients.close();
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
        text:  loader.getStringForKey(keybase+"title");
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
        //text: loader.getStringForKey(keybase+"diagViewRepSubTitle");
    }

    // The table header.
    Row {
        id: tableHeader
        anchors.top: diagViewRepSubTitle.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        height: 30
        Rectangle {
            id: headerPatient
            color: "#adadad"
            border.width: 2
            border.color: "#EDEDEE"
            radius: 4
            width: columnWidth
            height: parent.height
            Text {
                id: patientText
                text: loader.getStringForKey(keybase+"patient_col");
                width: parent.width
                font.family: gothamB.name
                font.pixelSize: 15
                horizontalAlignment: Text.AlignHCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle {
            id: headerDoctor
            color: "#adadad"
            border.width: 2
            border.color: "#EDEDEE"
            radius: 4
            width: columnWidth
            height: parent.height
            Text {
                id: readingText
                text: loader.getStringForKey(keybase+"doctor_col");
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
                id: allPatientListView
                anchors.fill: parent
                model: allPatientList
                delegate: VMPatientDoctorEntry {
                    vmItemIndex: index
                }
                onCurrentIndexChanged: {
                    for (var i = 0; i < model.count; i++){
                        if (i != currentIndex){
                            allPatientList.setProperty(i,"vmIsSelected",false);
                        }
                    }
                }
            }
        }
    }

    VMButton{
        id: diagBtnView
        height: 50
        vmText: loader.getStringForKey(keybase+"view");
        vmFont: viewHome.gothamM.name
        enabled: allPatientListView.currentIndex !== -1
        anchors.top: tableBackground.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: {
            viewAllPatients.close();
            var dir = loader.getWorkingDirectory();
            var selected = allPatientList.get(allPatientListView.currentIndex);
            viewShowReports.vmPatientDirectory = dir + "/" + selected.vmDoctorUID  + "/" + selected.vmPatientUID;
            viewShowReports.vmPatientName = selected.vmPatientName;
            swiperControl.currentIndex = swiperControl.vmIndexShowReports;
        }
    }


}



