import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2

Window {
    visible: true
    title: qsTr("EyeExperimenter - ") + loader.getWindowTilteVersion()
    minimumHeight: 768
    maximumHeight: 768
    maximumWidth: 1280
    minimumWidth: 1280
//    x: 0
//    y: 0


    Component.onCompleted: {
        console.log("SCREEN " + Screen.width + "x" + Screen.height);
        console.log("TOTAL SCREEN " + Screen.desktopAvailableWidth + "x" + Screen.desktopAvailableHeight);
        loader.setScreenResolution(Screen.width, Screen.height);
    }

    // The configurations dialog.
    ViewSettings{
        id: viewSettings
        x: 333
        y: (parent.height - viewSettings.height)/2
        onUpdateMenus: {
            viewPatientReg.updateMenus();
            viewPresentExperimet.updateMenus();
            viewResults.updateMenus();
        }
    }

    SwipeView {

        readonly property int vmIndexHome: 0
        readonly property int vmIndexPatientReg: 1
        readonly property int vmIndexCalibrationStart: 2
        readonly property int vmIndexCalibrationDone: 3
        readonly property int vmIndexPresentExperiment: 4
        readonly property int vmIndexResults: 5

        id: swiperControl
        currentIndex: vmIndexHome
        interactive: false
        anchors.fill: parent

        Item{
            ViewHome{
                id: viewHome
                isHomePage: true;
                anchors.fill: parent
            }
        }

        Item{
            ViewPatientReg{
                id: viewPatientReg
                anchors.fill: parent
                onBackButtonPressed: swiperControl.currentIndex = swiperControl.vmIndexHome
                Component.onCompleted: {
                    updateMenus();
                }
            }
        }

        Item{
            ViewCalibration{
                id: viewCalibrationStart
                anchors.fill: parent
            }
        }

        Item{
            ViewCalibrationDone{
                id: viewCalibrationDone
                anchors.fill: parent
            }
        }

        Item{
            ViewPresentExperiment{
                id: viewPresentExperimet
                anchors.fill: parent
                onBackButtonPressed: swiperControl.currentIndex = swiperControl.vmIndexPatientReg
                Component.onCompleted: {
                    updateMenus();
                }
            }
        }

        Item{
            ViewResults{
                id: viewResults
                anchors.fill: parent
                onBackButtonPressed: swiperControl.currentIndex = swiperControl.vmIndexPatientReg
                Component.onCompleted: {
                    updateMenus();
                }
            }
        }

        onCurrentIndexChanged: {
            switch(currentIndex){
                case vmIndexCalibrationStart:
                    viewCalibrationStart.openDiag()
                    break;
                case vmIndexCalibrationDone:
                    viewCalibrationDone.openDiag()
                    break;
            }
        }

    }





}
