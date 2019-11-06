import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2

Window {
    visible: true
    title: qsTr("EyeExperimenter - ") + loader.getWindowTilteVersion()
    id: mainWindow
    visibility: Window.Maximized

    Component.onCompleted: {        
        flowControl.resolutionCalculations();
        //var frameGeomtry = loader.frameSize(mainWindow)
        //console.log(frameGeomtry)
        minimumHeight = height;
        maximumHeight = height;
        minimumWidth = width;
        maximumWidth = width;
        //var hdiff = frameGeomtry.height - height - 10;
        //x = (Screen.width - width)/2;
        //x = 0;
        //y = hdiff; // This should put the window at the top of the screen.
        swiperControl.currentIndex = swiperControl.vmIndexHome;

        //swiperControl.currentIndex = swiperControl.vmIndexPatientList;
        //swiperControl.currentIndex = swiperControl.vmIndexPresentExperiment;
        //swiperControl.currentIndex = swiperControl.vmIndexPatientReg;
        //swiperControl.currentIndex = swiperControl.vmIndexStudyStart;
        //swiperControl.currentIndex = swiperControl.vmIndexResults;
        //swiperControl.currentIndex = swiperControl.vmIndexShowReports;
        //swiperControl.currentIndex = swiperControl.vmIndexMedicalInformation;
    }


    // The configurations dialog.
    ViewSettings{
        id: viewSettings
        x: 333
        y: (parent.height - viewSettings.height)/2
        onUpdateMenus: {
            viewHome.updateDrMenuText();
        }
    }

    ViewDoctorSelection {
        id: viewDrSelection
        x: (parent.width - viewDrSelection.width)/2
        y: (parent.height - viewDrSelection.height)/2
    }

    ViewProtocols{
        id: viewProtocols
        x: (parent.width - viewProtocols.width)/2
        y: (parent.height - viewProtocols.height)/2
    }

    SwipeView {

        readonly property int vmIndexHome: 0
        readonly property int vmIndexDrProfile: 1
        readonly property int vmIndexPatientList: 2
        readonly property int vmIndexShowReports: 3
        readonly property int vmIndexStudyStart: 4
        readonly property int vmIndexPatientReg: 5
        readonly property int vmIndexMedicalRecordList: 6
        readonly property int vmIndexMedicalInformation: 7
        readonly property int vmIndexCalibrationStart: 8
        readonly property int vmIndexCalibrationDone: 9
        readonly property int vmIndexPresentExperiment: 10
        readonly property int vmIndexResults: 11
        readonly property int vmIndexStudyDone: 12

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
            ViewDoctorProfile{
                id: viewDrInfo
                //isHomePage: true;
                anchors.fill: parent
            }
        }

        Item {
            ViewPatientList {
                id: viewPatList
                anchors.fill: parent
            }
        }

        Item{
            ViewShowReports{
                id: viewShowReports
                anchors.fill: parent
            }
        }

        Item{
            ViewStudyStart{
                id: viewStudyStart
                anchors.fill: parent
            }
        }

        Item{
            ViewPatientRegistration{
                id: viewPatientReg
                anchors.fill: parent
            }
        }

        Item{
            ViewMedicalRecordList{
                id: viewMedRecordList
                anchors.fill: parent
            }
        }

        Item{
            ViewMedicalInformation{
                id: viewMedicalInformation
                anchors.fill: parent
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
            }
        }

        Item{
            ViewResults{
                id: viewResults
                anchors.fill: parent
            }
        }

        Item{
            ViewStudyDone{
                id: viewStudyDone
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
            case vmIndexPresentExperiment:
                // The continue button is disabled to avoid pressing space or enter and starting the experiment again.
                // viewStudyStart.testPresentExperimentScreen()
                viewPresentExperimet.enableContinue();
                break;
            case vmIndexPatientList:
                //loader.setValueForConfiguration(viewHome.vmDefines.vmCONFIG_DOCTOR_UID,"0_0000_D0000");
                //loader.setValueForConfiguration(viewHome.vmDefines.vmCONFIG_DOCTOR_NAME,"Test Doctor");
                // This will laod patients which will also trigger the search for unprocessed information.
                //viewPatList.test();
                //viewPatList.openAskPasswordDialog();
                viewPatList.loadPatients();
                break;
            case vmIndexShowReports:
                viewShowReports.loadReportsForPatient();
                break;
            case vmIndexStudyStart:
                viewStudyStart.setPatientName();
                viewStudyStart.setDefaultSelections();
                break;
            case vmIndexResults:
                viewResults.fillFieldsFromReportInfo();
                break
            case vmIndexMedicalRecordList:
                viewMedRecordList.loadPatientRecordList();
                break;
//            case vmIndexStudyDone:
//                //viewStudyDone.openDiag();
//                break;
            }

        }

    }

}
