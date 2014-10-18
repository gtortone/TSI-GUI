#include "portmon.h"

#include <QWidgetItem>
#include <QMessageBox>
#include <QDebug>

PortMonForm::PortMonForm(QTopology &tpl, QWidget *parent) : QWidget(parent), topo(tpl) {

    setupUi(this);

    // build TSI ports combobox
    for(int i=0; i<TSIport_str.size(); i++)
       portComboBox->addItem(TSIport_str[i], (TSIport)i);

    connect(portComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(refreshLineList()));
    connect(portComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(refreshMonTable()));
    connect(lineComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(refreshMonList()));
    connect(addButton, SIGNAL(clicked()), this, SLOT(addMonLine()));
    connect(delButton, SIGNAL(clicked()), this, SLOT(delMonLine()));
}

void PortMonForm::init(void) {

   monList->setColumnWidth(0, 420);
   monList->setColumnWidth(1, 0);
   monList->setColumnHidden(1, true);

   monTable->setColumnWidth(0, 50);
   monTable->setColumnWidth(1, 370);

   refreshLineList();
   refreshMonTable();
}

void PortMonForm::refreshLineList(void) {

   TSIport p = (TSIport) portComboBox->itemData( portComboBox->currentIndex()).toInt();

   lineComboBox->clear();

   for(int i=0; i<TSImon_max[p]; i++)
      lineComboBox->addItem(TSImon_str[i], (TSImon)i);
}

void PortMonForm::refreshMonList(void) {

   TSIport p = (TSIport) portComboBox->itemData( portComboBox->currentIndex()).toInt();
   TSImon m = (TSImon) lineComboBox->itemData( lineComboBox->currentIndex()).toInt();
   QList<QMonitorParamInfo> mplist = topo.mpset.monparamList(p, m);

   monList->clear();
   monList->setRowCount(mplist.size());

   for(int i=0; i<mplist.size(); i++) {

      QTableWidgetItem *entry = new QTableWidgetItem();
      QTableWidgetItem *seqitem = new QTableWidgetItem();

      seqitem->setText(QString("%1").arg(mplist.size() - i, 2));

      entry->setText(mplist[i].desc);
      entry->setData(Qt::UserRole, mplist[i].seqid);
      monList->setRowHeight(i, 15);
      monList->setItem(i, 0, entry);
      monList->setItem(i, 1, seqitem);
   }

   monList->sortItems(1, Qt::AscendingOrder);
   monList->setCurrentCell(0,0);
}

void PortMonForm::refreshMonTable(void) {

   TSIport p = (TSIport) portComboBox->itemData( portComboBox->currentIndex()).toInt();
   QList<QMonitorParamInfo> mlist = topo.monparamList( (TSIport) (portComboBox->itemData( portComboBox->currentIndex()).toInt()) );

   monTable->clear();
   monTable->setRowCount(TSImon_max[p]);

   for(int i=0; i<TSImon_max[p]; i++) {

      QTableWidgetItem *entry = new QTableWidgetItem();

      entry->setText(QDetector::mon_str((TSImon)i));
      entry->setData(Qt::UserRole, (TSImon)i);
      monTable->setRowHeight(i, 20);
      monTable->setItem(i, 0, entry);
   }

   for(int i=0; i<mlist.size(); i++) {

      QTableWidgetItem *entry = new QTableWidgetItem();

      entry->setText(mlist[i].desc);
      entry->setData(Qt::UserRole, mlist[i].mline);
      monTable->setRowHeight(mlist[i].mline, 20);
      monTable->setItem(mlist[i].mline, 1, entry);
   }

   monTable->setCurrentCell(0,0);
}

void PortMonForm::addMonLine(void) {

   TSIport p = (TSIport) portComboBox->itemData( portComboBox->currentIndex()).toInt();
   TSImon m = (TSImon) lineComboBox->itemData( lineComboBox->currentIndex()).toInt();
   uint16_t seqid = monList->row(monList->currentItem());

   if(topo.MonitorFree(p, m)) {

      topo.addMonline(p, m, seqid);

      refreshMonTable();

   } else {

      int r = QMessageBox::warning(this, tr("Message"),
                                   tr("Monitoring line " + QDetector::mon_str(m).toAscii() + " already assigned!\n" "Do you want to replace it ?"),
                                   QMessageBox::Yes, QMessageBox::No);

      if(r == QMessageBox::Yes) {

         topo.delMonline(p, m);
         topo.addMonline(p, m, seqid);

         refreshMonTable();
      }
   }
}

void PortMonForm::delMonLine(void) {

   TSIport p = (TSIport) portComboBox->itemData( portComboBox->currentIndex()).toInt();
   TSImon m = (TSImon) monTable->currentRow();

   if(topo.MonitorBusy(p, m)) {        // ask for overwrite

      int r = QMessageBox::warning(this, tr("Message"),
                                   tr("Monitoring line will be released\n" "Are you sure ?"),
                                   QMessageBox::Yes, QMessageBox::No);

      if(r == QMessageBox::Yes) {

         topo.delMonline(p, m);
         topo.MonitorFree(p, m);

         refreshMonTable();
      }
   }
}
