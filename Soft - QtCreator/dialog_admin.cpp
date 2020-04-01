#include "dialog_admin.h"
#include "ui_dialog_admin.h"

Dialog_Admin::Dialog_Admin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_Admin)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(), this->height());  //Seteo fijo de tamaño de ventana

    CargarDatos();
    EnumerarPuertos();
}

Dialog_Admin::~Dialog_Admin()
{
    delete ui;
}

/* Eventos */

//Click Botón Salir - Salir de la ventana
void Dialog_Admin::on_Admin_pushButton_Exit_clicked()
{
    QMessageBox msgSalir;

    msgSalir.setText("Desea salir del programa?\n");
    msgSalir.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgSalir.setIcon(QMessageBox::Question);

    int Op = msgSalir.exec();

    if(Op == QMessageBox::Ok)
        this->close();
}

//Habilitar edición Preferencias del Admin - SpinBox Temperatura, Humedad y Luz
void Dialog_Admin::on_Admin_Pref_checkBox_THL_clicked()
{
    if(ui->Admin_Pref_checkBox_THL->isChecked() == true){
        ui->Admin_Pref_spinBox_Temp->setEnabled(true);
        ui->Admin_Pref_spinBox_Luz->setEnabled(true);
        ui->Admin_Pref_checkBox_THL->setText("Finalizar");
    }else{
        ui->Admin_Pref_checkBox_THL->setText("Habilitar Edición");
        ui->Admin_Pref_spinBox_Temp->setEnabled(false);
        ui->Admin_Pref_spinBox_Luz->setEnabled(false);
        Reemplazar("ADMIN");                            //Método para reemplazar las preferencias de un usuario
    }
}

//Habilitar edición Preferencias del Admin - Contraseña
void Dialog_Admin::on_Admin_Pref_checkBox_Pass_clicked()
{
    if(ui->Admin_Pref_checkBox_Pass->isChecked() == true){
        ui->Admin_Pref_lineEdit_Ingr1->setEnabled(true);
        ui->Admin_Pref_lineEdit_Ingre2->setEnabled(true);
        ui->Admin_Pref_checkBox_Pass->setText("Finalizar");
    }else{
        ui->Admin_Pref_lineEdit_Ingr1->setEnabled(false);
        ui->Admin_Pref_lineEdit_Ingre2->setEnabled(false);
        if(ui->Admin_Pref_lineEdit_Ingr1->text() == ui->Admin_Pref_lineEdit_Ingre2->text()){
            Reemplazar("ADMIN");                            //Método para reemplazar las preferencias de un usuario
            QMessageBox msgFin; msgFin.setText("Cambios realizados satisfactoriamente ! \n");
            msgFin.exec();
        }else{
            QMessageBox msgError;
            msgError.setText("Las contraseñas no son iguales! \n");
            msgError.setStandardButtons(QMessageBox::Ok);
            msgError.exec();
            //ui->Admin_Pref_checkBox_Pass->setChecked(false);
            ui->Admin_Pref_lineEdit_Ingr1->clear(); ui->Admin_Pref_lineEdit_Ingre2->clear();
        }
        ui->Admin_Pref_checkBox_Pass->setText("Habilitar Edición");
    }
}

//Selección de un usuario en lista para mostrar los datos
void Dialog_Admin::on_Admin_User_listWidget_itemActivated(QListWidgetItem *item)
{
    QStringList lista_auxiliar = Buscar(item->text());

    if(lista_auxiliar.isEmpty() != true){
        ui->Admin_User_lineEdit_User->setText(lista_auxiliar.at(0));
        ui->Admin_User_lineEdit_Pass->setText(lista_auxiliar.at(1));
        ui->Admin_User_lineEdit_Word->setText(lista_auxiliar.at(2));
        ui->label_ID_User_Num->setText(lista_auxiliar.at(4));
        ui->Admin_User_spinBox_Temp->setValue(lista_auxiliar.at(6).toInt());
        ui->Admin_User_spinBox_Luz->setValue(lista_auxiliar.at(8).toInt());
    }else{
        QMessageBox Win;
        Win.setText("Error al buscar el usuario!\n");
        Win.setIcon(QMessageBox::Warning);
        Win.setStandardButtons(QMessageBox::Ok);
        Win.exec();
    }
}

