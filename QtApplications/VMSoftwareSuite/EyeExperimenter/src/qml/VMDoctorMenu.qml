import QtQuick 2.6

Rectangle {
    id: container
    property string vmFont: "Mono"

    color: "#88b2d0"
    height: vmBanner.height

    signal showMenu();

    property string vmDrName: "NO NAME";

    function updateText(){
        var evaluator = loader.getCurrentEvaluatorInfo();
        vmDrName = evaluator.lastname + ",\n" + evaluator.name
    }

    Text{
        id: doctorsName
        text: vmDrName;
        height: parent.height
        font.family: vmFont
        font.pixelSize: 13*viewHome.vmScale
        color: "#ffffff"
        verticalAlignment: Text.AlignVCenter
        anchors.left: imgDrAvatar.right;
        anchors.leftMargin: mainWindow.width*0.016
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
