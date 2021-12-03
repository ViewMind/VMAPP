import QtQuick
import QtQuick.Controls
import "../"

Rectangle {

    id: progressLine
    color: "transparent"
    border.width: 2

        Canvas{
            id: drawCanvas
            anchors.fill: parent
            contextType: "2d"

            onPaint: {

                var ctx = getContext("2d")
                //ctx.font = VMGlobals.vmFontVeryLarge + "px sans-serif"
                ctx.font = '%1px "%2"'.arg(VMGlobals.vmFontVeryLarge).arg(mainWindow.vmSegoeNormal.name);
                //ctx.font = "16px sans-serif"
                //console.log("Font set to " + ctx.font)
                ctx.fillStyle = "#242424"
                //ctx.fillText("Go NoGo",0,10)
                ctx.fillText("Patient",10,50)
                //ctx.fillText("Evaluation2",0,40)
                //ctx.fillText("Evaluation3",0,50)
                //ctx.fillText("Evaluation4",0,60)

            }
        }

}

