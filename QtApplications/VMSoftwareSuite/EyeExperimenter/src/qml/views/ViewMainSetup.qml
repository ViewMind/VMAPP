import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"
import "../subscreens"

ViewBase {

    id: viewMainSetup

    readonly property int indexNoPatients: 0
    readonly property int indexPatList:    1
    readonly property int indexAccount:    2
    readonly property int indexNoReports:  3
    readonly property int indexReportList: 4


    property string vmInitials : "XX";
    property string vmEmail: ""
    property string vmEvaluator: ""

    /**
      * Dispite it's name this function also loads the evaluator data.
      */
    function loadPatients(){
        var evaluator = loader.getCurrentEvaluatorInfo();
        vmEvaluator = evaluator.name  + " " + evaluator.lastname
        vmEmail     = evaluator.email
        vmInitials = "";
        if (evaluator.name.length > 0)  vmInitials = evaluator.name[0]
        if (evaluator.lastname.length > 0) vmInitials = vmInitials + evaluator.lastname[0]
        vmInitials = vmInitials.toUpperCase()

        if (loader.areThereAnySubjects()) {
            viewer.currentIndex = indexPatList
            patlist.loadPatients()
        }
        else {
            viewer.currentIndex = indexNoPatients;
        }

        sideNavigationBarAccountOnly.vmCurrentIndex = -1

    }

    function swipeIntoMain(){
        if (sideNavigationBar.vmCurrentIndex === 0){
            loadPatients();
        }
        else if (sideNavigationBar.vmCurrentIndex === 1){
            reportlist.loadReports()
        }
    }

    function goBackToPatientList(){
        sideNavigationBar.vmCurrentIndex = 0
        sideNavigationBarAccountOnly.vmCurrentIndex = -1;
        viewer.setCurrentIndex(indexPatList)
    }

    function enableStudyStart(enable){
        patlist.vmStudiesEnabled = enable;
    }

    function showNoReports(){
        viewer.currentIndex = indexNoReports
    }

    //function show

    Rectangle {
        id: initials
        width: VMGlobals.adjustWidth(62)
        height: width
        radius: width/2
        color: VMGlobals.vmBluePanelBKGSelected
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.leftMargin: VMGlobals.adjustWidth(15)
        anchors.bottomMargin: VMGlobals.adjustHeight(608)


        Text {
            id: initialsText
            font.pixelSize: VMGlobals.vmFontExtraLarge
            color: VMGlobals.vmBlueSelected
            anchors.fill: parent
            text: vmInitials
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter

        }
    }

    Text {
        id: evaluatorName
        text: vmEvaluator
        font.pixelSize: VMGlobals.vmFontLarge
        font.weight: 600
        height: VMGlobals.adjustHeight(20)
        color: VMGlobals.vmBlackText
        verticalAlignment: Text.AlignVCenter
        anchors.left: initials.right
        anchors.leftMargin: VMGlobals.adjustWidth(15)
        anchors.bottom: parent.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(640)
    }

    Text {
        id: evaluatorEmail
        text: vmEmail
        font.pixelSize: VMGlobals.vmFontLarge
        font.weight: 400
        color: VMGlobals.vmGrayPlaceholderText
        height: evaluatorName.height
        verticalAlignment: Text.AlignVCenter
        anchors.left: evaluatorName.left
        anchors.top: evaluatorName.bottom
        anchors.topMargin: VMGlobals.adjustHeight(2)
    }

    VMSideNavigationBar {
        id: sideNavigationBar
        vmLikeButton: true
        width: VMGlobals.adjustWidth(290)
        anchors.left: parent.left
        anchors.leftMargin: VMGlobals.adjustWidth(15)
        anchors.top: initials.bottom
        anchors.topMargin: VMGlobals.adjustHeight(20)

        Component.onCompleted: {
            var list = [];
            var item = {};

            item["text"]        = loader.getStringForKey("viewpatlist_title");
            item["icon_normal"] = "qrc:/images/users.png"
            item["icon_selected"] = "qrc:/images/users_blue.png"
            list.push(item)

            item = {};
            item["text"]        = loader.getStringForKey("viewpatlist_qc");
            item["icon_normal"] = "qrc:/images/shield.png"
            item["icon_selected"] = "qrc:/images/shield_blue.png"
            list.push(item)

            item = {};
            item["text"]        = loader.getStringForKey("viewpatlist_protocol");
            item["icon_normal"] = "qrc:/images/file-text.png"
            item["icon_selected"] = "qrc:/images/file-text-blue.png"
            list.push(item)


            sideNavigationBar.setItems(list);
            sideNavigationBar.setItemSelected(0)
        }
        onVmCurrentIndexChanged: {
            setItemSelected(vmCurrentIndex)
            if (vmCurrentIndex != -1){
                sideNavigationBarAccountOnly.vmCurrentIndex = -1
                switch (vmCurrentIndex){
                case 0:
                    viewer.currentIndex = indexPatList
                    break;
                case 1:
                    viewer.currentIndex = indexReportList
                    break;
                }
            }
        }
    }

    Column {
        id: lowerActions
        width: sideNavigationBar.width
        spacing: 0
        anchors.left: sideNavigationBar.left
        anchors.bottom: parent.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(36)

        VMButton {
            id: settingsButton
            vmIconSource: "qrc:/images/Settings.png"
            vmIconToTheRight: false
            vmButtonType: settingsButton.vmTypeTertiary
            vmText: loader.getStringForKey("viewstart_settings");
            onClickSignal: {
                settingsDialog.open()
            }
        }

        VMSideNavigationBar {
            id: sideNavigationBarAccountOnly
            width: parent.width
            vmLikeButton: true
            Component.onCompleted: {
                var list = [];
                var item = {};

                item = {};
                item["text"]        = loader.getStringForKey("viewpatlist_account");
                item["icon_normal"] = "qrc:/images/contact.png"
                item["icon_selected"] = "qrc:/images/contact-blue.png"
                list.push(item)

                sideNavigationBarAccountOnly.setItems(list);
                sideNavigationBarAccountOnly.setItemSelected(-1)
            }
            onVmCurrentIndexChanged: {
                // Since this contains only one element, we need to "unselect" any element in the other sidebar
                setItemSelected(vmCurrentIndex)
                if (vmCurrentIndex == 0) sideNavigationBar.vmCurrentIndex = -1;
                viewer.currentIndex = indexAccount
            }
        }

        VMButton {
            id: logoutButton
            vmIconSource: "qrc:/images/log-out.png"
            vmIconToTheRight: false
            vmButtonType: settingsButton.vmTypeTertiary
            vmText: loader.getStringForKey("viewpatlist_logout");
            onClickSignal: {
                mainWindow.swipeTo(VMGlobals.vmSwipeIndexHome)
            }
        }

    }

    Rectangle {

        property double vmBorderPadding: VMGlobals.adjustWidth(1)

        id: clipRect
        clip: true
        radius: VMGlobals.adjustHeight(10)
        width: VMGlobals.adjustWidth(969);
        height: VMGlobals.adjustHeight(670+10);
        anchors.right: parent.right
        anchors.rightMargin: VMGlobals.adjustWidth(15)
        anchors.bottom: parent.bottom
        anchors.bottomMargin: -radius
        border.width: vmBorderPadding
        border.color: VMGlobals.vmGrayUnselectedBorder

        SwipeView {
            id: viewer
            interactive: false
            currentIndex: 0
            width: clipRect.width - clipRect.vmBorderPadding*2
            height: clipRect.height - clipRect.radius - clipRect.vmBorderPadding
            x: clipRect.vmBorderPadding
            y: clipRect.vmBorderPadding

            Item {
                SCNoPatients {
                    id: nopatients
                    radius: clipRect.radius
                    border.width:  clipRect.border.width
                    border.color: clipRect.border.color
                    anchors.fill: parent
                }
            }

            Item {
                SCPatList {
                    id: patlist
                    radius: clipRect.radius
                    border.width:  clipRect.border.width
                    border.color: clipRect.border.color
                    anchors.fill: parent
                }
            }

            Item {
                SCAccount {
                    id: account
                    radius: clipRect.radius
                    border.width:  clipRect.border.width
                    border.color: clipRect.border.color
                    anchors.fill: parent
                }
            }

            Item {
                SCNoReports {
                    id: noreports
                    radius: clipRect.radius
                    border.width:  clipRect.border.width
                    border.color: clipRect.border.color
                    anchors.fill: parent
                }
            }

            Item {
                SCReportList {
                    id: reportlist
                    radius: clipRect.radius
                    border.width:  clipRect.border.width
                    border.color: clipRect.border.color
                    anchors.fill: parent
                }
            }

            onCurrentIndexChanged: {
                switch(currentIndex){
                case indexPatList:
                    loadPatients();
                    break;
                case indexAccount:
                    account.loadAccountInfo();
                    break;
                case indexReportList:
                    reportlist.loadReports();
                    break;
                }
            }


        }


    }


}
