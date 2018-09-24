import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

VMBase {

    id: viewWaitFor
    width: viewCalibrationStart.vmWIDTH
    height: viewCalibrationStart.vmHEIGHT

    readonly property string keysearch: "viewwaitfor_"
    property string vmTitle: "TITLE";
    property string vmMessage: "Message";


//    Connections{
//        target: flowControl
//        onSslTransactionFinished: {
//            viewWaitFor.closeDiag();

//            // Saving the report first.
//            flowControl.saveReport();

//            // Now doing this.
//            // viewResults.fillFieldsFromReportInfo();
//            swiperControl.currentIndex = swiperControl.vmIndexStudyStart;
//        }
//    }


    function openDiag(){
        waitForDiag.open();
    }

    function closeDiag(){
        waitForDiag.close();
    }

    Dialog {

        id: waitForDiag;
        modal: true
        width: 614
        height: 600
        y: (parent.height - height)/2
        x: (parent.width - width)/2
        closePolicy: Popup.CloseOnEscape

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

        // The instruction text
        Text {
            id: diagTitle
            font.family: viewHome.gothamB.name
            font.pixelSize: 43
            anchors.top: parent.top
            anchors.topMargin: 88
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#297fca"
            text: vmTitle
        }

        // The instruction text
        Text {
            id: diagMessage
            font.family: viewHome.robotoR.name
            font.pixelSize: 13
            anchors.top:  diagTitle.bottom
            anchors.topMargin: 26
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#297fca"
            text: vmMessage
        }

        AnimatedImage {
            id: slideAnimation
            source: "qrc:/images/LOADING.gif"
            anchors.top: diagMessage.bottom
            anchors.topMargin: 30
            x: (parent.width - slideAnimation.width)/2;
        }

    }


}
