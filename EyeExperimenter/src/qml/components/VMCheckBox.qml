import QtQuick
import QtQuick.Controls
import "../"

Item {

    id: vmCheckBox

    property bool vmIsOn: false
    property string vmText: ""

    readonly property double vmAirBeforeText: VMGlobals.adjustWidth(15)

    property alias vmAnchorTextLeft: text.left

    width: square.width + text.width + vmAirBeforeText
    height: VMGlobals.adjustHeight(20)

    signal linkClicked(url: string);

    Rectangle {

        id: square
        height: parent.height
        width: height
        radius: VMGlobals.adjustHeight(3)
        border.color:  vmIsOn ? VMGlobals.vmBlueSelected : VMGlobals.vmGrayAccented
        border.width: VMGlobals.adjustWidth(1)
        color: vmIsOn ? VMGlobals.vmBlueSelected : VMGlobals.vmGrayToggleOff

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            onClicked: {
                vmIsOn = !vmIsOn
            }
        }

        Canvas {
            id: checkMark
            contextType: "2d"
            width: square.width/2
            height: VMGlobals.adjustHeight(7)
            visible: vmIsOn
            anchors.centerIn: parent

            onPaint: {
                var ctx = checkMark.getContext("2d");
                ctx.beginPath();
                ctx.strokeStyle = VMGlobals.vmWhite;
                ctx.lineWidth = 2;
                ctx.moveTo(0, height*0.57);
                ctx.lineTo(width*0.4, height);
                ctx.lineTo(width, 0);
                ctx.stroke()
            }

        }
    }

    Text {
        id: text
        text: vmText
        linkColor: "#0000aa"
        font.pixelSize: VMGlobals.vmFontLarge
        font.weight: 600
        height: VMGlobals.adjustHeight(18)
        verticalAlignment: Text.AlignVCenter
        anchors.verticalCenter: square.verticalCenter
        anchors.left: square.right
        anchors.leftMargin: vmAirBeforeText
        onLinkActivated: function (link) {
            linkClicked(link);
        }
    }

}
