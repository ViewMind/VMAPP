import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.0

VMBase {

    id: viewPatientList

    readonly property real vmTableWidth: 0.70*mainWindow.width
    readonly property real vmTableHeight: 0.33*mainWindow.height
    readonly property string keybase: "viewpatientlist_"
    property bool disableStudyStart: false;


    Dialog {
        id: studyPreSetup;
        modal: true
        width: mainWindow.width*0.5
        height: mainWindow.height*0.5

        property var doctorList: []
        property var protocolList: []

        y: (parent.height - height)/2
        x: (parent.width - width)/2
        closePolicy: Popup.NoAutoClose

        contentItem: Rectangle {
            id: rectDialog
            anchors.fill: parent
            layer.enabled: true
            layer.effect: DropShadow{
                radius: 5
            }
        }

        function fillComboBoxes(){
            var medics = loader.getMedicList();
            doctorList = [];

            var medic_instruction = loader.getStringForKey(keybase + "destinatary")
            doctorList.push({ "value" : medic_instruction, "metadata" : "-1" });
            for (var key in medics){
                doctorList.push({ "value" : medics[key], "metadata" : key });
            }
            doctorSelection.setModelList(doctorList);

            protocolList = loader.getProtocolList();
            protocolList.unshift(loader.getStringForKey(keybase  + "protocol_inst"));
            protocolSelection.setModelList(protocolList);

            // Setting the selected doctor for the current patient.
            var preferred_doctor = loader.getCurrentlySelectedAssignedDoctor();
            //console.log("Preffered doctor: " + preferred_doctor);
            for (var i in doctorList){
                if (doctorList[i]["metadata"] === preferred_doctor){
                    doctorSelection.setSelection(i);
                    break;
                }
            }

            // Setting the last selected protocol
            var selectedprotocol = loader.getConfigurationString("last_selected_protocol");
            //console.log("Selected protocol: " + selectedprotocol);
            for (i = 0; i < protocolList.length; i++){
                if (protocolList[i] === selectedprotocol){
                    protocolSelection.setSelection(i);
                }
            }

        }

        Column {

            id: selectionColumn
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: parent.height*0.2
            spacing: parent.height*0.1
            z: 3

            Column {

                id: doctorSelectionColumn
                spacing: parent.height*0.02
                z: 4

                Text{
                    id: labelDoctor
                    text: loader.getStringForKey(keybase + "doctor") + " " +  loader.getInstitutionName();
                    color:  "#297fca"
                    font.family: viewHome.gothamM.name
                    font.pixelSize: 16*viewHome.vmScale
                    anchors.left: parent.left
                }

                VMComboBox2 {
                    id: doctorSelection
                    z: 4
                    vmMaxDisplayItems: 3
                    width: studyPreSetup.width*0.8
                }

            }

            Column {

                id: protocolSelectionColumn
                spacing: parent.height*0.02

                Text{
                    id: labelProtocol
                    text: loader.getStringForKey(keybase + "protocol")
                    color:  "#297fca"
                    font.family: viewHome.gothamM.name
                    font.pixelSize: 16*viewHome.vmScale
                    anchors.left: parent.left
                }

                VMComboBox2 {
                    id: protocolSelection
                    z: 3
                    vmMaxDisplayItems: 3
                    width: studyPreSetup.width*0.8
                }

            }

        }


        // Buttons
        Row{
            id: buttonRowDiag
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: parent.height*0.058
            spacing: parent.width*0.3

            VMButton{
                id: btnBackDiag
                height: mainWindow.height*0.072
                vmText: loader.getStringForKey(keybase+"btnBack");
                vmFont: viewHome.gothamM.name
                vmInvertColors: true
                onClicked: {
                    studyPreSetup.close()
                }
            }

            VMButton{
                id: btnStartDiag
                height: mainWindow.height*0.072
                vmText: loader.getStringForKey(keybase+"btnStart");
                vmFont: viewHome.gothamM.name
                enabled: doctorSelection.vmCurrentIndex > 0;
                onClicked: {
                    // SET the protocol and the doctor.
                    if (protocolSelection.vmCurrentIndex > 0 ) viewStudyStart.vmSelectedProtocol = protocolSelection.vmCurrentText
                    else viewStudyStart.vmSelectedProtocol = "";
                    viewStudyStart.vmSelectedMedic = studyPreSetup.doctorList[doctorSelection.vmCurrentIndex]["metadata"]
                    loader.setSettingsValue("last_selected_protocol",protocolSelection.vmCurrentText);
                    studyPreSetup.close();
                    swiperControl.currentIndex = swiperControl.vmIndexStudyStart;
                }
            }
        }

    }


    ListModel {
        id: patientList
    }

    // Loads the list model with the patient information
    function loadPatients(filterText) {

        if (filterText === undefined){
            searchBar.clearSearch();
            filterText = "";
        }

        // Getting the filtered list of patient map with all the info.
        var patientNameList = loader.filterSubjectList(filterText);

        // Clearing the current model.
        patientList.clear()

        var index = 0;
        for (var key in patientNameList) {
            var map = patientNameList[key];
            map["vmIsSelected"] = false;
            map["vmItemIndex"] = index;
            index++;
            patientList.append(map);
        }

        patientListView.currentIndex = -1;

        if (!flowControl.isVROk()){
            btnStart.enabled = false;
            disableStudyStart = true;
            vmErrorDiag.vmErrorCode = vmErrorDiag.vmErrorCodeNotClose;
            var titleMsg = viewHome.getErrorTitleAndMessage("error_vr_init_failed");
            vmErrorDiag.vmErrorMessage = titleMsg[1];
            vmErrorDiag.vmErrorTitle = titleMsg[0];
            vmErrorDiag.open();
            return;
        }

    }

    function setCurrentPatient(){
        if (patientListView.currentIndex == -1) return;

        //console.log("Pat name: " + patientList.get(patientListView.currentIndex).vmPatientName)
        //var displayName = patientList.get(patientListView.currentIndex).name + " " + patientList.get(patientListView.currentIndex).lastname;
        // Settign the titles for the medical record list and the medical record info screen.
        //viewMedRecordList.vmPatientName = displayName;
        //viewMedicalInformation.vmPatientName = displayName;

        if (!loader.setSelectedSubject(patientList.get(patientListView.currentIndex).local_id)){
            vmErrorDiag.vmErrorCode = vmErrorDiag.vmErrorCodeNotClose;
            var titleMsg = viewHome.getErrorTitleAndMessage("error_patient_dir");
            vmErrorDiag.vmErrorMessage = titleMsg[1];
            vmErrorDiag.vmErrorTitle = titleMsg[0];
            vmErrorDiag.open();
            return;
        }

        //console.log("PNAME: " + patientList.get(patientListView.currentIndex).pname + ". UID: " + patientList.get(patientListView.currentIndex).uid);
    }

    // The Doctor Information Title and subtitle
    Text {
        id: title
        font.pixelSize: 43*viewHome.vmScale
        font.family: gothamB.name
        color: "#297FCA"
        text: loader.getStringForKey(keybase+"titleSubject");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: vmBanner.bottom
        anchors.topMargin: mainWindow.height*0.043
    }

    Text {
        id: subTitle
        font.pixelSize: 11*viewHome.vmScale
        font.family: gothamR.name
        color: "#cfcfcf"
        text: loader.getStringForKey(keybase+"subtitleSubject");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: title.bottom
        anchors.topMargin: mainWindow.height*0.016
    }

    Rectangle {

        id: toolBar
        color: "#ffffff"
        border.width: mainWindow.width*0.002
        border.color: "#EDEDEE"
        radius: 4
        width: vmTableWidth
        height: 0.08*parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: subTitle.bottom
        anchors.topMargin: mainWindow.height*0.029

        VMPlusButton {
            id: btnAddPatient
            height: mainWindow.height*0.043
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 20;
            onClicked: {
                loader.clearSubjectSelection();
                viewPatientReg.loadPatientInformation();
                swiperControl.currentIndex = swiperControl.vmIndexPatientReg
            }
        }

        VMPencilButton {
            id: btnEditPatient
            height: mainWindow.height*0.043
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: btnAddPatient.right
            anchors.leftMargin: 10;
            enabled: patientListView.currentIndex !== -1
            onClicked: {
                viewPatientReg.loadPatientInformation();
                swiperControl.currentIndex = swiperControl.vmIndexPatientReg
            }
        }

        VMButton{
            id: btnReports
            height: mainWindow.height*0.043
            vmText: loader.getStringForKey(keybase+"btnReports");
            vmFont: viewHome.gothamM.name
            vmInvertColors: true
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: mainWindow.width*0.016
            onClicked: {
                viewFinishedStudies.loadEvaluatorStudies();
                swiperControl.currentIndex = swiperControl.vmIndexFinishedStudies;
            }
        }

    }

    Rectangle {
        id: searchBar
        color: "#ffffff"
        border.width: mainWindow.width*0.002
        border.color: "#EDEDEE"
        radius: 4
        width: vmTableWidth
        height: 0.05*parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: toolBar.bottom
        anchors.topMargin: mainWindow.height*0.029
        function clearSearch(){
            inputRect.clearSearch();
        }

        Text {
            id: filter
            font.pixelSize: 13*viewHome.vmScale
            font.family: viewHome.gothamM.name
            text: loader.getStringForKey(keybase+"filter");
            color: "#297fca"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: mainWindow.width*0.008
        }

        Rectangle {
            id: inputRect
            color: "#f0f0f0"
            width: 0.9*parent.width
            height: 0.82*parent.height
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: filter.right
            anchors.leftMargin: mainWindow.width*0.008
            radius: 10
            function clearSearch(){
                searchInput.text = "";
            }
            TextInput {
                id: searchInput
                font.pixelSize:  12*viewHome.vmScale
                color: "#000000"
                verticalAlignment: TextInput.AlignVCenter
                font.family: viewHome.gothamR.name
                anchors.fill: parent
                leftPadding: 10
                onTextChanged: {
                    loadPatients(text);
                }
            }
        }



    }

    // The table header.
    Row {
        id: tableHeader
        anchors.top: searchBar.bottom
        anchors.topMargin: mainWindow.height*0.014
        anchors.left: toolBar.left
        height: mainWindow.height*0.043

        Rectangle {
            id: headerPatient
            color: "#ffffff"
            border.width: mainWindow.width*0.002
            border.color: "#EDEDEE"
            radius: 4
            width: vmTableWidth/3
            height: parent.height
            Text {
                id: patientText
                text:loader.getStringForKey(keybase+"headerSubject");
                width: parent.width
                font.family: gothamB.name
                font.pixelSize: 15*viewHome.vmScale
                horizontalAlignment: Text.AlignHCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle {
            id: headerID
            color: "#ffffff"
            border.width: mainWindow.width*0.002
            border.color: "#EDEDEE"
            radius: 4
            width: headerPatient.width
            height: parent.height
            Text {
                id: idText
                text: "ID";
                width: parent.width
                font.family: gothamB.name
                font.pixelSize: 15*viewHome.vmScale
                horizontalAlignment: Text.AlignHCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle {
            id: headerCreationDate
            color: "#ffffff"
            border.width: mainWindow.width*0.002
            border.color: "#EDEDEE"
            radius: 4
            width: headerPatient.width
            height: parent.height
            Text {
                id: creationDateText
                text: loader.getStringForKey(keybase+"headerCreationDate");
                width: parent.width
                font.family: gothamB.name
                font.pixelSize: 15*viewHome.vmScale
                horizontalAlignment: Text.AlignHCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }

    }

    // The table where each of the patient entries will be put. and its background
    Rectangle {
        id: tableBackground
        color: "#ffffff"
        border.width: mainWindow.width*0.002
        border.color: "#EDEDEE"
        radius: 4
        anchors.top: tableHeader.bottom
        anchors.left: tableHeader.left
        width: vmTableWidth
        height: vmTableHeight - tableHeader.height

        ScrollView {
            id: tableArea
            anchors.fill: parent
            clip: true
            ListView {
                id: patientListView
                anchors.fill: parent
                model: patientList
                delegate: VMPatientEntry {
                }
                onCurrentIndexChanged: {
                    for (var i = 0; i < model.count; i++){
                        if (i !== currentIndex){
                            patientList.setProperty(i,"vmIsSelected",false)
                        }
                    }
                    setCurrentPatient();
                }
            }
        }
    }

    // Buttons
    Row{
        id: buttonRow
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: mainWindow.height*0.058
        spacing: mainWindow.width*0.023

        VMButton{
            id: btnBack
            height: mainWindow.height*0.072
            vmText: loader.getStringForKey(keybase+"btnBack");
            vmFont: viewHome.gothamM.name
            vmInvertColors: true
            onClicked: {
                swiperControl.currentIndex = swiperControl.vmIndexHome;
            }
        }

        VMButton{
            id: btnStart
            height: mainWindow.height*0.072
            vmText: loader.getStringForKey(keybase+"btnStart");
            vmFont: viewHome.gothamM.name
            enabled: (patientListView.currentIndex !== -1) && (!disableStudyStart)
            onClicked: {
                setCurrentPatient();
                //swiperControl.currentIndex = swiperControl.vmIndexStudyStart;
                studyPreSetup.fillComboBoxes();
                studyPreSetup.open();
            }
        }
    }

}
