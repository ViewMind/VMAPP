import QtQuick 2.6
import QtQuick.Controls 2.3

Button{

    id: vmButton
    property string vmText: "VM Button"
    property string vmFont: "Mono"
    property variant vmSize: [131,50]
    property bool vmInvertColors: false
    property string vmColorBkg: "#ffffff"
    property string vmColorText: "#297fca"

    width: vmSize[0]
    height: vmSize[1]

    function determineColors(){
        if (vmButton.vmInvertColors){
            if (vmButton.enabled){
                vmButton.vmColorBkg = "#ffffff";
                if (pressed){
                    vmButton.vmColorText = "#4984b3";
                }
                else{
                    vmButton.vmColorText = "#297fca";
                }
            }
            else {
                vmButton.vmColorBkg = "#ffffff"
                vmButton.vmColorText = "#bcbec0"
            }
        }
        else{
            vmButton.vmColorText = "#ffffff"
            if (vmButton.enabled){
                if (vmButton.pressed){
                    vmButton.vmColorBkg = "#4984b3";
                }
                else{
                    vmButton.vmColorBkg = "#297fca";
                }
            }
            else{
                vmButton.vmColorBkg = "#bcbec0"
            }
        }
    }

    scale: vmButton.pressed? 0.8:1

    Behavior on scale{
        NumberAnimation {
            duration: 25
        }
    }

    Component.onCompleted: {
        determineColors();
    }

    onEnabledChanged: {
        determineColors();
    }

    onPressed: {
        determineColors();
    }

    background: Rectangle {
        id: rectArea
        radius: 3
        border.color: vmColorText
        color: vmColorBkg
        anchors.fill: parent
    }
    contentItem: Text{
        anchors.centerIn: parent
        font.family: vmFont
        font.pixelSize: 13
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: vmText
        color: vmColorText
    }
}
