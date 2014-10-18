#include "odetmon.h"

#include <QMessageBox>

OdetMonForm::OdetMonForm(QTopology &tpl, QWidget *parent) : QWidget(parent), topo(tpl) {

   setupUi(this);

   connect(editLineI, SIGNAL(clicked()), this, SLOT(setEditLineI()));
   connect(editStretch, SIGNAL(clicked()), this, SLOT(setEditStretch()));
   connect(okMonButton, SIGNAL(clicked()), this, SLOT(confirmChanges()));
   connect(cancelMonButton, SIGNAL(clicked()), this, SLOT(setViewMode()));
}

void OdetMonForm::init(void) {

   helpLabel->show();

   descBox->hide();
   wiringBox->hide();
   detBox->hide();
}

void OdetMonForm::showDetector(QTableWidgetItem *item) {

   curritem = item;

   QMap <QString, QVariant> tmpmap = curritem->data(Qt::UserRole).toMap();
   unsigned int telindex = tmpmap["telindex"].toUInt();
   unsigned int detindex = tmpmap["detindex"].toUInt();
   QDetector det = topo.detector(telindex, detindex);

   if(!det.allocated()) {  // detector is not wired/allocated

      QMessageBox::warning(this, "Message",
                           "Selected detector is not wired. Please specify input lines using \"Detectors -> Wiring and Setup\" dialog",
                           QMessageBox::Ok);

   } else {

      // detector header info
      telNameLabel->setText(topo.telescopeName(telindex));
      detNameLabel->setText(det.name());
      detTypeLabel->setText(QDetector::type_str(det.type()));

      setViewMode();
   }
}

void OdetMonForm::setViewMode(void) {

   QMap <QString, QVariant> tmpmap = curritem->data(Qt::UserRole).toMap();
   unsigned int telindex = tmpmap["telindex"].toUInt();
   unsigned int detindex = tmpmap["detindex"].toUInt();
   QDetector det = topo.detector(telindex, detindex);

   helpLabel->hide();
   descBox->show();
   wiringBox->show();
   detBox->show();
   comboLineI->clear();
   comboStretch->clear();
   comboLineI->setEnabled(false);
   comboStretch->setEnabled(false);
   editLineI->setEnabled(true);
   editStretch->setEnabled(true);
   okMonButton->hide();
   cancelMonButton->hide();

   if(det.allocated()) {

      wiringStatusIcon->setPixmap(QPixmap(QString::fromUtf8(":/icon/icons/allocated-small.png")));

      portLabel->setText(TSIport_str[det.port()]);

      if(det.type() == DSSD)
         lineLabel->setText(TSIline_str[det.line()] + " - " + TSIline_str[det.line()+4]);
      else
         lineLabel->setText(TSIline_str[det.line()]);

   } else {

      wiringStatusIcon->setPixmap(QPixmap(QString::fromUtf8(":/icon/icons/warning-small.png")));
      portLabel->setText("-");
      lineLabel->setText("-");
   }

   wiringStatusIcon->show();

   refreshLabels();
}

void OdetMonForm::setEditLineI(void) {

   QMap <QString, QVariant> tmpmap = curritem->data(Qt::UserRole).toMap();
   unsigned int telindex = tmpmap["telindex"].toUInt();
   unsigned int detindex = tmpmap["detindex"].toUInt();
   QDetector det = topo.detector(telindex, detindex);
   int index;

   QList<TSImon> tplBusyMon;
   QList<int16_t> idlist;

   tplBusyMon = topo.busyMonitorLines(det.port());
   qSort(tplBusyMon);

   // create combo content
   comboLineI->clear();

   idlist = topo.mpset.monlineList(det.port(), det.line(), TRG_MONGROUP);
   qSort(idlist);

   for(int i=0; i<tplBusyMon.size(); i++) {

      if((index = idlist.indexOf(tplBusyMon[i])) != -1)
         idlist.removeAt(index);
   }

   comboLineI->addItem("NONE", -1);
   for(int i=0; i<idlist.size(); i++)
      comboLineI->addItem(QDetector::port_str(det.port()) + "." + QDetector::mon_str((TSImon)idlist[i]), idlist[i]);

   comboLineI->setEnabled(true);
   editStretch->setEnabled(false);

   okMonButton->show();
   cancelMonButton->show();
}

void OdetMonForm::setEditStretch(void) {

   QMap <QString, QVariant> tmpmap = curritem->data(Qt::UserRole).toMap();
   unsigned int telindex = tmpmap["telindex"].toUInt();
   unsigned int detindex = tmpmap["detindex"].toUInt();
   QDetector det = topo.detector(telindex, detindex);
   int index;

   QList<TSImon> tplBusyMon;
   QList<int16_t> idlist;

   tplBusyMon = topo.busyMonitorLines(det.port());
   qSort(tplBusyMon);

   // create combo content
   comboStretch->clear();

   idlist = topo.mpset.monlineList(det.port(), det.line(), TRGSTRETCH_MONGROUP);
   qSort(idlist);

   for(int i=0; i<tplBusyMon.size(); i++) {

      if((index = idlist.indexOf(tplBusyMon[i])) != -1)
         idlist.removeAt(index);
   }

   comboStretch->addItem("NONE", -1);
   for(int i=0; i<idlist.size(); i++)
      comboStretch->addItem(QDetector::port_str(det.port()) + "." + QDetector::mon_str((TSImon)idlist[i]), idlist[i]);

   comboStretch->setEnabled(true);
   editLineI->setEnabled(false);

   okMonButton->show();
   cancelMonButton->show();
}

