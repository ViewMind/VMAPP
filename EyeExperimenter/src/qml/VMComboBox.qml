import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

ComboBox {

    property variant vmModel: ["Item 1", "Item 2"]

    id: control
    model: vmModel
    hoverEnabled: true
    font.pixelSize: 13
    height: 29
    property string vmErrorMsg: ""

    // Each of the items in the list.
    delegate: ItemDelegate {
        width: control.width
        hoverEnabled: true
        contentItem: Rectangle {
            anchors.fill: parent
            border.width: 1
            border.color: "#dadada"
            layer.enabled: true
            layer.effect: FastBlur{
                radius: 2
            }
            color: highlighted ? "#eeeeee" : "#ffffff"
        }
        Text {
            text: modelData
            //color: hovered? "yellow" :  "#21be2b"
            font: control.font            
            elide: Text.ElideRight
            padding: 5
            anchors{
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: 2
            }
        }
        highlighted: control.highlightedIndex === index
    }

    onFocusChanged: {
        if (activeFocus) vmErrorMsg = "";
    }

    // The Combo Box Indicator.
    indicator: Canvas {
        id: canvas
        x: control.width - width - control.rightPadding
        y: control.topPadding + (control.availableHeight - height) / 2
        width: 12
        height: 8
        contextType: "2d"

        Connections {
            target: control
            onPressedChanged: canvas.requestPaint()
        }

        onPaint: {
            var ctx = canvas.getContext("2d");
            ctx.reset();
            ctx.moveTo(0, 0);
            ctx.lineTo(width, 0);
            ctx.lineTo(width / 2, height);
            ctx.closePath();
            ctx.fillStyle = control.pressed ? "#000000" : "#000000";
            ctx.fill();
        }
    }

    // What is actually shown when not pressed
    contentItem: Rectangle{
        color: "#ebf3fa"
        anchors.fill: parent
        Text {
            rightPadding: control.indicator.width + control.spacing
            text: control.displayText
            font: control.font
            color: control.pressed ? "#58595b" : "#58595b"
            verticalAlignment: Text.AlignVCenter
            padding: 5
            elide: Text.ElideRight
            anchors.bottom: divisorLine.top
            anchors.bottomMargin: 2
        }
        // The divisor line.
        Rectangle{
            id: divisorLine
            color: "#297fca"
            border.color: "#297fca"
            border.width: 0;
            height: 1;
            width: parent.width
            anchors.bottom: parent.bottom
        }
    }

    background: Rectangle {
        implicitWidth: 120
        implicitHeight: 29
        radius: 2
    }

    Text{
        id: errorMsg
        text: vmErrorMsg
        color:  "#ca2026"
        font.family: control.font.family
        font.pixelSize: 12
        anchors.left: parent.left
        anchors.top: parent.bottom
        anchors.topMargin: 5
        visible: (vmErrorMsg !== "")
    }

    popup: Popup {
        y: control.height - 1
        width: control.width
        implicitHeight: contentItem.implicitHeight
        padding: 1

        contentItem: ListView {
            clip: false
            implicitHeight: contentHeight
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex
            ScrollIndicator.vertical: ScrollIndicator { }
        }

        background: Rectangle {
            //border.color: "#297FCA"
            //border.width: 2
        }
    }
}
