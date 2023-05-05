import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"
import "../subscreens"

ViewBase {

    id: qcView

    readonly property int vmMAX_L_COMMMENT_LINES: 19
    readonly property int vmMAX_C_COMMENT_CHARS: 92

    readonly property int vmSC_INDEX_SEND_REPORT:    0
    readonly property int vmSC_INDEX_ARCHIVE:        1

    function configurePatientInformation(name, id, reportdate, study_name, toSend){
        var patient = loader.getStringForKey("viewevaluation_patient") + ",<br>";
        patient = patient + "<b>" + name + "</b>";
        //console.log("Setting patient to " + patient)
        patientText.text = patient;
        personalIDValue.text = id;
        dateValue.text = reportdate
        studyValue.text = study_name
        if (toSend){
            viewer.currentIndex = vmSC_INDEX_SEND_REPORT
        }
        else {
            viewer.currentIndex = vmSC_INDEX_ARCHIVE
        }
    }

    VMButton {
        id: backButton
        vmText: loader.getStringForKey("viewlogin_back")
        vmIconSource: "arrow"
        vmButtonType: backButton.vmTypeTertiary
        anchors.bottom: parent.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(592)
        anchors.left: parent.left
        anchors.leftMargin: VMGlobals.adjustWidth(5)
        onClickSignal: {
            mainWindow.swipeTo(VMGlobals.vmSwipeIndexMainScreen)
        }
    }


    Text {
        id: patientText
        font.pixelSize: VMGlobals.vmFontVeryLarge
        font.weight: 400
        /// VERY VERY IMPORTANT. Qt BUG: So here is what happens. When I developed this rich text formatting got disabled if
        /// before I set the text string a context form a Canvas object set a font. The work around I found is to simply 'have'
        /// HTML text, BEFORE the Canvas does it's operations and then when over writing the text, it shows up correctly.
        /// I managed to build a MWE (which I have since deleted). Steps to reproduce:
        ///    Create a Text QML object with "<b>Bold</b><br>Not Bold" (call it id: text)
        ///    Create a Canvas and on the onPaint method, get the canvas (ctx variable) and do cxt.font = "10px sans-serif"
        ///    Create a button. On the onClicked Method of the button set the text.text = "<b>More Bold</b><br>More Not Bold"
        ///    This will work.
        ///    Now simply comment the text property in the id: text object and re run.
        ///    When pressing the button, the text will appear but it won't be in bold.
        text: "Patient,<br><b>patientname</b>"
        textFormat: Text.RichText
        color: VMGlobals.vmBlackText
        height: VMGlobals.adjustHeight(64)
        verticalAlignment: Text.AlignVCenter
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(511)
        anchors.leftMargin: VMGlobals.adjustWidth(15)
    }

    Row {
        id: personalIDRow
        height: VMGlobals.adjustHeight(20)
        anchors.top: patientText.bottom
        anchors.left: patientText.left
        anchors.topMargin: VMGlobals.adjustHeight(14)
        spacing: VMGlobals.adjustWidth(5)

        Text {
            id: personalIDName
            height: parent.height
            verticalAlignment: Text.AlignVCenter
            text: loader.getStringForKey("viewpatlist_id") + ":"
            color: VMGlobals.vmGrayPlaceholderText
            font.pixelSize: VMGlobals.vmFontBaseSize
            font.weight: 400
        }
        Text {
            id: personalIDValue
            height: parent.height
            verticalAlignment: Text.AlignVCenter
            color: VMGlobals.vmBlackText
            font.pixelSize: VMGlobals.vmFontBaseSize
            font.weight: 400
        }
    }

    Row {
        id: dateRow
        height: VMGlobals.adjustHeight(20)
        anchors.top: personalIDRow.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)
        anchors.left: patientText.left
        spacing: VMGlobals.adjustWidth(5)

        Text {
            id: dateName
            text: loader.getStringForKey("viewevaluation_date") + ":"
            height: parent.height
            verticalAlignment: Text.AlignVCenter
            color: VMGlobals.vmGrayPlaceholderText
            font.pixelSize: VMGlobals.vmFontBaseSize
            font.weight: 400
        }
        Text {
            id: dateValue
            height: parent.height
            verticalAlignment: Text.AlignVCenter
            color: VMGlobals.vmBlackText
            font.pixelSize: VMGlobals.vmFontBaseSize
            font.weight: 400
        }
    }

    Row {
        id: studyRow
        height: VMGlobals.adjustHeight(20)
        anchors.top: dateRow.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)
        anchors.left: patientText.left
        spacing: VMGlobals.adjustWidth(5)

        Text {
            id: studyName
            text: loader.getStringForKey("viewqc_study") + ":"
            height: parent.height
            verticalAlignment: Text.AlignVCenter
            color: VMGlobals.vmGrayPlaceholderText
            font.pixelSize: VMGlobals.vmFontBaseSize
            font.weight: 400
        }
        Text {
            id: studyValue
            height: parent.height
            verticalAlignment: Text.AlignVCenter
            color: VMGlobals.vmBlackText
            font.pixelSize: VMGlobals.vmFontBaseSize
            font.weight: 400
        }
    }

    Rectangle {
        property double vmBorderPadding: VMGlobals.adjustWidth(1)
        //property double vmBorderPadding: VMGlobals.adjustWidth(10)
        id: mainRect
        clip: true
        radius: VMGlobals.adjustHeight(10)
        width: VMGlobals.adjustWidth(969);
        height: VMGlobals.adjustHeight(670+10-84);
        anchors.right: parent.right
        anchors.rightMargin: VMGlobals.adjustWidth(15)
        anchors.bottom: horizontalDivider.top
        anchors.bottomMargin: -radius
        border.width: vmBorderPadding
        border.color: VMGlobals.vmGrayUnselectedBorder

        SwipeView {
            id: viewer
            interactive: false
            currentIndex: 0
            width: mainRect.width - mainRect.vmBorderPadding*2
            height: mainRect.height - mainRect.radius - mainRect.vmBorderPadding
            x: mainRect.vmBorderPadding
            y: mainRect.vmBorderPadding

            Item {
                SCSendReport {
                    id: sendReport
                    radius: mainRect.radius
                    border.width:  mainRect.border.width
                    border.color: mainRect.border.color
                    //border.color: "#ff0000"
                    width: parent.width
                    height: parent.height + radius
                }
            }

            Item {
                SCArchive {
                    id: archiveReport
                    radius: mainRect.radius
                    border.width:  mainRect.border.width
                    border.color: mainRect.border.color
                    width: parent.width
                    height: parent.height + radius
                }
            }

            onCurrentIndexChanged: {
                if (currentIndex === vmSC_INDEX_ARCHIVE) archiveReport.reset()
                else if (currentIndex === vmSC_INDEX_SEND_REPORT) sendReport.reset();
            }

        }


    }

    Rectangle {
        id: horizontalDivider
        height: VMGlobals.adjustHeight(84-1)
        anchors.right: mainRect.right
        anchors.bottom: parent.bottom
        width: mainRect.width
        border.width: VMGlobals.adjustHeight(1)
        border.color: VMGlobals.vmGrayUnselectedBorder

        VMButton {
            id: nextButton
            vmText: loader.getStringForKey("viewqc_send")
            //vmIconSource: qcgraphs.vmIsLastGraph ? "" : "next"
            vmIconSource: "next"
            vmIconToTheRight: true
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: VMGlobals.adjustWidth(29)
            visible: (viewer.currentIndex !== vmSC_INDEX_ARCHIVE)
            onClickSignal: {
                sendReport.sendToProcess();
            }
        }

        VMButton {
            id: confirmArchiveButton
            vmText: loader.getStringForKey("viewqc_confirm_archive")
            visible: (viewer.currentIndex === vmSC_INDEX_ARCHIVE)
            vmButtonType: confirmArchiveButton.vmTypeWarning
            vmIconSource: "qrc:/images/archive-white.png"
            anchors.verticalCenter: parent.verticalCenter
            anchors.right : parent.right
            anchors.rightMargin: VMGlobals.adjustWidth(29)
            onClickSignal: {
                archiveReport.archive();
            }
        }

    }

}
