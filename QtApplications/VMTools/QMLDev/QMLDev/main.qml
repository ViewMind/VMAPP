import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.0
import "DateStuff.js" as DS

ApplicationWindow {

    id: mainWindow

    visible: true
    //visibility: Window.Maximized
    width: 1000
    height: 800
    title: qsTr("Hello World")


//    VMComboBox {
//        id: testbox
//        width: 839.0399999999997
//        height: 89.31099999999999
//        anchors.horizontalCenter: parent.horizontalCenter
//        anchors.top: parent.top
//        anchors.topMargin: 20
//        Component.onCompleted: {

//            var list = []
//            //for (var i = 0; i < 8; i++){
//            for (var i = 0; i < 100; i++){
//                list.push("Item " + i);
//            }

//            testbox.setModelList(list);

//            var dateString = "2021-08-25";
//            var dateObject = new Date(dateString);
//            console.log(dateObject.toLocaleDateString(Locale.ShortFormat));

//        }
//    }

    Button {
        id: myTestButton
        width: 100
        height: 50
        text: "Click Me!"
        anchors.centerIn: parent
        hoverEnabled: false
        onClicked: {
            console.log("Button Was clicked")
            DS.setDate("21/03/2020 18:45","dd/MM/yyyy HH:mm");
            console.log(DS.getDate2())
        }
    }

//    VMComboBox2 {
//        id: testbox2
//        width: 839.0399999999997
//        height: 89.31099999999999
//        anchors.top: testbox.bottom
//        anchors.topMargin: width*0.5;
//        anchors.left: testbox.left
//        Component.onCompleted: {

//            var list = []
//            //for (var i = 0; i < 8; i++){
//            for (var i = 0; i < 100; i++){
//                list.push("Item " + i);
//            }

//            testbox2.model = list;


//        }

//    }


}
