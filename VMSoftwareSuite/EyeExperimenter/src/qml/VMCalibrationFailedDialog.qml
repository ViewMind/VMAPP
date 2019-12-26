import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

Dialog {

    id: viewCalibrationFailedDiag
    modal: true
    width: mainWindow.width*0.479
    height: mainWindow.height*0.758
    y: (parent.height - height)/2
    x: (parent.width - width)/2
    closePolicy: Popup.NoAutoClose

    readonly property string keysearch: "viewcalibfailed_"

    property bool vmLeftEyePassed: false
    property bool vmRightEyePassed: false

    // The Drop shadow
    contentItem: Rectangle {
        id: rectDialog
        anchors.fill: parent
        layer.enabled: true
        layer.effect: DropShadow{
            radius: 5
        }
    }

    Text {
        id: viewTitle
        font.pixelSize: 43*viewHome.vmScale
        font.family: viewCalibrationStart.gothamB.name
        color: "#297FCA"
        text: loader.getStringForKey(keysearch+"viewTitle");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: mainWindow.height*0.124
    }

    Text {
        id: viewSubTitle
        font.pixelSize: 13*viewHome.vmScale
        font.family: viewCalibrationStart.robotoR.name
        color: "#297FCA"
        text: loader.getStringForKey(keysearch+"viewSubTitle");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: viewTitle.bottom
        anchors.topMargin: mainWindow.height*0.040
    }

    Column{
        id: colEyeReport
        //anchors.horizontalCenter: parent.horizontalCenter
        anchors.centerIn: parent
        spacing: mainWindow.height*0.04
//        anchors.top: viewSubTitle.bottom
//        anchors.topMargin: mainWindow.height*0.05

        Text {
            id: leftEyeText
            font.pixelSize: 16*viewHome.vmScale
            font.family: viewCalibrationStart.gothamB.name
            color: {
                if (!vmLeftEyePassed) return "#ca2026";
                else return "#5da655";
            }
            text: {
                var res = loader.getStringForKey(keysearch+"leftEye") + ": ";
                if (vmLeftEyePassed) res = res + loader.getStringForKey(keysearch+"msgOK")
                else res = res + loader.getStringForKey(keysearch+"msgFailed")
                return res
            }
        }

        Text {
            id: rightEyeText
            font.pixelSize: 16*viewHome.vmScale
            font.family: viewCalibrationStart.gothamB.name
            color: {
                if (!vmRightEyePassed) return "#ca2026";
                else return "#5da655";
            }
            text: {
                var res = loader.getStringForKey(keysearch+"rightEye") + ": ";
                if (vmRightEyePassed) res = res + loader.getStringForKey(keysearch+"msgOK")
                else res = res + loader.getStringForKey(keysearch+"msgFailed")
                return res
            }
        }

    }

    VMButton{
        id: btnRetry
        vmFont: gothamM.name
        vmSize: [mainWindow.width*0.148, mainWindow.height*0.072]
        vmText: loader.getStringForKey(keysearch+"btnRetry");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: mainWindow.height*0.061
        onClicked: {
            viewCalibrationFailedDiag.close();
        }
    }

}
