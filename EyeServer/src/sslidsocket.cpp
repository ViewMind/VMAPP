#include "sslidsocket.h"

SSLIDSocket::SSLIDSocket():QObject(){
    sslSocket = nullptr;
    ID = 0;
    s3Address = "";
}

SSLIDSocket::SSLIDSocket(QSslSocket *newSocket, quint64 id, const QString &s3):QObject()
{

    sslSocket = nullptr;
    ID = id;
    if (newSocket == nullptr) return;

    sslSocket = newSocket;

    // Creating all the connections from signals and slots.
    connect(sslSocket,SIGNAL(encrypted()),this,SLOT(on_encryptedSuccess()));
    connect(sslSocket,SIGNAL(sslErrors(QList<QSslError>)),this,SLOT(on_sslErrors(QList<QSslError>)));
    connect(sslSocket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(on_socketStateChanged(QAbstractSocket::SocketState)));
    connect(sslSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(on_socketError(QAbstractSocket::SocketError)));
    connect(sslSocket,&QSslSocket::readyRead,this,&SSLIDSocket::on_readyRead);
    connect(sslSocket,&QSslSocket::disconnected,this,&SSLIDSocket::on_disconnected);
    connect(&timer,&QTimer::timeout,this,&SSLIDSocket::on_timeout);
    connect(&process,SIGNAL(finished(int)),this,SLOT(on_processFinished(qint32)));
    s3Address = s3;
    workingDirectory = "";
}

void SSLIDSocket::processData(const QString &processorPath, const QStringList &args){
    QFileInfo info(processorPath);
    process.setWorkingDirectory(info.absolutePath());
    process.start(processorPath,args);
}

void SSLIDSocket::on_processFinished(qint32 status){
    Q_UNUSED(status);
    emit(sslSignal(ID,SSL_SIGNAL_PROCESS_DONE));
}

QString SSLIDSocket::setWorkingDirectoryAndSaveAllFiles(const QString &baseDir){
    // Assuming that the base directory exists

    QString patient = "unknown_patient";
    DataPacket::Field f = rx.getField(DataPacket::DPFI_PATIENT_ID);
    if (f.fieldType == DataPacket::DPFT_STRING){
        patient = f.data.toString();
        patient = patient.replace(' ','_');
    }

    QString doctor = "unknown_doctor";
    f = rx.getField(DataPacket::DPFI_DOCTOR_ID);
    if (f.fieldType == DataPacket::DPFT_STRING){
        doctor = f.data.toString();
        doctor = doctor.replace(' ','_');
    }

    QDir bdir(baseDir);
    if (!QDir(baseDir + "/" + doctor).exists()){
        if (!bdir.mkdir(doctor)){
            return "Could not create doctor subdirectory: " + doctor;
        }
    }

    QDir ddir(baseDir + "/" + doctor);
    if (!QDir(ddir.path() + "/" + patient).exists()){
        if (!ddir.mkdir(patient)){
            return "Could not create patient subdirectory: " + patient + " in " + ddir.path();
        }
    }

    // The time stamp dir should never exist.
    QString wdir = QDateTime::currentDateTime().toString(TIME_FORMAT_STRING);
    QDir pdir(ddir.path() + "/" + patient);
    if (!pdir.mkdir(wdir)){
        return "Could not create time stamp dir in " + pdir.path();
    }

    // Since the working directory was generated the files can now be saved.
    workingDirectory = pdir.path() + "/" + wdir;
    if (!rx.saveFiles(workingDirectory)){
        return "There were errors saving packet files.";
    }

    // If this is demo mode the information is NOT saved.
    if (rx.getField(DataPacket::DPFI_DEMO_MODE).data.toInt() == 1) return "";

    // Pushing the files to the using AWS to the S3 storarge.
    QString baseNameForS3 = patient +"/"+wdir+"/";
    QStringList savedFiles = rx.getLastFilesSaved();
    for (qint32 i = 0; i < savedFiles.size(); i++){
        QFileInfo info(savedFiles.at(i));
        QString cmd = S3_BASE_COMMAND;
        cmd = cmd + " " + savedFiles.at(i) + " ";
        cmd = cmd + "s3://"  + s3Address  + "/"  + baseNameForS3 + info.baseName() + "." + info.suffix() + " ";
        cmd = cmd + S3_PARMETERS;
        //qWarning() << "S3 Command is " << cmd;
        QProcess::execute(cmd);
    }

    return "";
}

void SSLIDSocket::startTimeoutTimer(qint32 ms){
    timer.setInterval(ms);
    timer.start();
}

void SSLIDSocket::on_timeout(){
    emit(sslSignal(ID,SSL_SIGNAL_TIMEOUT));
}

void SSLIDSocket::on_disconnected(){
    emit(sslSignal(ID,SSL_SIGNAL_DISCONNECTED));
}

void SSLIDSocket::on_encryptedSuccess(){
    emit(sslSignal(ID,SSL_SIGNAL_ENCRYPTED));
}

void SSLIDSocket::on_readyRead(){
    // Should buffer the data
    QByteArray ba = socket()->readAll();
    quint8 ans = rx.bufferByteArray(ba);
    if (ans == DataPacket::DATABUFFER_RESULT_DONE){
        emit(sslSignal(ID,SSL_SIGNAL_DATA_RX_DONE));
    }
    else if (ans == DataPacket::DATABUFFER_RESULT_ERROR){
        emit(sslSignal(ID,SSL_SIGNAL_DATA_RX_ERROR));
    }
}

void SSLIDSocket::on_socketError(QAbstractSocket::SocketError error){
    Q_UNUSED(error);
    emit(sslSignal(ID,SSL_SIGNAL_ERROR));
}

void SSLIDSocket::on_socketStateChanged(QAbstractSocket::SocketState state){
    Q_UNUSED(state);
    emit(sslSignal(ID,SSL_SIGNAL_STATE_CHANGED));
}

void SSLIDSocket::on_sslErrors(const QList<QSslError> &errors){
    Q_UNUSED(errors);
    emit(sslSignal(ID,SSL_SIGNAL_SSL_ERROR));
}

SSLIDSocket::~SSLIDSocket(){
    if (sslSocket != nullptr) delete sslSocket;
    if (timer.isActive()) timer.stop();
}
