#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include <QMessageBox>
#include <QInputDialog>

#include "ui_wdg-topology.h"
#include "editdetdialog.h"
#include "QTopology.h"

class TopologyForm : public QWidget, public Ui::topoForm {

    Q_OBJECT

public:

    TopologyForm(QTopology &tpl, QWidget *parent = 0);

    void init(void);
    enum mode { MODE_SELECTION = 0, MODE_EDIT };
    void setMode(mode m);
    void refresh(void);

private:

    mode dlgmode;
    QTopology &topo;
    EditDetDialog *detdialog;

private slots:

    void newTelescope();
    void editTelescope();
    void delTelescope();

    void newDetector();
    void editDetector();
    void delDetector();

    void refreshTelList();
    void refreshDetList();
};

#endif // TOPOLOGY_H
