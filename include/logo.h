#ifndef LOGO_H
#define LOGO_H

#include "ui_wdg-logo.h"

class LogoForm : public QWidget, public Ui::logoForm {

    Q_OBJECT

public:

    LogoForm(QWidget *parent = 0);

};

#endif // LOGO_H
