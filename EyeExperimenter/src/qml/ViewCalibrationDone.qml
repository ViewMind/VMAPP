import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

VMBase {

    id: viewCalibrationDone
    width: viewCalibrationDone.vmWIDTH
    height: viewCalibrationDone.vmHEIGHT

    function openDiag(){
        viewCalibrationDoneDiag.open()
    }

    readonly property string keysearch: "viewcalibdone_"

    Dialog {

        id: viewCalibrationDoneDiag
        modal: true
        width: 614
        height: 523
        y: (parent.height - height)/2
        x: (parent.width - width)/2
        closePolicy: Popup.NoAutoClose

        // The Drop shadow
        contentItem: Rectangle {
            id: rectDialog
            anchors.fill: parent
            layer.enabled: true
            layer.effect: DropShadow{
                radius: 5
            }
        }

        // The configure settings title
        Text {
            id: viewTitle
            font.pixelSize: 18
            font.family: viewCalibrationDone.gothamB.name
            color: "#297FCA"
            text: loader.getStringForKey(keysearch+"viewTitle");
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 124
        }

        // Loading the image
        Image {
            id: imgCalibration
            source: "qrc:/images/CALIBRACION.png"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: viewTitle.bottom
            anchors.topMargin: 48
        }

        VMButton{
            id: btnContinue
            vmFont: gothamM.name
            vmSize: [180, 50]
            vmText: loader.getStringForKey(keysearch+"btnContinue");
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: imgCalibration.bottom
            anchors.topMargin: 48
            onClicked: {
                viewCalibrationDoneDiag.close();
                swiperControl.currentIndex = swiperControl.vmIndexPresentExperiment
            }
        }

        // Creating the close button
        VMDialogCloseButton {
            id: btnClose
            anchors.top: parent.top
            anchors.topMargin: 22
            anchors.right: parent.right
            anchors.rightMargin: 25
            onClicked: {
                viewCalibrationDoneDiag.close()
                swiperControl.currentIndex = swiperControl.vmIndexPresentExperiment
            }
        }
    }

}
