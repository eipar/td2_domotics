#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setFixedSize(this->width(), this->height());  //Seteo fijo de tamaño de ventana

    TestDB();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/* Método para pasar a mayúscula el usuario */
void MainWindow::on_mw_u_lineEdit_editingFinished()
{
    ui->mw_u_lineEdit->setText(ui->mw_u_lineEdit->text().toUpper());
}

/* Se desea salir del programa */
void MainWindow::on_mw_buttonBox_rejected()
{
    QMessageBox msgCancel;

    msgCancel.setIcon(QMessageBox::Warning);
    msgCancel.setText("Desea salir?\n");
    msgCancel.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);

    int Op = msgCancel.exec();

    if(Op == QMessageBox::Ok)
        this->close();
}

/* Se ingresó usuario y pass y se desea ingresar al programa */
void MainWindow::on_mw_buttonBox_accepted()
{
    if(ui->mw_u_lineEdit->text().isEmpty() | ui->mw_p_lineEdit->text().isEmpty()){
        QMessageBox msgVacio;

        msgVacio.setText("No completó los datos!");
        msgVacio.setIcon(QMessageBox::Information);
        msgVacio.setStandardButtons(QMessageBox::Ok);

        msgVacio.exec();
    }else{
        //Verifico user & pass
        QString aux;

        if((aux = Check(ui->mw_u_lineEdit->text(), ui->mw_p_lineEdit->text())) == NULL){
            QMessageBox error;
            error.setText("Usuario o contraseña incorrectos\n");
            error.setIcon(QMessageBox::Warning);
            error.exec();
        }else{
            if(aux == "ADMIN"){
                Dialog_Admin WinAdmin;

                WinAdmin.setModal(true);
                WinAdmin.exec();
                //this->close();
            }else{
                dialog_user WinUser;

                WinUser.setModal(true);
                WinUser.setData(ui->mw_u_lineEdit->text());
                WinUser.CargarDatos();
                WinUser.exec();
                //this.close();
            }
        }
    }
    ui->mw_u_lineEdit->clear(); ui->mw_p_lineEdit->clear();
}

/* Testea si está creado el archivo con los datos. Si no lo está, lo crea y pone el usuario admin por default. */
void MainWindow::TestDB(){
    QFile Archivo;

    if(Archivo.exists("db.txt") == false){
        //qDebug() << "no existe";
        Archivo.setFileName("db.txt");

        if(!Archivo.open(QFile::Append | QFile::ReadWrite | QFile::Text)){
            //qDebug() << "Error al abrir el archivo desde funcion TestDB";
            QMessageBox error;
            error.setText("No se pudo abrir el archivo!\nDeberá reiniciar el programa!\n");
            error.exec();
            this->close();
        }else{
            //qDebug() << "Lo abrio";
            QTextStream in(&Archivo);

            QString Renglon = in.readLine();
            in << QString("ADMIN,admin,admin,ID,0,T,0,L,0\n");
        }

        Archivo.close();
    }
}

QString MainWindow::Check(QString usuario, QString contrs){
    QFile Archivo("db.txt");

    if(!Archivo.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox error;
        error.setText("No se pudo abrir el archivo!\nSe reiniciará el programa!\n");
        error.exec();
        this->close();
    }

    QTextStream in(&Archivo);

    QString aux = in.readLine();
    QStringList spliteado = aux.split(',');

    while(!aux.isNull()){
        if(usuario == spliteado.at(0) && contrs == spliteado.at(1)){
            return usuario;
        }
        aux = in.readLine();

        spliteado = aux.split(',');
    }

    return NULL;
}

void MainWindow::on_actionOlvid_su_contrase_a_triggered()
{
    dialog_forgot Win;

    Win.setModal(true);
    Win.exec();
}
