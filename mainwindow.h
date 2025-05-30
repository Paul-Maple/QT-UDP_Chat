#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QUdpSocket>
#include <QTimer>
#include <QMap>
#include <QListWidgetItem>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
   QUdpSocket* m_pudp;
   QUdpSocket* socket_online;
   QUdpSocket* socket_ofline;
   QUdpSocket* socket_LS;
   quint16 nomer_porta = 228;
   quint16 port_online = 6500;
   quint16 port_ofline = 9999;
   quint16 port_LS = 2;
   QString Name;
   QTimer *ptimer_1;
   QTimer *ptimer_2;
   QHostAddress my_adress;
   QMap <QString,QString> Table_Name_IP;
   QList<QHostAddress> List_IP;

private:
    Ui::MainWindow *ui;

public slots:
    void slotProcessDatagrams();
    void slotSendDatagram();
    void slot_Magic();
    void slot_Change_PORT();

    void slot_Online_Messege();
    void slot_Ofline_Messege();
    void slot_Show_Host();
    void slot_Send_Name();
    void slot_Privet_Messege();

    void slot_Lichka_Messege_Send(QListWidgetItem *item);
    void slot_Lichka_Messege_Show_Window();

};

#endif // MAINWINDOW_H
