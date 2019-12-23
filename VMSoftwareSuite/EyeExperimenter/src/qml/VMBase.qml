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

    property alias vmErrorDiag: errorDialog

    readonly property double vmScale: mainWindow.width/1280;

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
        x: (mainWindow.width-width)/2
        y: (mainWindow.height-height)/2
    }

    id: viewBase
    anchors.fill: parent

    function updateText(){
        doctorMenu.updateText();
        expMenu.updateText();
    }

    // The top banner
    Rectangle {
        id: topBanner
        width: parent.width
        height: mainWindow.height*0.117
        color: "#88b2d0"
        anchors.top: parent.top
        anchors.left: parent.left
    }

    // Top right screen decoration, under the banner
    Image {
        id: topRight
        source: "qrc:/images/ESQUINA_DERECHA_ARRIBA.png"
    }

    // Bottom left screen decoration
    Image {
        id: bottomLeft
        source: "qrc:/images/ESQUINA_IZQUIERDA_ABAJo.png"
    }

    function setMenuVisibility(visible){
        doctorMenu.visible = visible;
    }

    VMDoctorMenu{
        id: doctorMenu
        visible: !isHomePage
        vmFont: gothamM.name
        anchors.verticalCenter: vmBanner.verticalCenter                
        width: mainWindow.width*0.156
        anchors.right: parent.right
        anchors.rightMargin: mainWindow.width*0.0478
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
        anchors.rightMargin: mainWindow.width*0.0478
        onHideMenu: {
            expMenu.z = -10
        }
    }

    Component.onCompleted: {
        // PATCH: For some reason even after scaling the program recognizes the with and size of the border images incorrectly.
        // The math here calculates the position correctly.
        var scale = mainWindow.width/1280;
        var correction = (scale-1)/2

        bottomLeft.scale = scale;
        bottomLeft.x = bottomLeft.width*correction
        bottomLeft.y = mainWindow.height - bottomLeft.height*scale + bottomLeft.height*correction

        topRight.scale = scale
        topRight.x = mainWindow.width - topRight.width*scale + topRight.width*correction
        // ANOTHER PATCH: The value below should be the same as banner height that for some reason is zero when I use it here. I'm giving up and just putting the same value again.
        topRight.y = mainWindow.height*0.117 + topRight.height*correction

    }


}
