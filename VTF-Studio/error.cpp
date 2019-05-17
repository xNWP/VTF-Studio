#include "error.h"

#include <QMessageBox>

void VTFStudio::Error::Display(QString cause)
{
    QMessageBox m;
    m.setIcon(QMessageBox::Critical);
    m.setStandardButtons(QMessageBox::Ok);
    m.setWindowTitle("Error");
    m.setText("An unexpected error has occured, if you would like to report it please note the information below.");\
    m.setInformativeText(cause);
    m.exec();
}
