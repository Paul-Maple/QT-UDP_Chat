#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "lichka.h"

#include <QUdpSocket>
#include <QTimer>
#include <QString>
#include <QHostInfo>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Telegramm_2214");

    m_pudp = new QUdpSocket(this);
    m_pudp->bind(nomer_porta);
    ui->Vvod_Nomer_Porta->setValue(nomer_porta);
    connect(m_pudp, SIGNAL(readyRead()), this, SLOT(slotProcessDatagrams()));

    socket_LS = new QUdpSocket(this);
    socket_LS->bind(port_LS);
    connect(socket_LS, SIGNAL(readyRead()), SLOT(slot_Lichka_Messege_Show_Window()));
    connect(ui->list_for_IP, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(slot_Lichka_Messege_Send(QListWidgetItem*)));

    connect(ui->Set_Port, SIGNAL(clicked()), SLOT(slot_Change_PORT()));
    connect(ui->go_fly, SIGNAL(clicked()), SLOT(slotSendDatagram()));
    connect(ui->go, SIGNAL(clicked()), SLOT(slot_Magic()));
    connect(ui->go, SIGNAL(clicked()), SLOT(slot_Privet_Messege()));
    connect(ui->Name_Set, SIGNAL(returnPressed()), SLOT(slot_Magic()));

    ui->Chat->setTextBackgroundColor(QColor(50,50,50));
    ui->Chat->setTextColor(QColor(20,255,255));
    ui->Chat->setAlignment(Qt::AlignCenter);
    ui->Name_Set->setAlignment(Qt::AlignCenter);
    ui->Chat->append("Установлен порт: " + QString::number(nomer_porta) + "\n");
    ui->Chat->setVisible(false);
    ui->Vvedite_vash_message->setVisible(false);
    ui->Port->setVisible(false);
    ui->go_fly->setVisible(false);
    ui->Vvod->setVisible(false);
    ui->Vvod_Nomer_Porta->setVisible(false);
    ui->Name->setVisible(false);
    ui->Set_Port->setVisible(false);
    ui->Name_Set->setVisible(true);
    ui->Start_text->setVisible(true);
    ui->go->setVisible(true);
    ui->Chat->setReadOnly(1);
    ui->list_online->setViewMode(QListView::ListMode);
    ui->list_online->setFlow(QListView::TopToBottom);
    ui->list_online->setItemAlignment(Qt::AlignHCenter);

    QString localhostname =  QHostInfo::localHostName();
    QList<QHostAddress> hostList = QHostInfo::fromName(localhostname).addresses();
    for(int i=0; i<hostList.count(); i++)
    {
        if (hostList[i].protocol() == QAbstractSocket::IPv4Protocol && hostList[i].isLoopback() == false) {
           this->my_adress = hostList[i];
        }
    }

    for(int i = 0; i<5; i++)
    {
        QListWidgetItem *item = new QListWidgetItem;
        QListWidgetItem *item_IP = new QListWidgetItem;
        ui->list_online->addItem(item);
        ui->list_for_IP->addItem(item_IP);
    }

    socket_online = new QUdpSocket(this);
    socket_online->bind(port_online);
    connect(socket_online, SIGNAL(readyRead()), SLOT(slot_Online_Messege()));

    socket_ofline = new QUdpSocket(this);
    socket_ofline->bind(port_ofline);
    connect(socket_ofline, SIGNAL(readyRead()), SLOT(slot_Ofline_Messege()));

    ptimer_1 = new QTimer(this);
    ptimer_2 = new QTimer(this);
    connect(ptimer_1, SIGNAL(timeout()), SLOT(slot_Show_Host()));
    connect(ptimer_2, SIGNAL(timeout()), SLOT(slot_Send_Name()));
    ptimer_1->setInterval(500);
    ptimer_1->start();
    ptimer_2->setInterval(2000);
    ptimer_2->start();

}

MainWindow::~MainWindow()
{
    QString bb_messege = Name + ":\nЯ устал, я ухожу\nВсем пока!!!";
    socket_ofline->writeDatagram(bb_messege.toUtf8(), QHostAddress::Broadcast, port_ofline);
    delete ui;
}

void MainWindow::slot_Magic()
{
    ui->Chat->setVisible(true);
    ui->Vvedite_vash_message->setVisible(true);
    ui->Port->setVisible(true);
    ui->go_fly->setVisible(true);
    ui->Vvod->setVisible(true);
    ui->Vvod_Nomer_Porta->setVisible(true);
    ui->Name->setVisible(true);
    ui->Set_Port->setVisible(true);
    ui->Name_Set->setVisible(false);
    ui->Start_text->setVisible(false);
    ui->go->setVisible(false);
    Name = ui->Name_Set->text();
    ui->Name->setText("Ваше Имя: " + ui->Name_Set->text());
}

void MainWindow::slot_Change_PORT()
{
    int chislo = ui->Vvod_Nomer_Porta->value();
    nomer_porta = static_cast<quint16>(chislo);
    QString str = QString::number(chislo);
    ui->Chat->setTextColor(QColor(20,255,255));
    ui->Chat->setAlignment(Qt::AlignCenter); // Выравнивание по центру
    ui->Chat->append("Выбран новый порт: " + str + "\n");
    m_pudp->close();
    m_pudp->bind(nomer_porta);
}

