#ifndef DIALOG_FORGOT_H
#define DIALOG_FORGOT_H

#include <QDialog>

#include <QMessageBox>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>

namespace Ui {
class dialog_forgot;
}

class dialog_forgot : public QDialog
{
    Q_OBJECT

public:
    explicit dialog_forgot(QWidget *parent = 0);
    ~dialog_forgot();

private slots:
    void on_Forgot_User_lineEdit_editingFinished();
    void on_Forgot_buttonBox_rejected();
    void on_Forgot_buttonBox_accepted();

private:
    Ui::dialog_forgot *ui;

    void MostrarError(unsigned int);
    bool ComprobarDatos(void);
    bool ComprobarUP(QString, QString);
    void GuardarPass(QString);
};

#endif // DIALOG_FORGOT_H
