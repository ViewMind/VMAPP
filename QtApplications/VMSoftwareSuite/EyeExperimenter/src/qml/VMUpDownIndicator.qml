import QtQuick

Canvas {

    property bool vmIsUp: true
    property string vmColor: "#000000"

    id: updownIndicator
    contextType: "2d"

    function toggle(){
        vmIsUp = !vmIsUp;
        requestPaint();
    }

    onPaint: {
        var ctx = updownIndicator.getContext("2d");
        ctx.reset();
        if (vmIsUp){
            ctx.moveTo(0,height);
            ctx.lineTo(width/2,0);
            ctx.lineTo(width, height);
        }
        else{
            ctx.moveTo(0, 0);
            ctx.lineTo(width, 0);
            ctx.lineTo(width / 2, height);
        }
        ctx.closePath();
        ctx.fillStyle = "#000000";
        ctx.fill();
        //console.log("W " + width + ". H " + height)
    }

}
