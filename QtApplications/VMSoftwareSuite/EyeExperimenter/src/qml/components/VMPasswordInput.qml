import QtQuick
import QtQuick.Controls
import "../"

Item {

    id: vmPasswordInput
    height: VMGlobals.vmControlsHeight

    property string vmErrorMsg: ""
    property bool vmEnabled: true
    property string vmPlaceHolderText: "Password"
    property string vmLabel: ""
    property string vmCurrentText: ""
    property string vmShowText: "Show"
    property string vmHideText: "Hide"
    property bool vmIsPaswordShown: false
    property bool vmFocus: false

    readonly property double vmLeftMargin: Math.ceil(width*10/338);
    readonly property double vmRightMargin: Math.ceil(width*10/338);

    signal textChanged()

    function clear(){
        inputText.text = vmPlaceHolderText
        vmCurrentText = "";
        vmErrorMsg = ""
        vmFocus = false
        vmIsPaswordShown = false
    }

    // Text input display.
    Rectangle {

        id: mainRect
        anchors.fill: parent
        color: {
            if (vmErrorMsg === "") return  VMGlobals.vmWhite
            else VMGlobals.vmRedErrorBackground
        }
        border.color: {
            if (vmErrorMsg === ""){
                return VMGlobals.vmGrayUnselectedBorder;
            }
            else {
                return VMGlobals.vmRedError;
            }
        }
        border.width: Math.ceil(vmPasswordInput.width/300);
        radius: Math.round(10*vmPasswordInput.height/44);


        // The display Text.
        TextInput {
            id: inputText
            focus: vmFocus
            height: parent.height
            width: parent.width - vmLeftMargin - vmRightMargin - showButton.width
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: vmLeftMargin

            echoMode: {
                if ((vmCurrentText == "") || vmIsPaswordShown) return TextInput.Normal
                else return TextInput.Password
            }

            font.pixelSize: VMGlobals.vmFontBaseSize
            font.weight: 400

            text: vmPlaceHolderText

            color: {
                if (vmErrorMsg == ""){
                    return (vmCurrentText == "")? VMGlobals.vmGrayPlaceholderText : VMGlobals.vmBlackText
                }
                else{
                    return VMGlobals.vmBlackText
                }
            }

            verticalAlignment: Text.AlignVCenter

            onFocusChanged: {
                if (focus){
                    if (vmCurrentText == "") text = ""
                }
            }

            onTextEdited: {
                vmErrorMsg = ""
                vmCurrentText = inputText.text
                vmPasswordInput.textChanged();
            }

            onEditingFinished: {
                if (inputText.text == "") {
                    inputText.text = vmPlaceHolderText
                    vmCurrentText = "";
                }
            }
        }

        MouseArea {
            id: showButton
            width: vmPasswordInput.width*0.18;
            height: vmPasswordInput.height
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: vmRightMargin

            cursorShape: showButton.containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor

            hoverEnabled: true

            scale: showButton.pressed ? 0.8 : 1.0

            onClicked: {
                vmIsPaswordShown = !vmIsPaswordShown
            }

            Text {
                id: showHideText
                text: vmIsPaswordShown ? vmHideText : vmShowText
                anchors.fill: parent
                font.pixelSize: VMGlobals.vmFontBaseSize
                font.weight: 400
                font.underline: showButton.containsMouse ? true : false
                color: showButton.down ? VMGlobals.vmBlueTextHighlight : VMGlobals.vmBlueTextHighlight
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                //elide: Text.ElideRight
            }

        }

    }


    //////////////////// THE ERROR MESSAGE
    Text{
        id: errorMsg
        text: vmErrorMsg
        color:  VMGlobals.vmRedError
        font.pixelSize: VMGlobals.vmFontBaseSize
        font.weight: 400
        anchors.left: parent.left
        anchors.top: parent.bottom
        anchors.topMargin: 10
        visible: (vmErrorMsg !== "")
    }

    //////////////////// THE LABEL (if present).
    Text {
        id: label
        text: vmLabel
        color: VMGlobals.vmBlackText
        font.pixelSize: VMGlobals.vmFontLarge
        font.weight: 600
        anchors.left: parent.left
        anchors.bottom: parent.top
        anchors.bottomMargin: 10;
        visible: (vmLabel !== "")
    }

}
