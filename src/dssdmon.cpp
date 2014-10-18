#include "dssdmon.h"

#include <QMessageBox>

DssdMonForm::DssdMonForm(QTopology &tpl, QWidget *parent) : QWidget(parent), topo(tpl) {

   setupUi(this);

   connect(editLineX, SIGNAL(clicked()), this, SLOT(setEditLineX()));
   connect(editLineY, SIGNAL(clicked()), this, SLOT(setEditLineY()));
   connect(editCoinc, SIGNAL(clicked()), this, SLOT(setEditCoinc()));
   connect(editFreqDiv, SIGNAL(clicked()), this, SLOT(setEditFreqDiv()));
   connect(editStretch, SIGNAL(clicked()), this, SLOT(setEditStretch()));
   connect(okMonButton, SIGNAL(clicked()), this, SLOT(confirmChanges()));
   connect(cancelMonButton, SIGNAL(clicked()), this, SLOT(setViewMode()));
}

void DssdMonForm::init(void) {

   helpLabel->show();

   descBox->hide();
   wiringBox->hide();
   detBox->hide();
}

void DssdMonForm::showDetector(QTableWidgetItem *item) {

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

void DssdMonForm::setViewMode(void) {

   QMap <QString, QVariant> tmpmap = curritem->data(Qt::UserRole).toMap();
   unsigned int telindex = tmpmap["telindex"].toUInt();
   unsigned int detindex = tmpmap["detindex"].toUInt();
   QDetector det = topo.detector(telindex, detindex);

   helpLabel->hide();
   descBox->show();
   wiringBox->show();
   detBox->show();
   comboLineX->clear();
   comboLineY->clear();
   comboCoinc->clear();
   comboFreqDiv->clear();
   comboStretch->clear();
   comboLineX->setEnabled(false);
   comboLineY->setEnabled(false);
   comboCoinc->setEnabled(false);
   comboFreqDiv->setEnabled(false);
   comboStretch->setEnabled(false);
   editLineX->setEnabled(true);
   editLineY->setEnabled(true);
   editCoinc->setEnabled(true);
   editFreqDiv->setEnabled(true);
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

   if(det.coincop() == AND)
      gateLabel->setPixmap(QPixmap(QString::fromUtf8(":icon/icons/and-gate.png")));
   else if(det.coincop() == OR)
      gateLabel->setPixmap(QPixmap(QString::fromUtf8(":icon/icons/or-gate.png")));

   refreshLabels();
}

void DssdMonForm::setEditLineX(void) {

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
   comboLineX->clear();

   idlist = topo.mpset.monlineList(det.port(), det.line(), TRG_MONGROUP);
   qSort(idlist);

   for(int i=0; i<tplBusyMon.size(); i++) {

      if((index = idlist.indexOf(tplBusyMon[i])) != -1)
         idlist.removeAt(index);
   }

   comboLineX->addItem("NONE", -1);
   for(int i=0; i<idlist.size(); i++)
      comboLineX->addItem(QDetector::port_str(det.port()) + "." + QDetector::mon_str((TSImon)idlist[i]), idlist[i]);

   comboLineX->setEnabled(true);

   editLineX->setEnabled(false);
   editLineY->setEnabled(false);
   editCoinc->setEnabled(false);
   editFreqDiv->setEnabled(false);
   editStretch->setEnabled(false);

   okMonButton->show();
   cancelMonButton->show();
}

void DssdMonForm::setEditLineY(void) {

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
   comboLineY->clear();

   idlist = topo.mpset.monlineList(det.port(), (TSIline)(det.line()+4), TRG_MONGROUP);
   qSort(idlist);

   for(int i=0; i<tplBusyMon.size(); i++) {

      if((index = idlist.indexOf(tplBusyMon[i])) != -1)
         idlist.removeAt(index);
   }

   comboLineY->addItem("NONE", -1);
   for(int i=0; i<idlist.size(); i++)
      comboLineY->addItem(QDetector::port_str(det.port()) + "." + QDetector::mon_str((TSImon)idlist[i]), idlist[i]);

   comboLineY->setEnabled(true);

   editLineX->setEnabled(false);
   editLineY->setEnabled(false);
   editCoinc->setEnabled(false);
   editFreqDiv->setEnabled(false);
   editStretch->setEnabled(false);

   okMonButton->show();
   cancelMonButton->show();
}

void DssdMonForm::setEditCoinc(void) {

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
   comboCoinc->clear();

   idlist = topo.mpset.monlineList(det.port(), det.line(), COINC_MONGROUP);
   qSort(idlist);

   for(int i=0; i<tplBusyMon.size(); i++) {

      if((index = idlist.indexOf(tplBusyMon[i])) != -1)
         idlist.removeAt(index);
   }

   comboCoinc->addItem("NONE", -1);
   for(int i=0; i<idlist.size(); i++)
      comboCoinc->addItem(QDetector::port_str(det.port()) + "." + QDetector::mon_str((TSImon)idlist[i]), idlist[i]);

   comboCoinc->setEnabled(true);

   editLineX->setEnabled(false);
   editLineY->setEnabled(false);
   editCoinc->setEnabled(false);
   editFreqDiv->setEnabled(false);
   editStretch->setEnabled(false);

   okMonButton->show();
   cancelMonButton->show();
}

void DssdMonForm::setEditFreqDiv(void) {

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
   comboFreqDiv->clear();

   idlist = topo.mpset.monlineList(det.port(), det.line(), FREQDIV_MONGROUP);
   qSort(idlist);

   for(int i=0; i<tplBusyMon.size(); i++) {

      if((index = idlist.indexOf(tplBusyMon[i])) != -1)
         idlist.removeAt(index);
   }

   comboFreqDiv->addItem("NONE", -1);
   for(int i=0; i<idlist.size(); i++)
      comboFreqDiv->addItem(QDetector::port_str(det.port()) + "." + QDetector::mon_str((TSImon)idlist[i]), idlist[i]);

   comboFreqDiv->setEnabled(true);

   editLineX->setEnabled(false);
   editLineY->setEnabled(false);
   editCoinc->setEnabled(false);
   editFreqDiv->setEnabled(false);
   editStretch->setEnabled(false);

   okMonButton->show();
   cancelMonButton->show();
}

void DssdMonForm::setEditStretch(void) {

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

   idlist = topo.mpset.monlineList(det.port(), det.line(), COINCSTRETCH_MONGROUP);
   qSort(idlist);

   for(int i=0; i<tplBusyMon.size(); i++) {

      if((index = idlist.indexOf(tplBusyMon[i])) != -1)
         idlist.removeAt(index);
   }

   comboStretch->addItem("NONE", -1);
   for(int i=0; i<idlist.size(); i++)
      comboStretch->addItem(QDetector::port_str(det.port()) + "." + QDetector::mon_str((TSImon)idlist[i]), idlist[i]);

   comboStretch->setEnabled(true);

   editLineX->setEnabled(false);
   editLineY->setEnabled(false);
   editCoinc->setEnabled(false);
   editFreqDiv->setEnabled(false);
   editStretch->setEnabled(false);

   okMonButton->show();
   cancelMonButton->show();
}

void DssdMonForm::confirmChanges(void) {

   QMap <QString, QVariant> tmpmap = curritem->data(Qt::UserRole).toMap();
   unsigned int telindex = tmpmap["telindex"].toUInt();
   unsigned int detindex = tmpmap["detindex"].toUInt();
   QDetector det = topo.detector(telindex, detindex);
   TSImon ml, findml;

   if(comboLineX->isEnabled()) {

      ml = (TSImon) comboLineX->itemData( comboLineX->currentIndex() ).toInt();
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

   } else if(comboLineY->isEnabled()) {

      ml = (TSImon) comboLineY->itemData( comboLineY->currentIndex() ).toInt();
      findml = topo.monline(det.port(), (TSIline)(det.line()+4), TRG_MONGROUP);

      if (ml == UNDEF_MON) {     // current item == NONE

         if( findml != UNDEF_MON) {  // this TRG-line is already assigned to a MON-line

            int r = QMessageBox::warning(this, "Message",
                       " Do you want to free monitoring line " + QDetector::mon_str(findml) + " ?",
                       QMessageBox::Yes, QMessageBox::No);

            if(r == QMessageBox::Yes) {

               // free MON line
               topo.delMonline(det.port(), findml, (TSIline)(det.line()+4), TRG_MONGROUP);
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
               topo.delMonline(det.port(), findml, (TSIline)(det.line()+4), TRG_MONGROUP);
            }
         }

         // assign MON line
         topo.addMonline(det.port(), ml, (TSIline)(det.line()+4), TRG_MONGROUP);

      }  // end if(current item != NONE)

   } else if(comboCoinc->isEnabled()) {

      ml = (TSImon) comboCoinc->itemData( comboCoinc->currentIndex() ).toInt();
      findml = topo.monline(det.port(), det.line(), COINC_MONGROUP);

      if (ml == UNDEF_MON) {     // current item == NONE

         if( findml != UNDEF_MON) {  // this line is already assigned to a MON-line

            int r = QMessageBox::warning(this, "Message",
                       " Do you want to free monitoring line " + QDetector::mon_str(findml) + " ?",
                       QMessageBox::Yes, QMessageBox::No);

            if(r == QMessageBox::Yes) {

               // free MON line
               topo.delMonline(det.port(), findml, det.line(), COINC_MONGROUP);
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
               topo.delMonline(det.port(), findml, det.line(), COINC_MONGROUP);
            }
         }

         // assign MON line
         topo.addMonline(det.port(), ml, det.line(), COINC_MONGROUP);

      }  // end if(current item != NONE)

   } else if(comboFreqDiv->isEnabled()) {

      ml = (TSImon) comboFreqDiv->itemData( comboFreqDiv->currentIndex() ).toInt();
      findml = topo.monline(det.port(), det.line(), FREQDIV_MONGROUP);

      if (ml == UNDEF_MON) {     // current item == NONE

         if( findml != UNDEF_MON) {  // this line is already assigned to a MON-line

            int r = QMessageBox::warning(this, "Message",
                       " Do you want to free monitoring line " + QDetector::mon_str(findml) + " ?",
                       QMessageBox::Yes, QMessageBox::No);

            if(r == QMessageBox::Yes) {

               // free MON line
               topo.delMonline(det.port(), findml, det.line(), FREQDIV_MONGROUP);
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
               topo.delMonline(det.port(), findml, det.line(), FREQDIV_MONGROUP);
            }
         }

         // assign MON line
         topo.addMonline(det.port(), ml, det.line(), FREQDIV_MONGROUP);

      }  // end if(current item != NONE)

   } else if(comboStretch->isEnabled()) {

      ml = (TSImon) comboStretch->itemData( comboStretch->currentIndex() ).toInt();
      findml = topo.monline(det.port(), det.line(), COINCSTRETCH_MONGROUP);

      if (ml == UNDEF_MON) {     // current item == NONE

         if( findml != UNDEF_MON) {  // this line is already assigned to a MON-line

            int r = QMessageBox::warning(this, "Message",
                       " Do you want to free monitoring line " + QDetector::mon_str(findml) + " ?",
                       QMessageBox::Yes, QMessageBox::No);

            if(r == QMessageBox::Yes) {

               // free MON line
               topo.delMonline(det.port(), findml, det.line(), COINCSTRETCH_MONGROUP);
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
               topo.delMonline(det.port(), findml, det.line(), COINCSTRETCH_MONGROUP);
            }
         }

         // assign MON line
         topo.addMonline(det.port(), ml, det.line(), COINCSTRETCH_MONGROUP);

      }  // end if(current item != NONE)

   }

   setViewMode();
}

void DssdMonForm::refreshLabels(void) {

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
      labelLineX->setText(portstr + "." + QDetector::mon_str(findml));
   else
      labelLineX->setText("NONE");

   findml = topo.monline(det.port(), (TSIline)(det.line()+4), TRG_MONGROUP);
   if(findml != UNDEF_MON)
      labelLineY->setText(portstr + "." + QDetector::mon_str(findml));
   else
      labelLineY->setText("NONE");

   findml = topo.monline(det.port(), det.line(), COINC_MONGROUP);
   if(findml != UNDEF_MON)
      labelCoinc->setText(portstr + "." + QDetector::mon_str(findml));
   else
      labelCoinc->setText("NONE");

   findml = topo.monline(det.port(), det.line(), FREQDIV_MONGROUP);
   if(findml != UNDEF_MON)
      labelFreqDiv->setText(portstr + "." + QDetector::mon_str(findml));
   else
      labelFreqDiv->setText("NONE");

   findml = topo.monline(det.port(), det.line(), COINCSTRETCH_MONGROUP);
   if(findml != UNDEF_MON)
      labelStretch->setText(portstr + "." + QDetector::mon_str(findml));
   else
      labelStretch->setText("NONE");
}
