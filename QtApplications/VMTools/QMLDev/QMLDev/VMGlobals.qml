import QtQuick
import QtQuick.Controls

Item {

    function adjustWidth(value){
        return value*mainWidth/(1366-60)
    }

    function adjustHeight(value){
        return value*mainHeight/(768-30)
    }

    // These should be over written on boot up.
    property double mainHeight: 100
    property double mainWidth:  100

    property double vmReferenceControlHeight: 44
    property double vmControlsHeight: vmReferenceControlHeight*mainHeight/(768 - 30)

    readonly property double vmFontReference: 14
    readonly property double vmFontSmall: vmFontBaseSize*12/vmFontReference
    readonly property double vmFontBaseSize: Math.round(vmFontReference*vmControlsHeight/44)
    readonly property double vmFontLarge: vmFontBaseSize*16/vmFontReference
    readonly property double vmFontLarger: vmFontBaseSize*18/vmFontReference
    readonly property double vmFontVeryLarge: vmFontBaseSize*24/vmFontReference
    readonly property double vmFontExtraLarge: vmFontBaseSize*28/vmFontReference
    readonly property double vmFontExtraExtraLarge: vmFontBaseSize*32/vmFontReference
    readonly property double vmFontHuge: vmFontBaseSize*42/vmFontReference


    // Colors
    readonly property string vmBlueSelected:             "#3397ef"
    readonly property string vmBlueBackgroundHighlight:  "#F7FBFF"
    readonly property string vmBlueTextHighlight:        "#3397EF"
    readonly property string vmBlueButtonHighlight:      "#5AAFFB"

    readonly property string vmGrayUnselectedBorder:     "#D8DDE8"
    readonly property string vmGrayAccented:             "#939393"
    readonly property string vmGrayPlaceholderText:      "#686868"
    readonly property string vmGrayTranslucent:          "#1a3d3d3d"
    readonly property string vmGrayToggleOff:            "#F9F9F9"

    readonly property string vmWhite:                    "#ffffff"
    readonly property string vmWhite50Translucent:       "#80ffffff"

    readonly property string vmBlackText:                "#242424"

    readonly property string vmRedErrorBackground:       "#0DFD3D60"
    readonly property string vmRedError:                 "#FD3D60"


}
