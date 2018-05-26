import QtQuick 2.6
import QtQuick.Controls 2.3

Rectangle {

    readonly property alias vmBanner: topBanner
    readonly property alias gothamM: gothamMedium
    readonly property alias gothamB: gothamBold
    readonly property alias gothamR: gothamRegular
    readonly property alias robotoM: robottoMed
    readonly property alias robotoB: robottoBold
    readonly property alias robotoR: robottoReg

    // The Fonts
    FontLoader {
        id: gothamMedium
        source: "qrc:/fonts/Gotham-Medium.otf"
    }

    FontLoader {
        id: gothamBold
        source: "qrc:/fonts/Gotham-Bold.otf"
    }

    FontLoader {
        id: gothamRegular
        source: "qrc:/fonts/Gotham-Book.otf"
    }

    FontLoader{
        id: robottoReg
        source: "qrc:/fonts/Roboto-Regular.ttf"
    }

    FontLoader{
        id: robottoMed
        source: "qrc:/fonts/Roboto-Medium.ttf"
    }

    FontLoader{
        id: robottoBold
        source: "qrc:/fonts/Roboto-Bold.ttf"
    }

    id: viewBase
    width: 1280
    height: 768
    anchors.fill: parent

    // The top banner
    Rectangle {
        id: topBanner
        width: parent.width
        height: 81
        color: "#88b2d0"
        anchors.top: parent.top
        anchors.left: parent.left
    }

    // Top right screen decoration, under the banner
    Image {
        id: topRight
        source: "qrc:/images/ESQUINA_DERECHA_ARRIBA.png"
        anchors.top: topBanner.bottom
        anchors.right: parent.right
    }

    // Bottom left screen decoration
    Image {
        id: bottomLeft
        source: "qrc:/images/ESQUINA_IZQUIERDA_ABAJo.png"
        anchors.bottom: parent.bottom
        anchors.left: parent.left
    }

}
