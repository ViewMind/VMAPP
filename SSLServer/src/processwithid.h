#ifndef PROCESSWITHID_H
#define PROCESSWITHID_H

#include <QProcess>

class ProcessWithID : public QProcess
{
    Q_OBJECT
public:
    ProcessWithID(qint32 id);
    quint64 getID() const { return ID; }

signals:
    void finishedWithID(qint32 id);

private slots:
    void on_finished(int exit_code, QProcess::ExitStatus es);

private:
    quint64 ID;
};

#endif // PROCESSWITHID_H
