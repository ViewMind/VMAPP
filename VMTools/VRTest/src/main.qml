import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.0
import com.qml 1.0


Window {
    visible: true
    width: 1000
    height: 600
    title: qsTr("Qt-QML-VR Tester")

    Connections{
        target: control
        onNewImageAvailable: {
            hmdView.image = control.image;
        }
//        onCalibrationFinished:{
//            if (control.isCalibrated()){
//                startCalibration.enabled = true;
//                startBinding.enabled = true;
//                startReading.enabled = true
//            }
//        }
    }

    Rectangle {
        id: rectBase
        color: "#3843e0"
        anchors.fill: parent

        QImageDisplay {
            id: hmdView
            width: parent.width*0.9;
            height: parent.height*0.8;
            anchors.top: parent.top
            anchors.topMargin: parent.height*0.01
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Row{

            id: buttonRow
            anchors.top: hmdView.bottom
            anchors.topMargin: 0.01*parent.height
            spacing: 0.01*parent.width
            anchors.horizontalCenter: parent.horizontalCenter

            Button {
                id: startCalibration
                width: rectBase.width*0.125
                height: rectBase.height*0.08
                text: "Start Calibration"
                onClicked: {
                    control.startCalibration();
                    enabled = false;
                }
            }

            Button {
                id: startTest
                width: rectBase.width*0.125
                height: rectBase.height*0.08
                text: "Test calibration"
                onClicked: {
                    control.testEyeTracking();
                }
            }

            Button {
                id: startBinding
                enabled: false
                width: rectBase.width*0.125
                height: rectBase.height*0.08
                text: "Start Binding"
                onClicked: {

                }
            }

            ComboBox {
                id: cbBindingType
                width: startBinding.width
                height: startBinding.height
                model: [ "Bound", "Unbound"]
            }

            Button {
                id: startReading
                //enabled: false
                width: rectBase.width*0.125
                height: rectBase.height*0.08
                text: "Start Reading"
                onClicked: {
                    switch(cbReadingLang.currentIndex){
                    case 0:
                        control.startReadingExperiment("es");
                        break;
                    case 1:
                        control.startReadingExperiment("en");
                        break;
                    case 2:
                        control.startReadingExperiment("de");
                        break;
                    case 3:
                        control.startReadingExperiment("fr");
                        break;
                    }
                }
            }

            ComboBox {
                id: cbReadingLang
                width: startBinding.width
                height: startBinding.height
                model: [ "Spanish", "English", "German", "French"];
            }


        }

        Keys.onPressed: {
            control.keyboardKeyPressed(event.key);
//            switch(event.key){
//            case Qt.Key_S:
//                console.log("Pressed S")
//                break;
//            case Qt.Key_D:
//                console.log("Pressed D")
//                break;
//            case Qt.Key_Escape:
//                console.log("Pressed ESC")
//                break;
//            case Qt.Key_Right:
//                console.log("Pressed RIGHT");
//            }
        }

    }

}
