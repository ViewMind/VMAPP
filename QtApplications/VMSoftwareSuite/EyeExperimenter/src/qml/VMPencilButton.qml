import QtQuick
import QtQuick.Controls

Button{

    id: vmPencilButton
    height: mainWindow.height*0.058
    width:  4*height/3;

    scale: vmPencilButton.pressed? 0.8:1

    property double vmStickWidth: width*0.08
    property double vmStickLength: height*0.5

    Behavior on scale{
        NumberAnimation {
            duration: 25
        }
    }

    onWidthChanged: repositionPencil()
    onHeightChanged: repositionPencil()
    hoverEnabled: false

    background: Rectangle {
        id: rectArea
        radius: 6
        border.color: enabled? (vmPencilButton.pressed ? "#4984b3" : "#297FCA") : ("#bcbec0")
        color: enabled? (vmPencilButton.pressed ? "#4984b3" : "#297FCA") : "#bcbec0"
        anchors.fill: parent
    }
    contentItem: Item{
        id: pencil
        anchors.fill: parent
        Image {
            id: pencilImage
            source: "qrc:/images/edit_icon.png"
            scale: viewHome.vmScale
            transformOrigin: Item.TopLeft
            onScaleChanged: vmPencilButton.repositionPencil();
        }
    }

    function repositionPencil() {
        pencilImage.x = (vmPencilButton.width - pencilImage.width*viewHome.vmScale)/2
        pencilImage.y = (vmPencilButton.height - pencilImage.height*viewHome.vmScale)/2
    }

}
