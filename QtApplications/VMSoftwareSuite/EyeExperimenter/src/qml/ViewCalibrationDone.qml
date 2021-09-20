import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects

VMBase {

    id: viewCalibrationDone
    width: viewCalibrationDone.vmWIDTH
    height: viewCalibrationDone.vmHEIGHT

    function openDiag(){
        viewCalibrationDoneDiag.open()
    }

    VMDialogBase {

        id: viewCalibrationDoneDiag
        vmNoCloseButton: true
        width: mainWindow.width*0.479
        height: mainWindow.height*0.758
        y: (parent.height - height)/2
        x: (parent.width - width)/2

        onWidthChanged: repositionImage();
        onHeightChanged: repositionImage();

        // The configure settings title
        Text {
            id: viewTitle
            font.pixelSize: 18*viewHome.vmScale
            font.family: viewCalibrationDone.gothamB.name
            color: "#297FCA"
            text: loader.getStringForKey("viewcalibdone_viewTitle");
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: mainWindow.height*0.179
        }

        // Loading the image
        Image {
            id: imgCalibration
            source: "qrc:/images/CALIBRACION.png"
            scale: viewHome.vmScale
            transformOrigin: Item.TopLeft
            onScaleChanged: {
                viewCalibrationDoneDiag.repositionImage();
            }
        }

        function repositionImage(){
            imgCalibration.x = (viewCalibrationDoneDiag.width - imgCalibration.width*viewHome.vmScale)/2;
            imgCalibration.y = viewTitle.y + viewTitle.height + mainWindow.height*0.069;
        }

        VMButton{
            id: btnContinue
            vmFont: gothamM.name
            vmSize: [mainWindow.width*0.140, mainWindow.height*0.072]
            vmText: loader.getStringForKey("viewcalibdone_btnContinue");
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: mainWindow.height*0.061
            onClicked: {
                viewCalibrationDoneDiag.close();
                swiperControl.currentIndex = swiperControl.vmIndexPresentExperiment
            }
        }

    }

}
