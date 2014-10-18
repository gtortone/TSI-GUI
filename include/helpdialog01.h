#ifndef HELPDIALOG01_H
#define HELPDIALOG01_H

#include <QDialog>
#include "ui_dlg-tsidia.h"

class HelpDialog01 : public QDialog, public Ui::TSIschDialog {

    Q_OBJECT

public:

    HelpDialog01(QWidget *parent = 0);
};

#endif // HELPDIALOG01_H
