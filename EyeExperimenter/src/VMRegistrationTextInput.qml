import QtQuick 2.6

Item {

    property string vmFont: "Mono"
    property string vmPlaceHolder: "Some text ..."
    property bool vmNumbersOnly: false

    // Qt Quick approach to make internal variables.
    Item {
        id: own
        property string enteredText: ""
    }

    Rectangle {
        id: lineEditRect
        anchors.fill: parent
        color: "#e4f1fd"
        radius: 2
    }

    TextInput {
        id: lineEdit
        text: vmPlaceHolder
        color: "#5499d5"
        font.family: vmFont
        font.pixelSize: 13
        anchors.bottom: parent.bottom
        //inputMethodHints: vmNumbersOnly ? Qt.ImhDigitsOnly : Qt.ImhNone
        verticalAlignment: TextInput.AlignVCenter
        leftPadding: 10
        width: lineEditRect.width
        height: lineEditRect.height
        onActiveFocusChanged: {
            if (activeFocus){
                if (own.enteredText === ""){
                    // Removing the placeholder
                    lineEdit.text = "";
                }
            }
        }

        onEditingFinished: {
            own.enteredText = lineEdit.text;
            if (lineEdit.text === ""){
                lineEdit.text = vmPlaceHolder
            }
            // Checking input validity
            else if (vmNumbersOnly){
                var parsed = parseInt(lineEdit.text)
                if (isNaN(parsed)){
                    own.enteredText = "";
                    lineEdit.text = vmPlaceHolder
                }
            }
        }
    }

}
