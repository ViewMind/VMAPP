#include "messagelogger.h"

MessageLogger::MessageLogger(QListWidget *lister) {

    // Creating the log file based on the date.
    this->uilist = lister;
    this->uilist->clear();
    this->uilist->setIconSize(QSize(30,30));
    this->logFile = Globals::LOG_FILE_BASE + QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm") + ".log";

    // Creating/checking the Logs directory.
    QDir(".").mkdir(Globals::DIR_LOGS);
    bool noLogDir = false;
    if (!QDir(Globals::DIR_LOGS).exists()){
        noLogDir = true;
    }
    else {
        this->logFile = Globals::DIR_LOGS + "/" + this->logFile;
    }

    // Adding the title of the log to the log file.
    QFile file(logFile);
    if (!file.open(QFile::Append)) return;
    QTextStream writer(&file);
    writer << "===================== ViewMind Maintenance Tool V. " + Globals::VERSION + " =====================\n";
    file.close();

    // If unable to create the log directory, we log that as well.
    if (noLogDir){
        this->error("Unable to create log directory at path: " + Globals::DIR_LOGS);
    }

    // Creating all the colors and constants.
    font.setPointSize(11);
    font.setFamily("Segoe UI");
    font.setWeight(QFont::Bold);
    colorDisplay.setColor(QColor("#939393"));
    colorDisplay.setStyle(Qt::SolidPattern);
    colorError.setColor(QColor("#fd3d60"));
    colorError.setStyle(Qt::SolidPattern);
    colorSuccess.setColor(QColor("#2db117"));
    colorSuccess.setStyle(Qt::SolidPattern);
    colorWarning.setColor(QColor("#aca70d"));
    colorWarning.setStyle(Qt::SolidPattern);
    colorText.setColor(QColor("#242424"));
    colorText.setStyle(Qt::SolidPattern);


}

void MessageLogger::log(const QString msg){
    this->addToLog(MSG_LOG,msg);
}

void MessageLogger::display(const QString msg){
    QListWidgetItem *item = new QListWidgetItem(this->uilist);
    item->setFont(font);
    item->setForeground(colorText);
    item->setBackground(colorDisplay);
    item->setIcon(QIcon(":/images/info_notification.png"));
    item->setText(msg);
    this->uilist->addItem(item);
    this->addToLog(MSG_DISPLAY,msg);
}

void MessageLogger::error(const QString msg){
    QListWidgetItem *item = new QListWidgetItem(this->uilist);
    item->setFont(font);
    item->setForeground(colorText);
    item->setBackground(colorError);
    item->setIcon(QIcon(":/images/alert-triangle-white.png"));
    item->setText(msg);
    this->uilist->addItem(item);
    this->addToLog(MSG_ERROR,msg);
}

void MessageLogger::warning(const QString msg){
    QListWidgetItem *item = new QListWidgetItem(this->uilist);
    item->setFont(font);
    item->setForeground(colorText);
    item->setBackground(colorWarning);
    item->setIcon(QIcon(":/images/alert-triangle-white.png"));
    item->setText(msg);
    this->uilist->addItem(item);
    this->addToLog(MSG_WARNING,msg);
}

void MessageLogger::success(const QString msg){
    QListWidgetItem *item = new QListWidgetItem(this->uilist);
    item->setFont(font);
    item->setForeground(colorText);
    item->setBackground(colorSuccess);
    item->setIcon(QIcon(":/images/check-circle-white.png"));
    item->setText(msg);
    this->uilist->addItem(item);
    this->addToLog(MSG_SUCCESS,msg);
}

void MessageLogger::addToLog(const QString &type, const QString &message){

    QString ts = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString msg = "[" + type  + "][" + ts + "] " + message + "\n";

    QFile file(logFile);
    if (!file.open(QFile::Append)) return;
    QTextStream writer(&file);
    writer << msg;
    file.close();

}
