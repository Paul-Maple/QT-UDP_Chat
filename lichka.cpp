#include "lichka.h"
#include "ui_lichka.h"
#include "mainwindow.h"

#include <QHostInfo>
#include <QUdpSocket>

Lichka::Lichka(QString Name_transmitter, QHostAddress IP_transmitter, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Lichka)
{
    ui->setupUi(this);
    setWindowTitle("Личные сообщения с " + Name_transmitter);
    ui->Lichka_Chat->setReadOnly(1);

    IP = IP_transmitter;

    QString localhostname =  QHostInfo::localHostName();
    QList<QHostAddress> hostList = QHostInfo::fromName(localhostname).addresses();
    for(int i=0; i<hostList.count(); i++)
    {
        if (hostList[i].protocol() == QAbstractSocket::IPv4Protocol && hostList[i].isLoopback() == false) {
           this->my_adress = hostList[i];
        }
    }
    ui->Lichka_Chat->setTextBackgroundColor(QColor(50,50,50));
    connect(ui->lineEdit, SIGNAL(returnPressed()), SLOT(slotSendDatagram()));

    LS_pudp = new QUdpSocket(this);
    LS_pudp->bind(nomer_porta);
    connect(LS_pudp, SIGNAL(readyRead()), this, SLOT(slotProcessDatagrams()));

}

Lichka::~Lichka()
{
    delete ui;
}

void Lichka::slotProcessDatagrams()
{
    QByteArray baDatagram;
    QHostAddress fly_adress;
    do
    {
    baDatagram.resize(LS_pudp->pendingDatagramSize());
    LS_pudp->readDatagram(baDatagram.data(), baDatagram.size(), &fly_adress);
    }
    while(LS_pudp->hasPendingDatagrams());
    QString stroka_messege(baDatagram);
    if (fly_adress.toString().remove(0,7) != my_adress.toString())
    {
        ui->Lichka_Chat->setAlignment(Qt::AlignLeft); // Выравнивание
        ui->Lichka_Chat->setTextColor(QColor(255,255,0)); // Цвет текста
        ui->Lichka_Chat->append(stroka_messege + "\n");
    }
    //ui->Lichka_Chat->append("Не мой IP: \n" + fly_adress.toString().remove(0,7)); //.remove(0,7)); //IP-adress
    //ui->Lichka_Chat->append("Мой IP: \n" + my_adress.toString()); // ip компа-отправителя
    //ui->Lichka_Chat->append(QHostInfo::localHostName());
}

void Lichka::slotSendDatagram()
{
    QString dt = ui->lineEdit->text();
    if(ui->lineEdit->text() != nullptr)
    {
        LS_pudp->writeDatagram(dt.toUtf8(), IP, nomer_porta);
        ui->Lichka_Chat->setTextColor(QColor(0,255,0)); // Цвет текста
        ui->Lichka_Chat->setAlignment(Qt::AlignRight);
        ui->Lichka_Chat->append(ui->lineEdit->text() + "\n");
        ui->Lichka_Chat->setAlignment(Qt::AlignLeft);
        ui->lineEdit->clear();
    }
}
