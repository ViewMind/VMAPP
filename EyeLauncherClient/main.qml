import QtQuick 2.9
import QtQuick.Window 2.2

Window {    
    visible: true
    width: 960
    height: 480
    title: qsTr("EyeExperimenter")

    Connections {
        target: loader
        onChangeMessage: {
            message.setMessage(msg);
        }
    }

    // The Fonts
    FontLoader {
        id: gothamMedium
        source: "qrc:/../EyeExperimenter/src/fonts/Gotham-Medium.otf"
    }

    FontLoader {
        id: gothamBold
        source: "qrc:/../EyeExperimenter/src/fonts/Gotham-Bold.otf"
    }

    FontLoader {
        id: gothamRegular
        source: "qrc:/../EyeExperimenter/src/fonts/Gotham-Book.otf"
    }

    FontLoader{
        id: robottoReg
        source: "qrc:/../EyeExperimenter/src/fonts/Roboto-Regular.ttf"
    }

    FontLoader{
        id: robottoMed
        source: "qrc:/../EyeExperimenter/src/fonts/Roboto-Medium.ttf"
    }

    FontLoader{
        id: robottoBold
        source: "qrc:/../EyeExperimenter/src/fonts/Roboto-Bold.ttf"
    }

    // The top banner
    Rectangle {
        id: topBanner
        width: parent.width
        height: 60
        color: "#88b2d0"
        anchors.top: parent.top
        anchors.left: parent.left

        // ViewMind Logo
        Image {
            id: logo
            source: "qrc:/../EyeExperimenter/src/images/LOGO.png"
            anchors{
                verticalCenter: topBanner.verticalCenter
                left: parent.left
                leftMargin: 30
            }
        }

    }

    // ViewMind Logo
    Image {
        id: decoratingImage
        source: "qrc:/../EyeExperimenter/src/images/ILUSTRACION.png"
        anchors{
            top: topBanner.bottom
            topMargin: 30
            left: parent.left
            leftMargin: 30
        }
    }

    Text {
        id: slideTitle
        font.family: gothamBold.name
        font.pixelSize: 43
        anchors.top:  topBanner.bottom
        anchors.topMargin: 50
        anchors.horizontalCenter: loadingImage.horizontalCenter
        color: "#297fca"
        text: "EyeExperimenter";
    }

    // The explanation
    Text{
        function setMessage(msg){
            message.text = msg;
        }
        id: message
        font.pixelSize: 16
        font.family: robottoReg.name
        color: "#297fca"
        text: "";
        anchors{
            top: slideTitle.bottom
            topMargin: 40
            horizontalCenter: loadingImage.horizontalCenter
        }
    }

    // Loading animations
    AnimatedImage {
        id: loadingImage
        source: "qrc:/../EyeExperimenter/src/images/LOADING.gif"
        anchors.top: message.bottom
        anchors.topMargin: -40
        z: -1
        x: (decoratingImage.width + parent.width - loadingImage.width)/2
    }

    Component.onCompleted: {
        loader.checkForUpdates();
    }

}
