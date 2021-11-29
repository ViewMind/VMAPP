pragma Singleton
import QtQuick
QtObject {

    function adjustWidth(value){
        return value*mainWidth/(1366-60)
    }

    function adjustHeight(value){
        return value*mainHeight/(768-30)
    }

    // These should be over written on boot up.
    property double mainHeight: 100
    property double mainWidth:  100

    // The most important constant. All sizes (fonts, buttons, lists, everythign) is mostly computed based on this value.
    property double vmReferenceControlHeight: 44
    property double vmControlsHeight: vmReferenceControlHeight*mainHeight/(768 - 30)

    readonly property double vmFontReference: 14
    readonly property double vmFontBaseSize: Math.round(vmFontReference*vmControlsHeight/44)
    readonly property double vmFontLarge: vmFontBaseSize*16/vmFontReference
    readonly property double vmFontLarger: vmFontBaseSize*18/vmFontReference
    readonly property double vmFontVeryLarge: vmFontBaseSize*24/vmFontReference
    readonly property double vmFontExtraLarge: vmFontBaseSize*28/vmFontReference
    readonly property double vmFontExtraExtraLarge: vmFontBaseSize*32/vmFontReference
    readonly property double vmFontHuge: vmFontBaseSize*42/vmFontReference

    // Colors
    readonly property color vmBlueSelected:             "#3397ef"
    readonly property color vmBlueBackgroundHighlight:  "#F7FBFF"
    readonly property color vmBlueTextHighlight:        "#3397EF"
    readonly property color vmBlueButtonHighlight:      "#5AAFFB"
    readonly property color vmBluePanelBKGSelected:     "#DDEAF6"
    readonly property color vmBlueButtonPressed:        "#2883D3"


    readonly property color vmGrayUnselectedBorder:     "#D8DDE8"
    readonly property color vmGrayAccented:             "#939393"
    readonly property color vmGrayPlaceholderText:      "#686868"
    readonly property color vmGrayTranslucent:          "#1a3d3d3d"
    readonly property color vmGrayToggleOff:            "#F9F9F9"
    readonly property color vmGrayDialogOverlay:        "#aa333333"
    readonly property color vmGrayHoverButton:          "#F1F3F9"
    readonly property color vmGrayButtonPressed:        "#E6EAEE"
    readonly property color vmGrayTextButtonDisabled:   "#C6CCD9"
    readonly property color vmGrayDialogDivider:        "#C4C4C4"
    readonly property color vmGrayLightGrayText:        "#BAC0CF"

    readonly property color vmWhite:                    "#ffffff"
    readonly property color vmWhite50Translucent:       "#80ffffff"
    readonly property color vmWhite100Translucent:      "#ffffffff"
    readonly property color vmWhiteMainBackground:      "#EEF9FE"
    readonly property color vmWhiteButtonHiglight:      "#F6FBFF"

    readonly property color vmBlackText:                "#242424"

    readonly property color vmRedErrorBackground:       "#0DFD3D60"
    readonly property color vmRedError:                 "#FD3D60"

    // Codes
    readonly property int vmAPI_OPINFO_REQUEST: 1
    readonly property int vmAPI_UPDATE_REQUEST: 3

    // Screens
    readonly property int vmSwipeIndexHome:  0
    readonly property int vmSwipeIndexLogin: 1

}
