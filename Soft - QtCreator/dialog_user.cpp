#include "dialog_user.h"
#include "ui_dialog_user.h"

dialog_user::dialog_user(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialog_user)
{
    ui->setupUi(this);

    EnumerarPuertos();
    this->setFixedSize(this->width(), this->height());
}

dialog_user::~dialog_user()
{
    delete ui;
}

void dialog_user::on_User_pushButton_clicked()
{
    QMessageBox msgSalir;

    msgSalir.setText("Desea salir del programa?\n");
    msgSalir.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgSalir.setIcon(QMessageBox::Question);

    int Op = msgSalir.exec();

    if(Op == QMessageBox::Ok)
        this->close();
}

void dialog_user::on_User_Config_checkBox_clicked()
{
    QMessageBox msgError;

    if(ui->User_Config_checkBox->isChecked() == true){
        ui->User_Config_Pass_lineEdit->setEnabled(true);
        ui->User_Config_lineEdit_Word->setEnabled(true);

        ui->User_Config_checkBox->setText("Finalizar");
    }else{
        ui->User_Config_Pass_lineEdit->setEnabled(false);
        ui->User_Config_lineEdit_Word->setEnabled(false);

        ui->User_Config_checkBox->setText("Habilitar Edición");

        if(Reemplazar(ui->User_Config_User_lineEdit->text()) == true){
            msgError.setText("Cambios realizados correctamente ! \n");
            msgError.setIcon(QMessageBox::Information);
            CargarDatos();
        }else{
            msgError.setText("Error al intentar cambiar datos. Vuelva a intentarlo \n");
            msgError.setIcon(QMessageBox::Critical);
        }
        msgError.setStandardButtons(QMessageBox::Ok);
        msgError.exec();
    }
}

void dialog_user::on_User_Pref_checkBox_clicked()
{
    QMessageBox msgError;

    if(ui->User_Pref_checkBox->isChecked() == true){
        ui->User_Pref_Temp_spinBox->setEnabled(true);
        ui->User_Pref_Luz_spinBox->setEnabled(true);

        ui->User_Pref_checkBox->setText("Finalizar");
    }else{
        ui->User_Pref_Temp_spinBox->setEnabled(false);
        ui->User_Pref_Luz_spinBox->setEnabled(false);

        ui->User_Pref_checkBox->setText("Habilitar Edición");
        if(Reemplazar(ui->User_Config_User_lineEdit->text()) == true){
            msgError.setText("Cambios realizados correctamente ! \n");
            msgError.setIcon(QMessageBox::Information);
            CargarDatos();
        }else{
            msgError.setText("Error al intentar cambiar datos. Vuelva a intentarlo \n");
            msgError.setIcon(QMessageBox::Critical);
        }
        msgError.setStandardButtons(QMessageBox::Ok);
        msgError.exec();
    }
}

void dialog_user::setData(QString usuario){
    ui->User_Config_User_lineEdit->setText(usuario);
}

bool dialog_user::Reemplazar(QString user){
    QFile Archivo("db.txt");                                    //Puntero al archivo
    QFile Archivo_Copy("temp.txt");                             //Puntero a un archivo temporal
    QStringList Renglon;

    if(!Archivo.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "Error al abrir archivo db.txt";
        return false;
    }

    if(!Archivo_Copy.open(QFile::WriteOnly | QFile::Text)){
        qDebug() << "Error al abrir archivo temp.txt";
        return false;
    }

    QTextStream in(&Archivo);
    QTextStream out(&Archivo_Copy);

    while(!in.atEnd()){
        QString aux = in.readLine();
        Renglon = aux.split(',');

        if(Renglon.at(0) == user){
            Renglon.insert(1, ui->User_Config_Pass_lineEdit->text());
            Renglon.insert(2, ui->User_Config_lineEdit_Word->text());
            Renglon.insert(3, "ID");
            Renglon.insert(4, ui->label_ID_User_Num_User->text());
            Renglon.insert(5, "T");
            Renglon.insert(6, ui->User_Pref_Temp_spinBox->text());
            Renglon.insert(7, "L");
            Renglon.insert(8, ui->User_Pref_Luz_spinBox->text());
        }
        out << Renglon.at(0) << "," << Renglon.at(1) << "," << Renglon.at(2) << "," << Renglon.at(3) << "," << Renglon.at(4) << "," << Renglon.at(5) << "," << Renglon.at(6) << "," << Renglon.at(7) << "," << Renglon.at(8) << "\n";
    }

    in.flush();                 //Vacio el flujo de datos
    out.flush();

    Archivo.close();            //Cierro ambos archivos
    Archivo_Copy.close();

    Archivo.remove();
    Archivo_Copy.rename("temp.txt", "db.txt");

    return true;
}

