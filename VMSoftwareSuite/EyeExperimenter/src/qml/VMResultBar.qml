import QtQuick 2.0

Item {

    readonly property int vmSECTION_RED:    2;
    readonly property int vmSECTION_YELLOW: 1;
    readonly property int vmSECTION_GREEN:  0;

    property bool vmTwoSection: false
    property int vmIndicatorInSection: 0
    property bool vmApplyYCorrectionToCircles: false

    width: mainWindow.width*0.103
    height: own.sectionHeight + own.arrowHeight*2 + own.arrowOverShoot*2;

    // Private constants
    Item {
        id: own
        readonly property int sectionHeight: 8
        readonly property int arrowHeight: 7
        readonly property int arrowOverShoot: 4
        readonly property int semiCircleRadious: sectionHeight/2
        readonly property int sectionWidth: (vmTwoSection)? parent.width/2 : parent.width/3
        readonly property int posArrow: {
            if (vmTwoSection){
                if (vmIndicatorInSection === vmSECTION_GREEN) return parent.width/4;
                // Assuming red section
                else return 3*parent.width/4;
            }
            else{
                if (vmIndicatorInSection == vmSECTION_GREEN) return parent.width/6
                else if (vmIndicatorInSection == vmSECTION_YELLOW) return parent.width/2;
                // Assuming red section.
                else return 5*parent.width/6;
            }
        }
    }

    // The end tip, always green
    Canvas {
        id: greenTip
        width: own.sectionHeight
        height: own.sectionHeight
        contextType: "2d"
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        onPaint: {
            var ctx = greenTip.getContext("2d");
            ctx.reset();
            ctx.ellipse(0,0,width,height);
            ctx.closePath();
            ctx.fillStyle = "#009949"
            ctx.fill();
        }
    }

    // The green rectangle. Width depends on number of sections
    Rectangle{
        id: greenRect
        color: "#009949"
        width:  own.sectionWidth - own.semiCircleRadious
        height: own.sectionHeight
        anchors.left: greenTip.right
        anchors.leftMargin: -own.semiCircleRadious
        anchors.verticalCenter: parent.verticalCenter
    }

    // The yellow rectangle which might be full or end rectangle depending on number of sections
    Rectangle{
        id: yellowRect
        color: "#e4b32c"
        //width: (vmTwoSection)? own.sectionWidth - own.semiCircleRadious : own.sectionWidth
        width: own.sectionWidth
        visible: !vmTwoSection
        height: own.sectionHeight
        anchors.left: greenRect.right
        anchors.verticalCenter: parent.verticalCenter
    }

    // The red rectangle might be invisible
    Rectangle{
        id: redRect
        color: "#ca2026"        
        //width: own.sectionWidth - own.semiCircleRadious
        width:  own.sectionWidth - own.semiCircleRadious
        height: own.sectionHeight
        anchors.left: (vmTwoSection)? greenRect.right : yellowRect.right
        anchors.verticalCenter: parent.verticalCenter
    }

    // The right tip's colors depends on if this is a two section or three section bar.
    Canvas {
        id: rightTip
        width: own.sectionHeight
        height: own.sectionHeight
        contextType: "2d"
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        onPaint: {
            var ctx = rightTip.getContext("2d");
            ctx.reset();
            ctx.ellipse(0,0,width,height);
            ctx.closePath();
            ctx.fillStyle = "#ca2026"
            ctx.fill();
        }
    }

    // The down arrow
    Canvas {
        id: downArrow
        x: own.posArrow - own.arrowHeight/2
        y: own.arrowOverShoot
        width: own.arrowHeight
        height: own.arrowHeight
        contextType: "2d"
        onPaint: {
            var ctx = downArrow.getContext("2d");
            ctx.reset();
            ctx.moveTo(0, 0);
            ctx.lineTo(width, 0);
            ctx.lineTo(width / 2, height);
            ctx.closePath();
            ctx.fillStyle = "#000000"
            ctx.fill();
        }
    }

    // The arrow poiting up
    Canvas {
        id: upArrow
        x: own.posArrow - own.arrowHeight/2
        y: parent.height - own.arrowOverShoot - own.arrowHeight
        width: own.arrowHeight
        height: own.arrowHeight
        contextType: "2d"
        onPaint: {
            var ctx = upArrow.getContext("2d");
            ctx.reset();
            ctx.moveTo(width/2, 0);
            ctx.lineTo(width,height);
            ctx.lineTo(0,height);
            ctx.closePath();
            ctx.fillStyle = "#000000"
            ctx.fill();
        }
    }

    // The line through
    Rectangle{
        id: indicatorLine
        x: own.posArrow-width/2
        y: 0
        width: mainWindow.width*0.001
        height: parent.height
        color: "#000000"
    }


}
