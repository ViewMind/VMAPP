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
        width: mainWindow.width*0.48
        height: mainWindow.height*0.87
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
            anchors.topMargin: mainWindow.height*0.032
            anchors.right: parent.right
            anchors.rightMargin: mainWindow.width*0.02
        }

        // The Title
        Text {
            id: diagTitle
            font.family: viewHome.gothamB.name
            font.pixelSize: 43*viewHome.vmScale
            anchors.top: parent.top
            anchors.topMargin: mainWindow.height*0.128
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#297fca"
            text: loader.getStringForKey(keysearch+"title");
        }

        // The question.
        Text {
            id: diagMessage
            font.family: viewHome.robotoR.name
            font.pixelSize: 26*viewHome.vmScale
            anchors.top:  diagTitle.bottom
            anchors.centerIn: parent
            color: "#454545"
            text: loader.getStringForKey(keysearch+"question");
        }

        Row{
            id: buttonRow
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: mainWindow.height*0.058
            spacing: mainWindow.width*0.023

            VMButton{
                id: btnBack
                height: mainWindow.height*0.072
                width: mainWindow.width*0.141
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
                height: mainWindow.height*0.072
                width: mainWindow.width*0.141
                vmText: loader.getStringForKey(keysearch+"yes");
                vmFont: viewHome.gothamM.name
                onClicked: {
                    studyDoneDiag.close();
                    swiperControl.currentIndex = swiperControl.vmIndexPatientList;
                }
            }
        }

    }

    Text {
        id: viewTitle
        font.family: gothamB.name
        font.pixelSize: 43*viewHome.vmScale
        anchors.top:  vmBanner.bottom
        anchors.topMargin: mainWindow.height*0.067
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#3fa2f7"
        text: loader.getStringForKey(keysearch+"viewTitle");
    }


    Text {
        id: message
        font.pixelSize: 16*viewHome.vmScale
        font.family: robotoR.name
        color: "#297fca"
        anchors.top:  viewTitle.bottom
        anchors.topMargin: mainWindow.height*0.022
        anchors.horizontalCenter: parent.horizontalCenter
        text: loader.getStringForKey(keysearch+"labelMessage");
    }

    // The head graph
    Image {
        id: headDesign
        source: "qrc:/images/ILUSTRACION.png"
        anchors.horizontalCenter: parent.horizontalCenter
        //        anchors.top: message.bottom
        //        anchors.topMargin: mainWindow.height*0.022
        //        scale: viewHome.vmScale
    }


    VMButton{
        id: btnContinue
        vmText: loader.getStringForKey(keysearch+"btnContinue");
        vmFont: gothamM.name
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: mainWindow.height*0.072
        width: mainWindow.width*0.156
        onClicked: {
            swiperControl.currentIndex = swiperControl.vmIndexPatientList;
        }
    }

    Component.onCompleted: {
        // PATCH: For some reason even after scaling the program recognizes the with and size of the border images incorrectly.
        // It makes absolutely no sense. Furthermore I CANNOT, BY ANY MEANS FIGURE OUT HOW TO GET THE Y POSITION OF THE TEXT
        // which lies right above the image, so I have to compute it as well.
        // The math here calculates the position correctly.
        var scale = mainWindow.width/1280;
        var correction = (scale-1)/2
        headDesign.scale = scale;
        headDesign.y = correction*headDesign.height + mainWindow.height*0.328

    }

}
