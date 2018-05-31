import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

Rectangle {

    id: container
    property string vmFont: "Mono"
    property string vmDrName: "No name"
    property string vmSelectedEyeTracker: "NONE"
    property string vmSelectedLanguage: "NONE"

    z: -10
    height: 260
    color: "#ffffff"

    layer.enabled: true
    layer.effect: DropShadow{
        radius: 5
    }

    signal hideMenu();

    Rectangle{
        id: textContainer
        height: vmBanner.height
        width: parent.width - imgDrAvatar.width
        anchors.left: parent.left
        anchors.top: parent.top
        Text{
            id: doctorsName
            anchors.fill: parent
            text: vmDrName
            font.pixelSize: 13
            padding: 30
            verticalAlignment: Text.AlignVCenter
            color: "#88B2D0"
            font.family: vmFont
        }
    }

    Image {
        id: imgDrAvatar
        source: "qrc:/images/AVATAR_CELESTE.png"
        anchors.verticalCenter: textContainer.verticalCenter
        anchors.left: textContainer.right
        anchors.leftMargin: -20
    }

    MouseArea{
        id: mArea
        anchors.fill: parent
        hoverEnabled: true
        onExited: {
            //console.log("Has mouse: " + btnChangeSettingMouseArea.containsMouse)
            //if (!btnChangeSettingMouseArea.containsMouse)
            hideMenu()
        }
    }

    // The main divisor
    Rectangle{
        id: mainDivisor
        color: "#88B2D0"
        width: parent.width*0.9
        height: 2
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: textContainer.bottom
    }

    // Language name
    Text{
        id: labLang
        color: "#88B2D0"
        text: vmSelectedLanguage
        font.family: gothamRegular.name
        font.pixelSize: 13
        anchors.left: parent.left
        anchors.leftMargin: 30
        anchors.top: mainDivisor.bottom
        anchors.topMargin: 19
    }

    // Language divisor
    Rectangle{
        id: divLang
        color: "#88B2D0"
        width: parent.width*0.8
        height: 1
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: labLang.bottom
        anchors.topMargin: 11
    }

    // Eye tracker selected
    Text{
        id: labETSel
        color: "#88B2D0"
        text: vmSelectedEyeTracker
        font.family: gothamRegular.name
        font.pixelSize: 13
        anchors.left: parent.left
        anchors.leftMargin: 30
        anchors.top: divLang.bottom
        anchors.topMargin: 19
    }

    // Eye tracker selected divisor
    Rectangle{
        id: divETSel
        color: "#88B2D0"
        width: parent.width*0.7
        height: 1
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: labETSel.bottom
        anchors.topMargin: 11
    }

    // The button is a mouse area otherwise the menu exists.
    MouseArea{
        id: btnChangeSettingMouseArea
        width: parent.width*0.9
        height: 38
        Rectangle {
            id: btnConfSettingsRect
            anchors.fill: parent
            radius: 3
            color: (btnChangeSettingMouseArea.pressed)? "ff0000" : "#ffffff"
            border.color: "#BCBEC0"
            Text{
                anchors.centerIn: btnConfSettingsRect
                font.family: gothamM.name
                font.pixelSize: 13
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                text: loader.getStringForKey("viewdrdiag_btnChangeSettings");
                color: "#BCBEC0"
            }

        }
        anchors.top: divETSel.bottom
        anchors.topMargin: 18
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: {
            console.log("Was clicked!");
        }
    }

}
