#ifndef SUMDIALOG_H
#define SUMDIALOG_H

#include <QDialog>
#include "ui_dlg-summary.h"
#include "QTopology.h"

class SummaryDialog : public QDialog, public Ui::summaryDialog {

    Q_OBJECT

private:

   QTopology &topo;
   QString htmlDetector(QDetector det);
   QString htmlPort(TSIport p);
   QString htmlMonlines(TSIport p);
   QString htmlTL1config(void);
   QString htmlTL2config(void);

public:

    SummaryDialog(QTopology &tpl, QWidget *parent = 0);

    void showVMEConfig(void);
    void showSummary(void);

 public slots:

    void init(void);
    void sendVMEConfig(void);

 private slots:

    void print(void);

};

#endif // SUMDIALOG_H
