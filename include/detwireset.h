#ifndef DETWIRESET_H
#define DETWIRESET_H

#include "ui_wdg-detwireset.h"
#include "QTopology.h"
#include <QDebug>
#include <QTableWidgetItem>

class DetWireSetForm : public QWidget, public Ui::detWireSetForm {

    Q_OBJECT

private:

   QTopology &topo;
   QTableWidgetItem *curritem;

public:

    DetWireSetForm(QTopology &tpl, QWidget *parent = 0);

 public slots:

   void init(void);
   void showDetector(QTableWidgetItem *item);

 private slots:

    void setWireEditMode(void);
    void setSetupEditMode(void);
    void setViewMode(void);
    void confirmWireChanges(void);
    void confirmSetupChanges(void);

    void refreshPortList(void);
    void refreshLineList(void);

    void updateDelayVal(int val);
    void updatePeriodVal(int val);

    void refreshStatus(void);
};

#endif // DETWIRESET_H