//Edición de un usuario seleccioando
void Dialog_Admin::on_Admin_User_checkBox_clicked()
{
    if(ui->Admin_User_lineEdit_User->text().isEmpty() == false){
        if(ui->Admin_User_checkBox->isChecked() == true){
            ui->Admin_User_lineEdit_Pass->setEnabled(true);
            ui->Admin_User_lineEdit_Word->setEnabled(true);
            ui->Admin_User_spinBox_Temp->setEnabled(true);
            ui->Admin_User_spinBox_Luz->setEnabled(true);

            ui->Admin_User_checkBox->setText("Finalizar");
        }else{

            ui->Admin_User_lineEdit_Pass->setEnabled(false);
            ui->Admin_User_lineEdit_Word->setEnabled(false);
            ui->Admin_User_spinBox_Temp->setEnabled(false);
            ui->Admin_User_spinBox_Luz->setEnabled(false);

            ui->Admin_User_checkBox->setText("Habilitar Edición");
            Reemplazar(ui->Admin_User_lineEdit_User->text());
            ui->Admin_User_listWidget->clear();
            CargarDatos();
        }
    }else{
        ui->Admin_User_checkBox->setChecked(false);
        QMessageBox msgError;
        msgError.setText("No seleccionó ningún usuario!\n");
        msgError.setStandardButtons(QMessageBox::Ok);
        msgError.exec();
    }
}

/* Métodos */

void Dialog_Admin::CargarDatos(void){
    QFile File("db.txt");

    if(!File.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "Error al abrir el archivo desde CargarDatos";
        return;
    }

    QTextStream in(&File);

    while(!in.atEnd()){
        QStringList Renglon = in.readLine().split(',');

        if(Renglon.at(0) == "ADMIN"){
            ui->Admin_Pref_spinBox_Temp->setValue(Renglon.at(6).toInt());
            ui->Admin_Pref_spinBox_Luz->setValue(Renglon.at(8).toInt());
            ui->Admin_Pref_lineEdit_Word->setText(Renglon.at(2));
            ui->Admin_Pref_lineEdit_Ingr1->setText(Renglon.at(1));
            ui->Admin_Pref_lineEdit_Ingre2->setText(Renglon.at(1));
            ui->label_ID_Admin->setText("ID: 0");
        }else{
            ui->Admin_User_listWidget->addItem(Renglon.at(0));
            ui->Admin_User_spinBox_Temp->setValue(0);
            ui->Admin_User_spinBox_Luz->setValue(0);
            ui->label_ID_User_Num->setText(0);
        }
    }

    ui->Admin_User_lineEdit_User->setEnabled(false);

    in.flush();
    File.close();
}

bool Dialog_Admin::Reemplazar(QString user){
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
            if(user == "ADMIN"){
                Renglon.insert(1, ui->Admin_Pref_lineEdit_Ingr1->text());
                Renglon.insert(2, ui->Admin_Pref_lineEdit_Word->text());
                Renglon.insert(3, "ID");
                Renglon.insert(4, "0");
                Renglon.insert(5, "T");
                Renglon.insert(6, ui->Admin_Pref_spinBox_Temp->text());
                Renglon.insert(7, "L");
                Renglon.insert(8, ui->Admin_Pref_spinBox_Luz->text());
            }else{
                Renglon.insert(1, ui->Admin_User_lineEdit_Pass->text());
                Renglon.insert(2, ui->Admin_User_lineEdit_Word->text());
                Renglon.insert(3, "ID");
                Renglon.insert(4, ui->label_ID_User_Num->text());
                Renglon.insert(5, "T");
                Renglon.insert(6, ui->Admin_User_spinBox_Temp->text());
                Renglon.insert(7, "L");
                Renglon.insert(8, ui->Admin_User_spinBox_Luz->text());
            }
        }
        out << Renglon.at(0) << "," << Renglon.at(1) << "," << Renglon.at(2) << "," << Renglon.at(3) << "," << Renglon.at(4) << "," << Renglon.at(5) << "," << Renglon.at(6) << "," << Renglon.at(7) << "," << Renglon.at(8) << "\n";
        Renglon.clear();
    }

    in.flush();                 //Vacio el flujo de datos
    out.flush();

    Archivo.close();            //Cierro ambos archivos
    Archivo_Copy.close();

    Archivo.remove();
    Archivo_Copy.rename("temp.txt", "db.txt");
}

