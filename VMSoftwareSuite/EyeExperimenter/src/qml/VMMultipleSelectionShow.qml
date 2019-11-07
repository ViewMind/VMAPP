import QtQuick 2.0

Item {

    readonly property int baseItemWidth: 160

    ListModel {
        id: elementList
        property var elementArray: []
        function redraw(){
            elementList.clear();
            for (var i = 0; i < elementArray.length; i++){
                elementArray[i].itemOrderIndex = i + 1;
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
        spacing: mainWindow.width*0.003
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
                    border.color: "#505050"
                    anchors.fill: parent
                    color: "#505050"
                }
                Text {
                    text: itemOrderIndex + " - " + itemText
                    font.family: robotoR.name
                    font.pixelSize: 13*viewHome.vmScale
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
        elementList.elementArray.push({ "itemText": text, "itemID" : itemID, "itemOrderIndex": 0});
        elementList.redraw();
    }



}
