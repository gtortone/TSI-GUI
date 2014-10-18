#ifndef DSSDMON_H
#define DSSDMON_H

#include <QTableWidgetItem>

#include "ui_wdg-dssdmonform.h"
#include "QTopology.h"

class DssdMonForm : public QWidget, public Ui::dssdMonForm {

    Q_OBJECT

private:

    QTopology &topo;
    QTableWidgetItem *curritem;

public:

    DssdMonForm(QTopology &tpl, QWidget *parent = 0);

 public slots:

    void init(void);
    void showDetector(QTableWidgetItem *item);

 private slots:

    void setViewMode(void);
    void setEditLineX(void);
    void setEditLineY(void);
    void setEditCoinc(void);
    void setEditFreqDiv(void);
    void setEditStretch(void);
    void confirmChanges(void);
    void refreshLabels(void);
};

#endif // DSSDMON_H
