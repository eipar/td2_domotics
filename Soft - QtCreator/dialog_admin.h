#ifndef DIALOG_ADMIN_H
#define DIALOG_ADMIN_H

#include <QDialog>

//Consola
#include <QDebug>

//Widget de Lisa
#include <QListWidget>

//Ventanas
#include <QMessageBox>

//Manejo Strings
#include <QString>

//Manejo de Archivos
#include <QFile>
#include <QTextStream>

#include <QTime>

//Libreria Puerto Serie
#include <qextserialport.h>
#include <qextserialenumerator.h>

namespace Ui {
class Dialog_Admin;
}

class Dialog_Admin : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_Admin(QWidget *parent = 0);
    ~Dialog_Admin();

private slots:
    void onDatosRecibidos(void);
    void on_Admin_pushButton_Exit_clicked();
    void on_Admin_Pref_checkBox_THL_clicked();
    void on_Admin_Pref_checkBox_Pass_clicked();
    void on_Admin_User_checkBox_clicked();
    void on_Admin_User_listWidget_itemActivated(QListWidgetItem *item);
    void on_Admin_User_pushButton_Eliminar_clicked();
    void on_Admin_Config_pushButton_Serie_clicked();

private:
    Ui::Dialog_Admin *ui;

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

    //Manejo de archivos
    void CargarDatos(void);
    bool Reemplazar(QString);
    QStringList Buscar(QString);
    void BuscarID(quint8, quint8*vecaux);
    bool Clear(void);
    void Guardar(void);
    void CargarUsuarios(void);
    bool Eliminar(QString);
    bool Agregar(quint8);
};

#endif // DIALOG_ADMIN_H
