import QtQuick

Item {

    property string vmFont: "Mono"
    property string vmPlaceHolder: "Some text ..."
    property string vmEnteredText: ""
    property string vmErrorMsg: ""
    property bool vmIsPasswordField: false
    property bool vmNumbersOnly: false
    property bool vmAcceptFloat: false
    property bool vmFocus: false


    height: mainWindow.height*0.058
    z: 0

    signal enterPressed();
    signal hasFocus();

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
        vmErrorMsg = ""
        vmEnteredText = "";
        if (vmIsPasswordField){
            lineEdit.echoMode = TextInput.Normal;
        }
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
        font.pixelSize: 13*viewHome.vmScale
        anchors.bottom: subLine.top
        anchors.bottomMargin: mainWindow.height*0.007
        verticalAlignment: TextInput.AlignVCenter
        leftPadding: 0
        width: lineEditRect.width
        echoMode: ((vmIsPasswordField) && (vmEnteredText !== ""))? TextInput.Password : TextInput.Normal;
        onActiveFocusChanged: {
            if (activeFocus){
                vmErrorMsg = "";
                hasFocus();
                if (vmEnteredText === ""){
                    // Removing the placeholder and making the labelText visible
                    lineEdit.text = "";
                    labelText.visible = true;
                }
                if (vmIsPasswordField){
                    echoMode = TextInput.Password;
                }
            }            
            else{
                vmFocus = false;
                if (vmIsPasswordField){
                    if (vmEnteredText === ""){
                        echoMode = TextInput.Normal;
                    }
                }
            }
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
//        onTextChanged: {
//            vmEnteredText = lineEdit.text
//            if ((vmIsPasswordField) && (vmEnteredText !== "")){
//                echoMode = TextInput.Password;
//            }
//            else {
//                echoMode = TextInput.Normal;
//            }
//        }
        Keys.onEnterPressed: {
            enterPressed();
        }
        Keys.onReturnPressed: {
            enterPressed();
        }
    }

    Text{
        id: labelText
        text: vmPlaceHolder
        color:  "#297fca"
        font.family: vmFont
        font.pixelSize: 11*viewHome.vmScale
        anchors.left: lineEditRect.left
        anchors.bottom: lineEdit.top
        anchors.bottomMargin: mainWindow.height*0.007
        visible: (vmEnteredText !== "")
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
