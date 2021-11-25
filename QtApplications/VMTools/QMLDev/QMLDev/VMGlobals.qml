import QtQuick
import QtQuick.Controls

Item {

    property double vmControlsHeight: 44
    readonly property double vmFontBaseSize: Math.round(14*vmControlsHeight/44)
    readonly property double vmFontLarge: vmFontBaseSize*1.14

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
