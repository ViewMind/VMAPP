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

    // Codes
    readonly property int vmAPI_OPINFO_REQUEST: 1
    readonly property int vmAPI_REPORT_REQUEST: 2
    readonly property int vmAPI_UPDATE_REQUEST: 3
    readonly property int vmAPI_OPERATING_INFO_AND_LOG: 6

    readonly property int vmFAIL_CODE_NONE: 0
    readonly property int vmFAIL_CODE_SERVER: 2


    // Screens
    readonly property int vmSwipeIndexHome:             0
    readonly property int vmSwipeIndexLogin:            1
    readonly property int vmSwipeIndexAddEval:          2
    readonly property int vmSwipeIndexMainScreen:       3
    readonly property int vmSwipeIndexAddPatient:       4
    readonly property int vmSwipeIndexEvalView:         5
    readonly property int vmSwipeIndexQCView:           6
    readonly property int vmSwipeIndexEditProtocol:     7
    readonly property int vmSwipeIndexGetVMConfig:      8
    readonly property int vmSwipeIndexInstanceDisabled: 9

    // Notification colors
    readonly property color vmNotificationBlue:       vmBlueSelected
    readonly property color vmNotificationGreen:      "#2DB117"
    readonly property color vmNotificationRed:        "#FD3D60"
    readonly property color vmNotificationBlack:      vmBlackText

    // Study Number Identification
    readonly property int vmINDEX_READING:                    0
    readonly property int vmINDEX_BINDING_BC:                 1
    readonly property int vmINDEX_BINDING_UC:                 2
    readonly property int vmINDEX_NBACKRT:                    4
    readonly property int vmINDEX_NBACKVS:                    5
    readonly property int vmINDEX_PASSBALL:                   6
    readonly property int vmINDEX_GONOGO:                     7
    readonly property int vmINDEX_GONOGO3D:                   8
    readonly property int vmINDEX_NBACK:                      9

    readonly property string vmSTUDY_BINDING:                "Binding";
    readonly property string vmSTUDY_GONOGO :                "Go No-Go";
    readonly property string vmSTUDY_NBACKMS :               "NBack MS";
    readonly property string vmSTUDY_NBACKRT:                "NBack RT";
    readonly property string vmSTUDY_NBACK:                  "NBack";
    readonly property string vmSTUDY_NBACKVS:                "NBack VS";
    readonly property string vmSTUDY_PASSBALL:               "PassBall";
    readonly property string vmSTUDY_READING:                "Reading";
    readonly property string vmSTUDY_GONOGO3D:               "Go No-Go 3D Spheres";

    // Study configuration parameter names.
    readonly property string vmSCP_EYES:                     "valid_eye";
    readonly property string vmSCP_LANGUAGE:                 "language";
    readonly property string vmSCP_NUMBER_OF_TARGETS:        "number_targets";
    readonly property string vmSCP_NUMBER_OF_TRIALS:         "number_of_trials";
    readonly property string vmSCP_TARGET_SIZE:              "target_size";
    readonly property string vmSCP_BINDING_TYPE:             "binding_type";
    readonly property string vmSCP_NBACK_LIGHTUP:            "nback_light_all"
    readonly property string vmSCP_IS_STUDY_3D:              "is_3d_study";
    readonly property string vmSCP_STUDY_REQ_H_CALIB:        "study_reqs_hand_calib";
    readonly property string vmSCP_HAND_TO_USE:              "hand_to_use";
    readonly property string vmSCP_MAX_SPEED:                "max_speed";
    readonly property string vmSCP_MIN_SPEED:                "min_speed";
    readonly property string vmSCP_INITIAL_SPEED:            "initial_speed";
    readonly property string vmSCP_GNG3DSPEED:               "speed"
    readonly property string vmSCP_NBACKRT_HOLD_TIME:        "nback_rt_hold_time";
    readonly property string vmSCP_NBACKRT_TIMEOUT:          "nback_timeout";
    readonly property string vmSCP_NBACKRT_TRANSITION:       "nback_transition";

    // Study configuration parameter values
    readonly property string vmSCV_BINDING_TARGETS_2:        "2";
    readonly property string vmSCV_BINDING_TARGETS_3:        "3";
    readonly property string vmSCV_BINDING_TARGETS_LARGE:    "large";
    readonly property string vmSCV_BINDING_TARGETS_SMALL:    "small";
    readonly property string vmSCV_EYE_BOTH:                 "both";
    readonly property string vmSCV_EYE_LEFT:                 "left";
    readonly property string vmSCV_EYE_RIGHT:                "right";
    readonly property string vmSCV_LANG_DE:                  "German";
    readonly property string vmSCV_LANG_ES:                  "Spanish";
    readonly property string vmSCV_LANG_FR:                  "French";
    readonly property string vmSCV_LANG_EN:                  "English";
    readonly property string vmSCV_LANG_IS:                  "Iselandic";
    readonly property string vmSCV_BINDING_TYPE_BOUND:       "bound";
    readonly property string vmSCV_BINDING_TYPE_UNBOUND:     "unbound";
    readonly property string vmSCV_NBACKVS_TARGETS_3:        "3";
    readonly property string vmSCV_NBACKVS_TARGETS_4:        "4";
    readonly property string vmSCV_NBACKVS_TARGETS_5:        "5";
    readonly property string vmSCV_NBACKVS_TARGETS_6:        "6";

    // Option Key Names for The Study Configuration options
    readonly property string vmSCO_OPTION_NAME:              "option_name"
    readonly property string vmSCO_OPTION_VAlUES:            "option_values"
    readonly property string vmSCO_OPTION_SELECTED:          "option_selected"
    readonly property string vmSCO_OPTION_WIDTH:             "option_width"

    // This is only for UI unique identification
    readonly property string vmUNIQUE_STUDY_ID :             "unique_study_id";


}
