import QtQuick 2.0
import QtQuick.Controls 1.4 as Controls1
import QtQuick 2.6

Item {

    property string vmFont: "Mono"
    property string vmPlaceHolder: "Some text ..."
    property string vmEnteredText: ""
    property string vmErrorMsg: ""
    property bool vmNumbersOnly: false
    property bool vmCalendarInput : false;
    property bool vmFocus: false
    property bool vmEnableDateCheck: true

    // For now, we only support ONE date format.
    readonly property string vmDateFormat: "DD/MM/YYYY"

    height: 40

    function clear(){
        labelText.visible = false;
        lineEdit.text = vmPlaceHolder + " - (" + vmDateFormat + ")"
        vmEnteredText = "";
    }

    function setISODate(text){
        var parts = text.split("-")
        var dateText = parts[2] + "/" + parts[1] + "/" +parts[0];
        labelText.visible = true;
        lineEdit.text = dateText;
        vmEnteredText = dateText;
    }

    function getISODate(){
        var parts = vmEnteredText.split("/");
        if (parts.length !== 3) return "";
        return parts[2] + "-" + parts[1] + "-" + parts[0]
    }

    Rectangle {
        id: lineEditRect
        anchors.fill: parent
        color: "#ffffff"
        border.width: (vmErrorMsg === "")? 0 : 2
        border.color: "#e4f1fd"
        radius: 2
        layer.enabled: true
        clip: true
    }

    Rectangle {
        id: subLine
        width: lineEditRect.width
        height: 1;
        border.color: "#297fca"
        color: "#297fca"
        anchors.bottom: lineEditRect.bottom
    }

    TextInput {
        property bool vmIgnoreTextChanged: false

        id: lineEdit
        focus: vmFocus
        text: vmPlaceHolder + " - (" + vmDateFormat + ")"
        color: (labelText.visible)? "#58595b" : "#cfcfcf"
        font.family: vmFont
        font.pixelSize: 13
        anchors.bottom: subLine.top
        anchors.bottomMargin: 5
        verticalAlignment: TextInput.AlignVCenter
        leftPadding: 0
        width: lineEditRect.width
        readOnly: vmCalendarInput

        function setDate(date){
            var day = date.getDate();
            var month = date.getMonth() + 1;
            var year = date.getFullYear();
            var ans = "";
            if (day < 10) ans = "0" + day;
            else ans = day;
            if (month < 10) ans = ans + "/0" + month;
            else ans = ans + "/" + month;
            ans = ans + "/" + year;
            vmIgnoreTextChanged = true;
            lineEdit.text = ans;
            vmIgnoreTextChanged = false;
            vmEnteredText = ans;
        }
        onActiveFocusChanged: {
            if (activeFocus){
                vmErrorMsg = "";
                if (vmCalendarInput){
                    calendarPopUp.visible = true;
                    lineEdit.text = "";
                    labelText.visible = true;
                }
                else{                    
                    if (vmEnteredText === ""){
                        // Removing the placeholder and making the labelText visible
                        lineEdit.text = "";
                        labelText.visible = true;
                    }
                }
            }
            else{
                calendarPopUp.visible = false;
                if (vmEnteredText === ""){
                    labelText.visible = false;
                    lineEdit.text = vmPlaceHolder + " - (" + vmDateFormat + ")"
                }
                else{
                    lineEdit.text = vmEnteredText;
                }
                vmFocus = false;
            }
        }


        onTextChanged: {

            if (vmCalendarInput) return;
            if (vmIgnoreTextChanged) return;

            if (labelText.visible){
                if (lineEdit.text !== ""){
                    var temp = lineEdit.text;
                    if (temp.length < vmEnteredText.length){
                        // This means that the user is deleting the text
                        vmEnteredText = temp;
                        return;
                    }
                    // Max length check
                    if (temp.length == 11){
                        temp = temp.slice(0,-1);
                        vmIgnoreTextChanged = true;
                        lineEdit.text = temp;
                        vmIgnoreTextChanged = false;
                    }
                    if (!isNaN(temp[temp.length-1])){
                        // Valid input. Formatting
                        vmEnteredText = temp
                        vmIgnoreTextChanged = true;
                        switch(temp.length){
                        case 2: lineEdit.text = lineEdit.text + "/";
                            break;
                        case 5: lineEdit.text = lineEdit.text + "/"
                            break;
                        }
                        vmIgnoreTextChanged = false;
                    }
                    else{
                        // Removing the last character
                        temp = temp.slice(0,-1);
                        lineEdit.text = temp;
                    }
                }
                //console.log("vmEnteredText is " + vmEnteredText);
            }
        }

        onEditingFinished: {
            if (vmCalendarInput) return;

            vmEnteredText = lineEdit.text;
            if (lineEdit.text === ""){
                labelText.visible = false;
                lineEdit.text = vmPlaceHolder + " - (" + vmDateFormat + ")"
            }
            else{
                // Checking validity
                var reset = false;

                // First for length
                if (lineEdit.text.length != vmDateFormat.length){
                    reset = true;
                }
                else if (vmEnableDateCheck){

                    // Splitting the date
                    var parts = lineEdit.text.split("/")
                    var day = parseInt(parts[0]);
                    var month = parseInt(parts[1]);
                    var year = parseInt(parts[2]);

                    var cdate = new Date();
                    // Javascript returns the year - 1900
                    var cyear = cdate.getYear() + 1900;

                    if ((year >= cyear) || ((cyear-year) > 120)){
                        // This is an implausible year.
                        reset = true;
                    }
                    else if ((month >= 1) && (month <= 12)){
                        var maxDay = 31;
                        if (month === 2) maxDay = 28;
                        else if ((month === 4) || (month === 6) || (month === 9) || (month === 11)) maxDay = 30;
                        if ((day < 1) || (day > maxDay)) reset = true;
                    }
                    else{
                        reset = true;
                    }
                }

                if (reset){
                    vmEnteredText = "";
                    labelText.visible = false;
                    lineEdit.text = vmPlaceHolder + " - (" + vmDateFormat + ")"
                }
            }
        }

    }

    Text{
        id: labelText
        text: vmPlaceHolder
        color:  "#297fca"
        font.family: vmFont
        font.pixelSize: 11
        anchors.left: lineEditRect.left
        anchors.bottom: lineEdit.top
        anchors.bottomMargin: 5
        visible: (vmEnteredText !== "")
    }

    Text{
        id: errorMsg
        text: vmErrorMsg
        color:  "#ca2026"
        font.family: vmFont
        font.pixelSize: 12
        anchors.left: lineEditRect.left
        anchors.top: lineEditRect.bottom
        anchors.topMargin: 5
        visible: (vmErrorMsg !== "")
    }

    Rectangle {
        id: calendarPopUp
        visible: false;
        border.width: 0
        color: "#ffffff"
        anchors.left: parent.right
        anchors.top: parent.top
        width: parent.width
        VMComboBox{
            id: yearPicker
            width: parent.width
            vmModel: {
                var ans = [];
                var cdate = new Date();
                var cyear = cdate.getYear() + 1900;
                var fyear = cyear - 120;
                for (var i = cyear; i >= fyear; i--){
                    ans.push(i)
                }
                calendar.minimumDate = new Date(fyear, 0, 1);
                calendar.maximumDate = new Date(cyear, 0, 1);
                return ans;
            }
            font.family: viewHome.robotoR.name
            anchors.top: parent.top
            anchors.topMargin: 2
            anchors.horizontalCenter: parent.horizontalCenter
            onCurrentIndexChanged: {
                calendar.visibleYear = 2018 - yearPicker.currentIndex;
            }
        }
        Controls1.Calendar {
            id: calendar
            width: parent.width
            anchors.top: yearPicker.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            onDoubleClicked: {
                calendarPopUp.visible = false;
                var sdate = calendar.selectedDate;
                lineEdit.setDate(sdate);
                labelText.visible = true;
            }
        }
    }

}
