import QtQuick 2.6

Rectangle {
    id: container
    property string vmFont: "Mono"
    property string vmDrName: ""

    color: "#88b2d0"
    height: vmBanner.height

    signal showMenu();

    Text{
        id: doctorsName
        text: vmDrName
        height: parent.height
        font.family: vmFont
        font.pixelSize: 13
        color: "#ffffff"
        verticalAlignment: Text.AlignVCenter
        Component.onCompleted: {
            // This is required to correctly calculate the item's dimensions.
            var name = "Dr. " + loader.getDoctorsName();
            var space = 10
            var leftMargin = 30; var rightMargin = 15;
            vmDrName = name.toUpperCase()
            container.width = doctorsName.width + imgDrAvatar.width + space + leftMargin + rightMargin
            imgDrAvatar.anchors.left = doctorsName.right
            imgDrAvatar.anchors.leftMargin = space - leftMargin
            doctorsName.padding = leftMargin
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
