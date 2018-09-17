import QtQuick 2.6
import QtQuick.Controls 2.3

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
        viewWaitFor.updateText()
        viewPatList.updateText()
    }

    Component.onCompleted: {
        // Checking that the everything was loaded correctly.
        if (loader.getLoaderError()){
            viewHome.vmErrorDiag.vmErrorCode = viewHome.vmErrorDiag.vmERROR_LOAD_CONFIG;
            var titleMsg = getErrorTitleAndMessage("error_loading_config");
            console.log("Title msg is " + titleMsg);
            viewHome.vmErrorDiag.vmErrorMessage = titleMsg[1];
            viewHome.vmErrorDiag.vmErrorTitle = titleMsg[0];
            viewHome.vmErrorDiag.open();
        }
        else if (!loader.hasValidOutputRepo()){
            viewSettings.vmInvalidRepoError = true;
            viewSettings.open();
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

    // The explanation
    Text{
        id: description
        textFormat: Text.RichText
        font.pixelSize: 16
        font.family: robotoR.name
        color: "#297fca"
        text: loader.getStringForKey(keysearch+"description");
        anchors{
            top: slideTitle.bottom
            topMargin: 40
            left: headDesign.right
            leftMargin: 75
        }
    }

    VMButton{
        id: btnGetStarted
        vmText: loader.getStringForKey(keysearch+"btnGetStarted");
        vmFont: gothamM.name
        anchors{
            left: headDesign.right
            leftMargin: 75
            top: description.bottom
            topMargin: 53
        }
        onClicked: {
            viewDrSelection.open();
        }
    }
}
