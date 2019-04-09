import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

VMBase {

    id: viewStudyDone
    width: viewCalibrationStart.vmWIDTH
    height: viewCalibrationStart.vmHEIGHT

    readonly property string keysearch: "viewexpdone_"

    function openDiag(){
        studyDoneDiag.open();
    }

    function closeDiag(){
        studyDoneDiag.close();
    }

    Dialog {

        id: studyDoneDiag;
        modal: true
        width: 614
        height: 600
        y: (parent.height - height)/2
        x: (parent.width - width)/2
        closePolicy: Popup.NoAutoClose

        contentItem: Rectangle {
            id: rectDialog
            anchors.fill: parent
            layer.enabled: true
            layer.effect: DropShadow{
                radius: 5
            }
        }

        VMDialogCloseButton {
            id: btnClose
            anchors.top: parent.top
            anchors.topMargin: 22
            anchors.right: parent.right
            anchors.rightMargin: 25
        }

        // The Title
        Text {
            id: diagTitle
            font.family: viewHome.gothamB.name
            font.pixelSize: 43
            anchors.top: parent.top
            anchors.topMargin: 88
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#297fca"
            text: loader.getStringForKey(keysearch+"title");
        }

        // The question.
        Text {
            id: diagMessage
            font.family: viewHome.robotoR.name
            font.pixelSize: 26
            anchors.top:  diagTitle.bottom
            anchors.centerIn: parent
            color: "#454545"
            text: loader.getStringForKey(keysearch+"question");
        }

        Row{
            id: buttonRow
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 40
            spacing: 30

            VMButton{
                id: btnBack
                height: 50
                width: 180
                vmText: loader.getStringForKey(keysearch+"goback");
                vmFont: viewHome.gothamM.name
                vmInvertColors: true
                onClicked: {
                    studyDoneDiag.close();
                    swiperControl.currentIndex = swiperControl.vmIndexPatientList;
                }
            }

            VMButton{
                id: btnYes
                height: 50
                width: 180
                vmText: loader.getStringForKey(keysearch+"yes");
                vmFont: viewHome.gothamM.name
                onClicked: {
                    studyDoneDiag.close();
                    swiperControl.currentIndex = swiperControl.vmIndexPatientList;
                    if (loader.getConfigurationBoolean(vmDefines.vmCONFIG_DEMO_MODE) || loader.getConfigurationBoolean(vmDefines.vmCONFIG_USE_MOUSE)){
                        viewPatList.startDemoTransaction();
                    }
                    else{
                        // One is passed as a parameter to force the used of the files in the current run files.
                        //viewPatList.requestReportToServer(1);
                        viewPatList.openDatSelectionDialog();
                    }

                }
            }
        }

    }

    Text {
        id: viewTitle
        font.family: gothamB.name
        font.pixelSize: 43
        anchors.top:  vmBanner.bottom
        anchors.topMargin: 46
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#3fa2f7"
        text: loader.getStringForKey(keysearch+"viewTitle");
    }

    // The head graph
    Image {
        id: headDesign
        source: "qrc:/images/ILUSTRACION.png"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: message.bottom
        anchors.topMargin: 15
    }

    Text {
        id: message
        font.pixelSize: 16
        font.family: robotoR.name
        color: "#297fca"
        anchors.top:  viewTitle.bottom
        anchors.topMargin: 15
        anchors.horizontalCenter: parent.horizontalCenter
        text: loader.getStringForKey(keysearch+"labelMessage");
    }

    VMButton{
        id: btnContinue
        vmText: loader.getStringForKey(keysearch+"btnContinue");
        vmFont: gothamM.name
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: headDesign.bottom
        anchors.topMargin: 30
        onClicked: {
            if (loader.isDoctorValidated(-1)){
                openDiag();                
            }
            else swiperControl.currentIndex = swiperControl.vmIndexPatientList;
        }
    }

}
