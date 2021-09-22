import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects

VMBase {

    id: viewHome
    width: mainWindow.width
    height: mainWindow.height

    readonly property double vmScale: mainWindow.width/1280;
    readonly property int vmAPI_OPINFO_REQUEST: 1
    readonly property int vmAPI_UPDATE_REQUEST: 3

    Connections {
        target: loader
        function onFinishedRequest () {
            // Close the connection dialog and open the user selection dialog.
            //console.log("Finished request")
            if (loader.getLastAPIRequest() === vmAPI_OPINFO_REQUEST){
                //console.log("Last request was an OP INFO call")
                connectionDialog.close();

                if (loader.getNewUpdateVersionAvailable() !== ""){
                    //console.log("New version Available " + loader.getNewUpdateVersionAvailable())
                    updateRequestDialog.vmVersion = loader.getNewUpdateVersionAvailable()
                    updateRequestDialog.open();
                }
                else {
                    viewDrSelection.open();
                }
            }
            else if (loader.getLastAPIRequest() === vmAPI_UPDATE_REQUEST){
                // If the application got here, we move on. But it means that the update failed.
                connectionDialog.close();
                viewDrSelection.open();
            }
        }
        function onPartnerSequenceDone() {
            connectionDialog.close();
            if (!allok){
                var titleMsg = viewHome.getErrorTitleAndMessage("error_partner_failed");
                vmErrorDiag.vmErrorCode = vmErrorDiag.vmErrorCodeNotClose;
                vmErrorDiag.vmErrorMessage = titleMsg[1];
                vmErrorDiag.vmErrorTitle = titleMsg[0];
                vmErrorDiag.open();
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
            viewHome.vmErrorDiag.vmErrorCode = vmErrorDiag.vmErrorCodeClose;
            var titleMsg = getErrorTitleAndMessage("error_loading_config");
            //console.log("Title msg is " + titleMsg);
            viewHome.vmErrorDiag.vmErrorMessage = titleMsg[1];
            viewHome.vmErrorDiag.vmErrorTitle = titleMsg[0];
            viewHome.vmErrorDiag.open();
            return;
        }

        // If this is the first time running this version the changes are shown.
        if (loader.isFirstTimeRun()){
            var title_and_body = loader.getLatestVersionChanges();
            if (title_and_body.length !== 2) return;
            showTextDialog.vmTitle = title_and_body[0];
            showTextDialog.vmContent = title_and_body[1];
            showTextDialog.open();
        }

    }

    // Dialog used to show a wall of text.
    VMDialogBase {
        id: showTextDialog;
        width: mainWindow.width*0.6
        height: mainWindow.height*0.87

        property string vmContent: ""
        property string vmTitle: ""

        y: (parent.height - height)/2
        x: (parent.width - width)/2

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
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
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

    // Dialog to show the restart message.
    VMDialogBase {
        id: restartDialog;

        width: mainWindow.width*0.48
        height: mainWindow.height*0.29
        vmNoCloseButton: true
        property string vmContent: ""
        property string vmTitle: ""

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

    // Wait dialog for synching up.
    VMDialogBase {

        property string vmTitle: "TITLE"
        property string vmMessage: "MESSAGE"

        id: connectionDialog;
        vmNoCloseButton: true
        width: mainWindow.width*0.48
        height: mainWindow.height*0.87

        onWidthChanged: {
            connectionDialog.repositionSlideAnimation();
        }

        onHeightChanged: {
            connectionDialog.repositionSlideAnimation();
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
            scale: viewHome.vmScale
            visible: true
            transformOrigin: Item.TopLeft
            onScaleChanged: {
                connectionDialog.repositionSlideAnimation();
            }
        }

        function repositionSlideAnimation(){
            slideAnimation.y = (connectionDialog.height - slideAnimation.height*vmScale)/2
            slideAnimation.x = (connectionDialog.width - slideAnimation.width*viewHome.vmScale)/2
        }

    }

    // Update request dialog.
    VMDialogBase {
        id: updateRequestDialog;
        vmNoCloseButton: true
        width: mainWindow.width*0.6
        height: mainWindow.height*0.5

        property string vmVersion: "0.0.0"

        // The title
        Text {
            id: updateRequestTitle
            font.family: viewHome.gothamB.name
            font.pixelSize: 33*viewHome.vmScale
            anchors.top: parent.top
            anchors.topMargin: mainWindow.height*0.128
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#297fca"
            text: loader.getStringForKey("viewhome_updateTitle") + " " + updateRequestDialog.vmVersion;
        }

        // The instruction text
        Text {
            id: updateRequestMessge
            font.family: viewHome.robotoR.name
            font.pixelSize: 13*viewHome.vmScale
            anchors.top:  updateRequestTitle.bottom
            anchors.topMargin: parent.height*0.1
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignHCenter
            color:  "#5f5f5f"
            text:  {
                var N = loader.getRemainingUpdateDenials();
                var message = loader.getStringForKey("viewhome_updateQuestion");
                message = message.replace("=N=",N);
                if (N === 0) btnNo.enabled = false;
                return message;
            }
        }

        // No update button
        VMButton{
            id: btnNo
            vmFont: viewHome.gothamM.name
            vmSize: [parent.width*0.15, parent.height*0.15]
            vmText: loader.getStringForKey("viewhome_updateNo");
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.leftMargin: parent.width*0.05
            anchors.bottomMargin: parent.height*0.05;
            onClicked: {
                loader.updateDenied();
                updateRequestDialog.close();
                viewDrSelection.open();
            }
        }

        // Proceed with update button.
        VMButton{
            id: btnYes
            vmFont: viewHome.gothamM.name
            vmSize: [parent.width*0.15, parent.height*0.15]
            vmText: loader.getStringForKey("viewhome_updateYes");
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.rightMargin: parent.width*0.05
            anchors.bottomMargin: parent.height*0.05;
            onClicked: {
                updateRequestDialog.close();
                var title_and_text = loader.getStringListForKey("msg_get_update");
                //console.log(title_and_text);
                connectionDialog.vmMessage = title_and_text[1];
                connectionDialog.vmTitle = title_and_text[0];
                connectionDialog.open();
                loader.startUpdate();
            }
        }


    }

    // Partner dialog
    VMDialogBase {
        id: partnerDialog;
        width: mainWindow.width*0.4
        height: mainWindow.height*0.5

        // The instruction text
        Text {
            id: partnerTitle
            font.family: viewHome.gothamB.name
            font.pixelSize: 33*viewHome.vmScale
            anchors.top: parent.top
            anchors.topMargin: mainWindow.height*0.05
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#297fca"
            text: loader.getStringForKey("viewpartner_title");
        }

        Text {
            id: partnerSubTitle
            font.family: viewHome.robotoR.name
            font.pixelSize: 13*viewHome.vmScale
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignHCenter
            color:  "#dfdfdf"
            anchors.top: partnerTitle.bottom
            anchors.topMargin: mainWindow.height*0.02
            text: loader.getStringForKey("viewpartner_subTitle");
        }

        VMComboBox2 {
            id: partnerSelection;
            width: parent.width*0.8;
            vmMaxDisplayItems: 3
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: partnerSubTitle.bottom
            anchors.topMargin: mainWindow.height*0.04
            z:2
            Component.onCompleted: {
                partnerSelection.setModelList(loader.getPartnerList());
            }

        }

        VMButton {
            id: btnImport
            vmText: loader.getStringForKey("viewpartner_btnImport");
            vmFont: gothamM.name;
            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.bottom: parent.bottom;
            anchors.bottomMargin: mainWindow.height*0.05;
            onClicked: {
                partnerDialog.close();
                var title_and_text = loader.getStringListForKey("msg_get_partner_data");
                //console.log(title_and_text);
                connectionDialog.vmMessage = title_and_text[1];
                connectionDialog.vmTitle = title_and_text[0];
                connectionDialog.open();
                loader.synchronizeToPartner(partnerSelection.vmCurrentText);
            }
        }

        // Creating the close button
        VMDialogCloseButton {
            id: btnPartnerClose
            anchors.top: parent.top
            anchors.topMargin: mainWindow.height*0.032
            anchors.right: parent.right
            anchors.rightMargin: mainWindow.width*0.019
            onClicked: {
                partnerDialog.close();
            }
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
        scale: vmScale
        transformOrigin: Item.TopLeft
        onScaleChanged: {

            // Scaling makes it a bit unpredictable where the anchors are located so the x and y position are corrected here.
            headDesign.y = (mainWindow.height - headDesign.height*vmScale)/2
            headDesign.x = mainWindow.width*0.05

            // Also the title's x position is computed accordingly
            slideTitle.x = headDesign.x + headDesign.width*vmScale + mainWindow.width*0.058
        }
    }


    // The configure settings button
    Button {
        id: btnConfSettings
        scale: btnConfSettings.pressed? 0.9:1
        width: mainWindow.width*0.139
        height: mainWindow.height*0.062
        hoverEnabled: false
        Behavior on scale{
            NumberAnimation {
                duration: 25
            }
        }
        background: Rectangle {
            id: btnConfSettingsRect
            radius: 3
            color: btnConfSettings.pressed? "#e8f2f8" :"#ffffff"
            width: btnConfSettings.width
            height: btnConfSettings.height
        }
        contentItem: Text{
            anchors.centerIn: parent
            font.family: gothamM.name
            font.pixelSize: 13*viewHome.vmScale
            text: loader.getStringForKey("viewhome_btnConfSettings");
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
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
        color: "#297fca"
        text: loader.getStringForKey("viewhome_slideTitle");
    }

    VMButton{
        id: btnGetStarted
        vmText: loader.getStringForKey("viewhome_btnGetStarted");
        vmFont: gothamM.name
        x: slideTitle.x
        anchors{
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
        }
    }

    // Combo Box To test ComboBox Design
//    VMComboBox2{
//        id: test_combo_box
//        width: mainWindow.width*0.273
//        anchors.top: btnGetStarted.bottom
//        anchors.left: btnGetStarted.left
//        onVmCurrentIndexChanged: {
//            console.log("CurrentIndex Changed.")
//        }
//        Component.onCompleted: {
//            var list = [];
//            for (var i = 0; i < 10; i++){
//                list.push("Item " + i);
//            }
//            test_combo_box.setModelList(list)
//        }
//    }

    VMButton {
        id: btnPartners;
        vmText: loader.getStringForKey("viewhome_btnPartners");
        vmFont: gothamM.name;
        anchors.bottom: parent.bottom;
        anchors.right: parent.right;
        anchors.bottomMargin: mainWindow.height*0.05;
        anchors.rightMargin: mainWindow.width*0.02;
        visible: (loader.getPartnerList().length > 0)
        onClicked: {
            partnerDialog.open();
        }
    }


}
