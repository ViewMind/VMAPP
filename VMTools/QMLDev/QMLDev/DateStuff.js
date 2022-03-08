
var dateObject
var localizedDateFormat
var localizedTimeFormat
var monthFirst

function setDate(date, date_format) {
    //console.log("Local Locale: " + Qt.locale().dateFormat(Locale.ShortFormat));
    dateObject = Date.fromLocaleString(Qt.locale(), date, date_format)
}

function isMonthFirst(){
    return monthFirst;
}

function getDateAndTime(){
    return dateObject.toLocaleString(Locale.ShortFormat)
}

function getDate2(){
    return dateObject.toLocaleDateString(Locale.ShortFormat)
    //return dateObject.toLocaleDateString()

}

function getDate(){
    return dateObject.toLocaleDateString()
}
