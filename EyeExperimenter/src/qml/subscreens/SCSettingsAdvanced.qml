import QtQuick
import "../"
import "../components"

Rectangle {

    id: settingsAdvanced
    anchors.fill: parent
    radius: VMGlobals.adjustWidth(10);
    color: VMGlobals.vmGrayToggleOff

    property string vmCurrentLang: ""
    property int vmLoadedCountry: -1
    property bool vmRestartRequired: false
    property bool vmAnyChanges: false

    function saveData(){
        loader.setSettingsValue("ui_language",uiLanguage.vmCurrentText);
    }

    function reset(){
        vmAnyChanges = false;
        vmRestartRequired = false;
        vmLoadedCountry = loader.getDefaultCountry(false)
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
        Component.onCompleted: {
            var langs = loader.getStringListForKey("viewsettings_langs")
            setModelList(loader.getStringListForKey("viewsettings_langs"))
            var lang = loader.getConfigurationString("ui_language")
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

}
