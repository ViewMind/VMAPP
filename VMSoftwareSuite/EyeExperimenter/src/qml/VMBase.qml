import QtQuick 2.6
import QtQuick.Controls 2.3

Rectangle {

    property bool isHomePage: false
    readonly property alias vmBanner: topBanner
    readonly property alias gothamM: gothamMedium
    readonly property alias gothamB: gothamBold
    readonly property alias gothamR: gothamRegular
    readonly property alias robotoM: robottoMed
    readonly property alias robotoB: robottoBold
    readonly property alias robotoR: robottoReg
    readonly property alias vmDefines: masterDefines

    readonly property int vmExpIndexReading:    1;
    readonly property int vmExpIndexBindingUC:  2;
    readonly property int vmExpIndexBindingBC:  3;
    readonly property int vmExpIndexFielding:   4;

    readonly property int vmWIDTH: 1280
    readonly property int vmHEIGHT: 720

    property alias vmErrorDiag: errorDialog

    signal backButtonPressed();

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

    // Scripts generated constants
    VMDefines {
        id: masterDefines;
    }

    VMErrorDialog{
        id: errorDialog
        x: 332
        y: (parent.height-height)/2
    }

    id: viewBase
    width: 1280
    height: 768
    anchors.fill: parent

    function updateText(){
        doctorMenu.updateText();
        expMenu.updateText();
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

    function setMenuVisibility(visible){
        doctorMenu.visible = visible;
    }

    VMDoctorMenu{
        id: doctorMenu
        visible: !isHomePage
        vmFont: gothamM.name
        anchors.verticalCenter: vmBanner.verticalCenter        
        width: 200
        anchors.right: parent.right
        anchors.rightMargin: 33
        onShowMenu: {
            expMenu.z = 10;
        }
    }

    VMDoctorMenuExpanded{
        id: expMenu
        vmFont: gothamM.name
        width: doctorMenu.width
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: 33
        onHideMenu: {
            expMenu.z = -10
        }
    }

}