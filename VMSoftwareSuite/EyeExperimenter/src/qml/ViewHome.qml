import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

VMBase {

    id: viewHome
    width: mainWindow.width
    height: mainWindow.height

    readonly property double vmScale: mainWindow.width/1280;
    readonly property string keysearch: "viewhome_"
    readonly property int vmAPI_OPINFO_REQUEST: 1

    Connections {
        target: loader
        onFinishedRequest: {
            // Close the connection dialog and open the user selection dialog.
            if (loader.getLastAPIRequest() === vmAPI_OPINFO_REQUEST){
                connectionDialog.close();
                viewDrSelection.open();
            }
        }
    }


    function getErrorTitleAndMessage(keyid){
        var res = loader.getStringListForKey(keyid);
        if (res.length !== 2){
            res[0] = "ERROR";
            res[1] = "CODE: " + keyid
        }
        return res;
    }

    function openSettingsDialog(){
        viewSettings.open();
    }

    function updateDrMenuText(){
        updateText()
        viewCalibrationDone.updateText();
        viewCalibrationStart.updateText();
        viewPatientReg.updateText()
        viewPresentExperimet.updateText()
        viewStudyStart.updateText()
        viewPatList.updateText()
        viewDrInfo.updateText();
        viewStudyDone.updateText();
        viewVRDisplay.updateText();
        viewFinishedStudies.updateText()
        viewQC.updateText();
    }


    // Debugging function to test error dialog.
    function testErrorDiag(which,code){
        var titleMsg;
        if (code === -1) titleMsg = viewHome.getErrorTitleAndMessage(which);
        else titleMsg = loader.getErrorMessageForCode(code)
        vmErrorDiag.vmErrorMessage = titleMsg[1];
        vmErrorDiag.vmErrorTitle = titleMsg[0];
        vmErrorDiag.open();
    }

    Component.onCompleted: {
        // Checking that the everything was loaded correctly.
        if (loader.getLoaderError()){
            viewHome.vmErrorDiag.vmErrorCode = viewHome.vmErrorDiag.vmERROR_LOAD_CONFIG;
            var titleMsg = getErrorTitleAndMessage("error_loading_config");
            //console.log("Title msg is " + titleMsg);
            viewHome.vmErrorDiag.vmErrorMessage = titleMsg[1];
            viewHome.vmErrorDiag.vmErrorTitle = titleMsg[0];
            viewHome.vmErrorDiag.open();
            return;
        }

    }

    // Dialog used to show a wall of text.
    Dialog {
        id: showTextDialog;
        modal: true
        width: mainWindow.width*0.6
        height: mainWindow.height*0.87

        property string vmContent: ""
        property string vmTitle: ""

        y: (parent.height - height)/2
        x: (parent.width - width)/2
        closePolicy: Popup.NoAutoClose

        contentItem: Rectangle {
            id: rectDialog
            anchors.fill: parent
            layer.enabled: true
            layer.effect: DropShadow{
                radius: 5
            }
        }

        Text {
            id: diagTitle
            font.family: viewHome.gothamB.name
            font.pixelSize: 43*viewHome.vmScale
            anchors.top: parent.top
            anchors.topMargin: mainWindow.height*0.029
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#297fca"
            text: showTextDialog.vmTitle

        }
        ScrollView {
            id: idScrollView
            width: showTextDialog.width*0.9;
            contentWidth: width
            height: showTextDialog.height*0.62;
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top : diagTitle.bottom
            anchors.topMargin: mainWindow.height*0.058
            clip: true
            //horizontalScrollBarPolicy : Qt.ScrollBarAlwaysOff
            TextEdit {
                id: idContent
                width: parent.width;
                height: parent.height;
                font.family: robotoR.name
                font.pixelSize: 13*viewHome.vmScale
                readOnly: true
                text: showTextDialog.vmContent
                wrapMode: Text.Wrap
            }
        }

        VMButton{
            id: btnClose
            height: mainWindow.height*0.072
            vmText: "OK";
            vmFont: viewHome.gothamM.name
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 20;
            onClicked: {
                showTextDialog.close();
            }
        }

    }

    Dialog {
        id: restartDialog;
        modal: true
        width: mainWindow.width*0.48
        height: mainWindow.height*0.29

        property string vmContent: ""
        property string vmTitle: ""

        y: (parent.height - height)/2
        x: (parent.width - width)/2
        closePolicy: Popup.NoAutoClose

        contentItem: Rectangle {
            id: restartRectDialog
            anchors.fill: parent
            layer.enabled: true
            layer.effect: DropShadow{
                radius: 5
            }
        }

        Column {
            id: restartDiagRow
            spacing: 20
            anchors.centerIn: parent

            Text {
                id: restartDiagTitle
                font.family: viewHome.gothamB.name
                font.pixelSize: 43*viewHome.vmScale
                color: "#297fca"
                anchors.horizontalCenter: parent.horizontalCenter
                text: restartDialog.vmTitle
            }

            // The instruction text
            Text {
                id: restartDiagMessage
                font.family: viewHome.robotoR.name
                font.pixelSize: 13*viewHome.vmScale
                textFormat: Text.RichText
                anchors.horizontalCenter: parent.horizontalCenter
                text: restartDialog.vmContent
            }

        }

    }

    Dialog {

        property string vmTitle: "TITLE"
        property string vmMessage: "MESSAGE"

        id: connectionDialog;
        modal: true
        width: mainWindow.width*0.48
        height: mainWindow.height*0.87
        y: (parent.height - height)/2
        x: (parent.width - width)/2
        closePolicy: Popup.NoAutoClose

        contentItem: Rectangle {
            id: rectConnectionDialog
            anchors.fill: parent
            layer.enabled: true
            layer.effect: DropShadow{
                radius: 5
            }
        }

        // The instruction text
        Text {
            id: diagConnectionTitle
            font.family: viewHome.gothamB.name
            font.pixelSize: 43*viewHome.vmScale
            anchors.top: parent.top
            anchors.topMargin: mainWindow.height*0.128
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#297fca"
            text: connectionDialog.vmTitle
        }


        // The instruction text
        Text {
            id: diagMessage
            font.family: viewHome.robotoR.name
            font.pixelSize: 13*viewHome.vmScale
            anchors.top:  diagConnectionTitle.bottom
            anchors.topMargin: mainWindow.height*0.038
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#297fca"
            text:  connectionDialog.vmMessage;
            z: 2 // Sometimes the border of the image covers the text. This fixes it.
        }

        AnimatedImage {
            id: slideAnimation
            source: "qrc:/images/LOADING.gif"
            anchors.top: diagMessage.bottom
            anchors.topMargin: mainWindow.height*0.043
            anchors.horizontalCenter: parent.horizontalCenter
            scale: viewHome.vmScale
            visible: true
        }

    }

    Timer {
        id: closeTimer;
        interval: 5000
        repeat: false
        onTriggered: {
            loader.replaceEyeLauncher();
        }
    }

    // ViewMind Logo
    Image {
        id: logo
        source: "qrc:/images/LOGO.png"
        anchors{
            top: parent.top
            topMargin: mainWindow.height*0.043
            left: parent.left
            leftMargin: mainWindow.width*0.048
        }
        scale: vmScale
    }

    // The head graph
    Image {
        id: headDesign
        source: "qrc:/images/ILUSTRACION.png"
        anchors{
            left: parent.left
            leftMargin: mainWindow.width*0.113
            bottom: parent.bottom
            bottomMargin: mainWindow.height*0.277
        }
        scale: vmScale
    }

    // The configure settings button
    Button {
        id: btnConfSettings
        scale: btnConfSettings.pressed? 0.9:1
        Behavior on scale{
            NumberAnimation {
                duration: 25
            }
        }
        background: Rectangle {
            id: btnConfSettingsRect
            radius: 3
            color: btnConfSettings.pressed? "#e8f2f8" :"#ffffff"
            width: mainWindow.width*0.139
            height: mainWindow.height*0.062
        }
        contentItem: Text{
            anchors.centerIn: btnConfSettingsRect
            font.family: gothamM.name
            font.pixelSize: 13*viewHome.vmScale
            text: loader.getStringForKey(keysearch+"btnConfSettings");
            color: "#88b2d0"
        }
        anchors{
            right: parent.right
            rightMargin: mainWindow.width*0.062
            top: parent.top
            topMargin: mainWindow.height*0.027
        }
        onClicked: {
            viewSettings.open()
        }
    }

    Text {
        id: slideTitle
        font.family: gothamB.name
        font.pixelSize: 43*viewHome.vmScale
        anchors.top:  vmBanner.bottom
        anchors.topMargin: mainWindow.height*0.307
        anchors.left: headDesign.right
        anchors.leftMargin: mainWindow.width*0.058
        color: "#297fca"
        text: loader.getStringForKey(keysearch+"slideTitle");
    }

    VMButton{
        id: btnGetStarted
        vmText: loader.getStringForKey(keysearch+"btnGetStarted");
        vmFont: gothamM.name
        anchors{
            left: headDesign.right
            leftMargin: mainWindow.width*0.058
            top: slideTitle.bottom
            topMargin: mainWindow.height*0.033
        }
        onClicked: {
            var title_and_text = loader.getStringListForKey("msg_get_info_online");
            //console.log(title_and_text);
            connectionDialog.vmMessage = title_and_text[1];
            connectionDialog.vmTitle = title_and_text[0];
            connectionDialog.open();
            loader.requestOperatingInfo();
            //
        }
    }
}
