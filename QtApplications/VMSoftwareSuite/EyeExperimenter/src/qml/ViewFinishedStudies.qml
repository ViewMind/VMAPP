import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.0

VMBase {

    id: viewFinalizedStudies

    readonly property real vmTableWidth: 0.70*mainWindow.width
    readonly property real vmTableHeight: 0.5*mainWindow.height
    readonly property string keybase: "viewfinishedstudies_"

    Connections {
        target: loader
        onQualityControlDone: {
            // Close the connection dialog and open the user selection dialog.
            processingQCDialog.close();

            if (loader.qualityControlFailed()){
                var titleMsg = viewHome.getErrorTitleAndMessage("error_qc_verification_failed");
                vmErrorDiag.vmErrorMessage = titleMsg[1];
                vmErrorDiag.vmErrorTitle = titleMsg[0];
                vmErrorDiag.open();
                return;
            }

            swiperControl.currentIndex = swiperControl.vmIndexViewQC;
        }
    }

    Dialog {

        property string vmTitle: loader.getStringForKey(keybase+"waitTitle")
        property string vmMessage: loader.getStringForKey(keybase+"waitSubTitle")

        id: processingQCDialog;
        modal: true
        width: mainWindow.width*0.48
        height: mainWindow.height*0.87
        y: (parent.height - height)/2
        x: (parent.width - width)/2
        closePolicy: Popup.NoAutoClose

        onWidthChanged: {
            processingQCDialog.repositionSlideAnimation()
        }

        onHeightChanged: {
            processingQCDialog.repositionSlideAnimation();
        }

        contentItem: Rectangle {
            id: rectConnectionDialog
            anchors.fill: parent
            layer.enabled: true
            layer.effect: DropShadow{
                radius: 5
            }
        }

        // The instruction text
        Text {
            id: diagConnectionTitle
            font.family: viewHome.gothamB.name
            font.pixelSize: 43*viewHome.vmScale
            anchors.top: parent.top
            anchors.topMargin: mainWindow.height*0.128
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#297fca"
            text: processingQCDialog.vmTitle
        }


        // The instruction text
        Text {
            id: diagMessage
            font.family: viewHome.robotoR.name
            font.pixelSize: 13*viewHome.vmScale
            anchors.top:  diagConnectionTitle.bottom
            anchors.topMargin: mainWindow.height*0.038
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#297fca"
            text:  processingQCDialog.vmMessage;
            z: 2 // Sometimes the border of the image covers the text. This fixes it.
            onScaleChanged: {
                processingQCDialog.repositionSlideAnimation()
            }
        }

        AnimatedImage {
            id: slideAnimation
            source: "qrc:/images/LOADING.gif"
            scale: viewHome.vmScale
            visible: true
            onScaleChanged: {
                processingQCDialog.repositionSlideAnimation()
            }
        }

        function repositionSlideAnimation(){
            slideAnimation.y = (processingQCDialog.height - slideAnimation.height*vmScale)/2
            slideAnimation.x = (processingQCDialog.width - slideAnimation.width*viewHome.vmScale)/2
        }

    }


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
            id: btnSend
            height: mainWindow.height*0.072
            vmText: loader.getStringForKey(keybase+"btnQC");
            vmFont: viewHome.gothamM.name
            enabled: studyListView.currentIndex !== -1
            onClicked: {
                loader.setCurrentStudyFileForQC(studiesList.get(studyListView.currentIndex).file_path);
                //swiperControl.currentIndex = swiperControl.vmIndexViewQC;
                processingQCDialog.open();
            }
        }
    }

}
