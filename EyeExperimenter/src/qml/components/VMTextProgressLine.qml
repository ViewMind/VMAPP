import QtQuick
import QtQuick.Controls
import "../"

Rectangle {

    id: progressLine
    color: "transparent"
    //color: "#ff0000"

    /**
      Simplified version fo the progress line. This one the Idea is to start out with a text and then connect each of the lines of
      text to the one above, except the first one.
      The line should be in the center of the text and the current item should be highlighted.
      For this to work the with and the height MUST be set.
     **/

    property int vmCurrentIndex: 0
    property int vmItemHeight: 0
    property int vmLineWidth: 0
    property int vmVerticalMargin: 0

    ListModel {
        id: items;
    }

    // The setup only requires an item list.

    function setup(textList){


        items.clear();
        for (let i in textList){
            items.append({"text" : textList[i]})
        }
        vmCurrentIndex = 0;

        // The width and height must have been set. So we need do do the math, to compute the rendering constants.
        textMeasure.text = "AAAAA";
        let textH = textMeasure.height;
        let nItems = textList.length;
        vmItemHeight = (height - textH)/(nItems - 1);
        vmVerticalMargin = 0.07*vmItemHeight;

        // This should be  constant.
        vmLineWidth = VMGlobals.adjustWidth(4);

    }

    function reset(){
        vmCurrentIndex = 0;
    }

    function indicateNext(){
        if (vmCurrentIndex < (items.count - 1))
            vmCurrentIndex++;
    }

    function getCurrentText(){
        if (items.count > 1){
            if (vmCurrentIndex < items.count){
                return items.get(vmCurrentIndex).text;
            }
        }
        return "";
    }

    TextMetrics {
        id: textMeasure
        font.weight: 400
        font.pixelSize: VMGlobals.vmFontLarger
    }

    Column {

        spacing: 0
        anchors.fill: parent

        Repeater {

            id: repeater
            model: items
            anchors.fill: parent

            Item {
                id: textAndLine
                height: (index === 0) ? (text.height)  : vmItemHeight
                width: progressLine.width

                Rectangle {
                    id: line
                    width: vmLineWidth;
                    height: (index === 0)? 0 : vmItemHeight - text.height - 2*vmVerticalMargin;
                    anchors.top: textAndLine.top
                    anchors.horizontalCenter: textAndLine.horizontalCenter
                    anchors.topMargin: vmVerticalMargin
                    color: text.color
                    radius: line.width/2;
                }

                Text {
                    id: text
                    text: items.get(index).text
                    color: (index === vmCurrentIndex) ? VMGlobals.vmBlueSelected : VMGlobals.vmGrayPlaceholderText
                    font: textMeasure.font
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    width: progressLine.width
                    anchors.horizontalCenter: textAndLine.horizontalCenter
                    anchors.top: (index === 0)? textAndLine.top : line.bottom
                    anchors.topMargin: (index === 0)? 0 : vmVerticalMargin
                }

            }
        }

    }

}

