#include "timemeasurer.h"

TimeMeasurer::TimeMeasurer()
{
    lastID = "";
}

void TimeMeasurer::measure(const QString &id){
    if (!lastID.isEmpty()){
        times[lastID] = timer.elapsed();
    }
    lastID = id;
    timer.start();
}

void TimeMeasurer::end(){
    times[lastID] = timer.elapsed();
}


quint64 TimeMeasurer::getTotal() const {
    QStringList parts = times.keys();
    if (parts.isEmpty()) return 0;
    quint64 accumulated = 0;
    for (qint32 i = 0; i < parts.size(); i++){
        accumulated = accumulated + times.value(parts.at(i));
    }
    return accumulated;
}

QString TimeMeasurer::getTimeSummary() const {
    QStringList parts = times.keys();
    if (parts.isEmpty()) return "NO TIME DATA";
    parts.sort();
    quint64 accumulated = 0;
    QString ans = "";
    for (qint32 i = 0; i < parts.size(); i++){
        quint64 time = times.value(parts.at(i));
        ans = ans + "   " + parts.at(i) + " = " + QString::number(time) + " ms\n";
        accumulated = accumulated + time;
    }
    ans =  ans + "TOTAL: " + QString::number(accumulated) + "ms";
    ans = "\n" + ans;
    return ans;
}

QString TimeMeasurer::getTimeString(QHash<QString, QString> code) const{
    QStringList names = code.keys();
    QStringList ans;
    for (qint32 i = 0; i < names.size(); i++){
        if (times.contains(names.at(i))){
            ans << code.value(names.at(i)) + ":" + QString::number(times.value(names.at(i)));
        }
    }
    return ans.join("|");
}
