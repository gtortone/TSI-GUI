#ifndef PORTMON_H
#define PORTMON_H

#include <QWidget>
#include <QKeyEvent>
#include "ui_wdg-portmonform.h"
#include "QTopology.h"
#include "QMetrics.h"

class PortMonForm : public QWidget, public Ui::portMonForm {

    Q_OBJECT

private:

   QTopology &topo;

public:

    PortMonForm(QTopology &tpl, QWidget *parent = 0);

 private slots:

    void refreshLineList(void);
    void refreshMonList(void);
    void refreshMonTable(void);

    void addMonLine(void);
    void delMonLine(void);

 public slots:

    void init(void);

};

#endif // PORTMON_H
