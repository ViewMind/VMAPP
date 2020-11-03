import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.0
import com.qml 1.0


Window {
    visible: true
    ///width: 1920
    //height: 1080
    title: qsTr("Qt-QML-VR Tester")
    visibility: Window.Maximized

    Connections{
        target: control
        onNewImageAvailable: {
            hmdView.image = control.image;
        }
    }

    Rectangle {
        id: rectBase
        color: "#3843e0"
        anchors.fill: parent

        property int elementWidth: rectBase.width*0.10
        property int elementHeight: rectBase.height*0.03

        QImageDisplay {
            id: hmdView
            width: parent.width*0.9;
            height: parent.height*0.8;
            anchors.top: parent.top
            anchors.topMargin: parent.height*0.01
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Column{
            id: btnColumn
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: hmdView.bottom
            anchors.topMargin: 0.01*parent.height

            spacing: parent.height*0.01

            Row{

                id: calibrationRow
                spacing: 0.01*parent.width
                anchors.horizontalCenter: parent.horizontalCenter

                Button {
                    id: startUpHeadSet
                    width: rectBase.elementWidth
                    height: rectBase.elementHeight
                    text: "Initialize"
                    onClicked: {
                        control.initialize();
                        enabled = false;
                    }
                }

                Button {
                    id: startCalibration
                    width: rectBase.elementWidth
                    height: rectBase.elementHeight
                    text: "Start Calibration"
                    onClicked: {
                        control.startCalibration();
                    }
                }

                Button {
                    id: startTest
                    width: rectBase.elementWidth
                    height: rectBase.elementHeight
                    text: "Test calibration"
                    onClicked: {
                        control.testEyeTracking();
                    }
                }

                Button {
                    id: loadLastCalibration
                    width: rectBase.elementWidth
                    height: rectBase.elementHeight
                    text: "Load Last Calibration"
                    onClicked: {
                        control.loadLastCalibration();
                    }
                }


                Button {
                    id: testViewMindWaitScreen
                    width: rectBase.elementWidth
                    height: rectBase.elementHeight
                    text: "View Mind Wait Screen"
                    onClicked: {
                        control.loadViewMindWaitScreen();
                    }
                }


            }

            Row{

                id: readinRow
                spacing: 0.01*parent.width
                anchors.horizontalCenter: parent.horizontalCenter


                Button {
                    id: startReading
                    width: rectBase.elementWidth
                    height: rectBase.elementHeight
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

                Button {
                    id: startFielding
                    width: rectBase.elementWidth
                    height: rectBase.elementHeight
                    text: "Start Fielding"
                    onClicked: {
                        control.startFieldingExperiment();
                    }
                }

                Button {
                    id: startGoNoGo
                    width: rectBase.elementWidth
                    height: rectBase.elementHeight
                    text: "Start Go No Go"
                    onClicked: {
                        control.startGoNoGoExperiment();
                    }
                }

            }

            Row{

                id: bindingRow
                spacing: 0.01*parent.width
                anchors.horizontalCenter: parent.horizontalCenter

                Button {
                    id: startBinding
                    width: rectBase.elementWidth
                    height: rectBase.elementHeight
                    text: "Start Binding"
                    onClicked: {
                        control.startBindingExperiment(
                                    cbBindingType.currentIndex == 0,            // Is Bound?
                                    cbBindingTargetNum.currentIndex + 2,        // How many targets?
                                    cbBindingSize.currentIndex == 1             // Are Targets Small?
                                    );
                    }
                }

                ComboBox {
                    id: cbBindingType
                    width: rectBase.elementWidth
                    height: rectBase.elementHeight
                    model: [ "Bound", "Unbound"]
                }

                ComboBox {
                    id: cbBindingSize
                    width: rectBase.elementWidth
                    height: rectBase.elementHeight
                    model: [ "Grandes", "Pequeños"]
                }

                ComboBox {
                    id: cbBindingTargetNum
                    width: rectBase.elementWidth
                    height: rectBase.elementHeight
                    model: [ "2", "3"];
                }

            }

            Row{

                id: nbackRow
                spacing: 0.01*parent.width
                anchors.horizontalCenter: parent.horizontalCenter

                Button {
                    id: startNBackRT
                    width: rectBase.elementWidth
                    height: rectBase.elementHeight
                    text: "Start NBack RT"
                    onClicked: {
                        control.startNBackRTExperiment(cbSeqLengthNback.currentIndex+3,cbNBackType.currentIndex);
                    }
                }

                ComboBox {
                    id: cbSeqLengthNback
                    width: startBinding.width
                    height: startBinding.height
                    model: [ "3", "4", "5", "6"];
                }


                ComboBox {
                    id: cbNBackType
                    width: startBinding.width
                    height: startBinding.height
                    model: [ "Training" , "Standard"];
                }

            }
        }

        Keys.onPressed: {
            control.keyboardKeyPressed(event.key);
        }

    }

}
