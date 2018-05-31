import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

VMBase {

    id: viewCalibrationStart
    width: viewCalibrationStart.vmWIDTH
    height: viewCalibrationStart.vmHEIGHT

    function openDiag(){
        viewCalibrationStartDiag.open()
    }

    readonly property string keysearch: "viewcalibstart_"

    Dialog {

        id: viewCalibrationStartDiag
        modal: true
        width: 614
        height: 523
        y: 139
        x: (parent.width - width)/2
        closePolicy: Popup.CloseOnEscape

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
            font.pixelSize: 43
            font.family: viewCalibrationStart.gothamB.name
            color: "#297FCA"
            text: loader.getStringForKey(keysearch+"viewTitle");
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 86
        }

        // The configure settings title
        Text {
            id: viewSubTitle
            font.pixelSize: 13
            font.family: viewCalibrationStart.robotoR.name
            color: "#297FCA"
            text: loader.getStringForKey(keysearch+"viewSubTitle");
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: viewTitle.bottom
            anchors.topMargin: 28
        }

        // Loading the image
        Image {
            id: imgEye
            source: "qrc:/images/OJO.png"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: viewSubTitle.bottom
            anchors.topMargin: 37
        }

        VMButton{
            id: btnStart
            vmFont: gothamM.name
            vmSize: [180, 50]
            vmText: loader.getStringForKey(keysearch+"btnStart");
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: imgEye.bottom
            anchors.topMargin: 42
            onClicked: {
                viewCalibrationStartDiag.close()
                swiperControl.currentIndex = swiperControl.vmIndexCalibrationDone;
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
                viewCalibrationStartDiag.close()
                swiperControl.currentIndex = swiperControl.vmIndexPatientReg
            }
        }
    }

}
