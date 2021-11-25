import QtQuick
import QtQuick.Controls

Item {

    id: vmPasswordInput
    height: vmGlobals.vmControlsHeight

    property string vmErrorMsg: ""
    property bool vmEnabled: true
    property string vmPlaceHolderText: "Password"
    property string vmLabel: ""
    property string vmCurrentText: ""
    property string vmShowText: "Show"
    property string vmHideText: "Hide"
    property bool vmIsPaswordShown: false

    readonly property double vmLeftMargin: Math.ceil(width*10/338);
    readonly property double vmRightMargin: Math.ceil(width*10/338);

    signal textChanged()

    // Text input display.
    Rectangle {

        id: mainRect
        anchors.fill: parent
        color: {
            if (vmErrorMsg === "") return  vmGlobals.vmWhite
            else vmGlobals.vmRedErrorBackground
        }
        border.color: {
            if (vmErrorMsg === ""){
                return vmGlobals.vmGrayUnselectedBorder;
            }
            else {
                return vmGlobals.vmRedError;
            }
        }
        border.width: Math.ceil(vmPasswordInput.width/300);
        radius: Math.round(10*vmPasswordInput.height/44);


        // The display Text.
        TextInput {
            id: inputText

            height: parent.height
            width: parent.width - vmLeftMargin - vmRightMargin - showButton.width
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: vmLeftMargin

            echoMode: {
                if ((vmCurrentText == "") || vmIsPaswordShown) return TextInput.Normal
                else return TextInput.Password
            }

            font.pixelSize: vmGlobals.vmFontBaseSize
            font.weight: 400

            text: vmPlaceHolderText

            color: {
                if (vmErrorMsg == ""){
                    return (vmCurrentText == "")? vmGlobals.vmGrayPlaceholderText : vmGlobals.vmBlackText
                }
                else{
                    return vmGlobals.vmBlackText
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
                font.pixelSize: vmGlobals.vmFontBaseSize
                font.weight: 400
                font.underline: showButton.containsMouse ? true : false
                color: showButton.down ? vmGlobals.vmBlueTextHighlight : vmGlobals.vmBlueTextHighlight
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
        color:  vmGlobals.vmRedError
        font.pixelSize: vmGlobals.vmFontBaseSize
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
        color: vmGlobals.vmBlackText
        font.pixelSize: vmGlobals.vmFontLarge
        font.weight: 600
        anchors.left: parent.left
        anchors.bottom: parent.top
        anchors.bottomMargin: 10;
        visible: (vmLabel !== "")
    }

}
