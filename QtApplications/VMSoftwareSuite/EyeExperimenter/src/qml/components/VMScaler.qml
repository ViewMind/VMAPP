import QtQuick

Item {

    QtObject {
        id: internal
        property double m: 0;
        property double k: 0;
    }

    function setScale(minRealValue, maxRealValue, dimForMin, dimForMax){
        //console.log("Mapping range " + minRealValue  + " - " + maxRealValue + " to Y Range: " + yForMin + " - " + yForMax)
        internal.m = (dimForMin-dimForMax)/(minRealValue - maxRealValue)
        internal.k = dimForMax - internal.m*maxRealValue
    }

    function convert(value){
        //console.log("For value " + value  + " slope " + internal.m + " and k " + internal.k)
        return internal.m*value + internal.k
    }


}
