#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QMainWindow>
#include <QSplitter>
#include <QTimer>
#include "ui_dlg-main.h"
#include "logo.h"
#include "topology.h"
#include "detwireset.h"
#include "dssdmon.h"
#include "odetmon.h"
#include "portmon.h"
#include "sumdialog.h"
#include "tl1form.h"
#include "tl2form.h"
#include "cpform.h"
#include "helpdialog01.h"
#include "QRegister.h"
#include "QRegisterSet.h"
#include "QTopology.h"

#include <QFileDialog>

class MainDialog : public QMainWindow, public Ui::MainDialog {

    Q_OBJECT

protected:

   void closeEvent(QCloseEvent *event);

public:

    MainDialog();
    void init(void);

private:

    QString dlgtitle;
    bool wsactive;

    QTopology topo;
    QString filename;

    LogoForm *logoform;
    TopologyForm *topoform;
    DetWireSetForm *dwiresetform;
    DssdMonForm *dmonform;
    OdetMonForm *omonform;
    PortMonForm *pmonform;
    SummaryDialog *sumdialog;
    TL1Form *tl1form;
    TL2Form *tl2form;
    CPForm *cpform;
    HelpDialog01 *helpform01;

    QSplitter *splitter;
    // QTimer *timer;

 public slots:

    void refreshMenubar(void);

 private slots:

    void newWorkspace(void);
    void openWorkspace(void);

    void saveWorkspace(void);
    void saveWorkspaceAs(void);
    void closeWorkspace(void);
    void exit(void);

    void newTopology(void);
    void editTopology(void);

    void detWiringSetup(void);
    void assignMonDet(void);
    void assignMonPort(void);
    void selectMonForm(QTableWidgetItem *item);
    void showSummary(void);

    void setvmebaddr(void);
    void setvmeconn(void);
    void TSIpanel(void);
    void dumpConfig(void);

    void TL1Config(void);
    void TL2Config(void);
    void TL3Config(void);

    void help01(void);
    void aboutGui(void);
    void aboutQt(void);
};

#endif
