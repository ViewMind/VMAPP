import QtQuick 2.0

Item {

    readonly property int baseItemWidth: 110

    ListModel {
        id: elementList
        property var elementArray: []
        function redraw(){
            elementList.clear();
            for (var i = 0; i < elementArray.length; i++){
                elementList.append(elementArray[i]);
            }
        }
    }

    function getListID(){
        var ans = [];
        for (var i = 0; i < elementList.elementArray.length; i++){
            ans.push(elementList.elementArray[i].itemID);
        }
        return ans;
    }

    Rectangle {
        id: container
        radius: 10
        border.color: "#d5d5d5"
        anchors.fill: parent
        color: "#FFFFFF"
    }

    Row {
        anchors.verticalCenter: parent.verticalCenter
        width: container.width*0.9
        anchors.left: container.left
        anchors.leftMargin: container.width*0.05
        spacing: 4
        Repeater {
            model: elementList
            Item {
                width: {
                   var cwidth = container.width / elementList.count;
                   if (cwidth > baseItemWidth) return baseItemWidth;
                   else return cwidth;
                }
                height: container.height*0.8;
                Rectangle {
                    radius: 5
                    border.color: "#3a8bd1"
                    anchors.fill: parent
                    color: "#3a8bd1"
                }
                Text {
                    text: itemText
                    font.family: robotoR.name
                    font.pixelSize: 13
                    color: "#ffffff"
                    anchors.centerIn: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }
    }

    function removeItemByID(itemID){
        for (var i = 0; i < elementList.elementArray.length; i++){
            if (elementList.elementArray[i].itemID === itemID){
                elementList.elementArray.splice(i,1);
                break;
            }
        }
        elementList.redraw();
    }

    function addItem(text, itemID){
        elementList.elementArray.push({ "itemText": text, "itemID" : itemID });
        elementList.redraw();
    }



}