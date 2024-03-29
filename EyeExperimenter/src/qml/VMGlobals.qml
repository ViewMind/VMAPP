pragma Singleton
import QtQuick

QtObject {

    function adjustWidth(value){
        return value*mainWidth/(1366-60)
    }

    function adjustHeight(value){
        return value*mainHeight/(768-30)
    }

    function stringReplaceAll(string_to_replace, replace_with, main_string){
        let result = main_string
        while (true){
            let temp = result.replace(string_to_replace,replace_with)
            if (temp === result) break; // there were no changes. We replaced all.
            result = temp;
        }
        return result
    }

    // These should be over written on boot up.
    property double mainHeight: 100
    property double mainWidth:  100

    // The most important constant. All sizes (fonts, buttons, lists, everythign) is mostly computed based on this value.
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
    readonly property color vmBlueSelected:             "#3397ef"
    readonly property color vmBlueBackgroundHighlight:  "#F7FBFF"
    readonly property color vmBlueTextHighlight:        "#3397EF"
    readonly property color vmBlueButtonHighlight:      "#5AAFFB"
    readonly property color vmBluePanelBKGSelected:     "#DDEAF6"
    readonly property color vmBlueButtonPressed:        "#2883D3"
    readonly property color vmBlueNotSortIndicator:     "#B7D4ED"
    readonly property color vmBlueVeryLightBkg:         "#EEF7FF";


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
    readonly property color vmGrayStudyDivisor:         "#828996"
    readonly property color vmGrayTooltipBackground:    "#F2F2F2"

    readonly property color vmWhite:                    "#ffffff"
    readonly property color vmWhite50Translucent:       "#80ffffff"
    readonly property color vmWhite100Translucent:      "#ffffffff"
    readonly property color vmWhiteMainBackground:      "#EEF9FE"
    readonly property color vmWhiteButtonHiglight:      "#F6FBFF"

    readonly property color vmGreenBKGStudyMessages:    "#EAF7E8"
    readonly property color vmGreenSolidQCIndicator:    "#2DB117"

    readonly property color vmBlackText:                "#242424"

    readonly property color vmRedErrorBackground:       "#0DFD3D60"
    readonly property color vmRedCalibrationFailed:     "#3AFD3D60"
    readonly property color vmRedError:                 "#FD3D60"
    readonly property color vmRedBadIndexBackground:    "#FFF5F7"

    // API Last Request Codes
    readonly property int vmAPI_OPERATING_INFO         : 1;
    readonly property int vmAPI_REQUEST_REPORT         : 2;
    readonly property int vmAPI_REQUEST_UPDATE         : 3;
    readonly property int vmAPI_ACTIVATE               : 4;
    readonly property int vmAPI_OPERATING_INFO_AND_LOG : 6;
    readonly property int vmAPI_SENT_SUPPORT_EMAIL     : 7;
    readonly property int vmAPI_FUNC_CTL_NEW           : 8;
    readonly property int vmAPI_FUNC_CTL_HMD_CHANGE    : 9;

    // HTTP Codes used by API
    readonly property int vmHTTP_CODE_OK                          : 200;
    readonly property int vmHTTP_CODE_INSTANCE_DISABLED           : 599;
    readonly property int vmHTTP_CODE_FUNC_VERIF_FAIL_BAD_PASSWD  : 700;
    readonly property int vmHTTP_CODE_FUNC_VERIF_FAIL_UNAUTHUSER  : 701;
    readonly property int vmHTTP_CODE_FUNC_VERIF_FAIL_BADSN       : 702;
    readonly property int vmHTTP_CODE_ACT_FAILED_NOT_LIBERATED    : 600;
    readonly property int vmHTTP_CODE_ACT_FAILED_INVALID_SN       : 601;

    // Indicate server failure or not.
    readonly property int vmFAIL_CODE_NONE: 0
    readonly property int vmFAIL_CODE_SERVER: 2

    // References to the task codes.
    readonly property string vmTASK_BINDING_BC_2_SHORT               : "binding_bc_2_short";
    readonly property string vmTASK_BINDING_UC_2_SHORT               : "binding_uc_2_short";
    readonly property string vmTASK_BINDING_BC_3_SHORT               : "binding_bc_3_short";
    readonly property string vmTASK_BINDING_UC_3_SHORT               : "binding_uc_3_short";
    readonly property string vmTASK_NBACK_3                          : "nback_3";
    readonly property string vmTASK_NBACK_4                          : "nback_4";
    readonly property string vmTASK_GONOGO                           : "gonogo";
    readonly property string vmTASK_SPHERES                          : "spheres";
    readonly property string vmTASK_MOVING_DOT                       : "moving_dot";
    readonly property string vmTASK_SPHERES_VS                       : "spheres_vs";

    // Screens
    readonly property int vmSwipeIndexHome:             0
    readonly property int vmSwipeIndexLogin:            1
    readonly property int vmSwipeIndexAddEval:          2
    readonly property int vmSwipeIndexMainScreen:       3
    readonly property int vmSwipeIndexAddPatient:       4
    readonly property int vmSwipeIndexEvalView:         5
    readonly property int vmSwipeIndexEditProtocol:     6
    readonly property int vmSwipeIndexGetVMConfig:      7
    readonly property int vmSwipeIndexInstanceDisabled: 8
    readonly property int vmSwipeIndexFunctionalVerif:  9

    // Notification colors
    readonly property color vmNotificationBlue:       vmBlueSelected
    readonly property color vmNotificationGreen:      "#2DB117"
    readonly property color vmNotificationRed:        "#FD3D60"
    readonly property color vmNotificationBlack:      vmBlackText

}