void MainWindow::slot_Online_Messege()
{
    QByteArray baDatagram;
    QHostAddress fly_adress;
    do
    {
    baDatagram.resize(socket_online->pendingDatagramSize());
    socket_online->readDatagram(baDatagram.data(), baDatagram.size(), &fly_adress, &port_online);
    }
    while(socket_online->hasPendingDatagrams());
    QString Sender_Name(baDatagram);
    if (!Table_Name_IP.contains(fly_adress.toString()))
    {
        Table_Name_IP.insert(fly_adress.toString(), Sender_Name);
        List_IP.append(fly_adress);
    }
    else
    {
      Table_Name_IP[fly_adress.toString()]=Sender_Name;
    }
}

void MainWindow::slot_Ofline_Messege()
{
    QByteArray baDatagram;
    QHostAddress fly_adress;
    do
    {
    baDatagram.resize(socket_ofline->pendingDatagramSize());
    socket_ofline->readDatagram(baDatagram.data(), baDatagram.size(), &fly_adress, &port_ofline);
    }
    while(socket_ofline->hasPendingDatagrams());
    QString Sender_Name(baDatagram);
    ui->Chat->setTextColor(QColor(255,255,0));
    ui->Chat->setAlignment(Qt::AlignCenter); // Выравнивание по центру
    ui->Chat->append(Sender_Name);
    Table_Name_IP.remove(fly_adress.toString());
    List_IP.removeOne(fly_adress);
}

void MainWindow::slot_Show_Host()
{

    for(int i = 0; i<5; i++)
    {
        ui->list_online->item(i)->setText(" ");
        ui->list_for_IP->item(i)->setText(" ");

    }
    int itemNumber = 0;
    QMap <QString,QString>::iterator i;
    for (i = Table_Name_IP.begin(); i != Table_Name_IP.end();  i++)
    {
        //ui->list_online->setViewMode(QListView::ListMode);
        //ui->list_online->setFlow(QListView::TopToBottom);
        //ui->list_online->setItemAlignment(Qt::AlignHCenter);

       ui->list_online->item(itemNumber)->setText(i.value());
       itemNumber++;
    }
    for(int i=0; i<List_IP.count(); i++)
    {
     ui->list_for_IP->item(i)->setText(List_IP[i].toString().remove(0,7));
    }
}

void MainWindow::slot_Send_Name()
{
    QByteArray ba = Name.toUtf8();
    socket_online->writeDatagram(ba.data(), ba.size(), QHostAddress::Broadcast, port_online);
}

void MainWindow::slot_Privet_Messege()
{
    QString privet = Name + " теперь в чате!\n";
    socket_ofline->writeDatagram(privet.toUtf8(), QHostAddress::Broadcast, port_ofline);
}

void MainWindow::slotProcessDatagrams()
{
    QByteArray baDatagram;
    QHostAddress fly_adress;
    do
    {
    baDatagram.resize(m_pudp->pendingDatagramSize());
    m_pudp->readDatagram(baDatagram.data(), baDatagram.size(), &fly_adress);
    }
    while(m_pudp->hasPendingDatagrams());
    QString stroka_messege(baDatagram);
    if (fly_adress.toString().remove(0,7) != my_adress.toString())
    {
        ui->Chat->setAlignment(Qt::AlignLeft); // Выравнивание
        ui->Chat->setTextColor(QColor(255,255,0)); // Цвет текста
        ui->Chat->append(stroka_messege + "\n");
    }
    //ui->Chat->append("Не мой IP: \n" + fly_adress.toString().remove(0,7)); //.remove(0,7)); //IP-adress
    //ui->Chat->append("Мой IP: \n" + my_adress.toString()); // ip компа-отправителя
    //ui->Chat->append(QHostInfo::localHostName());
}

void MainWindow::slotSendDatagram()

{
    QString dt = Name + ": \n" + ui->Vvod->toPlainText(); //Имя: + Сообщение
    if(ui->Vvod->toPlainText() != nullptr)
    {
        m_pudp->writeDatagram(dt.toUtf8(), QHostAddress::Broadcast, nomer_porta);
        ui->Chat->setTextColor(QColor(0,255,0)); // Цвет текста
        ui->Chat->setAlignment(Qt::AlignRight);
        ui->Chat->append("Вы: \n" + ui->Vvod->toPlainText() + "\n");
        ui->Chat->setAlignment(Qt::AlignLeft);
        ui->Vvod->clear();
    }
}

void MainWindow::slot_Lichka_Messege_Send(QListWidgetItem *item)
{
    QHostAddress receiver_IP;
    receiver_IP.setAddress(item->text());
    socket_LS->writeDatagram(Name.toUtf8(), receiver_IP, port_LS);
    Lichka *LS = new Lichka(Name.toUtf8(), receiver_IP);
    LS->show();
}

void MainWindow::slot_Lichka_Messege_Show_Window()
{
    QByteArray baDatagram;
    QHostAddress fly_adress;
    do
    {
    baDatagram.resize(socket_LS->pendingDatagramSize());
    socket_LS->readDatagram(baDatagram.data(), baDatagram.size(), &fly_adress);
    }while(socket_LS->hasPendingDatagrams());

    QString Name_transmitter(baDatagram);
    if (fly_adress.toString().remove(0,7) != my_adress.toString())
    {
        Lichka *LS = new Lichka(Name_transmitter, fly_adress);
        LS->show();
        //ui->Chat->append("proverka\n");
    }
}
