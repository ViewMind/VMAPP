import QtQuick 2.6
import QtQuick.Controls 2.3

VMBase {

    id: viewHome
    width: viewHome.vmWIDTH
    height: viewHome.vmHEIGHT

    readonly property string keysearch: "viewhome_"

    // ViewMind Logo
    Image {
        id: logo
        source: "qrc:/images/LOGO.png"
        anchors{
            top: parent.top
            topMargin: 29
            left: parent.left
            leftMargin: 61
        }
    }

    // The head graph
    Image {
        id: headDesign
        source: "qrc:/images/ILUSTRACION.png"
        anchors{
            left: parent.left
            leftMargin: 145
            bottom: parent.bottom
            bottomMargin: 191
        }
    }

    // The configurations dialog
    ViewSettings{
        id: viewSettings
        x: 333
        y: 94
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
            font.family: gothamM.name
            font.pixelSize: 13
            text: loader.getStringForKey(keysearch+"btnConfSettings");
            color: "#88b2d0"
        }
        anchors{
            right: parent.right
            rightMargin: 63
            top: parent.top
            topMargin: 19
        }
        onClicked: {
            viewSettings.open()
        }
    }

    Text {
        id: slideTitle
        font.family: gothamB.name
        font.pixelSize: 43
        anchors.top:  vmBanner.bottom
        anchors.topMargin: 212
        anchors.left: headDesign.right
        anchors.leftMargin: 75
        color: "#297fca"
        text: loader.getStringForKey(keysearch+"slideTitle");
    }

    // The explanation
    Text{
        id: description
        textFormat: Text.RichText
        font.pixelSize: 16
        font.family: robotoR.name
        color: "#297fca"
        text: loader.getStringForKey(keysearch+"description");
        anchors{
            top: slideTitle.bottom
            topMargin: 40
            left: headDesign.right
            leftMargin: 75
        }
    }

    VMButton{
        id: btnGetStarted
        vmText: loader.getStringForKey(keysearch+"btnGetStarted");
        vmFont: gothamM.name
        anchors{
            left: headDesign.right
            leftMargin: 75
            top: description.bottom
            topMargin: 53
        }
        onClicked: {
            swiperControl.currentIndex = swiperControl.vmIndexPatientReg;
        }
    }
}
