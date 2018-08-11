import QtQuick 2.6

Item {

    property string vmFont: "Mono"
    property string vmPlaceHolder: "Some text ..."
    property string vmEnteredText: ""
    property string vmErrorMsg: ""
    property bool vmNumbersOnly: false

    Rectangle {
        id: lineEditRect
        anchors.fill: parent
        color: "#ffffff"
        border.width: (vmErrorMsg === "")? 1 : 2
        border.color: (vmErrorMsg === "")? "#e4f1fd" : "#ffffff"
        radius: 2
        layer.enabled: true
        clip: true
    }

    Rectangle {
        id: subLine
        width: lineEditRect.width
        height: 2;
        color: "#297fca"
        anchors.bottom: lineEditRect.bottom
    }

    Text{
        id: labelText
        text: vmPlaceHolder
        color:  "#297fca"
        font.family: vmFont
        font.pixelSize: 9
        anchors.left: lineEditRect.left
        anchors.top: lineEditRect.top
        visible: (vmEnteredText !== "")
    }

    TextInput {
        id: lineEdit
        text: vmPlaceHolder
        color: "#5499d5"
        font.family: vmFont
        font.pixelSize: 13
        anchors.bottom: subLine.top
        verticalAlignment: TextInput.AlignVCenter
        leftPadding: 0
        width: lineEditRect.width
        height: lineEditRect.height - subLine.height - labelText.height
        onActiveFocusChanged: {
            if (activeFocus){
                vmErrorMsg = "";
                if (vmEnteredText === ""){
                    // Removing the placeholder
                    lineEdit.text = "";
                }
            }
        }

        onEditingFinished: {
            vmEnteredText = lineEdit.text;
            if (lineEdit.text === ""){
                lineEdit.text = vmPlaceHolder
            }
            // Checking input validity
            else if (vmNumbersOnly){
                var parsed = parseInt(lineEdit.text)
                if (isNaN(parsed)){
                    vmEnteredText = "";
                    lineEdit.text = vmPlaceHolder
                }
            }
        }
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
