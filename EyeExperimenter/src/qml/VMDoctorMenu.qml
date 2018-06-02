import QtQuick 2.6

Rectangle {
    id: container
    property string vmFont: "Mono"
    property string vmDrName: "DOCTOR"

    color: "#88b2d0"
    height: vmBanner.height

    signal showMenu();

    function doctorNameChanged(){
        var name = "";
        name = loader.getConfigurationString(vmDefines.vmCONFIG_DOCTOR_NAME);
        if (name !== ""){
            name = "Dr. " + name;
        }
        var space = 10
        var leftMargin = 30; var rightMargin = 15;
        vmDrName = name.toUpperCase()
        container.width = doctorsName.width + imgDrAvatar.width + space + leftMargin + rightMargin
        imgDrAvatar.anchors.left = doctorsName.right
        imgDrAvatar.anchors.leftMargin = space - leftMargin
        doctorsName.padding = leftMargin
    }

    Text{
        id: doctorsName
        text: vmDrName
        height: parent.height
        font.family: vmFont
        font.pixelSize: 13
        color: "#ffffff"
        verticalAlignment: Text.AlignVCenter
        Component.onCompleted: {
            doctorNameChanged();
        }
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
