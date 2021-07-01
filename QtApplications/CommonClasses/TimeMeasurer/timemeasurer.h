#ifndef TIMEMEASURER_H
#define TIMEMEASURER_H

#include <QElapsedTimer>
#include <QHash>

class TimeMeasurer
{
public:
    TimeMeasurer();

    quint64 getTimeFor(const QString &id) { return  times.value(id); }
    void measure(const QString &id);
    void end();
    QString getTimeSummary() const;
    quint64 getTotal() const;    
    QString getTimeString (QHash<QString,QString> code) const;

private:
    QHash<QString,quint64> times;
    QElapsedTimer timer;
    QString lastID;
};

#endif // TIMEMEASURER_H