QStringList Dialog_Admin::Buscar(QString user){
    QFile Arch("db.txt");
    QStringList retorno;

    if(!Arch.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "Error al abrir archivo db.txt";
    }

    QTextStream in(&Arch);

    while(!in.atEnd()){
        QString aux = in.readLine();
        retorno = aux.split(',');

        if(retorno.at(0) == user){
            in.flush();
            Arch.close();
            return retorno;
        }
    }

    in.flush();
    Arch.close();
    return QStringList(NULL);
}

void Dialog_Admin::BuscarID(quint8 id, quint8 * vecaux){
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

bool Dialog_Admin::Clear(){
    if((ui->Admin_User_lineEdit_User->text().isEmpty() == true)||(ui->Admin_User_lineEdit_Pass->text().isEmpty() == true)||(ui->Admin_User_lineEdit_Word->text().isEmpty() == true)){
        return false;
    }
    return true;
}

void Dialog_Admin::Guardar(){
    QFile Arch("db.txt");

    if(!Arch.open(QFile::Append | QFile::Text)){
        qDebug() << "Error al abrir archivo db.txt";
    }

    QTextStream out(&Arch);

    out << "\n" << ui->Admin_User_lineEdit_User->text() << "," << ui->Admin_User_lineEdit_Pass->text() << "," << ui->Admin_User_lineEdit_Word->text() << ",ID," << ui->label_ID_User_Num->text() << ",T," << ui->Admin_User_spinBox_Temp->text() << ",L," << ui->Admin_User_spinBox_Luz->text();

    out.flush();
    Arch.close();
}

void Dialog_Admin::CargarUsuarios(){
    QFile File("db.txt");

    if(!File.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "Error al abrir el archivo desde CargarDatos";
        return;
    }

    QTextStream in(&File);

    while(!in.atEnd()){
        QStringList Renglon = in.readLine().split(',');

        if(Renglon.at(0) != "ADMIN"){
            ui->Admin_User_listWidget->addItem(Renglon.at(0));
            Renglon = in.readLine().split(',');
        }
    }

    in.flush();
    File.close();
}

void Dialog_Admin::on_Admin_User_pushButton_Eliminar_clicked()
{
    QMessageBox msgEliminar;

    if(ui->Admin_User_listWidget->currentItem()->text().isEmpty() == true){
        msgEliminar.setText("No seleccionó ningún usuario!\n");
        msgEliminar.setIcon(QMessageBox::Warning);
    }else{
        if(Eliminar(ui->Admin_User_listWidget->currentItem()->text()) == true){
            msgEliminar.setText("Usuario Eliminado satisfactoriamente ! \n");
            msgEliminar.setIcon(QMessageBox::Information);
            ui->Admin_User_listWidget->clear();
            CargarDatos();
        }else{
            msgEliminar.setText("No se pudo eliminar el usuario\n");
            msgEliminar.setIcon(QMessageBox::Critical);
        }
    }
    msgEliminar.exec();
}

bool Dialog_Admin::Eliminar(QString user){
    QFile Archivo("db.txt");                                    //Puntero al archivo
    QFile Archivo_Copy("temp.txt");                             //Puntero a un archivo temporal
    QStringList Renglon; QString aux;
    quint8 send[3]; bool ok;

    if(!Archivo.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "Error al abrir archivo db.txt";
        return false;
    }

    if(!Archivo_Copy.open(QFile::WriteOnly | QFile::Text)){
        qDebug() << "Error al abrir archivo temp.txt";
        return false;
    }

    Renglon = Buscar(user);
    if(Renglon == (QStringList)NULL){
        return false;
    }

    aux = Renglon.at(4);

    QTextStream in(&Archivo);
    QTextStream out(&Archivo_Copy);

    while(!in.atEnd()){
        QString aux = in.readLine();
        Renglon = aux.split(',');

        if(Renglon.at(0) != user){
            out << Renglon.at(0) << "," << Renglon.at(1) << "," << Renglon.at(2) << "," << Renglon.at(3) << "," << Renglon.at(4) << "," << Renglon.at(5) << "," << Renglon.at(6) << "," << Renglon.at(7) << "," << Renglon.at(8) << "\n";
        }
    }
    in.flush();                 //Vacio el flujo de datos
    out.flush();

    Archivo.close();            //Cierro ambos archivos
    Archivo_Copy.close();

    Archivo.remove();
    Archivo_Copy.rename("temp.txt", "db.txt");

    send[0] = aux.toInt(&ok, 10);
    send[1] = send[2] = 250;
    EnviarComandoBis(send);

    return true;
}

bool Dialog_Admin::Agregar(quint8 id){
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

    ui->Admin_User_listWidget->addItem(aux);

    return true;
}

/* ************ Puerto Serie ************ */

//Conectar al puerto
void Dialog_Admin::on_Admin_Config_pushButton_Serie_clicked()
{
    if(Serial_Port_Conected == false){  //No está conectado
        //Obtenemos el nombre del puerto
        QString sPort;

        int i = ui->Admin_Config_comboBox_PuertosDisp->currentIndex();

        if(i != -1 && ui->Admin_Config_comboBox_PuertosDisp->itemText(i) == ui->Admin_Config_comboBox_PuertosDisp->currentText()){
            sPort = ui->Admin_Config_comboBox_PuertosDisp->itemData(i).toString();
        }else{
            sPort = ui->Admin_Config_comboBox_PuertosDisp->currentText();
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
            ui->Admin_Config_plainTextEdit_Serie->appendPlainText("Conectado");
            ui->Admin_Config_pushButton_Serie->setText("Desconectar");
        }else{
            //Hubo un error en abrir el puerto
            ui->Admin_Config_pushButton_Serie->setText("Conectar");
            ui->Admin_Config_plainTextEdit_Serie->appendPlainText("ERROR - No se pudo conectar");
            CerrarPuerto();
        }
    }else{
        ui->Admin_Config_pushButton_Serie->setText("Conectar");
        ui->Admin_Config_plainTextEdit_Serie->appendPlainText("Desconectar");
        Serial_Port_Conected = false;
        CerrarPuerto();
    }
}

void Dialog_Admin::onDatosRecibidos(){
    //Este método está linkeado a la senial readyRead, que sólo se emite
    //cuando nueva data está disponible
    QByteArray bytes;

    int cant = Serial_Port->bytesAvailable();       //Determino la cant de bytes disponibles
    bytes.resize(cant);

    if(cant == 3){
        Serial_Port->read(bytes.data(), bytes.size());

        ProcesarDatosRecibidos(bytes);
    }
}

void Dialog_Admin::EnumerarPuertos(){
    ui->Admin_Config_comboBox_PuertosDisp->clear();

    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();

    for(int i = 0; i < ports.size(); i++){
    #ifdef Q_OS_WIN
        ui->Admin_Config_comboBox_PuertosDisp->addItem(ports.at(i).friendName, ports.at(i).portName);
    #elif (defined Q_OS_UNIX)
        ui->Admin_Config_comboBox_PuertosDisp->addItem(ports.at(i).friendName, ports.at(i).physName);
    #endif

    }
}

void Dialog_Admin::ProcesarDatosRecibidos(QByteArray & bytes){
    int i; quint8 info[3], aux[3];
    QTime now = QTime::currentTime();

    for(i = 0; i < bytes.size(); i++){
        Buffer_Recepcion.append(bytes.at(i));
    }

    ui->Admin_Config_plainTextEdit_Serie->appendPlainText("Recibido:");
    ui->Admin_Config_plainTextEdit_Serie->appendPlainText(Buffer_Recepcion);

    for(i = 0; i < Buffer_Recepcion.size(); i++){
        info[i] = static_cast<quint8>(Buffer_Recepcion.at(i));
        qDebug() << info[i];
    }

    Buffer_Recepcion.clear();

    ui->Admin_Config_plainTextEdit_Serie->appendPlainText("Recibí");

    if(info[0] == 255){
        qDebug() << "Envío estado actual";

        if(info[2] == 3) ui->Admin_Config_lcdNumber_Luz->display("50");     //Luz Natural 50%
        if(info[2] == 4) ui->Admin_Config_lcdNumber_Luz->display("90");     //Luz Light   90%
        if(info[2] == 5) ui->Admin_Config_lcdNumber_Luz->display("10");     //Luz Oscura  10%

        ui->Admin_Config_lcdNumber_Temp->display(info[1]);
    }else{
        if(info[0] == 250){
            ui->Admin_Config_plainTextEdit_Serie->appendPlainText("Bien");
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

void Dialog_Admin::EnviarComandoBis(quint8* aux){
    int i;

    if(Serial_Port){
        for(i = 0; i<3; i++){
            Serial_Port->putChar(aux[i]);
        }
    }

    qDebug() << "Enviado x Bis";
}

void Dialog_Admin::CerrarPuerto(){
    Serial_Port->close();
    delete Serial_Port;
    Serial_Port = NULL;
}
