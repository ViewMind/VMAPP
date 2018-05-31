import QtQuick 2.6
import QtQuick.Controls 2.3

ComboBox {

    property variant vmModel: ["Item 1", "Item 2"]

    id: control
    model: vmModel

    hoverEnabled: true

    // Each of the items in the list.
    delegate: ItemDelegate {
        width: control.width
        hoverEnabled: true
        contentItem: Rectangle {
            anchors.fill: parent
            border.width: 1
            border.color: "#297FCA"
            color: highlighted ? "#dfdfdf" : "#ffffff"
            radius: 2
            Text {
                text: modelData
                //color: hovered? "yellow" :  "#21be2b"
                font: control.font
                elide: Text.ElideRight
                anchors{
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    leftMargin: 2
                }
            }

        }
        highlighted: control.highlightedIndex === index
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
            ctx.fillStyle = control.pressed ? "#297FCA" : "#297FCA";
            ctx.fill();
        }
    }

    // What is actually shown when not pressed
    contentItem: Text {
        rightPadding: control.indicator.width + control.spacing
        text: control.displayText
        font: control.font
        color: control.pressed ? "#757575" : "#757575"
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        implicitWidth: 120
        implicitHeight: 40
        radius: 2
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
