import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.0

VMBase {

    id: viewPatientList
    width: viewPatientList.vmWIDTH
    height: viewPatientList.vmHEIGHT

    readonly property real vmTableWidth: 0.70*viewPatientList.vmWIDTH
    readonly property real vmTableHeight: 0.33*viewPatientList.vmHEIGHT
    readonly property string keybase: "viewpatientlist_"

    property bool vmShowAll: false;
    property string vmPatIDForMedRecSync: ""

    Connections {
        target: flowControl
        onSslTransactionFinished:{
            connectionDialog.close();
            if (!flowControl.isSSLTransactionOK()){
                // console.log("SSL Eye Server Transaction Error Code: " + flowControl.getSSLTransactionError());
                var errorTitleMsg = loader.getErrorMessageForCode(flowControl.getSSLTransactionError());
                if (errorTitleMsg.length === 2){ // If the code was all ok but the transaction was NOT ok, then it was a communications error.
                    vmErrorDiag.vmErrorCode = vmErrorDiag.vmERROR_PROC_ACK;
                    vmErrorDiag.vmErrorMessage = errorTitleMsg[1];
                    vmErrorDiag.vmErrorTitle = errorTitleMsg[0];
                    vmErrorDiag.open();
                    return;
                }
                else{
                    vmErrorDiag.vmErrorCode = vmErrorDiag.vmERROR_SERVER_COMM;
                    var titleMsg = viewHome.getErrorTitleAndMessage("error_server_comm");
                    vmErrorDiag.vmErrorMessage = titleMsg[1];
                    vmErrorDiag.vmErrorTitle = titleMsg[0];
                    vmErrorDiag.open();
                    return;
                }
            }
            else{
                if (vmPatIDForMedRecSync === ""){
                    loader.setNumberOfEvaluations(flowControl.numberOfEvaluationsReceived());
                    viewDatSelectionDiag.updateNumberOfEvals();
                }
                else {
                    loader.cleanMedicalRecordUpdateList(vmPatIDForMedRecSync)
                    vmPatIDForMedRecSync = ""
                }
                updateText();
                loadPatients();
            }
        }
    }

    Dialog {

        property string vmTitle: "TITLE"
        property string vmMessage: "MESSAGE"

        id: connectionDialog;
        modal: true
        width: 614
        height: 600
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

        // The instruction text
        Text {
            id: diagTitle
            font.family: viewHome.gothamB.name
            font.pixelSize: 43
            anchors.top: parent.top
            anchors.topMargin: 88
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#297fca"
            text: connectionDialog.vmTitle
        }

        // The instruction text
        Text {
            id: diagMessage
            font.family: viewHome.robotoR.name
            font.pixelSize: 13
            anchors.top:  diagTitle.bottom
            anchors.topMargin: 26
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#297fca"
            text: connectionDialog.vmMessage
        }

        AnimatedImage {
            id: slideAnimation
            source: "qrc:/images/LOADING.gif"
            anchors.top: diagMessage.bottom
            anchors.topMargin: 30
            x: (parent.width - slideAnimation.width)/2;
        }

    }

    Dialog {

        property string vmMsgTitle: ""
        property string vmMsgText: ""
        property bool vmRequireAns: false
        property string vmYesButtonLabel: ""
        property string vmNoButtonLabel: ""
        property var vmParameters: []

        id: showMsgDialog;
        modal: true
        width: 614
        height: 250
        y: (parent.height - height)/2
        x: (parent.width - width)/2
        closePolicy: Popup.NoAutoClose

        contentItem: Rectangle {
            id: rectShowMsgDialog
            anchors.fill: parent
            layer.enabled: true
            layer.effect: DropShadow{
                radius: 5
            }
        }

        VMDialogCloseButton {
            id: btnClose
            anchors.top: parent.top
            anchors.topMargin: 22
            anchors.right: parent.right
            anchors.rightMargin: 25
            onClicked: {
                showMsgDialog.close();
            }
        }

        // The instruction text
        Text {
            id: showMsgDialogTitle
            font.family: viewHome.gothamB.name
            font.pixelSize: 43
            anchors.top: parent.top
            anchors.topMargin: 50
            anchors.left: parent.left
            anchors.leftMargin: 20
            color: "#297fca"
            text: showMsgDialog.vmMsgTitle
        }

        // The instruction text
        Text {
            id: showMsgDialogMessage
            font.family: viewHome.robotoR.name
            font.pixelSize: 13
            textFormat: Text.RichText
            anchors.top:  showMsgDialogTitle.bottom
            anchors.topMargin: 20
            anchors.left: showMsgDialogTitle.left
            text: showMsgDialog.vmMsgText
        }

        VMButton{
            id: btnNegative
            height: 40
            vmText: showMsgDialog.vmNoButtonLabel
            vmFont: viewHome.gothamM.name
            vmInvertColors: true
            visible: showMsgDialog.vmRequireAns
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 20
            anchors.left: parent.left
            anchors.leftMargin: 50
            onClicked: {
                showMsgDialog.vmRequireAns = false;
                showMsgDialog.close();
            }
        }

        VMButton{
            id: btnPositive
            height: 40
            vmText: showMsgDialog.vmYesButtonLabel
            vmFont: viewHome.gothamM.name
            visible: showMsgDialog.vmRequireAns
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 20
            anchors.right: parent.right
            anchors.rightMargin: 50
            onClicked: {
                showMsgDialog.vmRequireAns = false;
                showMsgDialog.close();
                viewDatSelectionDiag.archiveFile(showMsgDialog.vmParameters[0],showMsgDialog.vmParameters[1]);
            }
        }

    }

    Dialog {

        property string vmDrName : ""

        id: askPasswordDialog;
        modal: true
        width: 614
        height: 280
        y: (parent.height - height)/2
        x: (parent.width - width)/2
        closePolicy: Popup.NoAutoClose

        contentItem: Rectangle {
            id: rectPassDialog
            anchors.fill: parent
            layer.enabled: true
            layer.effect: DropShadow{
                radius: 5
            }
        }

        VMDialogCloseButton {
            id: btnClosePass
            anchors.top: parent.top
            anchors.topMargin: 22
            anchors.right: parent.right
            anchors.rightMargin: 25
            onClicked: {
                askPasswordDialog.close();
            }
        }

        // The instruction text
        Text {
            id: diagPassTitle
            font.family: viewHome.gothamB.name
            font.pixelSize: 30
            anchors.top: parent.top
            anchors.topMargin: 50
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#297fca"
            text:  loader.getStringForKey("viewhome_btnTableID");
        }

        VMPasswordField{
            id: passwordInput
            anchors.bottom: btnCheckPassword.top
            anchors.bottomMargin: 30
            anchors.horizontalCenter: parent.horizontalCenter;
            width: diagPassTitle.width*1.5;
            vmLabelText: loader.getStringForKey("viewdrsel_labelInstPassword");
        }

        VMButton{
            id: btnCheckPassword
            height: 50
            vmText: "OK";
            vmFont: viewHome.gothamM.name
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 30
            onClicked:{
                if (loader.verifyInstitutionPassword(passwordInput.getText())){
                    askPasswordDialog.close();
                    fileDialog.open();
                }
                else{
                    passwordInput.vmErrorMsg =  loader.getStringForKey("viewdrsel_instpassword_wrong");
                }
            }
        }

        onOpened: {
            swiperControl.currentIndex = swiperControl.vmIndexPatientList;
            passwordInput.setText("");
            passwordInput.vmErrorMsg = "";
        }
    }

    FileDialog {
        id: fileDialog
        title: loader.getStringForKey("viewhome_tableIDtitle");
        folder: shortcuts.home
        nameFilters: ["CSV (*.csv)"]
        selectExisting: false
        onAccepted: {
            loader.generateIDTable(fileUrl)
        }
        onRejected: {
        }
    }

    ViewDatSelectionDiag{
        id: viewDatSelectionDiag;
        y: (parent.height - height)/2
        x: (parent.width - width)/2
    }

    ListModel {
        id: patientList
    }

    function openAskPasswordDialog(){
        askPasswordDialog.open();
    }

    // Loads the list model with the patient information
    function loadPatients(filterText) {

        if (filterText === undefined){
            searchBar.clearSearch();
            filterText = "";
        }

        // WARNING: Get patient list call fills the other two lists. It NEEDS to be called first.
        var patientNameList = loader.generatePatientLists(filterText, vmShowAll);
        var patientDisplayID = loader.getPatientDisplayIDList();
        var uidList = loader.getPatientUIDLists();
        var isOkList = loader.getPatientIsOKList();
        var isMedRecUpToDateList = loader.getPatientMedRecUpToDateList();
        var drName = loader.getDoctorNameList();
        var drUID  = loader.getDoctorUIDList();

        // Clearing the current model.
        patientList.clear()

        var drValid = loader.isDoctorValidated(-1);
        if (loader.getViewAllFlag()) drValid = true;

        for (var i = 0; i < patientNameList.length; i++){
            //console.log("PATIENTLIST: " + patientNameList[i] + " PUID: " +  uidList[i] + "  OK: " + isOkList[i] + " DR: " + drName[i] + "  DRUID: " + drUID[i]);
            var display_doctor_name = "";
            if (vmShowAll) display_doctor_name = drName[i];
            patientList.append({"vmPatientUID": uidList[i],
                                   "vmDisplayID": patientDisplayID[i],
                                   "vmPatientName": patientNameList[i],
                                   "vmIsOk": (isOkList[i] === "true"),
                                   "vmMedRecUpToDate": (isMedRecUpToDateList[i] === "true"),
                                   "vmEnableGenRepButon": drValid,
                                   "vmDrName" : display_doctor_name,
                                   "vmDrUID": drUID[i],
                                   "vmItemIndex" : i,
                                   "vmIsSelected" : false});
        }

        patientListView.currentIndex = -1;
        if (loader.getViewAllFlag()){
            btnViewAll.enabled = true;
        }
        else {
            if (drValid){
                btnViewAll.enabled = true;
            }
            else{
                btnViewAll.enabled = false;
            }
        }
    }

    function startDemoTransaction(){
        connectionDialog.vmMessage = loader.getStringForKey(keybase+"diagRepTitle");
        connectionDialog.vmTitle = loader.getStringForKey(keybase+"diagRepMessage");
        connectionDialog.open();
        loader.updateCurrentDoctorAndPatientDBFiles();
        flowControl.startDemoTransaction();
    }

    function reprocessRequest(reportName, fileList){
        connectionDialog.vmMessage = loader.getStringForKey(keybase+"diagRepTitle");
        connectionDialog.vmTitle = loader.getStringForKey(keybase+"diagRepMessage");
        connectionDialog.open();
        loader.updateCurrentDoctorAndPatientDBFiles();
        flowControl.requestDataReprocessing(reportName,fileList,loader.getEvaluationID(reportName));
    }

    function openDatSelectionDialog(){
        viewDatSelectionDiag.open();
    }

    function requestReportToServer(){
        if (!loader.isDoctorValidated(-1)){
            showMessage("msg_notvalid");
            return;
        }
        if (!loader.doesCurrentDoctorHavePassword()){
            showMessage("msg_nopass");
            return;
        }
        //loader.prepareForRequestOfPendingReports();
        connectionDialog.vmMessage = loader.getStringForKey(keybase+"diagRepTitle");
        connectionDialog.vmTitle = loader.getStringForKey(keybase+"diagRepMessage");
        connectionDialog.open();
        loader.updateCurrentDoctorAndPatientDBFiles();
        flowControl.requestReportData();
    }

    function showMessage(msg){
        var list = loader.getStringListForKey(keybase+msg);
        showMsgDialog.vmMsgText = list[1];
        showMsgDialog.vmMsgTitle = list[0];
        showMsgDialog.open();
    }

    function setCurrentPatient(){
        if (patientListView.currentIndex == -1) return;

        //console.log("Pat name: " + patientList.get(patientListView.currentIndex).vmPatientName)
        var displayName;
        if (uimap.getStructure() === "P") displayName = patientList.get(patientListView.currentIndex).vmPatientName + " (" +  patientList.get(patientListView.currentIndex).vmDisplayID + ")"
        else displayName = patientList.get(patientListView.currentIndex).vmDisplayID;

        loader.setValueForConfiguration(vmDefines.vmCONFIG_PATIENT_NAME,patientList.get(patientListView.currentIndex).vmPatientName);
        loader.setValueForConfiguration(vmDefines.vmCONFIG_PATIENT_UID,patientList.get(patientListView.currentIndex).vmPatientUID);
        loader.setValueForConfiguration(vmDefines.vmCONFIG_DOCTOR_WORK_UID,patientList.get(patientListView.currentIndex).vmDrUID);
        loader.setValueForConfiguration(vmDefines.vmCONFIG_PATIENT_DISPLAYID,patientList.get(patientListView.currentIndex).vmDisplayID);
        loader.setAgeForCurrentPatient();

        // Settign the titles for the medical record list and the medical record info screen.
        viewMedRecordList.vmPatientName = displayName;
        viewMedicalInformation.vmPatientName = displayName;

        if (!loader.createPatientDirectory()){
            vmErrorDiag.vmErrorCode = vmErrorDiag.vmERROR_CREATING_PDIR;
            var titleMsg = viewHome.getErrorTitleAndMessage("error_patient_dir");
            vmErrorDiag.vmErrorMessage = titleMsg[1];
            vmErrorDiag.vmErrorTitle = titleMsg[0];
            vmErrorDiag.open();
            return;
        }

        //console.log("PNAME: " + patientList.get(patientListView.currentIndex).pname + ". UID: " + patientList.get(patientListView.currentIndex).uid);
    }

    function configureShowMessageForArchive(which,index){
        showMsgDialog.vmRequireAns = true;
        showMsgDialog.vmParameters = [which, index];
        var parts = loader.getStringListForKey(keybase+"archive_msg");
        showMsgDialog.vmMsgTitle = parts[0];
        showMsgDialog.vmMsgText = parts[1];
        showMsgDialog.vmYesButtonLabel = parts[2];
        showMsgDialog.vmNoButtonLabel = parts[3];
        showMsgDialog.open();
    }

    // The Doctor Information Title and subtitle
    Text {
        id: title
        font.pixelSize: 43
        font.family: gothamB.name
        color: "#297FCA"
        text: {
            if (uimap.getStructure() === "P") return loader.getStringForKey(keybase+"title");
            if (uimap.getStructure() === "S") return loader.getStringForKey(keybase+"titleSubject");
        }
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: vmBanner.bottom
        anchors.topMargin: 30
    }

    Text {
        id: subTitle
        font.pixelSize: 11
        font.family: gothamR.name
        color: "#cfcfcf"
        text: {
            if (uimap.getStructure() === "P") return loader.getStringForKey(keybase+"subtitle");
            if (uimap.getStructure() === "S") return loader.getStringForKey(keybase+"subtitleSubject");
        }
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: title.bottom
        anchors.topMargin: 11
    }

    Rectangle {

        id: toolBar
        color: "#ffffff"
        border.width: 2
        border.color: "#EDEDEE"
        radius: 4
        width: vmTableWidth
        height: 0.08*parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: subTitle.bottom
        anchors.topMargin: 20

        VMPlusButton {
            id: btnAddPatient
            height: 30
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 20;
            onClicked: {
                viewPatientReg.clearAll();
                swiperControl.currentIndex = swiperControl.vmIndexPatientReg
            }
        }

        VMPencilButton {
            id: btnEditPatient
            height: 30
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: btnAddPatient.right
            anchors.leftMargin: 10;
            enabled: patientListView.currentIndex !== -1
            onClicked: {
                viewPatientReg.loadPatientInformation();
                swiperControl.currentIndex = swiperControl.vmIndexPatientReg
            }
        }

        VMFolderButton{
            id: btnMedRecord
            height: 30
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: btnEditPatient.right
            anchors.leftMargin: 10;
            enabled: patientListView.currentIndex !== -1
            onClicked: {
                swiperControl.currentIndex = swiperControl.vmIndexMedicalRecordList
            }
        }


        VMButton{
            id: btnView
            height: 30
            vmText: loader.getStringForKey(keybase+"btnView");
            vmFont: viewHome.gothamM.name
            vmInvertColors: true
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 20
            enabled: (patientListView.currentIndex !== -1) && (loader.isDoctorValidated(-1) || loader.getViewAllFlag() )
            onClicked: {
                if (uimap.getStructure() === "P") viewShowReports.vmPatientName = loader.getConfigurationString(vmDefines.vmCONFIG_PATIENT_NAME);
                else if (uimap.getStructure() === "S") viewShowReports.vmPatientName = loader.getConfigurationString(vmDefines.vmCONFIG_PATIENT_DISPLAYID);
                viewShowReports.vmPatientDirectory = loader.getConfigurationString(vmDefines.vmCONFIG_PATIENT_DIRECTORY);
                swiperControl.currentIndex = swiperControl.vmIndexShowReports;
            }
        }

        VMButton{
            id: btnViewAll
            height: 30
            vmText: {
                if (vmShowAll) {
                    if (uimap.getStructure() === "P") return loader.getStringForKey(keybase+"view_yours");
                    if (uimap.getStructure() === "S") return loader.getStringForKey(keybase+"view_yours_subject");
                }
                else {
                    if (uimap.getStructure() === "P") return loader.getStringForKey(keybase+"view_all");
                    if (uimap.getStructure() === "S") return loader.getStringForKey(keybase+"view_all_subject");
                }
            }
            vmFont: viewHome.gothamM.name
            width: 200
            vmInvertColors: true
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: btnView.left
            anchors.rightMargin: 20
            onClicked: {
                if (vmShowAll){
                    vmShowAll = false;
                    loadPatients();
                }
                else {
                    vmShowAll = true;
                    loadPatients();
                }
            }
        }
    }

    Rectangle {
        id: searchBar
        color: "#ffffff"
        border.width: 2
        border.color: "#EDEDEE"
        radius: 4
        width: vmTableWidth
        height: 0.05*parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: toolBar.bottom
        anchors.topMargin: 20
        function clearSearch(){
            inputRect.clearSearch();
        }

        Text {
            id: filter
            font.pixelSize: 13
            font.family: viewHome.gothamM.name
            text: loader.getStringForKey(keybase+"filter");
            color: "#297fca"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
        }

        Rectangle {
            id: inputRect
            color: "#f0f0f0"
            width: 0.9*parent.width
            height: 0.82*parent.height
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: filter.right
            anchors.leftMargin: 10
            radius: 10
            function clearSearch(){
                searchInput.text = "";
            }
            TextInput {
                id: searchInput
                font.pixelSize:  12
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
        anchors.topMargin: 10
        anchors.left: toolBar.left
        height: 30
        Rectangle {
            id: headerPatient
            color: "#ffffff"
            border.width: 2
            border.color: "#EDEDEE"
            radius: 4
            width: {
                if (vmShowAll) return 0.4*vmTableWidth;
                else return 0.6*vmTableWidth
            }
            height: parent.height
            Text {
                id: patientText
                text: {
                    if (uimap.getStructure() === "P") return loader.getStringForKey(keybase+"headerPatient");
                    if (uimap.getStructure() === "S") return loader.getStringForKey(keybase+"headerSubject");
                }
                width: parent.width
                font.family: gothamB.name
                font.pixelSize: 15
                horizontalAlignment: Text.AlignHCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle {
            id: headerDoctor
            color: "#ffffff"
            border.width: 2
            border.color: "#EDEDEE"
            radius: 4
            width: {
                if (vmShowAll) return 0.27*vmTableWidth;
                else return 0;
            }
            visible: vmShowAll
            height: parent.height
            Text {
                id: doctorText
                text: loader.getStringForKey(keybase+"headerDoctor");
                width: parent.width
                font.family: gothamB.name
                font.pixelSize: 15
                horizontalAlignment: Text.AlignHCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle {
            id: headerStatus
            color: "#ffffff"
            border.width: 2
            border.color: "#EDEDEE"
            radius: 4
            width: {
                if (vmShowAll) 0.18*vmTableWidth;
                else return 0.2*vmTableWidth;
            }
            height: parent.height
            Text {
                id: statusText
                text: loader.getStringForKey(keybase+"headerStatus");
                width: parent.width
                font.family: gothamB.name
                font.pixelSize: 15
                horizontalAlignment: Text.AlignHCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle {
            id: headerMedRecs
            color: "#ffffff"
            border.width: 2
            border.color: "#EDEDEE"
            radius: 4
            width: {
                if (vmShowAll) 0.15*vmTableWidth;
                else return 0.2*vmTableWidth;
            }
            height: parent.height
            Text {
                id: medicalRecordText
                text: loader.getStringForKey(keybase+"headerSynch");
                width: parent.width
                font.family: gothamB.name
                font.pixelSize: 15
                horizontalAlignment: Text.AlignHCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }

    }

    // The table where each of the patient entries will be put. and its background
    Rectangle {
        id: tableBackground
        color: "#ffffff"
        border.width: 2
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
                    onFetchReport: {
                        patientListView.currentIndex = index;
                        viewDatSelectionDiag.open();
                    }
                    onUpdateMedicalRecords: {
                        var patuid = patientList.get(index).vmPatientUID;
                        if (!loader.prepareMedicalRecordFiles(patuid)){
                            vmErrorDiag.vmErrorCode = vmErrorDiag.vmERROR_SERVER_COMM; // This is not a server comm error but the progam does not need to close.
                            var titleMsg = viewHome.getErrorTitleAndMessage("error_programming");
                            vmErrorDiag.vmErrorMessage = titleMsg[1];
                            vmErrorDiag.vmErrorTitle = titleMsg[0];
                            vmErrorDiag.open();
                        }
                        else {
                            connectionDialog.vmMessage = loader.getStringForKey(keybase+"diagRepTitle");
                            connectionDialog.vmTitle = loader.getStringForKey(keybase+"diagRepMessage");
                            connectionDialog.open();
                            vmPatIDForMedRecSync = patuid;
                            flowControl.sendMedicalRecordsToServer(patuid);
                        }
                    }
                }
                onCurrentIndexChanged: {
                    for (var i = 0; i < model.count; i++){
                        if (i != currentIndex){
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
        anchors.bottomMargin: 40
        spacing: 30

        VMButton{
            id: btnBack
            height: 50
            vmText: loader.getStringForKey(keybase+"btnBack");
            vmFont: viewHome.gothamM.name
            vmInvertColors: true
            onClicked: {
                swiperControl.currentIndex = swiperControl.vmIndexHome;
            }
        }

        VMButton{
            id: btnStart
            height: 50
            vmText: loader.getStringForKey(keybase+"btnStart");
            vmFont: viewHome.gothamM.name
            enabled: patientListView.currentIndex !== -1
            onClicked: {
                setCurrentPatient();
                swiperControl.currentIndex = swiperControl.vmIndexStudyStart;
            }
        }
    }

}
