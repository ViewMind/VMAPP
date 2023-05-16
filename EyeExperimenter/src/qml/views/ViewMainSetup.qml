import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"
import "../subscreens"

ViewBase {

    id: viewMainSetup

    readonly property int indexPatList:     0
    readonly property int indexAccount:     1
    readonly property int indexReportList:  2
    readonly property int indexProtocols:   3

    property string vmInitials : "XX";
    property string vmEmail: ""
    property string vmEvaluator: ""

    /**
      * Dispite it's name this function also loads the evaluator data.
      */
    function loadPatients(){
        var evaluator = loader.getCurrentEvaluatorInfo();
        if (evaluator.email === undefined) return;
        vmEvaluator = evaluator.name  + " " + evaluator.lastname
        vmEmail     = evaluator.email
        vmInitials = "";
        if (evaluator.name.length > 0)  vmInitials = evaluator.name[0]
        if (evaluator.lastname.length > 0) vmInitials = vmInitials + evaluator.lastname[0]
        vmInitials = vmInitials.toUpperCase()
        patlist.vmNoPatientsAtAll = !loader.areThereAnySubjects();
        patlist.loadPatients()
        sideNavigationBarAccountOnly.vmCurrentIndex = -1
    }

    function swipeIntoMain(){
        switch(viewer.currentIndex){
        case indexPatList:
            loadPatients();
            break;
        case indexAccount:
            account.loadAccountInfo();
            break;
        case indexReportList:
            reportlist.loadReports();
            break;
        case indexProtocols:
            protocollist.loadProtocols();
            break;
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

    function goToReportList(){
        viewer.setCurrentIndex(indexReportList)
        sideNavigationBar.vmCurrentIndex = 1; // This should be the index of the Report List.
    }

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
                case 2:
                    viewer.currentIndex = indexProtocols
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
                if (vmCurrentIndex == 0) {
                    sideNavigationBar.vmCurrentIndex = -1;
                    viewer.currentIndex = indexAccount
                }
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
            currentIndex: indexPatList
            width: clipRect.width - clipRect.vmBorderPadding*2
            height: clipRect.height - clipRect.radius - clipRect.vmBorderPadding
            x: clipRect.vmBorderPadding
            y: clipRect.vmBorderPadding

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
                SCReportList {
                    id: reportlist
                    radius: clipRect.radius
                    border.width:  clipRect.border.width
                    border.color: clipRect.border.color
                    anchors.fill: parent
                }
            }

            Item {
                SCProtocolList {
                    id: protocollist
                    radius: clipRect.radius
                    border.width:  clipRect.border.width
                    border.color: clipRect.border.color
                    anchors.fill: parent
                }
            }

            onCurrentIndexChanged: {
                //console.log("Index has been changed to " + currentIndex + " in subscreen viewer")
                //console.log("Calling swipe into main due to viewer in main setup change");
                swipeIntoMain()
            }


        }


    }


}
