import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

VMBase {

    id: viewHome
    width: viewHome.vmWIDTH
    height: viewHome.vmHEIGHT

    readonly property string keysearch: "viewhome_"

    function getErrorTitleAndMessage(keyid){
        var str = loader.getStringForKey(keyid);
        var res = str.split("<>");
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
        viewResults.updateText()
        viewStudyStart.updateText()
        viewPatList.updateText()
        viewDrInfo.updateText();
        viewShowReports.updateText();
        viewStudyDone.updateText();
        viewMedRecordList.updateText();
        viewMedicalInformation.updateText();
    }

    Dialog {
        id: showTextDialog;
        modal: true
        width: 614
        height: 600

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
            font.pixelSize: 43
            anchors.top: parent.top
            anchors.topMargin: 20
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
            anchors.topMargin: 40
            clip: true
            //horizontalScrollBarPolicy : Qt.ScrollBarAlwaysOff
            TextEdit {
                id: idContent
                width: parent.width;
                height: parent.height;
                font.family: robotoR.name
                font.pixelSize: 13
                readOnly: true
                text: showTextDialog.vmContent
                wrapMode: Text.Wrap
            }
        }

        VMButton{
            id: btnClose
            height: 50
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
        width: 614
        height: 200

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
                font.pixelSize: 43
                color: "#297fca"
                anchors.horizontalCenter: parent.horizontalCenter
                text: restartDialog.vmTitle
            }

            // The instruction text
            Text {
                id: restartDiagMessage
                font.family: viewHome.robotoR.name
                font.pixelSize: 13
                textFormat: Text.RichText
                anchors.horizontalCenter: parent.horizontalCenter
                text: restartDialog.vmContent
            }

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

        // SSL Check is done right now
        if (!flowControl.checkSSLAvailability()){
            vmErrorDiag.vmErrorCode = vmErrorDiag.vmERROR_NO_SSL;
            titleMsg = viewHome.getErrorTitleAndMessage("error_no_ssl");
            vmErrorDiag.vmErrorMessage = titleMsg[1];
            vmErrorDiag.vmErrorTitle = titleMsg[0];
            vmErrorDiag.open();
            return;
        }

        // Loading the Dr Options.
        viewDrSelection.updateDrProfile();

        // Checking for changelog
        var content = loader.checkForChangeLog();
        if (content !== ""){
            var lines = content.split("\n");
            showTextDialog.vmTitle = lines[0];
            lines.shift();
            content = lines.join("\n");
            showTextDialog.vmContent = content;
            showTextDialog.open();
        }

        if (loader.clearChangeLogFile()){
            titleMsg = viewHome.getErrorTitleAndMessage("error_launcher_update");
            restartDialog.vmContent = titleMsg[1];
            restartDialog.vmTitle = titleMsg[0];
            restartDialog.open();
            closeTimer.start()
        }

        //testErrorDiag("error_db_outofevals",-1);

    }

    // ViewMind Logo
    Image {
        id: logo
        source: "qrc:/images/LOGO.png"
        anchors{
            top: parent.top
            topMargin: 29
            left: parent.left
            leftMargin: 61
        }
    }

    // The head graph
    Image {
        id: headDesign
        source: "qrc:/images/ILUSTRACION.png"
        anchors{
            left: parent.left
            leftMargin: 145
            bottom: parent.bottom
            bottomMargin: 191
        }
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
            width: 178
            height: 43
        }
        contentItem: Text{
            anchors.centerIn: btnConfSettingsRect
            font.family: gothamM.name
            font.pixelSize: 13
            text: loader.getStringForKey(keysearch+"btnConfSettings");
            color: "#88b2d0"
        }
        anchors{
            right: parent.right
            rightMargin: 63
            top: parent.top
            topMargin: 19
        }
        onClicked: {
            viewSettings.open()
        }
    }

    Text {
        id: slideTitle
        font.family: gothamB.name
        font.pixelSize: 43
        anchors.top:  vmBanner.bottom
        anchors.topMargin: 212
        anchors.left: headDesign.right
        anchors.leftMargin: 75
        color: "#297fca"
        text: loader.getStringForKey(keysearch+"slideTitle");
    }

//    // The explanation
//    Text{
//        id: description
//        textFormat: Text.RichText
//        font.pixelSize: 16
//        font.family: robotoR.name
//        color: "#297fca"
//        text: loader.getStringForKey(keysearch+"description");
//        anchors{
//            top: slideTitle.bottom
//            topMargin: 40
//            left: headDesign.right
//            leftMargin: 75
//        }
//    }

    VMButton{
        id: btnGetStarted
        vmText: loader.getStringForKey(keysearch+"btnGetStarted");
        vmFont: gothamM.name
        anchors{
            left: headDesign.right
            leftMargin: 75
            top: slideTitle.bottom
            topMargin: 23
        }
        onClicked: {
            viewDrSelection.open();
        }
    }
}
