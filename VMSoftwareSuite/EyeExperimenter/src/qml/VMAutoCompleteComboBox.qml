import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 2.1

Item {

    // VMLIST: Needs to have two fields for each item: text and value. This way it will return one or the other.

    property string vmLabel : ""
    property var vmList: []
    property var vmValues: []
    property string vmCurrentText: ""
    property string vmCurrentValue: ""
    property int vmMaxOptions: 10;
    property bool vmFocus: false
    property bool vmEnabled: true

    function filter(){
        // Creating the model list
        modelList.clear()

        var count = 0;
        for (var i = 0; i < vmList.length; i++){
           if ((vmList[i].toLowerCase().search(lineEdit.text.toLowerCase()) !== -1) || (lineEdit.text === "")){
               var data = "";
               if (i < vmValues.length) data = vmValues[i];
               modelList.append({"vmText": vmList[i], "vmData": data, "vmIndex": count});
               count++;
               if (count == vmMaxOptions) break;
           }
        }

        // Updating the height of the patient list.
        listView.height = lineEditRect.height*(count+1);
    }


    function setCurrentText(ctext){
        for (var i = 0; i < vmList.length; i++){
           if (vmList[i] === ctext){
               var data = "";
               if (i < vmValues.length) data = vmValues[i];
               vmCurrentText = ctext;
               vmCurrentValue = data;
               lineEdit.vmFilterEnable = false;
               lineEdit.text = vmCurrentText;
               lineEdit.vmFilterEnable = true;
               break;
           }
        }
    }

    function setCurrentIndex(index){
        if ((index >= 0) && (index < vmList.length)){
            vmCurrentText = vmList[index];
            var data = "";
            if (index < vmValues.length) data = vmValues[index];
            lineEdit.vmFilterEnable = false;
            lineEdit.text = vmCurrentText;
            lineEdit.vmFilterEnable = true;
        }
    }

    Rectangle {
        id: lineEditRect
        anchors.fill: parent
        color: "#ebf3fa"
        border.width: mainWindow.width*0.0
    }

    ListModel {
        id: modelList
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
        property bool vmFilterEnable: true
        property bool vmSelected: false
        id: lineEdit
        focus: vmFocus
        enabled: vmEnabled
        font.family: viewHome.robotoR.name
        font.pixelSize: 13*viewHome.vmScale
        anchors.bottom: subLine.top
        anchors.bottomMargin: mainWindow.height*0.007
        verticalAlignment: TextInput.AlignVCenter
        leftPadding: 5
        width: lineEditRect.width
        onTextChanged: {
            if (!vmFilterEnable ) return;
            filter();
        }
        onCursorVisibleChanged: {
            listView.visible = cursorVisible;
            if (cursorVisible){
                vmSelected = false;
                filter();
            }
            else{
                if (!vmSelected){
                    // Return to previously made selection
                    lineEdit.vmFilterEnable = false;
                    lineEdit.text = vmCurrentText
                    lineEdit.vmFilterEnable = true;
                }
            }
        }
        onActiveFocusChanged: if (!activeFocus) vmFocus = false;
    }

    Text{
        id: labelText
        text: vmLabel
        color:  "#297fca"
        font.family: viewHome.robotoR.name
        font.pixelSize: 11*viewHome.vmScale
        anchors.left: lineEditRect.left
        anchors.bottom: lineEditRect.top
        anchors.bottomMargin: mainWindow.height*0.007
        visible: (vmLabel != "")
    }

    ListView {
        id: listView
        width: parent.width
        model: modelList
        anchors.top: lineEditRect.bottom
        anchors.left: lineEditRect.left
        delegate: VMAutoCompleteCBEntry {
            width: lineEditRect.width
            height: lineEditRect.height
            layer.enabled: true
            clip: true
            onClicked: {
                //console.log("Clicked on text: " + text + " that has data " + data + " with index " + index);
                vmCurrentText = text;
                vmCurrentValue = data;
                lineEdit.vmFilterEnable = false;
                lineEdit.text = text;
                lineEdit.vmFilterEnable = true;
                lineEdit.vmSelected = true;
                lineEdit.focus = false;
            }
        }
    }

}
