#include "dbview.h"
#include "ui_dbview.h"

DBView::DBView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DBView)
{
    ui->setupUi(this);
    lim = nullptr;
}

DBView::~DBView()
{
    delete ui;
}

void DBView::on_pbBrowse_clicked()
{

    QString selected = QFileDialog::getOpenFileName(this,"Select Local DB file",".",LOCAL_DB);;
    if (selected.isEmpty()) return;
    ui->leLocalDB->setText(selected);

    QFileInfo info(selected);
    QDir baseDir = info.absoluteDir();
    baseDir.cdUp();
    config.addKeyValuePair(CONFIG_OUTPUT_DIR,baseDir.absolutePath());

    // Should load the local DB file.
    qWarning() << baseDir.absolutePath();
    lim = new LocalInformationManager(&config);

    // Filling the doctor list
    fillDoctorList();

}


void DBView::fillDoctorList(){
    if (lim == nullptr) return;

    QList<QStringList> info = lim->getDoctorList(true);
    QStringList drlist = info.first();
    QStringList uidlist = info.last();
    ui->lvDoctors->clear();
    ui->pteDrInfo->clear();

    for (qint32 i = 0; i < drlist.size(); i++){
        if (ui->checkBox->isChecked()){
            if (!uidlist.at(i).contains(TEST_UID)) continue;
        }
        QListWidgetItem *item = new QListWidgetItem(drlist.at(i),ui->lvDoctors);
        item->setData(1000,uidlist.at(i));
    }

}

void DBView::on_checkBox_toggled(bool checked)
{
    Q_UNUSED(checked);
    fillDoctorList();
}

void DBView::on_lvDoctors_itemClicked(QListWidgetItem *item)
{
    QString uid = item->data(1000).toString();
    config.addKeyValuePair(CONFIG_DOCTOR_UID,uid);
    QVariantMap data = lim->getCurrentDoctorInfo();

    // Transforming the data to a string
    QStringList keys = data.keys();
    QString info = "";
    for (qint32 i = 0; i < keys.size(); i++){
        info = info + keys.at(i) + ": " + data.value(keys.at(i)).toString() + "\n";
    }
    ui->pteDrInfo->setPlainText(info);
}

void DBView::on_pbDeleteSelected_clicked()
{
    if (lim == nullptr) return;
    int res = QMessageBox::question(this,"Delete Doctor From DB","Are you sure you want to delete doctor: " + ui->lvDoctors->currentItem()->text() + "?",QMessageBox::Yes,QMessageBox::Cancel);
    if (res == QMessageBox::Yes){
        lim->deleteDoctor(ui->lvDoctors->currentItem()->data(1000).toString());
    }

    fillDoctorList();
}

void DBView::on_pbMakeVisible_clicked()
{
    if (lim == nullptr) return;
    lim->makeVisible(ui->lvDoctors->currentItem()->data(1000).toString());
}
