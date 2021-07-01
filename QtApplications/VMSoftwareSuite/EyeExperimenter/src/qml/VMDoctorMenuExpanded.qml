import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

Rectangle {

    id: container
    property string vmFont: "Mono"
    property string vmDrName: "NO NAME";
    property string vmDemoMode: "";    

    z: -10
    height: {
        var air = mainWindow.height*0.026;
        return (textContainer.height + btnChangeDoctorInfo.height + btnChangeSettingMouseArea.height + btnProtocolsRect.height + mainDivisor.height*2 + 4*air)*1.2
    }
    color: "#ffffff"

    layer.enabled: true
    layer.effect: DropShadow{
        radius: 5
    }

    signal hideMenu();

    function updateText(){
        var evaluator = loader.getCurrentEvaluatorInfo();
        vmDrName = evaluator.lastname + ",\n" + evaluator.name
        vmDemoMode = loader.getStringForKey("viewdrmenu_mode") + ": "
        if (loader.getConfigurationBoolean("use_mouse")) vmDemoMode = vmDemoMode + "Demo";
        else vmDemoMode = vmDemoMode + "Normal"
        // Checking if the doctor is validated
        btnProtocols.enabled = true;
    }

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
            font.pixelSize: 13*viewHome.vmScale
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
            hideMenu()
        }
    }

    // The main divisor
    Rectangle{
        id: mainDivisor
        color: "#88B2D0"
        width: parent.width*0.9
        height: mainWindow.height*0.003
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: textContainer.bottom
    }

    // Demo mode
    Text{
        id: labDemoMode
        color: "#88B2D0"
        text: vmDemoMode
        font.family: gothamRegular.name
        font.pixelSize: 13*viewHome.vmScale
        anchors.left: parent.left
        anchors.leftMargin: mainWindow.width*0.023
        anchors.top: mainDivisor.bottom
        anchors.topMargin: mainWindow.height*0.028
    }

    // Demo mode divisor
    Rectangle{
        id: divDemoMode
        color: "#88B2D0"
        width: parent.width*0.8
        height: mainWindow.height*0.001
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: labDemoMode.bottom
        anchors.topMargin: mainWindow.height*0.016
    }

//    // Language name
//    Text{
//        id: labLang
//        color: "#88B2D0"
//        text: vmSelectedLang
//        font.family: gothamRegular.name
//        font.pixelSize: 13*viewHome.vmScale
//        anchors.left: parent.left
//        anchors.leftMargin: mainWindow.width*0.023
//        anchors.top: divDemoMode.bottom
//        anchors.topMargin: mainWindow.height*0.016
//    }

//    // Language divisor
//    Rectangle{
//        id: divLang
//        color: "#88B2D0"
//        width: parent.width*0.8
//        height: mainWindow.height*0.001
//        anchors.horizontalCenter: parent.horizontalCenter
//        anchors.top: labLang.bottom
//        anchors.topMargin: mainWindow.height*0.016
//    }

//    // Eye tracker selected
//    Text{
//        id: labETSel
//        color: "#88B2D0"
//        text: vmSelectedET
//        font.family: gothamRegular.name
//        font.pixelSize: 13*viewHome.vmScale
//        anchors.left: parent.left
//        anchors.leftMargin: mainWindow.width*0.023
//        anchors.top: divLang.bottom
//        anchors.topMargin: mainWindow.height*0.016
//    }

//    // Eye tracker selected divisor
//    Rectangle{
//        id: divETSel
//        color: "#88B2D0"
//        width: parent.width*0.8
//        height: mainWindow.height*0.001
//        anchors.horizontalCenter: parent.horizontalCenter
//        anchors.top: labETSel.bottom
//        anchors.topMargin: mainWindow.height*0.016
//    }


    // The button is a mouse area otherwise the menu exits.
    MouseArea{
        id: btnChangeSettingMouseArea
        width: parent.width*0.9
        height: mainWindow.height*0.055
        Rectangle {
            id: btnConfSettingsRect
            anchors.fill: parent
            radius: 3
            color: (btnChangeSettingMouseArea.pressed)? "ff0000" : "#ffffff"
            border.color: "#BCBEC0"
            Text{
                anchors.centerIn: btnConfSettingsRect
                font.family: gothamM.name
                font.pixelSize: 13*viewHome.vmScale
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                text: loader.getStringForKey("viewhome_btnConfSettings");
                color: "#BCBEC0"
            }

        }
        anchors.top: divDemoMode.bottom
        anchors.topMargin: mainWindow.height*0.026
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: {
            swiperControl.currentIndex = swiperControl.vmIndexHome;
            viewHome.openSettingsDialog();
        }
    }

    // The button is a mouse area otherwise the menu exists.
    MouseArea{
        id: btnChangeDoctorInfo
        width: parent.width*0.9
        height: mainWindow.height*0.055
        Rectangle {
            id: btnDrInfoRect
            anchors.fill: parent
            radius: 3
            color: (btnChangeDoctorInfo.pressed)? "ff0000" : "#ffffff"
            border.color: "#BCBEC0"
            Text{
                anchors.centerIn: btnDrInfoRect
                font.family: gothamM.name
                font.pixelSize: 13*viewHome.vmScale
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                text: loader.getStringForKey("viewhome_btnEditDrInfo");
                color: "#BCBEC0"
            }

        }
        anchors.top: btnChangeSettingMouseArea.bottom
        anchors.topMargin: mainWindow.height*0.026
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: {
            viewDrInfo.loadDoctorInformation();
            swiperControl.currentIndex = swiperControl.vmIndexDrProfile;
        }
    }


    // The button is a mouse area otherwise the menu exists.
    MouseArea{
        id: btnProtocols
        width: parent.width*0.9
        height: mainWindow.height*0.055
        visible: true
        Rectangle {
            id: btnProtocolsRect
            anchors.fill: parent
            radius: 3
            color: (btnProtocols.pressed)? "ff0000" : "#ffffff"
            border.color: btnProtocols.enabled? "#BCBEC0" : "#dadcde"
            Text{
                anchors.centerIn: btnProtocolsRect
                font.family: gothamM.name
                font.pixelSize: 13*viewHome.vmScale
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                text: loader.getStringForKey("viewhome_btnProtocol");
                color: btnProtocols.enabled? "#BCBEC0" : "#dadcde"
            }

        }
        anchors.top: btnChangeDoctorInfo.bottom
        anchors.topMargin: mainWindow.height*0.026
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: {
            viewProtocols.open();
        }
    }


}