void OdetMonForm::confirmChanges(void) {

   QMap <QString, QVariant> tmpmap = curritem->data(Qt::UserRole).toMap();
   unsigned int telindex = tmpmap["telindex"].toUInt();
   unsigned int detindex = tmpmap["detindex"].toUInt();
   QDetector det = topo.detector(telindex, detindex);
   TSImon ml, findml;

   if(comboLineI->isEnabled()) {

      ml = (TSImon) comboLineI->itemData( comboLineI->currentIndex() ).toInt();
      findml = topo.monline(det.port(), det.line(), TRG_MONGROUP);

      if (ml == UNDEF_MON) {     // current item == NONE

         if( findml != UNDEF_MON) {  // this TRG-line is already assigned to a MON-line

            int r = QMessageBox::warning(this, "Message",
                       " Do you want to free monitoring line " + QDetector::mon_str(findml) + " ?",
                       QMessageBox::Yes, QMessageBox::No);

            if(r == QMessageBox::Yes) {

               // free MON line
               topo.delMonline(det.port(), findml, det.line(), TRG_MONGROUP);
            }
         }

      } else {      // current item != NONE

         if( findml != UNDEF_MON) {  // this TRG-line is already assigned to a MON-line

            int r = QMessageBox::warning(this, "Message",
                       "Monitoring line " + QDetector::mon_str(findml) +
                       " is already assigned to this input " +
                       " do you want overwrite it with " + QDetector::mon_str(ml) + " ?",
                       QMessageBox::Yes, QMessageBox::No);

            if(r == QMessageBox::Yes) {

               // free MON line
               topo.delMonline(det.port(), findml, det.line(), TRG_MONGROUP);
            }
         }

         // assign MON line
         topo.addMonline(det.port(), ml, det.line(), TRG_MONGROUP);

      }  // end if(current item != NONE)

   } else if(comboStretch->isEnabled()) {

      ml = (TSImon) comboStretch->itemData( comboStretch->currentIndex() ).toInt();
      findml = topo.monline(det.port(), det.line(), TRGSTRETCH_MONGROUP);

      if (ml == UNDEF_MON) {     // current item == NONE

         if( findml != UNDEF_MON) {  // this line is already assigned to a MON-line

            int r = QMessageBox::warning(this, "Message",
                       " Do you want to free monitoring line " + QDetector::mon_str(findml) + " ?",
                       QMessageBox::Yes, QMessageBox::No);

            if(r == QMessageBox::Yes) {

               // free MON line
               topo.delMonline(det.port(), findml, det.line(), TRGSTRETCH_MONGROUP);
            }
         }

      } else {      // current item != NONE

         if( findml != UNDEF_MON) {  // this line is already assigned to a MON-line

            int r = QMessageBox::warning(this, "Message",
                       "Monitoring line " + QDetector::mon_str(findml) +
                       " is already assigned to this input " +
                       " do you want overwrite it with " + QDetector::mon_str(ml) + " ?",
                       QMessageBox::Yes, QMessageBox::No);

            if(r == QMessageBox::Yes) {

               // free MON line
               topo.delMonline(det.port(), findml, det.line(), TRGSTRETCH_MONGROUP);
            }
         }

         // assign MON line
         topo.addMonline(det.port(), ml, det.line(), TRGSTRETCH_MONGROUP);

      }  // end if(current item != NONE)

   }

   setViewMode();
}

void OdetMonForm::refreshLabels(void) {

   QMap <QString, QVariant> tmpmap = curritem->data(Qt::UserRole).toMap();
   unsigned int telindex = tmpmap["telindex"].toUInt();
   unsigned int detindex = tmpmap["detindex"].toUInt();
   QDetector det = topo.detector(telindex, detindex);
   TSImon findml;
   QString portstr = QDetector::port_str(det.port());

   QString statsstr = "Number of monitoring lines available for this detector: ";
   int freelines = topo.freeMonitorLines(det.port()).size();
   statsstr.append(QString("%1").arg(freelines));

   if(freelines > 0)
      statsLabel->setText("<font color=\"green\"><b>" + statsstr + "</b>");
   else
      statsLabel->setText("<font color=\"red\"><b>" + statsstr + "</b>");

   findml = topo.monline(det.port(), det.line(), TRG_MONGROUP);
   if(findml != UNDEF_MON)
      labelLineI->setText(portstr + "." + QDetector::mon_str(findml));
   else
      labelLineI->setText("NONE");

   findml = topo.monline(det.port(), det.line(), TRGSTRETCH_MONGROUP);
   if(findml != UNDEF_MON)
      labelStretch->setText(portstr + "." + QDetector::mon_str(findml));
   else
      labelStretch->setText("NONE");
}
