#include "fuzzystringcompare.h"

FuzzyStringCompare::FuzzyStringCompare()
{

}


qreal FuzzyStringCompare::compare(QString slider, QString reference){

    //qDebug() << "INPUT -> SLIDER:" << slider << "REF:" << reference;

    QString a = this->prepareString(slider);
    QString b = this->prepareString(reference);

    qreal base = qMax(a.size(), b.size());

    if (a.size() < b.size()){
        slider = normalizeLength(a,b.size());
        reference = b;
    }
    else {
        slider = normalizeLength(b,a.size());
        reference = a;
    }

    //qDebug() << "BEFORE SLIDING -> SLIDER:" << slider << "REF:" << reference << "BASE: " << base;

    // Now both strings are the same length.
    qreal peak = 0;
    for (qint32 i = 0; i < a.size(); i++){
        qint32 value = countCompare(slider,reference);
        // qDebug() << "   " << slider << "->" << reference << " = " << value;
        if (value > peak) peak = value;
        slider = slide(slider);
    }


    //qDebug() << "PEAK" << peak;
    return peak/base;


}


QString FuzzyStringCompare::prepareString(QString s){
    QString temps = s.replace(" ","");
    while (temps != s){
        s = temps.replace(" ","");
    }
    // NO more spaces.
    return s.toLower();
}

QString FuzzyStringCompare::slide(QString s){
    s = " " + s.mid(0,s.length()-1);
    return s;
}

qint32 FuzzyStringCompare::countCompare(QString a, QString b){
    if (a.size() != b.size()) return -1;

    qint32 same_counter = 0;
    qint32 peak = 0;

    for (qint32 i = 0; i < a.size(); i++){
        if (a.at(i) == b.at(i)){
            same_counter++;
        }
        else {
            if (same_counter > peak) peak = same_counter;
            same_counter = 0;
        }
    }

    if (same_counter > peak) peak = same_counter;

    return peak;

}


QString FuzzyStringCompare::normalizeLength(QString s, qint32 n){
    while (s.length() < n) s = s + " ";
    return s;
}
