import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.0

VMBase {

    id: viewFinalizedStudies

    readonly property real vmTableWidth: 0.70*mainWindow.width
    readonly property real vmTableHeight: 0.5*mainWindow.height
    readonly property string keybase: "viewfinishedstudies_"

    ListModel {
        id: studiesList
    }

    // Loads the list model with the patient information
    function loadEvaluatorStudies() {

        //console.log("Loading evaluation studies");

        // Getting the filtered list of patient map with all the info.
        var studyList = loader.getReportsForLoggedEvaluator();

        // Clearing the current model.
        studiesList.clear()

        var index = 0;
        for (var key in studyList) {
            var map = studyList[key];
            map["vmIsSelected"] = false;
            map["vmItemIndex"] = index;
            index++;
            //console.log("Addign study with key " + key);
            studiesList.append(map);
        }

        studyListView.currentIndex = -1;
    }


    // The Doctor Information Title and subtitle
    Text {
        id: title
        font.pixelSize: 43*viewHome.vmScale
        font.family: gothamB.name
        color: "#297FCA"
        text: loader.getStringForKey(keybase+"title");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: vmBanner.bottom
        anchors.topMargin: mainWindow.height*0.043
    }

    Text {
        id: subTitle
        font.pixelSize: 11*viewHome.vmScale
        font.family: gothamR.name
        color: "#cfcfcf"
        text: loader.getStringForKey(keybase+"subtitle");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: title.bottom
        anchors.topMargin: mainWindow.height*0.016
    }


    // The table header.
    Row {
        id: tableHeader
        anchors.top: subTitle.bottom
        anchors.topMargin: mainWindow.height*0.014
        anchors.horizontalCenter: parent.horizontalCenter
        height: mainWindow.height*0.043

        Rectangle {
            id: headerDate
            color: "#ffffff"
            border.width: mainWindow.width*0.002
            border.color: "#EDEDEE"
            radius: 4
            width: vmTableWidth/4
            height: parent.height
            Text {
                id: dateText
                text: loader.getStringForKey(keybase+"headerDate");
                width: parent.width
                font.family: gothamB.name
                font.pixelSize: 15*viewHome.vmScale
                horizontalAlignment: Text.AlignHCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle {
            id: headerSubject
            color: "#ffffff"
            border.width: mainWindow.width*0.002
            border.color: "#EDEDEE"
            radius: 4
            width: headerDate.width
            height: parent.height
            Text {
                id: subjectText
                text: loader.getStringForKey(keybase+"headerSubject");
                width: parent.width
                font.family: gothamB.name
                font.pixelSize: 15*viewHome.vmScale
                horizontalAlignment: Text.AlignHCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle {
            id: headerStudy
            color: "#ffffff"
            border.width: mainWindow.width*0.002
            border.color: "#EDEDEE"
            radius: 4
            width: headerDate.width
            height: parent.height
            Text {
                id: studyText
                text: loader.getStringForKey(keybase+"headerStudy");
                width: parent.width
                font.family: gothamB.name
                font.pixelSize: 15*viewHome.vmScale
                horizontalAlignment: Text.AlignHCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle {
            id: headerDoctor
            color: "#ffffff"
            border.width: mainWindow.width*0.002
            border.color: "#EDEDEE"
            radius: 4
            width: headerDate.width
            height: parent.height
            Text {
                id: assignedDoctor
                text: loader.getStringForKey(keybase+"headerDoctor");
                width: parent.width
                font.family: gothamB.name
                font.pixelSize: 15*viewHome.vmScale
                horizontalAlignment: Text.AlignHCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }

    }

    // The table where each of the patient entries will be put. and its background
    Rectangle {
        id: tableBackground
        color: "#ffffff"
        border.width: mainWindow.width*0.002
        border.color: "#EDEDEE"
        radius: 4
        anchors.top: tableHeader.bottom
        anchors.left: tableHeader.left
        width: vmTableWidth
        height: vmTableHeight - tableHeader.height

        ScrollView {
            id: tableArea
            anchors.fill: parent
            clip: true
            ListView {
                id: studyListView
                anchors.fill: parent
                model: studiesList
                delegate: VMFinalizedStudyEntry {
                }
                onCurrentIndexChanged: {
                    for (var i = 0; i < model.count; i++){
                        if (i !== currentIndex){
                            studiesList.setProperty(i,"vmIsSelected",false)
                        }
                    }
                }
            }
        }
    }

    // Buttons
    Row{
        id: buttonRow
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: mainWindow.height*0.058
        spacing: mainWindow.width*0.15

        VMButton{
            id: btnBack
            height: mainWindow.height*0.072
            vmText: loader.getStringForKey(keybase+"btnBack");
            vmFont: viewHome.gothamM.name
            vmInvertColors: true
            onClicked: {
                swiperControl.currentIndex = swiperControl.vmIndexPatientList;
            }
        }

        VMButton{
            id: btnQC
            height: mainWindow.height*0.072
            vmText: loader.getStringForKey(keybase+"btnQC");
            vmFont: viewHome.gothamM.name
            enabled: studyListView.currentIndex !== -1
            onClicked: {

            }
        }

        VMButton{
            id: btnSend
            height: mainWindow.height*0.072
            vmText: loader.getStringForKey(keybase+"btnSend");
            vmFont: viewHome.gothamM.name
            enabled: studyListView.currentIndex !== -1
            onClicked: {

            }
        }
    }

}
