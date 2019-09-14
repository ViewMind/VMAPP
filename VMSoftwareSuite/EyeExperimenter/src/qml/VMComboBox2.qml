import QtQuick 2.6
import QtQuick.Controls 2.3

Item {

    id: vmComboBox
    height: 30

    property string vmErrorMsg: ""
    property bool vmEnabled: true
    property int vmMaxDisplayItems: 5
    property int vmCurrentIndex: -1
    property string vmCurrentText: ""
    property int vmListSize: 0

    function setModelList(list){
        for (var i = 0; i < list.length; i++){
            modelList.append({"vmText": list[i], "vmIndex": i});
        }
        vmListSize = list.length
        setSelection(0)
    }

    ListModel {
        id: modelList
    }

    MouseArea {
        id: mouseTitleDetector
        anchors.fill: parent
        onClicked: {
            if (vmEnabled){
                listContainer.visible = true
            }
        }
    }

    function setSelection(selectedIndex){
        vmCurrentIndex = selectedIndex;
        displayText.text = modelList.get(selectedIndex).vmText;
        vmCurrentText = displayText.text;
        listContainer.visible = false;
    }

    ///////////////////// MAIN DISPLAY
    Rectangle {
        id: display
        anchors.fill: parent
        color:  "#ebf3fa"

        // The display Text.
        Text {
            id: displayText
            text: "Test Text"
            font.family: viewHome.robotoR.name
            font.pixelSize: 13
            color: "#58595b"
            verticalAlignment: Text.AlignVCenter
            anchors.bottom: divisorLine.top
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 5
        }

        // The divisor line at the bottom.
        Rectangle{
            id: divisorLine
            color: "#297fca"
            border.color: "#297fca"
            border.width: 0;
            height: 1;
            width: parent.width
            anchors.bottom: parent.bottom
        }

        // The triangle that functions as an indicator.
        Canvas {
            id: canvas
            width: 12
            height: 8
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 5
            contextType: "2d"

            onPaint: {
                var ctx = canvas.getContext("2d");
                ctx.reset();
                ctx.moveTo(0, 0);
                ctx.lineTo(width, 0);
                ctx.lineTo(width / 2, height);
                ctx.closePath();
                ctx.fillStyle = "#000000";
                ctx.fill();
            }
        }

    }

    ///////////////////// DROP DOWN
    Rectangle {
        id: listContainer
        color: "#dadada"
        anchors.top: display.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width
        height: {
            var numDisplay = vmMaxDisplayItems;
            if (numDisplay > modelList.count) numDisplay = modelList.count;
            return numDisplay*vmComboBox.height;
        }
        visible: false

        ScrollView {
            id: listContainerScroll
            //anchors.fill: parent
            width: parent.width
            height: parent.height-2;
            clip: true
            ListView {
                id: listContainerListView
                anchors.fill: parent
                model: modelList
                delegate: Rectangle {
                    width: vmComboBox.width
                    height: vmComboBox.height
                    border.width: 2
                    border.color: "#dadada"
                    color: mouseItemDetector.containsMouse? "#eeeeee" : "#ffffff"

                    MouseArea {
                        id: mouseItemDetector
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            setSelection(vmIndex);
                        }
                    }

                    Text {
                        id: textItem
                        text: vmText
                        font.family: viewHome.robotoR.name
                        font.pixelSize: 13
                        color: "#58595b"
                        verticalAlignment: Text.AlignVCenter
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.leftMargin: 5
                    }
                }
            }
        }

    }


    //////////////////// THE ERROR MESSAGE
    Text{
        id: errorMsg
        text: vmErrorMsg
        color:  "#ca2026"
        font.family: viewHome.robotoR.name
        font.pixelSize: 12
        anchors.left: parent.left
        anchors.top: parent.bottom
        anchors.topMargin: 5
        z: vmComboBox.z - 1
        visible: (vmErrorMsg !== "")
    }
}
