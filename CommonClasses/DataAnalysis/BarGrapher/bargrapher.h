#ifndef BARGRAPHER_H
#define BARGRAPHER_H

#include <QFile>
#include <QTextStream>
#include <QStringList>

class BarGrapher
{
public:

    struct BarGraphOptions {
        qint32 width;
        qint32 height;
        QList< QList<qreal> > dataSets;
        QStringList colors;
        QStringList xtext;
        QString title;
        QString associatedFileName;
        qint32 fontSize;
        QStringList legend;
        bool drawValuesOnBars;
        QString ylabel;

        // Constructor
        BarGraphOptions();

        // Functions
        QString generateJavaScriptObj(const QString &varName, const QString &tab = "") const;
        QString verifyValues() const;
    };

    BarGrapher();
    void addGraphToDo(BarGraphOptions bgo);
    bool createBarGraphHTML(const QString &fileName);
    void clearGraphs() {graphs.clear();}

    QString getLastError() const { return error; }

private:

    QList<BarGraphOptions> graphs;
    static const QString STR_CANVAS_IDS;
    static const QString STR_RUN_CODE;
    QString error;

};

#endif // BARGRAPHER_H
