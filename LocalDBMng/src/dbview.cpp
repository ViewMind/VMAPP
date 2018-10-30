#include "dbview.h"
#include "ui_dbview.h"

DBView::DBView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DBView)
{
    ui->setupUi(this);
    lim = nullptr;
    this->setWindowTitle("DBModifier - V1.0.0");

}

DBView::~DBView()
{
    delete ui;
}

void DBView::on_pbBrowse_clicked()
{

    QString selected = QFileDialog::getOpenFileName(this,"Select Local DB file",".",LOCAL_DB);
    // HACK. REMOVE
    selected = "C:/Users/Viewmind/Documents/ExperimenterOutputs/viewmind_etdata/localdb.dat";
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
    ui->twTable->clear();

    for (qint32 i = 0; i < drlist.size(); i++){
        if (ui->checkBox->isChecked()){
            if (!uidlist.at(i).contains(TEST_UID)) continue;
        }
        QListWidgetItem *item = new QListWidgetItem(drlist.at(i),ui->lvDoctors);
        item->setData(ROLE,uidlist.at(i));
    }

}

void DBView::on_checkBox_toggled(bool checked)
{
    Q_UNUSED(checked);
    fillDoctorList();
}

void DBView::on_lvDoctors_itemClicked(QListWidgetItem *item)
{
    QString uid = item->data(ROLE).toString();
    config.addKeyValuePair(CONFIG_DOCTOR_UID,uid);
    QVariantMap data = lim->getCurrentDoctorInfo();

    //qWarning() << data;

    ui->twTable->clear();

    QStringList keys = data.keys();
    ui->twTable->setRowCount(keys.size());
    ui->twTable->setColumnCount(2);
    QStringList names; names << "Field Name" << "Field Value";
    ui->twTable->setHorizontalHeaderLabels(names);

    for (qint32 i = 0; i < keys.size(); i++){
        QVariant value = data.value(keys.at(i));
        ui->twTable->setItem(i,0,new QTableWidgetItem(keys.at(i)));
        QTableWidgetItem *item;
        if (keys.at(i) == "PATIENT_DATA"){
            item = new QTableWidgetItem("Map. No Edit.");
            item->setData(ROLE,ITEM_ACTION_PASS);
        }
        else if (value.type() == QVariant::Bool){
            item = new QTableWidgetItem();
            item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            item->setData(ROLE,ITEM_ACTION_BOOL);
            if (value.toBool()) item->setCheckState(Qt::Checked);
            else item->setCheckState(Qt::Unchecked);
        }
        else{
            item = new QTableWidgetItem(value.toString());
            item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled);
            item->setData(ROLE,ITEM_ACTION_STRING);
        }
        ui->twTable->setItem(i,1,item);
    }

}

void DBView::on_pbDeleteSelected_clicked()
{
    if (lim == nullptr) return;
    int res = QMessageBox::question(this,"Delete Doctor From DB","Are you sure you want to delete doctor: " + ui->lvDoctors->currentItem()->text() + "?",QMessageBox::Yes,QMessageBox::Cancel);
    if (res == QMessageBox::Yes){
        lim->deleteDoctor(ui->lvDoctors->currentItem()->data(ROLE).toString());
    }

    fillDoctorList();
}

void DBView::on_pbApplyChanges_clicked()
{
    // Run through table.
    QStringList keys;
    QVariantList values;

    for (qint32 i = 0; i < ui->twTable->rowCount(); i++){
        qint32 role = ui->twTable->item(i,1)->data(ROLE).toInt();
        if (role == ITEM_ACTION_BOOL){
            keys << ui->twTable->item(i,0)->text();
            values << (ui->twTable->item(i,1)->checkState() == Qt::Checked);
        }
        else if (role == ITEM_ACTION_STRING){
            keys << ui->twTable->item(i,0)->text();
            values << ui->twTable->item(i,1)->text();
        }
    }

    lim->setDoctorData(ui->lvDoctors->currentItem()->data(ROLE).toString(),keys,values);
    fillDoctorList();

}
