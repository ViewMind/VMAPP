import QtQuick
import QtQuick.Controls
import "../"

Rectangle {

    property string vmText: "<b>bold</b><br>not bold"
    property color vmType: VMGlobals.vmNotificationBlack

    readonly property double vmVisibleY: VMGlobals.mainHeight - VMGlobals.adjustHeight(20) - height
    readonly property double vmHiddenY: VMGlobals.mainHeight + height
    readonly property double vmNotificationDuration: 4000

    id: notificationPopUp
    width: VMGlobals.adjustWidth(389)
    height: VMGlobals.adjustHeight(88)
    radius: VMGlobals.adjustWidth(10);
    color: vmType
    border.color: VMGlobals.vmBlackText
    border.width: 0
    y: vmHiddenY
    x: VMGlobals.mainWidth -  VMGlobals.adjustWidth(31) - width
    z: 200

    NumberAnimation on y {
        id: animation
        duration: 200
    }

    Timer {
        id: timer
        interval: vmNotificationDuration + animation.duration
        repeat: false
        onTriggered: {
            //console.log("Hiding notification");
            hide();
        }
    }

    function show(type,message){
        vmType = type
        vmText = message;
        animation.to = vmVisibleY
        animation.start()
        timer.start()
    }

    function hide(){
        animation.to = vmHiddenY
        animation.start()
        timer.stop()
    }

    Image {
        id: info
        source: (vmType == VMGlobals.vmNotificationRed)? "qrc:/images/alert-triangle-white.png" : "qrc:/images/info_notification.png"
        height: VMGlobals.adjustHeight(32)
        fillMode: Image.PreserveAspectFit
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: VMGlobals.adjustHeight(21.67)
        anchors.leftMargin: VMGlobals.adjustWidth(21.67)
    }

    Text {
        id: message
        text: vmText
        font.pixelSize: VMGlobals.vmFontBaseSize
        color: VMGlobals.vmWhite
        width: VMGlobals.adjustWidth(255)
        height: VMGlobals.adjustHeight(48)
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: info.right
        anchors.leftMargin: VMGlobals.adjustWidth(11.07)
    }

    Rectangle {
        width: closeButton.width*1.1
        height: closeButton.height*1.1
        color: ma.containsMouse ? Qt.lighter(vmType,1.3) : vmType
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: VMGlobals.adjustHeight(23.37)
        anchors.rightMargin: VMGlobals.adjustWidth(23.37)

        MouseArea {
            id: ma
            anchors.fill: parent
            hoverEnabled: true;
            onClicked: {
                hide()
            }
        }

        Canvas {
            id: closeButton
            height: VMGlobals.adjustHeight(11.25)
            width: height
            anchors.centerIn: parent
            contextType: "2d"
            onPaint: {

                var lw = VMGlobals.adjustHeight(1);

                var ctx = closeButton.getContext("2d");
                ctx.reset();
                ctx.strokeStyle = VMGlobals.vmWhite
                ctx.lineWidth = lw
                ctx.lineCap = "round"

                ctx.moveTo(lw,lw);
                ctx.lineTo(width-lw,height-lw)
                ctx.moveTo(width-lw,lw)
                ctx.lineTo(lw,height-lw)

                ctx.stroke()
            }
        }

    }

}