void dialog_user::CargarDatos(void){
    QFile File("db.txt");

    if(!File.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "Error al abrir el archivo desde CargarDatos";
        return;
    }

    QTextStream in(&File);

    while(!in.atEnd()){
        QStringList Renglon = in.readLine().split(',');

        if(Renglon.at(0) == ui->User_Config_User_lineEdit->text()){
            ui->User_Config_Pass_lineEdit->setText(Renglon.at(1));
            ui->User_Config_lineEdit_Word->setText(Renglon.at(2));
            ui->label_ID_User_Num_User->setText(Renglon.at(4));
            ui->User_Pref_Temp_spinBox->setValue(Renglon.at(6).toInt());
            ui->User_Pref_Luz_spinBox->setValue(Renglon.at(8).toInt());
            Renglon = in.readLine().split(',');
        }
    }

    in.flush();
    File.close();
}

void dialog_user::BuscarID(quint8 id, quint8 *vecaux){
    QFile Arch("db.txt");
    QStringList retorno; quint8 auxid;
    bool ok;

    if(!Arch.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "Error al abrir archivo db.txt";
    }

    QTextStream in(&Arch);

    while(!in.atEnd()){
        QString aux = in.readLine();
        retorno = aux.split(',');
        aux = retorno.at(4);

        auxid = aux.toInt(&ok, 10);

        if(auxid == id){
            in.flush();
            Arch.close();
            vecaux[0] = id;
            aux = retorno.at(6);
            vecaux[1] = aux.toInt(&ok, 10);     //Temp
            aux = retorno.at(8);
            vecaux[2] = aux.toInt(&ok, 10);     //Luz
        }
    }

    in.flush();
    Arch.close();
}

bool dialog_user::Agregar(quint8 id){
    QFile Archivo("db.txt");
    QStringList Renglon; QString aux;

    if(!Archivo.open(QFile::Append | QFile::Text)){
        qDebug() << "Error al abrir archivo db.txt";
        return false;
    }

    QTextStream out(&Archivo);

    aux.append(QString("%1").arg(id));

    Renglon.insert(0, aux);
    Renglon.insert(1, aux);
    Renglon.insert(2, aux);
    Renglon.insert(3, "ID");
    Renglon.insert(4, aux);
    Renglon.insert(5, "T");
    Renglon.insert(6, "0");
    Renglon.insert(7, "L");
    Renglon.insert(8, "0");

    out << Renglon.at(0) << "," << Renglon.at(1) << "," << Renglon.at(2) << "," << Renglon.at(3) << "," << Renglon.at(4) << "," << Renglon.at(5) << "," << Renglon.at(6) << "," << Renglon.at(7) << "," << Renglon.at(8) << "\n";

    out.flush();

    Archivo.close();

    return true;
}

/* ************ Puerto Serie ************ */

void dialog_user::onDatosRecibidos(){
    QByteArray bytes;

    int cant = Serial_Port->bytesAvailable();       //Determino la cant de bytes disponibles
    bytes.resize(cant);

    if(cant == 3){
        Serial_Port->read(bytes.data(), bytes.size());

        ProcesarDatosRecibidos(bytes);
    }
}

void dialog_user::EnumerarPuertos(){
    ui->User_Serie_comboBox->clear();

    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();

    for(int i = 0; i < ports.size(); i++){
    #ifdef Q_OS_WIN
        ui->User_Serie_comboBox->addItem(ports.at(i).friendName, ports.at(i).portName);
    #elif (defined Q_OS_UNIX)
        ui->User_Serie_comboBox->addItem(ports.at(i).friendName, ports.at(i).physName);
    #endif

    }
}

