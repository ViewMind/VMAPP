import QtQuick 2.6

Item {

    property string vmFont: "Mono"
    property string vmPlaceHolder: "Some text ..."
    property string vmEnteredText: ""
    property string vmErrorMsg: ""
    property bool vmIsPasswordField: false
    property bool vmNumbersOnly: false
    property bool vmAcceptFloat: false
    property bool vmFocus: false

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

    Rectangle {
        id: subLine
        width: lineEditRect.width
        height: 1;
        border.color: "#297fca"
        color: "#297fca"
        anchors.bottom: lineEditRect.bottom
    }

    function clear(){
        labelText.visible = false;
        lineEdit.text = vmPlaceHolder
        vmEnteredText = "";
    }

    function setText(text){
        if ((text === "") || (text === undefined)) return;
        labelText.visible = true;
        vmEnteredText = text;
        lineEdit.text = text;
    }

    TextInput {
        id: lineEdit
        focus: vmFocus
        text: vmPlaceHolder
        color: (labelText.visible)? "#58595b" : "#cfcfcf"
        font.family: vmFont
        font.pixelSize: 13
        anchors.bottom: subLine.top
        anchors.bottomMargin: 5
        verticalAlignment: TextInput.AlignVCenter
        leftPadding: 0
        width: lineEditRect.width
        echoMode: (vmIsPasswordField)? TextInput.PasswordEchoOnEdit : TextInput.Normal;
        onActiveFocusChanged: {
            if (activeFocus){
                vmErrorMsg = "";
                if (vmEnteredText === ""){
                    // Removing the placeholder and making the labelText visible
                    lineEdit.text = "";
                    labelText.visible = true;
                }
            }
            else vmFocus = false;
        }

        onEditingFinished: {
            vmEnteredText = lineEdit.text;
            if (lineEdit.text === ""){
                lineEdit.text = vmPlaceHolder
                labelText.visible = false;
            }
            // Checking input validity
            else if (vmNumbersOnly){
                var parsed = parseInt(lineEdit.text)
                if (isNaN(parsed)){
                    vmEnteredText = "";
                    lineEdit.text = vmPlaceHolder
                }
            }
            else if (vmAcceptFloat){
                var parsedFloat = parseFloat(lineEdit.text)
                if (isNaN(parsedFloat)){
                    vmEnteredText = "";
                    lineEdit.text = vmPlaceHolder
                }
            }
        }
    }

    Text{
        id: labelText
        text: vmPlaceHolder
        color:  "#297fca"
        font.family: vmFont
        font.pixelSize: 11
        anchors.left: lineEditRect.left
        anchors.bottom: lineEdit.top
        anchors.bottomMargin: 5
        visible: (vmEnteredText !== "")
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
