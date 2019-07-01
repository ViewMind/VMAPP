import QtQuick 2.0

Item {

    property int btnSide: 50;
    property int btnAir: btnSide*0.1;
    property int airMargin: 10
    property int viewWindowWidth: width - 2*(btnSide+btnAir);
    property string backgroundColor: "#ffffff"
    property real imgScale: 1.0


    function setImageList(imlist){
        imgMng.imagelist = imlist
        if (imlist.length > 0) imgMng.currentIndex = 0;
        else imgMng.currentIndex = -1;
        mainViewer.switchImage(imgMng.imagelist[0])
    }


    Item {
        id: imgMng
        property var imagelist: [];
        property int currentIndex: -1;

        function moveImage(forward){
            if (currentIndex === -1) return;
            if ((forward) && (currentIndex < imagelist.length-1)) currentIndex++;
            else if (forward) currentIndex = 0;
            else if ((!forward) && (currentIndex > 0)) currentIndex--;
            else if (!forward) currentIndex = imagelist.length-1;
            mainViewer.switchImage(imagelist[currentIndex])
        }
    }

    Rectangle {
        id: mainViewer
        width: viewWindowWidth
        height: parent.height
        border.width: 2
        //border.color: "#297fca"
        border.color: "#ffffff"
        color: backgroundColor
        radius: 2
        anchors.top: parent.top
        x: btnSide + btnAir
        Image {
            id: showImage
            source: ""
            //anchors.centerIn: parent
            //scale: imgScale
            anchors.fill: parent
        }
        function switchImage(src){
            showImage.source = src;
        }
    }

    // The "go back" button
    MouseArea{
        id: btnBack
        width: btnSide
        height: btnSide

        scale: btnBack.pressed? 0.8:1
        Behavior on scale{
            NumberAnimation {
                duration: 25
            }
        }

        Rectangle {
            id: btnBackRect
            anchors.fill: parent
            radius: btnSide/2
            //color: (btnBack.pressed)? "#4984b3" : "#297fca"
            color: "#ffffff"
        }

        Canvas {
            id: backArrow
            width: parent.width*0.6
            height: parent.width*0.6
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
            contextType: "2d"
            onPaint: {
                var ctx = backArrow.getContext("2d");
                ctx.reset();
                ctx.strokeStyle = "#297fca";
                ctx.lineWidth = 3;
                ctx.moveTo(width, 0);
                ctx.lineTo(0, height/2);
                ctx.lineTo(width, height);
                ctx.stroke();
            }
        }

        x: 0
        anchors.verticalCenter: parent.verticalCenter
        onClicked: {
            imgMng.moveImage(false);
        }
    }

    // The "forward" button
    MouseArea{
        id: btnForward
        width: btnSide
        height: btnSide

        scale: btnForward.pressed? 0.8:1
        Behavior on scale{
            NumberAnimation {
                duration: 25
            }
        }

        Rectangle {
            id: btnForwardRect
            anchors.fill: parent
            radius: btnSide/2
            //color: (btnForward.pressed)? "#4984b3" : "#297fca"
            color: "#ffffff"
        }


        Canvas {
            id: forwardArrow
            width: parent.width*0.6
            height: parent.width*0.6
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 20
            contextType: "2d"
            onPaint: {
                var ctx = forwardArrow.getContext("2d");
                ctx.reset();
                ctx.strokeStyle = "#297fca";
                ctx.lineWidth = 3;
                ctx.moveTo(0, 0);
                ctx.lineTo(width, height/2);
                ctx.lineTo(0, height);
                ctx.stroke();
                //ctx.closePath();
                //ctx.fillStyle = btnForward.pressed? "#2873b4" : "#ffffff"
                //ctx.fill();
            }
        }

        x: viewWindowWidth+2*btnAir + btnSide
        anchors.verticalCenter: parent.verticalCenter
        onClicked: {
            imgMng.moveImage(true);
        }
    }

}
