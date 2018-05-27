import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2

Window {
    visible: true
    title: qsTr("EyeExperimenter")
    minimumHeight: 768
    maximumHeight: 768
    maximumWidth: 1280
    minimumWidth: 1280

    SwipeView {

        readonly property int vmIndexHome: 0
        readonly property int vmIndexPatientReg: 1
        readonly property int vmIndexCalibrationStart: 2
        readonly property int vmIndexCalibrationDone: 3
        readonly property int vmIndexPresentExperiment: 4

        id: swiperControl
        currentIndex: vmIndexHome
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
