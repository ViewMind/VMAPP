import QtQuick 2.9

Item {
    property string vmFont: "Mono"
    property bool vmHasError: false
    property bool vmAcceptInput: true
    property string vmErrorMsg: ""
    property alias vmTextField: lineEdit

    signal gotClicked()

    Text {
        id: errorMessage;
        color: "#ca2026"
        font: lineEdit.font
        anchors.top: lineEditCanvas.bottom
        anchors.topMargin: 5
        verticalAlignment: Text.AlignBottom
        horizontalAlignment: Text.AlignRight
        text: vmErrorMsg
        visible: false;
    }

    MouseArea{
        id: mouseArea
        z: vmAcceptInput? -1:1
        anchors.fill: parent
        onClicked: {
            vmErrorMsg = "";
            gotClicked();
        }
    }

    onVmErrorMsgChanged: {
        if (vmErrorMsg === ""){
            errorMessage.visible = false;
            vmHasError = false;
        }
        else{
            errorMessage.visible = true;
            vmHasError = true;
        }
        lineEditCanvas.requestPaint()
    }


    Canvas {
        id: lineEditCanvas
        contextType: "2d"
        anchors.fill: parent
        onPaint: {
            var ctx = lineEditCanvas.getContext("2d");
            ctx.reset();
            ctx.strokeStyle = vmHasError? "#ca2026" : "#297FCA";
            ctx.lineWidth = 2;
            ctx.lineCap = "round"
            ctx.moveTo(0,height);
            ctx.lineTo(width,height);
            ctx.closePath();
            ctx.stroke();
        }
    }

    TextInput {
        id: lineEdit
        color: "#757575"
        font.family: vmFont
        font.pixelSize: 13*viewHome.vmScale
        anchors.centerIn: parent
        clip: true
        width: parent.width
        height: parent.height
        onActiveFocusChanged: {
            if (!activeFocus){
                lineEdit.ensureVisible(0);
            }
        }
    }
}
