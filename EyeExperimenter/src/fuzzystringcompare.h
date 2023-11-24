#ifndef FUZZYSTRINGCOMPARE_H
#define FUZZYSTRINGCOMPARE_H

#include <QString>
#include <QDebug>

class FuzzyStringCompare
{
public:
    FuzzyStringCompare();

    qreal compare(QString slider, QString reference);

private:


    /**
     * @brief prepareString - Removes all spaces and makes it lower case
     * @param s - The input string.
     * @return The nomralized string.
     */
    QString prepareString(QString s);

    /**
     * @brief slide - Adds a space at the beginning of the string and removes a letter a charcter at the end.
     * @param s - The input stirng.
     * @return - The slide string.
     */
    QString slide(QString s);

    /**
     * @brief normalizeLength - Adds spaces to the end of the string so that s is of length n. If string is more than n, it does nothing.
     * @param s - The String to normailze
     * @param n - The desired string length
     * @return The string with spaces at the end.
     */
    QString normalizeLength(QString s, qint32 n);

    /**
     * @brief compare - Compares two strings of the same length and returns the largets number of successive letters that are the same.
     * @param a - One string
     * @param b - The other string.
     * @return The largest number of successive letters that are the same.
     */
    qint32 countCompare(QString a, QString b);


};

#endif // FUZZYSTRINGCOMPARE_H
