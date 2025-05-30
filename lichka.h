#ifndef LICHKA_H
#define LICHKA_H

#include <QDialog>
#include <QUdpSocket>
#include "mainwindow.h"

namespace Ui {
class Lichka;
}

class Lichka : public QDialog
{
    Q_OBJECT

public:
    explicit Lichka(QString Name, QHostAddress IP, QWidget *parent = nullptr);
    ~Lichka();

     QUdpSocket* LS_pudp;
     quint16 nomer_porta = 229;
     QHostAddress my_adress;
     QHostAddress IP;

public slots:
    void slotProcessDatagrams();
    void slotSendDatagram();

private:
    Ui::Lichka *ui;
};

#endif // LICHKA_H
