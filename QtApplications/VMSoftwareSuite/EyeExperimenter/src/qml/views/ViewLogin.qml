import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"

ViewBase {

    id: loginView

    function updateProfileList(){
        var ans = loader.getLoginEmails();
        evaluatorSelection.setModelList(ans)
        evaluatorSelection.setSelection(-1)
    }

    Text {
        id: title
        text: loader.getStringForKey("viewlogin_title")
        font.pixelSize: VMGlobals.vmFontExtraExtraLarge
        font.weight: 600
        color: VMGlobals.vmBlackText
        height: VMGlobals.adjustHeight(43)
        verticalAlignment: Text.AlignVCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(607)
    }

    VMButton {
        id: backButton
        vmText: loader.getStringForKey("viewlogin_back")
        vmIconSource: "arrow"
        vmButtonType: backButton.vmTypeTertiary
        anchors.verticalCenter: title.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: VMGlobals.adjustWidth(5)
        onClickSignal: {
            mainWindow.swipeTo(VMGlobals.vmSwipeIndexHome)
        }
    }

    Text {
        id: subtTitle
        text: loader.getStringForKey("viewlogin_subtitle")
        font.pixelSize: VMGlobals.vmFontLarge
        font.weight: 400
        color: VMGlobals.vmGrayPlaceholderText
        verticalAlignment: Text.AlignVCenter
        height: VMGlobals.adjustHeight(18)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: title.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)
    }

    VMComboBox {
        id: evaluatorSelection
        vmLabel: loader.getStringForKey("viewlogin_label_evaluator")
        vmPlaceHolderText: loader.getStringForKey("viewlogin_placeholder_evaluator")
        width: VMGlobals.adjustWidth(320)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: subtTitle.bottom
        anchors.topMargin: VMGlobals.adjustHeight(54)
        z: 1
    }

    VMPasswordInput {
        id: password
        width: evaluatorSelection.width
        vmLabel: loader.getStringForKey("viewlogin_label_pass")
        vmPlaceHolderText: loader.getStringForKey("viewlogin_placeholder_password")
        vmShowText: loader.getStringForKey("viewlogin_show")
        vmHideText: loader.getStringForKey("viewlogin_hide")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: evaluatorSelection.bottom
        anchors.topMargin: VMGlobals.adjustHeight(54)
    }


    VMButton {
        id: loginButton
        vmText: loader.getStringForKey("viewstart_login")
        vmCustomWidth: evaluatorSelection.width
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: password.bottom
        anchors.topMargin: VMGlobals.adjustHeight(30)
        onClickSignal: {

        }
    }

    Text {
        id: orlinetext
        text: loader.getStringForKey("viewlogin_orline")
        font.pixelSize: VMGlobals.vmFontLarger
        font.weight: 400
        verticalAlignment: Text.AlignVCenter
        color: VMGlobals.vmGrayLightGrayText
        anchors.horizontalCenter: parent.horizontalCenter
        height: VMGlobals.adjustHeight(21)
        anchors.top: loginButton.bottom
        anchors.topMargin: VMGlobals.adjustHeight(20)
    }

    Rectangle {
        id: leftDivider
        width: VMGlobals.adjustWidth(140)
        height: VMGlobals.adjustHeight(1)
        border.width: 0
        color: orlinetext.color
        anchors.verticalCenter: orlinetext.verticalCenter
        anchors.left: loginButton.left
    }

    Rectangle {
        id: rightDivider
        width: leftDivider.width
        height: leftDivider.height
        border.width: 0
        color: orlinetext.color
        anchors.verticalCenter: orlinetext.verticalCenter
        anchors.right: loginButton.right
    }


    VMButton {
        id: addNewEval
        vmButtonType: addNewEval.vmTypeSecondary
        vmText: loader.getStringForKey("viewstart_addneweval")
        vmCustomWidth: evaluatorSelection.width
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: loginButton.bottom
        anchors.topMargin: VMGlobals.adjustHeight(61)
        onClickSignal: {

        }
    }

}