void dialog_user::ProcesarDatosRecibidos(QByteArray & bytes){
    int i; quint8 info[3], aux[3];
    QTime now = QTime::currentTime();

    for(i = 0; i < bytes.size(); i++){
        Buffer_Recepcion.append(bytes.at(i));
    }

    for(i = 0; i < Buffer_Recepcion.size(); i++){
        info[i] = static_cast<quint8>(Buffer_Recepcion.at(i));
        qDebug() << info[i];
    }

    Buffer_Recepcion.clear();

    if(info[0] == 255){
        qDebug() << "Envío estado actual";

        if(info[2] == 3) ui->User_Data_lcdNumber_Luz->display("50");     //Luz Natural 50%
        if(info[2] == 4) ui->User_Data_lcdNumber_Luz->display("90");     //Luz Light   90%
        if(info[2] == 5) ui->User_Data_lcdNumber_Luz->display("10");     //Luz Oscura  10%

        ui->User_Data_lcdNumber_Temp->display(info[1]);
    }else{
        if(info[0] == 250){
            //Tengo que enviar fecha y hora
            aux[0] = 0; aux[1] = now.hour(); aux[2] = now.minute();
            EnviarComandoBis(aux);
        }else{
            if(info[1] == 255 && info[2] == 255){
                //Nuevo User
                qDebug() << "Nuevo User";
                Agregar(info[0]);
            }
            if(info[1] == 200 && info[2] == 200){
                //Enviar pref
                qDebug() << "Envio de pref";
                BuscarID(info[0], aux);
             EnviarComandoBis(aux);
             }
        }
    }

    for(i = 0; i < 3; i++) info[i] = 0;
}

void dialog_user::EnviarComandoBis(quint8*aux){
    int i;

    if(Serial_Port){
        for(i = 0; i<3; i++){
            Serial_Port->putChar(aux[i]);
        }
    }

    qDebug() << "Enviado x Bis";
}

void dialog_user::CerrarPuerto(){
    Serial_Port->close();
    delete Serial_Port;
    Serial_Port = NULL;
}

void dialog_user::on_User_Serie_pushButton_clicked()
{
    if(Serial_Port_Conected == false){  //No está conectado
        //Obtenemos el nombre del puerto
        QString sPort;

        int i = ui->User_Serie_comboBox->currentIndex();

        if(i != -1 && ui->User_Serie_comboBox->itemText(i) == ui->User_Serie_comboBox->currentText()){
            sPort = ui->User_Serie_comboBox->itemData(i).toString();
        }else{
            sPort = ui->User_Serie_comboBox->currentText();
        }

        Serial_Port = new QextSerialPort(sPort, QextSerialPort::EventDriven);       //Creo el objeto tipo PS

        //Seteo la config del puerto
        Serial_Port->setBaudRate(BAUD9600);
        Serial_Port->setFlowControl(FLOW_OFF);
        Serial_Port->setParity(PAR_NONE);
        Serial_Port->setDataBits(DATA_8);
        Serial_Port->setStopBits(STOP_1);

        //Abro el puerto en modo R/W
        if(Serial_Port->open(QIODevice::ReadWrite) == true){

            connect(Serial_Port, SIGNAL(readyRead()), this, SLOT(onDatosRecibidos()));

            Serial_Port_Conected = true;
            ui->User_Serie_label_Con->setText("Conectado");
            ui->User_Serie_pushButton->setText("Desconectar");

        }else{
            //Hubo un error en abrir el puerto
            ui->User_Serie_pushButton->setText("Conectar");
            ui->User_Serie_label_Con->setText("Error - Desconectado");
            CerrarPuerto();
        }
    }else{
        ui->User_Serie_pushButton->setText("Conectar");
        ui->User_Serie_label_Con->setText("Desconectar");
        Serial_Port_Conected = false;
        CerrarPuerto();
    }
}
