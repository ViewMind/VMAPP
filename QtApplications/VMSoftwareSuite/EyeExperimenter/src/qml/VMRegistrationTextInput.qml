import QtQuick

Item {

    property string vmFont: "Mono"
    property string vmPlaceHolder: "Some text ..."
    property string vmEnteredText: ""
    property string vmErrorMsg: ""
    property bool vmNumbersOnly: false

    Rectangle {
        id: lineEditRect
        anchors.fill: parent
        color: "#e4f1fd"
        border.width: (vmErrorMsg === "")? 1 : 2
        border.color: (vmErrorMsg === "")? "#e4f1fd" : "#ca2026"
        radius: 2
        layer.enabled: true
        clip: true
    }

    TextInput {
        id: lineEdit
        text: vmPlaceHolder
        color: "#5499d5"
        font.family: vmFont
        font.pixelSize: 13*viewHome.vmScale
        anchors.bottom: parent.bottom
        verticalAlignment: TextInput.AlignVCenter
        leftPadding: 10
        width: lineEditRect.width
        height: lineEditRect.height
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
        font.pixelSize: 12*viewHome.vmScale
        anchors.left: lineEditRect.left
        anchors.top: lineEditRect.bottom
        anchors.topMargin: mainWindow.height*0.007
        visible: (vmErrorMsg !== "")
    }

}
