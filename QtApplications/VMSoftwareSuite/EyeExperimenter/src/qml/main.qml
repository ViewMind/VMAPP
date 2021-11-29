import QtQuick
import QtQuick.Controls
import QtQuick.Window
import "./components"
import "./views"
import "."

ApplicationWindow {
    id: mainWindow
    visible: true
    title: qsTr("EyeExplorer - ") + loader.getWindowTilteVersion()
    visibility: Window.Maximized

    Component.onCompleted: {
        flowControl.resolutionCalculations();
        // This ensures that no resizing is possible.
        minimumHeight = height;
        maximumHeight = height;
        minimumWidth = width;
        maximumWidth = width;
        VMGlobals.mainHeight = height;
        VMGlobals.mainWidth = width;
    }

    // For showing restart message.
    VMMessageDialog {
        id: messageDiag
        onDismissed: {
            messageDiag.close();
            Qt.quit()
        }
    }


    // Settings is a global dialog.
    ViewSettings {
        id: settingsDialog
        onRestartRequired: {
            messageDiag.loadFromKey("viewsettings_restart_msg")
            messageDiag.open();
        }
    }

    // The wait screen
    ViewWait {
        id: waitScreen
    }

    SwipeView {

        id: swiperControl
        //currentIndex: vmIndexHome
        interactive: false
        anchors.fill: parent

        Item{
            ViewStart{
                id: viewHome
            }
        }

        Item{
            ViewLogin{
                id: viewLogin
            }
        }

        onCurrentIndexChanged: {
            switch (currentIndex){
            case VMGlobals.vmSwipeIndexLogin:
                viewLogin.updateProfileList();
                break;
            }
        }

    }

    function swipeTo(index){
        swiperControl.currentIndex = index;
    }

    function openWait(message){
        waitScreen.vmText = message;
        waitScreen.show()
    }

    function closeWait(){
        waitScreen.hide();
    }

}
