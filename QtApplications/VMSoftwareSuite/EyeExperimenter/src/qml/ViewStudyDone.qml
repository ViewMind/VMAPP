import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects

VMBase {

    id: viewStudyDone
    width: mainWindow.width
    height: mainWindow.height

    function openDiag(){
        studyDoneDiag.open();
    }

    function closeDiag(){
        studyDoneDiag.close();
    }

    VMDialogBase {

        id: studyDoneDiag;
        width: mainWindow.width*0.48
        height: mainWindow.height*0.87

        // The Title
        Text {
            id: diagTitle
            font.family: viewHome.gothamB.name
            font.pixelSize: 43*viewHome.vmScale
            anchors.top: parent.top
            anchors.topMargin: mainWindow.height*0.128
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#297fca"
            text: loader.getStringForKey("viewexpdone_title");
        }

        // The question.
        Text {
            id: diagMessage
            font.family: viewHome.robotoR.name
            font.pixelSize: 26*viewHome.vmScale
            anchors.top:  diagTitle.bottom
            anchors.centerIn: parent
            color: "#454545"
            text: loader.getStringForKey("viewexpdone_question");
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
                vmText: loader.getStringForKey("viewexpdone_goback");
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
                vmText: loader.getStringForKey("viewexpdone_yes");
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
        text: loader.getStringForKey("viewexpdone_viewTitle");
    }


    Text {
        id: message
        font.pixelSize: 16*viewHome.vmScale
        font.family: robotoR.name
        color: "#297fca"
        anchors.top:  viewTitle.bottom
        anchors.topMargin: mainWindow.height*0.022
        anchors.horizontalCenter: parent.horizontalCenter
        text: loader.getStringForKey("viewexpdone_labelMessage");
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
        vmText: loader.getStringForKey("viewexpdone_btnContinue");
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
