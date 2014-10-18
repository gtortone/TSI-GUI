#ifndef EDITDETDIALOG_H
#define EDITDETDIALOG_H

#include <QDialog>
#include "ui_dlg-editdet.h"
#include "QTopology.h"

class EditDetDialog : public QDialog, public Ui::editdetDialog {

    Q_OBJECT

public:
    EditDetDialog(QWidget *parent = 0);

    void init(void);
    QString detName(void);
    DetectorType detType(void);

    void setdetName(QString str);
    void setdetType(DetectorType type);

private:

    QString detname;
    DetectorType dettype;

private slots:

    void updateValues(void);

};

#endif // EDITDETDIALOG_H
