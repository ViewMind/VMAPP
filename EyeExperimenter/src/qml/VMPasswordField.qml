import QtQuick 2.6

Item {

    property string vmFont: "Mono"
    property string vmLabelText: "Some text ..."
    property string vmErrorMsg: ""

    height: 40
    z: 0

    Rectangle {
        id: lineEditRect
        anchors.fill: parent
        color: "#ffffff"
        border.width: (vmErrorMsg === "")? 0 : 2
        border.color: "#e4f1fd"
        radius: 2
        //layer.enabled: true
        //clip: true
    }

    function getText() {return lineEdit.text;}
    function setText(pass) {lineEdit.text = pass;}

    Rectangle {
        id: subLine
        width: lineEditRect.width
        height: 1;
        border.color: "#297fca"
        color: "#297fca"
        anchors.bottom: lineEditRect.bottom
    }

    function clear(){
        lineEdit.text = ""
    }

    TextInput {
        id: lineEdit
        color: "#58595b"
        font.family: vmFont
        font.pixelSize: 13
        anchors.bottom: subLine.top
        anchors.bottomMargin: 5
        verticalAlignment: TextInput.AlignVCenter
        leftPadding: 0
        width: lineEditRect.width
        echoMode: TextInput.Password;
    }

    Text{
        id: labelText
        text: vmLabelText
        color:  "#297fca"
        font.family: vmFont
        font.pixelSize: 11
        anchors.left: lineEditRect.left
        anchors.bottom: lineEdit.top
        anchors.bottomMargin: 5
    }

    Text{
        id: errorMsg
        text: vmErrorMsg
        color:  "#ca2026"
        font.family: vmFont
        font.pixelSize: 12
        anchors.left: lineEditRect.left
        anchors.top: lineEditRect.bottom
        anchors.topMargin: 5
        visible: (vmErrorMsg !== "")
    }

}
