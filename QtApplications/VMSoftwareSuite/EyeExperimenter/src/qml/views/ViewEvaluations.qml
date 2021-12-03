import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"
import "../subscreens"

ViewBase {

    id: evaluationsView

    property string vmSelectedPatientID: ""

    function setPatientForEvaluation(){

        var patientData = loader.getCurrentSubjectInfo();

        var date = new Date();
        var year = date.getFullYear()
        var day  = date.getDay()
        var month = date.getMonth();
        var hour = date.getHours()
        var minutes =  date.getMinutes();
        if (minutes.length < 2) minutes = "0" + minutes;
        var monthlist = loader.getStringListForKey("viewpatlist_months")
        //console.log("Current month as index is " + month)
        //console.log("Month List " + JSON.stringify(monthlist))
        month = monthlist[month]

        var patient = loader.getStringForKey("viewevaluation_patient") + ",<br>";
        patient = patient + "<b>" + patientData["name"] + " " + patientData["lastname"] + "</b>"
        //console.log("Setting patient to " + patient)
        patientText.text = patient;

        dateValue.text = day + " " + month + " " +  year + ", " + hour + ":" + minutes;
        personalIDValue.text = patientData["supplied_institution_id"]

        vmSelectedPatientID = patientData["local_id"]

        if (!loader.setSelectedSubject(vmSelectedPatientID)){
            mainWindow.showErrorMessage("viewevaluation_error_patient_dir")
            mainWindow.swipeTo(VMGlobals.vmSwipeIndexMainScreen)
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

    Rectangle {
        property double vmBorderPadding: VMGlobals.adjustWidth(1)
        id: mainRect
        clip: true
        radius: VMGlobals.adjustHeight(10)
        width: VMGlobals.adjustWidth(969);
        height: VMGlobals.adjustHeight(670+10);
        anchors.right: parent.right
        anchors.rightMargin: VMGlobals.adjustWidth(15)
        anchors.bottom: parent.bottom
        anchors.bottomMargin: -radius
        border.width: vmBorderPadding
        border.color: VMGlobals.vmGrayUnselectedBorder


        Rectangle {
            id: horizontalDivider
            height: VMGlobals.adjustHeight(1)
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.bottomMargin: VMGlobals.adjustHeight(84)
            width: parent.width
            border.width: 0
            color: VMGlobals.vmGrayUnselectedBorder
        }

        VMButton {
            id: nextButton
            vmText: loader.getStringForKey("viewevaluation_next_button")
            vmIconSource: "next"
            vmIconToTheRight: true
            anchors.top: horizontalDivider.top
            anchors.topMargin: VMGlobals.adjustHeight(20)
            anchors.right: parent.right
            anchors.rightMargin: VMGlobals.adjustWidth(29)
            onClickSignal: {
                mainWindow.popUpNotify(VMGlobals.vmNotificationBlue,"<b>Somethingbold</b><br>Something new");
            }
        }

    }

    VMProgressLine {
        id: progressLine
        anchors.top: dateRow.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 0
        width: parent.width - mainRect.width - mainRect.anchors.rightMargin
        height: 500
    }

}
