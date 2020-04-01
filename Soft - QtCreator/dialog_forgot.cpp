#include "dialog_forgot.h"
#include "ui_dialog_forgot.h"

dialog_forgot::dialog_forgot(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialog_forgot)
{
    ui->setupUi(this);

    this->setFixedSize(this->width(), this->height());
}

dialog_forgot::~dialog_forgot()
{
    delete ui;
}

/* Slots */

void dialog_forgot::on_Forgot_User_lineEdit_editingFinished()
{
    ui->Forgot_User_lineEdit->setText(ui->Forgot_User_lineEdit->text().toUpper());
}

void dialog_forgot::on_Forgot_buttonBox_rejected()
{
    QMessageBox msgSalir;

    msgSalir.setText("Desea cancelar?\n");
    msgSalir.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgSalir.setIcon(QMessageBox::Question);

    int Op = msgSalir.exec();

    if(Op == QMessageBox::Ok)
        this->close();
}

void dialog_forgot::on_Forgot_buttonBox_accepted()
{
    if(ComprobarDatos() == true){
        if(ComprobarUP(ui->Forgot_User_lineEdit->text(), ui->Forgot_Word_lineEdit->text()) == true){
            if(ui->Forgot_Ingr1_lineEdit->text() == ui->Forgot_Ingr2_lineEdit->text()){
                //Guardar nueva pass
                GuardarPass(ui->Forgot_User_lineEdit->text());
            }else{
                MostrarError(3);
            }
        }else{
            MostrarError(2);
        }
    }else{
        MostrarError(1);
    }
}

/* Métodos propios */

void dialog_forgot::MostrarError(unsigned int valor){
    QMessageBox msgError;

    switch(valor){
        case 1:
            msgError.setText("No completó todos los datos\n"); break;
        case 2:
            msgError.setText("No coincide Usuario con Palabra Secreta\n"); break;
        case 3:
            msgError.setText("No coinciden las contraseñas ingresadas\n"); break;
        case 4:
            msgError.setText("Error abriendo archivo. Vuelva a empezar\n"); break;
        case 5:
            msgError.setText("No se encontró el usuario\n"); break;
        case 6:
            msgError.setText("Contrasea reestablecida !\n"); break;
        default:
            msgError.setText("Error indefinido\n"); break;
    }

    msgError.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgError.setIcon(QMessageBox::Warning);
    int Op = msgError.exec();

    if(Op == QMessageBox::Ok){
        this->close();
    }
}

bool dialog_forgot::ComprobarDatos(){

    if((ui->Forgot_User_lineEdit->text().isEmpty() == false)&&(ui->Forgot_Word_lineEdit->text().isEmpty() == false)&&(ui->Forgot_Ingr1_lineEdit->text().isEmpty() == false)&&(ui->Forgot_Ingr2_lineEdit->text().isEmpty() == false)){
        //No están vacios
        return true;
    }
    //Estan vacios
    return false;
}

bool dialog_forgot::ComprobarUP(QString usuario, QString palabra){
    QFile arch("db.txt");
    QStringList linea;

    if(!arch.open(QFile::ReadOnly | QFile::Text)){
        MostrarError(4);
    }

    QTextStream in(&arch);

    while(!in.atEnd()){
        QString aux = in.readLine();
        linea = aux.split(',');

        if(linea.at(0) == usuario && linea.at(2) == palabra){
            in.flush();
            arch.close();
            return true;
        }
    }

    in.flush();
    arch.close();
    return false;
}

void dialog_forgot::GuardarPass(QString usuario){
    QFile arch("db.txt");
    QFile copy("temp.txt");
    QStringList linea; int flag = 0;

    if(!arch.open(QFile::ReadOnly | QFile::Text)){
        MostrarError(4);
    }

    if(!copy.open(QFile::ReadWrite | QFile::Text)){
        MostrarError(4);
    }

    QTextStream in(&arch);
    QTextStream out(&copy);

    while(!in.atEnd()){
        QString aux = in.readLine();
        linea = aux.split(',');

        qDebug() << linea.at(0);
        qDebug() << usuario;

        if(linea.at(0) == usuario){
            linea.replace(1, ui->Forgot_Ingr1_lineEdit->text());
            flag = 1;
        }

        out << linea.at(0) << "," << linea.at(1) << "," << linea.at(2) << "," << linea.at(3) << "," << linea.at(4) << "," << linea.at(5) << "," << linea.at(6) << "," << linea.at(7) << "," << linea.at(8) << "\n";
    }

    if(flag == 0)
        MostrarError(5);
    else
        MostrarError(6);

    in.flush();                 //Vacio el flujo de datos
    out.flush();

    arch.close();            //Cierro ambos archivos
    copy.close();

    arch.remove();
    copy.rename("temp.txt", "db.txt");

    this->close();
}
