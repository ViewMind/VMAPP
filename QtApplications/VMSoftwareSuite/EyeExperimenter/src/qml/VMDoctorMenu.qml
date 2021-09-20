import QtQuick

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

    onWidthChanged: repositionImage();
    onHeightChanged: repositionImage();

    Text{
        id: doctorsName
        text: vmDrName;
        height: parent.height
        font.family: vmFont
        font.pixelSize: 13*viewHome.vmScale
        color: "#ffffff"
        verticalAlignment: Text.AlignVCenter
    }

    Image {
        id: imgDrAvatar
        source: "qrc:/images/AVATAR_MEDICO.png"
        scale: viewHome.vmScale
        transformOrigin: Item.TopLeft
        onScaleChanged: {
            repositionImage();
        }
    }

    MouseArea{
        anchors.fill: parent
        onClicked: {
            showMenu()
        }
    }

    function repositionImage(){
        imgDrAvatar.x = 0;
        imgDrAvatar.y = (container.height - imgDrAvatar.height*viewHome.vmScale)/2;
        doctorsName.x = mainWindow.width*0.016 + imgDrAvatar.width*viewHome.vmScale;
    }
}
