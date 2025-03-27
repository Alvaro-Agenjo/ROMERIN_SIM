#include "emulationwnd.h"
#include "ui_emulationwnd.h"
#include <QFileDialog>

EmulationWnd::EmulationWnd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EmulationWnd)
{
    ui->setupUi(this);
}

EmulationWnd::~EmulationWnd()
{
    delete ui;
}

void EmulationWnd::on_Bfile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Seleccionar Archivo de Texto", "", "Archivos de Texto (*.txt)");
            if (!fileName.isEmpty()) {
                ui->Lfilename->setText(fileName);
            }
}
