import QtQuick 2.6

Rectangle {
    id: container
    property string vmFont: "Mono"

    color: "#88b2d0"
    height: vmBanner.height

    signal showMenu();

    property string vmDrName: "NO NAME";

    function updateText(){
        vmDrName = loader.getConfigurationString(vmDefines.vmCONFIG_DOCTOR_NAME);
    }

    Text{
        id: doctorsName
        text: vmDrName;
        height: parent.height
        font.family: vmFont
        font.pixelSize: 13
        color: "#ffffff"
        verticalAlignment: Text.AlignVCenter
        anchors.left: imgDrAvatar.right;
        anchors.leftMargin: 20
    }

    Image {
        id: imgDrAvatar
        source: "qrc:/images/AVATAR_MEDICO.png"
        anchors.verticalCenter: parent.verticalCenter
    }

    MouseArea{
        anchors.fill: parent
        onClicked: {
            showMenu()
        }
    }
}
