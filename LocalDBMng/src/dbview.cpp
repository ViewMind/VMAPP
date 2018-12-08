#include "dbview.h"
#include "ui_dbview.h"

DBView::DBView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DBView)
{
    ui->setupUi(this);
    this->setWindowTitle("DBModifier - V1.1.0");
    qsrand(static_cast<quint64>(QTime::currentTime().msecsSinceStartOfDay()));

}

DBView::~DBView()
{
    delete ui;
}

void DBView::on_pbBrowse_clicked()
{

    QString selected = QFileDialog::getOpenFileName(this,"Select Local DB file",".",LOCAL_DB);
    // HACK. REMOVE
    // selected = "C:/Users/Viewmind/Documents/ExperimenterOutputs/viewmind_etdata/localdb.dat";
    if (selected.isEmpty()) return;
    ui->leLocalDB->setText(selected);

    QFileInfo info(selected);
    QDir baseDir = info.absoluteDir();
    //baseDir.cdUp();
    lim.setDirectory(baseDir.absolutePath());

    // Should load the local DB file.
    qWarning() << baseDir.absolutePath();

    // Filling the doctor list
    fillDoctorList();

}


void DBView::fillDoctorList(){

    QList<QStringList> info = lim.getDoctorList(true);
    if (info.size() != 2) return;
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
    QVariantMap data = lim.getDoctorInfo(uid);

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
    int res = QMessageBox::question(this,"Delete Doctor From DB","Are you sure you want to delete doctor: " + ui->lvDoctors->currentItem()->text() + "?",QMessageBox::Yes,QMessageBox::Cancel);
    if (res == QMessageBox::Yes){
        lim.deleteDoctor(ui->lvDoctors->currentItem()->data(ROLE).toString());
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

    lim.setDoctorData(ui->lvDoctors->currentItem()->data(ROLE).toString(),keys,values);
    fillDoctorList();

}

void DBView::on_actionInsert_Random_Patients_triggered()
{
    QString drUID = ui->lvDoctors->currentItem()->data(ROLE).toString();
    HowManyDiag diag(this);
    if (diag.exec() == QDialog::Rejected) return;
    qint32 number = diag.getHowMany();

    if (number == 0) return;

    QStringList someRandomNames;
    someRandomNames << "Willia Griswold" << "Roberto Bagnell" << "Ignacio Deloach" << "Margarete Kearney" << "Charmaine Mcaninch" << "Ned Mendel" << "Joye Weinberg"
                    << "Sam Pankratz" << "Edgardo Dunneback" << "Jami Alpert" << "Maragret Magallanes" << "Arica Mcclaskey" << "Golda Elwell" << "Jeana Concha"
                    << "Shaina Stanback" << "Vivien Mccawley" << "Carly Wills" << "Buena Roehl" << "Felecia Peed" << "Onita Mcphee" << "Allena Rippey" << "Viviana Winchenbach"
                    << "Mirian Chiang" << "Phuong Bittinger" << "Tyrone Santucci" << "Oren Grindstaff" << "Vivan Gatling" << "Tierra Schaller" << "Corliss Hurd" << "Kathe Motyka"
                    << "Winnifred Yamasaki" << "Carmelita Nagata" << "Daria Almodovar" << "Kayce Plumb" << "Marylee Harkey" << "Otis Vanarsdale" << "Debrah Woods" << "Nenita Defilippo"
                    << "Eric Steimle" << "Jovita Yao" << "Eloisa Perrodin" << "Florentino Roop" << "Fay Machnik" << "Taryn Eberhardt" << "Cecila Lam" << "Carmine Hynes" << "Adela Basso"
                    << "Kyra Chute" << "Willodean Carmean" << "Estella Rickerson";

    for (qint32 k = 0; k < number; k++){
        QHash<QString,QString> data;

        qint32 dni = 20;
        dni = dni + (qrand() % 15);
        dni = dni * 1000000 + (qrand() % 999999);

        qint32 year = 2018 - (qrand() & 70);
        qint32 day = (qrand() % 29) + 1;
        qint32 month = (qrand() % 12) + 1;

        QString isodate = QString::number(year) + "-";
        QString mm = QString::number(month);
        if (month < 10) mm = "0" + mm;
        QString dd = QString::number(day);
        if (day < 10) dd = "0" + dd;
        isodate = isodate + mm + "-" + dd;

        data[TPATREQ_COL_BIRTHCOUNTRY] = "AR";
        data[TPATREQ_COL_DOCTORID] = drUID;
        data[TPATREQ_COL_FIRSTNAME] = getRandomFirstName(someRandomNames);
        data[TPATREQ_COL_BIRTHDATE] = isodate;
        data[TPATREQ_COL_LASTNAME] = getRandomLastName(someRandomNames);
        data[TPATREQ_COL_SEX] = ((qrand() % 2) == 0) ? "M" : "F";
        data[TPATREQ_COL_UID] = data[TPATREQ_COL_BIRTHCOUNTRY] + QString::number(dni);

        // Adding the transaction
        QStringList columns = data.keys();
        QStringList values;
        for (qint32 i = 0; i < columns.size(); i++){
            values << data.value(columns.at(i));
        }

        lim.addPatientData(drUID,data.value(TPATREQ_COL_UID),columns,values);
    }

    QMessageBox::information(this,"Done","Added " + QString::number(number) + " patients",QMessageBox::Ok);

}
