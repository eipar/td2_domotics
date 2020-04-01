#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

//Mostrar en consola
#include <QDebug>

//Manejo Strings
#include <QString>

//Manejo de Archivos
#include <QFile>
#include <QTextStream>

//Pop Up's
#include <QMessageBox>

//Ventanas
#include "dialog_admin.h"
#include "dialog_user.h"
#include "dialog_forgot.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_mw_u_lineEdit_editingFinished();    //MW: User en mayúscula

    void on_mw_buttonBox_rejected();            //MW: Cancel
    void on_mw_buttonBox_accepted();            //Mw: Ok

    void on_actionOlvid_su_contrase_a_triggered();  //MW: Olvidó contraseña

private:
    Ui::MainWindow *ui;

    void TestDB(void);                          //Verifica si está creado el archivo
    QString Check(QString, QString);            //Verifica usuario y contraseña en el archivo. Devuelve admin o el usuario
};

#endif // MAINWINDOW_H
