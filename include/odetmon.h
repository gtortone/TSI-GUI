#ifndef ODETMON_H
#define ODETMON_H

#include <QTableWidgetItem>

#include "ui_wdg-odetmonform.h"
#include "QTopology.h"

class OdetMonForm : public QWidget, public Ui::odetMonForm {

    Q_OBJECT

private:

    QTopology &topo;
    QTableWidgetItem *curritem;

public:

    OdetMonForm(QTopology &tpl, QWidget *parent = 0);

 public slots:

    void init(void);
    void showDetector(QTableWidgetItem *item);

 private slots:

    void setViewMode(void);
    void setEditLineI(void);
    void setEditStretch(void);
    void confirmChanges(void);
    void refreshLabels(void);
};

#endif // ODETMON_H
