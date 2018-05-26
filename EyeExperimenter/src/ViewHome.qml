import QtQuick 2.6
import QtQuick.Controls 2.3

Rectangle {

    id: viewHome
    width: 1280
    height: 768

    // The Fonts
//    FontLoader {
//        id: ghtcMedium
//        source: "qrc:/FONTS/GothaHTFMed"
//    }

    FontLoader{
        id: robottoReg
        source: "qrc:/fonts/Roboto-Regular.ttf"
    }

    FontLoader{
        id: robottoMed
        source: "qrc:/fonts/Roboto-Medium.ttf"
    }


    // The top banner
    Rectangle {
        id: topBanner
        width: parent.width
        height: 81
        color: "#88b2d0"
        anchors.top: parent.top
        anchors.left: parent.left
    }

    // The Images
    Image {
        id: logo
        source: "qrc:/images/LOGO.png"
        x: 61
        y: 29
    }

    Image {
        id: headDesign
        source: "qrc:/images/ILUSTRACION.png"
        x: 145
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 191
    }

    Image {
        id: topRight
        source: "qrc:/images/ESQUINA_DERECHA_ARRIBA.png"
        anchors.top: topBanner.bottom
        anchors.right: parent.right
    }

    Image {
        id: bottomLeft
        source: "qrc:/images/ESQUINA_IZQUIERDA_ABAJo.png"
        anchors.bottom: parent.bottom
        anchors.left: parent.left
    }


    // The configure settings button
    Button {
        id: btnConfSettings
        background: Rectangle {
            id: btnConfSettingsRect
            radius: 3
            color: "#ffffff"
            width: 178
            height: 43
        }
        contentItem: Text{
            anchors.centerIn: btnConfSettingsRect
            //font.family: ghtcMedium.name
            font.pointSize: 13
            text: qsTr("CONFIGURE SETTINGS")
            color: "#88b2d0"
        }
        y: 19
        x: parent.width - 63 - btnConfSettingsRect.width
        onClicked: {
            console.log("Los roboto son: " + robottoMed.name + " y " + robottoReg.name)
        }
    }


    // Title
    Text {
        id: slideTitle
        //font.family:
        font.pointSize: 43
        anchors.top:  topBanner.bottom
        anchors.topMargin: 212
        anchors.left: headDesign.right
        anchors.leftMargin: 75
        color: "#297fca"
        text: "Start your evaluation"
    }

    // The explanation
    Text{
        id: description
        textFormat: Text.RichText
        font.pointSize: 16
        font.family: robottoReg.name
        color: "#297fca"
        text: "Using high-precision Eye-Tracking, in <b> a 10-minutes, <br>
non-invasive and standarized test</b>, we are able to detect <br>
Mild Cognitive Impairment and to predict the onset of <br>
Alzheimer's Disease and other dementias, years before <br>
symptoms are visible."
        anchors{
            top: slideTitle.bottom
            topMargin: 40
            left: headDesign.right
            leftMargin: 75
        }
    }

//    Text{
//        id: test
//        textFormat: Text.RichText
//        color: "#297fca"
//        text: "<span style = 'font-family: Roboto Medium; font-size: 15px'>Text in on font.</span> And in another"
//        anchors{
//            top: description.bottom
//            topMargin: 40
//            left: headDesign.right
//            leftMargin: 75
//        }
//    }

    Button{
        id: btnGetStarted
        background: Rectangle {
            id: rectArea
            radius: 3
            color: "#297fca"
            width: 131
            height: 50
        }
        contentItem: Text{
            anchors.centerIn: rectArea
            //font.family: ghtcMedium.name
            font.pointSize: 13
            text: qsTr("GET STARTED")
            color: "#ffffff"
        }
        anchors{
            left: headDesign.right
            leftMargin: 75
            top: description.bottom
            topMargin: 53
        }

        onClicked: {
        }
    }


}
