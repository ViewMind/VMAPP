import QtQuick 2.6

Item {

    id: container
    property string vmFont: "Mono"

    Text{
        property string drname: ""

        id: doctorsName
        text: drname
        font.family: vmFont
        font.pixelSize: 13
        color: "#ffffff"
        elide: Text.ElideRight
        Component.onCompleted: {
            // This is required to correctly calculate the item's dimensions.
            var name = "Dr. " + loader.getDoctorsName();
            var margin = 10
            drname = name.toUpperCase()
            container.width = doctorsName.width + imgDrAvatar.width + margin
            imgDrAvatar.anchors.left = doctorsName.right
            imgDrAvatar.anchors.leftMargin = margin
            container.height = doctorsName.height;
        }
    }

    Image {
        id: imgDrAvatar
        source: "qrc:/images/AVATAR_MEDICO.png"
        anchors.verticalCenter: doctorsName.verticalCenter
    }
}
