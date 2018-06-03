import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2

Window {
    visible: true
    title: qsTr("EyeExperimenter - ") + loader.getWindowTilteVersion()
    id: mainWindow
    minimumHeight: 690
    maximumHeight: 690
    maximumWidth: 1280
    minimumWidth: 1280

    Component.onCompleted: {
        flowControl.resolutionCalculations();
        var frameGeomtry = loader.frameSize(mainWindow)
        var hdiff = frameGeomtry.height - height - 10;
        x = (Screen.width - width)/2;
        y = hdiff; // This should put the window at the top of the screen.
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
        readonly property int vmIndexWaitFor: 6

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

        Item{
            ViewWaitFor{
                id: viewWaitFor
                anchors.fill: parent
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
                case vmIndexWaitFor:
                    viewWaitFor.openDiag();
                    break;
            }
        }

    }





}
