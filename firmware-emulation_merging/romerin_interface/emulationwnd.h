#ifndef EMULATIONWND_H
#define EMULATIONWND_H

#include <QDialog>
#include <QDebug>
#include <QMessageBox>

namespace Ui {
class EmulationWnd;
}

class EmulationWnd : public QDialog
{
    Q_OBJECT

public:
    explicit EmulationWnd(QWidget *parent = nullptr);
protected:

    ~EmulationWnd();

private slots:
    void on_Bfile_clicked();

private:
    Ui::EmulationWnd *ui;
};

#endif // EMULATIONWND_H
