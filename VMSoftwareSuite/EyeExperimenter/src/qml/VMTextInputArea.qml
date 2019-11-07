import QtQuick 2.0
import QtQuick.Controls 1.4

Rectangle{

    property string vmErrorMsg: ""

    function getText(){
        return inpTextArea.text;
    }

    function setText(text){
        inpTextArea.text = text
    }

    id: textBoundRect
    radius: 2
    border.width: 1
    border.color: "#297fca"
    TextArea {
        id: inpTextArea
        anchors.centerIn: parent
        width: textBoundRect.width - 2*textBoundRect.border.width
        height: textBoundRect.height - 2*textBoundRect.border.width
        font.family: viewHome.robotoR.name
        font.pixelSize: 12*viewHome.vmScale
        textColor: "#58595b"
        frameVisible: false
    }
    Text{
        id: errorMsg
        text: vmErrorMsg
        color:  "#ca2026"
        font.family: viewHome.robotoR.name
        font.pixelSize: 12*viewHome.vmScale
        anchors.left: parent.left
        anchors.top: parent.bottom
        anchors.topMargin: 5
        visible: (vmErrorMsg !== "")
    }
}
