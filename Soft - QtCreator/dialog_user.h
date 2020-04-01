#ifndef DIALOG_USER_H
#define DIALOG_USER_H

#include <QDialog>

#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QFile>
#include <QDebug>
#include <QTime>

//Libreria Puerto Serie
#include <qextserialport.h>
#include <qextserialenumerator.h>

namespace Ui {
class dialog_user;
}

class dialog_user : public QDialog
{
    Q_OBJECT

public:
    explicit dialog_user(QWidget *parent = 0);
    void setData(QString);
    void CargarDatos(void);
    ~dialog_user();

private slots:
    //Puerto Serie
    void onDatosRecibidos(void);
    void on_User_Serie_pushButton_clicked();

    //User Window
    void on_User_pushButton_clicked();
    void on_User_Config_checkBox_clicked();
    void on_User_Pref_checkBox_clicked();

private:
    Ui::dialog_user *ui;

    QextSerialPort      *Serial_Port;
    bool                Serial_Port_Conected;
    int                 m_cant_bytes_recibios;
    int                 m_cant_bytes_enviados;
    QByteArray          Buffer_Recepcion;

    //Puerto Serie
    void EnumerarPuertos(void);
    void EnviarComandoBis(quint8*);
    void ProcesarDatosRecibidos(QByteArray&bytes);
    void CerrarPuerto(void);

    bool Reemplazar(QString);
    void BuscarID(quint8, quint8*vecaux);
    bool Agregar(quint8);
};

#endif // DIALOG_USER_H
