import QtQuick

Rectangle {

    id: bulletCard

    property string vmTitleText: ""
    property bool vmNoLeftMargin: false

    color: "#ffffff"
    border.color: "#bcbec0"
    radius: width*0.02

    ListModel {
        id: modelBulletList
    }

    function setItemState(index,enabled){
        modelBulletList.setProperty(index,"vmEnabled",enabled)
    }

    function setList (list,onlyText) {

        if (onlyText === undefined) onlyText = false;

        modelBulletList.clear();
        for (var i = 0; i < list.length; i++){
            modelBulletList.append({ "vmEnabled" : true, "vmText" : list[i], "visible": true, "vmTextOnly": onlyText });
        }
    }

    Text {
        id: title
        font.family: gothamM.name
        font.pixelSize: 12*viewHome.vmScale
        text: vmTitleText
        color: "#297fca"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.leftMargin: mainWindow.width*0.01;
        anchors.topMargin: mainWindow.height*0.01;
    }

    ListView {
        id: bulletList
        model: modelBulletList
        width: parent.width
        height: parent.height-title.height
        anchors.top: title.bottom
        anchors.left: parent.left
        anchors.leftMargin: vmNoLeftMargin? mainWindow.height*0.01 : mainWindow.height*0.03;
        anchors.topMargin: mainWindow.height*0.01;
        delegate: VMBulletedText {
            width: {
                // PATCH: This is done simply to avoid the warning "TypeError: Cannot read property 'width' of null". I don't know why it happens. It changes nothing.
                if (bulletCard === null) return 0
                else return bulletCard.width*0.8;
            }
            //height: parent.height/(modelBulletList.count+1);
            height: mainWindow.height*0.05;
        }
    }

}
