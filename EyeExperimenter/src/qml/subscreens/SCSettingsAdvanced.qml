import QtQuick
import "../"
import "../components"

Rectangle {

    id: settingsAdvanced
    anchors.fill: parent
    radius: VMGlobals.adjustWidth(10);
    color: VMGlobals.vmGrayToggleOff

    property string vmCurrentLang: ""
    property bool vmRestartRequired: false
    property bool vmAnyChanges: false

    function saveData(){
        loader.setSettingsValue("ui_language",uiLanguage.vmCurrentText);
    }

    function reset(){
        vmAnyChanges = false;
        vmRestartRequired = false;
    }

    // Rectangle that straightens the left side edge.
    Rectangle {
        id: straightDivisor
        height: parent.height
        anchors.left: parent.left
        width: parent.radius
        border.width: 0
        color: parent.color
    }

    // Title
    Text {
        id: title
        text: loader.getStringForKey("viewsettings_advanced");
        font.weight: 600
        font.pixelSize: VMGlobals.vmFontLarger
        height: VMGlobals.adjustHeight(18);
        verticalAlignment: Text.AlignVCenter
        anchors.top: parent.top
        anchors.topMargin: VMGlobals.adjustHeight(27)
        anchors.left: parent.left
        anchors.leftMargin: VMGlobals.adjustWidth(19)
    }


    VMComboBox {
        id: uiLanguage
        vmLabel: loader.getStringForKey("viewsettings_language")
        width: VMGlobals.adjustWidth(350)
        anchors.top: parent.top
        anchors.topMargin: VMGlobals.adjustHeight(106);
        anchors.left: parent.left
        anchors.leftMargin: VMGlobals.adjustWidth(20)
        z: parent.z + 10
        Component.onCompleted: {
            var langs = loader.getStringListForKey("viewsettings_langs")
            setModelList(loader.getStringListForKey("viewsettings_langs"))
            var lang = loader.getSettingsString("ui_language","English");
            //console.log("Current lang is: " + lang);
            for (var i = 0; i < langs.length; i++){
                if (langs[i] === lang){
                    vmCurrentLang = lang
                    uiLanguage.setSelection(i)
                    break;
                }
            }
        }

        onVmCurrentIndexChanged: {
            //console.log("Current lang text changed to: " + vmCurrentText + " while loaded lang is "  + vmCurrentLang)
            vmRestartRequired = (vmCurrentLang != vmCurrentText)
            vmAnyChanges = vmAnyChanges || vmRestartRequired;
        }

    }

    VMButton {
        id: checkEyeTracking
        vmText: loader.getStringForKey("viewsettings_run_et_check")
        width: uiLanguage.width
        anchors.top: uiLanguage.bottom
        anchors.topMargin: VMGlobals.adjustHeight(20)
        anchors.left: uiLanguage.left
        onClickSignal: {
            eyeTrackingSettingsCheck.open(eyeTrackingSettingsCheck.vmINTENT_CHECK);
            settingsDialog.close()
        }
    }

}
