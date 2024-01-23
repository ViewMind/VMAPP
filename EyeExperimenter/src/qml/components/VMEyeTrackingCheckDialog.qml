import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"

Item {
    id: eyeTrackerCheckDialog;
    anchors.fill: parent
    visible: false
    clip: true
    z: 10


    readonly property int vmSTATE_CHECKING: 0
    readonly property int vmSTATE_PASS: 1
    readonly property int vmSTATE_FAIL: 2
    readonly property string vmKEY_TO_CHECK: "update_sampling_frequecy"

    readonly property int vmINTENT_CALIBRATE: 0
    readonly property int vmINTENT_CHECK: 1

    property int vmIntent: vmINTENT_CALIBRATE

    property int vmState: 0

    signal dismissed(allok: bool);

    Connections {
        target: flowControl
        function onNotifyLoader(data){
            //console.log("Got Loader Notification for " + JSON.stringify(data));
            if (vmState !== vmSTATE_CHECKING) return; // Nothing to do.
            if (!visible) return; // Nothing to do
            if (vmKEY_TO_CHECK in data){
                if (data[vmKEY_TO_CHECK] > 100){
                    // We are good.
                    setCheckOK();
                }
            }
        }
    }

    ////////////////////////// BASE DIALOG FUNCTIONS ////////////////////////
    function open(intent){

        vmIntent = intent

        if (vmIntent === vmINTENT_CALIBRATE) flowControl.hideRenderWindow();

        vmState = vmSTATE_CHECKING
        visible = true

        if (flowControl.isSkipETCheckEnabled()){
            console.log("DBUG: Skipping ET Check");
            setCheckOK();
        }
        else {
            timer.running = true;
        }
    }

    function setCheckOK(){
        timer.running = false;
        vmState = vmSTATE_PASS;
        // If all is good we automatically move on.
        if (vmIntent === vmINTENT_CHECK) return;
        close();
        dismissed(true)
    }

    function close(){
        visible = false
        if (vmIntent === vmINTENT_CALIBRATE) flowControl.showRenderWindow();
    }

    MouseArea {
        id: mouseCatcher
        anchors.fill: parent
        hoverEnabled: true
    }

    Rectangle {
        id: overlay
        color: VMGlobals.vmGrayDialogOverlay
        anchors.fill: parent
    }

    Rectangle {
        id: dialog
        width: VMGlobals.adjustWidth(615)
        height: {

            let h = 0;
            let noTextOrButton = true;
            if (messageText.visible){
                h = h + messageText.height + messageText.anchors.topMargin*2
                noTextOrButton = false;
            }
            if ((okButton.visible) || (goBackButton.visible)) {
                h = h + okButton.height + okButton.anchors.topMargin*2
                noTextOrButton = false;
            }

            let m = 1;
            if (noTextOrButton) {
                m = 2;
            }

            h = h + dialogTitle.height + dialogTitle.anchors.topMargin*m;
            h = h + divider.height + divider.anchors.topMargin*m
            return h;

        }
        anchors.centerIn: parent
        radius: VMGlobals.adjustWidth(10);
        color: "#ffffff"
    }

    /////////////////////////////////////////////////////////////////////

    Text {
        id: dialogTitle
        color: VMGlobals.vmBlackText
        font.weight: 400
        font.pixelSize: VMGlobals.vmFontExtraLarge
        text: {
            if (vmState === vmSTATE_FAIL) return loader.getStringForKey("viewevaluation_checket_title_fail")
            else if (vmState === vmSTATE_PASS) return loader.getStringForKey("viewevaluation_checket_title_pass")
            else return loader.getStringForKey("viewevaluation_checket_title")
        }
        height: VMGlobals.adjustHeight(37)
        verticalAlignment: Text.AlignVCenter
        anchors.top: dialog.top
        anchors.topMargin: VMGlobals.adjustHeight(34)
        anchors.left: dialog.left //infoIcon.right
        anchors.leftMargin: VMGlobals.adjustWidth(22.74)
    }

    AnimatedImage {
        id: waitingAnimation
        source: "qrc:/images/loader.gif"
        height: dialogTitle.height
        fillMode: Image.PreserveAspectFit
        anchors.verticalCenter: dialogTitle.verticalCenter
        anchors.right: dialog.right
        anchors.rightMargin: dialogTitle.anchors.leftMargin
        visible: vmState == vmSTATE_CHECKING
    }

    Timer {
        id: timer
        interval: 8000
        running: false
        repeat: false
        onTriggered: {
            // This means there was a time out.
            timer.running = false;
            vmState = vmSTATE_FAIL;
            loader.logUIMessage("Failed ET Check",true);
        }
    }

    Image {
        id: result
        source: {
            if (vmState == vmSTATE_FAIL) return "qrc:/images/alert-triangle-red.png"
            else return "qrc:/images/check-circle.png"
        }
        height: dialogTitle.height
        fillMode: Image.PreserveAspectFit
        anchors.verticalCenter: dialogTitle.verticalCenter
        anchors.right: dialog.right
        anchors.rightMargin: dialogTitle.anchors.leftMargin
        visible: !waitingAnimation.visible
    }


    Text {
        id: messageText
        text: loader.getStringForKey("viewevaluation_checket_fail")
        font.pixelSize: VMGlobals.vmFontLarge
        font.weight: 400
        color: VMGlobals.vmGrayPlaceholderText
        anchors.top: dialogTitle.bottom
        anchors.topMargin: VMGlobals.adjustHeight(15)
        anchors.left: dialogTitle.left
        visible: vmState === vmSTATE_FAIL;
    }

    Rectangle {
        id: divider
        width: dialog.width
        height: VMGlobals.adjustHeight(1)
        anchors.top: {
            if (messageText.visible) messageText.bottom
            else dialogTitle.bottom
        }
        anchors.topMargin: VMGlobals.adjustHeight(20)
        anchors.horizontalCenter: dialog.horizontalCenter
        border.width: 0
        color: VMGlobals.vmGrayDialogDivider
    }

    VMButton {
        id: okButton
        vmText: loader.getStringForKey("viewevaluation_checket_btnfail")
        anchors.top: divider.top
        anchors.topMargin: VMGlobals.adjustHeight(10)
        anchors.right: dialog.right
        anchors.rightMargin: VMGlobals.adjustWidth(30)
        visible: (vmState === vmSTATE_FAIL)
        onClickSignal: {
            open(vmIntent);
        }
    }

    VMButton {
        id: goBackButton
        vmButtonType: goBackButton.vmTypeSecondary
        vmText: (vmIntent === vmINTENT_CHECK)? loader.getStringForKey("update_close") :  loader.getStringForKey("viewevaluation_checket_btnback")
        anchors.top: divider.top
        anchors.topMargin: VMGlobals.adjustHeight(10)
        anchors.left: dialog.left
        anchors.leftMargin: VMGlobals.adjustWidth(30)
        visible: (vmState === vmSTATE_FAIL) || ((vmState != vmSTATE_CHECKING) && (vmIntent === vmINTENT_CHECK))
        onClickSignal: {
            close();
            dismissed(false)
        }
    }

}
