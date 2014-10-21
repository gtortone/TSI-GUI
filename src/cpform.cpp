#include "cpform.h"
#include <QMainWindow>
#include <QStatusBar>
#include <QEvent>
#include <QShowEvent>
#include <QHideEvent>
#include <QMessageBox>
#include <QCheckBox>
#include <QComboBox>

#include <qwt_date.h>
#include <qwt_date_scale_engine.h>
#include <qwt_date_scale_draw.h>
#include <qwt_legend.h>
#include <qwt_legend_label.h>

CPForm::CPForm(QTopology &tpl, QWidget *parent) : QWidget(parent), topo(tpl) {

   setupUi(this);

   timer = new QTimer(this);
   connect(timer, SIGNAL(timeout()), this, SLOT(refresh()));
   connect(stopButton, SIGNAL(clicked()), this, SLOT(send_stop()));
   connect(runButton, SIGNAL(clicked()), this, SLOT(send_run()));
   connect(resetButton, SIGNAL(clicked()), this, SLOT(send_reset()));
   connect(refreshBox, SIGNAL(valueChanged(int)), this, SLOT(update_reftime()));
   connect(simButton, SIGNAL(clicked()), this, SLOT(fire_sim()));
   connect(divButton, SIGNAL(clicked()), this, SLOT(fire_div()));

   coincTable->setColumnWidth(0, 80);
   coincTable->setColumnWidth(1, 80);
   coincTable->setColumnWidth(2, 35);
   coincTable->setColumnWidth(3, 90);
   coincTable->setColumnWidth(4, 30);

   simTable->setColumnWidth(0, 100);
   simTable->setColumnWidth(1, 80);
   simTable->setColumnWidth(2, 40);
   simTable->setColumnWidth(3, 40);
   simTable->setColumnWidth(4, 20);

   divTable->setColumnWidth(0, 100);
   divTable->setColumnWidth(1, 80);
   divTable->setColumnWidth(2, 90);

   QFont f = this->font();
   f.setPixelSize(8);
   
   QColor cblue = QColor(Qt::darkBlue);
   cblue.setAlpha(150);

   QColor cred = QColor(Qt::darkRed);
   cred.setAlpha(150);

   dthisto = new QwtPlotCurve();
   dthisto->setBrush(cblue);
   dthisto->setStyle(QwtPlotCurve::Lines);
   dthisto->setRenderHint(QwtPlotItem::RenderAntialiased, true);
   dthisto->attach(dtPlot);

   dtPlot->setAxisFont(QwtPlot::xBottom,f);
   dtPlot->setAxisFont(QwtPlot::yLeft,f);

   trgraph = new QwtPlotCurve("Raw trigger rate");
   trgraph->setBrush(cblue);
   trgraph->setStyle(QwtPlotCurve::Lines);
   trgraph->setRenderHint(QwtPlotItem::RenderAntialiased, true);
   trgraph->attach(trPlot);

   acqgraph = new QwtPlotCurve("Acquired trigger rate");
   acqgraph->setBrush(cred);
   acqgraph->setStyle(QwtPlotCurve::Lines);
   acqgraph->setRenderHint(QwtPlotItem::RenderAntialiased, true);
   acqgraph->attach(trPlot);

   trPlot->setAxisFont(QwtPlot::xBottom,f);
   trPlot->setAxisFont(QwtPlot::yLeft,f);

   QwtDateScaleDraw *scaleDraw = new QwtDateScaleDraw(Qt::LocalTime);
   scaleDraw->setDateFormat(QwtDate::Millisecond, "hh:mm:ss");
   scaleDraw->setDateFormat(QwtDate::Second, "hh:mm:ss");
   scaleDraw->setDateFormat(QwtDate::Minute, "hh:mm");
   scaleDraw->setDateFormat(QwtDate::Hour, "hh:mm");
   scaleDraw->setDateFormat(QwtDate::Day, "");
   scaleDraw->setDateFormat(QwtDate::Week, "");
   scaleDraw->setDateFormat(QwtDate::Month, "");

   QwtDateScaleEngine *scaleEngine = new QwtDateScaleEngine(Qt::LocalTime);
   scaleEngine->setMargins(1,1);
   scaleEngine->setAttribute(QwtScaleEngine::Floating);

   trPlot->setAxisScaleDraw( QwtPlot::xBottom, scaleDraw );
   trPlot->setAxisScaleEngine( QwtPlot::xBottom, scaleEngine );
   trPlot->setAxisLabelRotation( QwtPlot::xBottom, -50.0 );
   trPlot->setAxisLabelAlignment( QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom );

   QwtLegend *legend = new QwtLegend;
   trPlot->insertLegend(legend, QwtPlot::BottomLegend);
}

void CPForm::showEvent(QShowEvent *ev)
{
   if(!ev->spontaneous()) { // a "not spontaneous" show event is generated inside code on form->show() call

      if(!topo.verifyVMEConfig()) {   // gui config differs to applied VME config

         if(topo.VMEbus.online()) {

            int r = QMessageBox::warning(this, "Message",
                                   "Current TSI configuration differs to applied VME config - Do you want to send current config to VME ?",
                                   QMessageBox::Yes, QMessageBox::No);

            if(r == QMessageBox::Yes) {

               bool retval = topo.sendVMEConfig();

               if(!retval)
                  QMessageBox::warning(this, "Warning", "VME2NET connection error. Verify connection setup", QMessageBox::Ok);
            }

         } else {

            QMessageBox::warning(this, "Warning", "VME2NET connection error. Verify connection setup", QMessageBox::Ok);
         }
      }

      init();

      refreshSim();
      refreshDiv();
   }
}

void CPForm::hideEvent(QHideEvent *ev)
{
   if(!ev->spontaneous()) { // a "not spontaneous" hide event is generated inside code on form->hide() call

      timer->stop();

      forminit = false;

      // reset simulation values
      topo.tsiregset.setregvalue("AbilitazioneSimTrg", XA, 0);
      topo.tsiregset.setregvalue("AbilitazioneSimTrg", XB, 0);
      topo.tsiregset.setregvalue("AbilitazioneSimTrg", XC, 0);
      topo.tsiregset.setregvalue("AbilitazioneSimTrg", X4, 0);

      topo.tsiregset.setregvalue("GenSimTrigger", XA, 0);
      topo.tsiregset.setregvalue("GenSimTrigger", XB, 0);
      topo.tsiregset.setregvalue("GenSimTrigger", XC, 0);
      topo.tsiregset.setregvalue("GenSimTrigger", X4, 0);
   }
}

void CPForm::init(void) {

   forminit = true;

   // start timer
   update_reftime();
   numfails = 0;

   update_tsistatus();
   refreshSim();
   refreshDiv();

   if(topo.tsiregset.parse_status() == RUN)
      timer->start();
   else
      timer->stop();

   clear_plots();

   of_acq_tnum = of_tot_tnum = of_elap_time = 0;
   offset_acq_tnum = offset_tot_tnum = offset_elap_time = 0;
}

void CPForm::clear_plots(void) {

   // initialize dead time xy axis
   for(int i=0; i<101; i++) {
      dtx[i] = i;
      dty[i] = 0;
   }

   dthisto->setSamples(dtx, dty, 101);
   dtPlot->replot();
}

void CPForm::refresh(void) {

   uint32_t data;

   if(numfails > MAX_FAILSNUM) {

      this->hide();

      QMessageBox::warning(this, "Warning", "Automatic refresh disabled due to connection error. Verify vme2net connection setup and restart control panel", \
                           QMessageBox::Ok);
   }

   // connect to VME2NET
   topo.VMEbus.connect();

   // VME2NET hostname
   if(topo.VMEbus.gethostname() != "")
      hostnameLabel->setText(topo.VMEbus.gethostname() + ":" + QString::number(topo.VMEbus.getport()));
   else
      hostnameLabel->setText("<font color=\"grey\"><b> n/a </b>");

   // try to read a "fake" VME register in order to detect if vme2net connection is up&running
   topo.VMEbus.read(topo.tsiregset.getregaddr("ControlWord", XF), &data);

   if(topo.VMEbus.online()) {

      numfails = 0;

      vmestatusLabel->setPixmap(QPixmap(QString::fromUtf8(":/icon/icons/ok-large.png")));

      update_tsistatus();

      // TSI statistics

      if(topo.tsiregset.parse_status() == RUN) {

         bool read_failed = false;

         if(topo.VMEbus.read(topo.tsiregset.getregaddr("TriggerTotali", XF), &data)) {

            tot_tnum = data;

         } else {

            read_failed = true;
            trigtot->setText("<font color=\"grey\"><b> n/a </b>");
         }

         if(topo.VMEbus.read(topo.tsiregset.getregaddr("TriggerAcquisiti", XF), &data)) {

            acq_tnum = data;

         }  else {

            read_failed = true;
            trigacqtot->setText("<font color=\"grey\"><b> n/a </b>");
         }

         if(topo.VMEbus.read(topo.tsiregset.getregaddr("RunTime", XF), &data)) {

            elap_time = data;

         } else {

            read_failed = true;
            tsiuptime->setText("<font color=\"grey\"><b> n/a </b>");
         }

         if(!read_failed) {

            if(forminit) {
               
               init();
               forminit = false;

               last_acq_tnum = offset_acq_tnum = acq_tnum;
               last_tot_tnum = offset_tot_tnum = tot_tnum;
               last_elap_time = offset_elap_time = elap_time;

            } else {

               if(last_acq_tnum > acq_tnum)
                  of_acq_tnum++;

               if(last_tot_tnum > tot_tnum)
                  of_tot_tnum++;

               if(last_elap_time > elap_time)
                  of_elap_time++;

               abs_acq_tnum = acq_tnum + (65535 * of_acq_tnum);
               abs_tot_tnum = tot_tnum + (65535 * of_tot_tnum);
               abs_elap_time = elap_time + (65535 * of_elap_time);

               // add 1 to avoid division by zero
               delta_acq_tnum = abs_acq_tnum - abs_last_acq_tnum + 1;
               delta_tot_tnum = abs_tot_tnum - abs_last_tot_tnum + 1;
               delta_elap_time = abs_elap_time - abs_last_elap_time + 1;

               trigacqtot->setText(QString::number(abs_acq_tnum - offset_acq_tnum));
               trigtot->setText(QString::number(abs_tot_tnum - offset_tot_tnum));
               trigfreq->setText(QString::number(delta_tot_tnum/delta_elap_time));
               trigacqfreq->setText(QString::number(delta_acq_tnum/delta_elap_time));
               dtimeBar->setValue(dtime_perc);

               uint16_t hh = (abs_elap_time - offset_elap_time) / 3600;
               uint16_t mm = ((abs_elap_time - offset_elap_time) / 60) - (hh * 60);
               uint16_t ss = (abs_elap_time - offset_elap_time) - (hh * 3600) - (mm * 60);
               tsiuptime->setText(QString("%1").arg(hh, 2, 10, QChar('0')) + ":" + QString("%1").arg(mm, 2, 10, QChar('0')) + ":" + QString("%1").arg(ss, 2, 10, QChar('0')));

               last_acq_tnum = acq_tnum;
               last_tot_tnum = tot_tnum;
               last_elap_time = elap_time;

               abs_last_acq_tnum = abs_acq_tnum;
               abs_last_tot_tnum = abs_tot_tnum;
               abs_last_elap_time = abs_elap_time;

               // update dead time y-axis
               dtime_perc = 100 - ( (100*delta_acq_tnum) / delta_tot_tnum);
               dty[dtime_perc]++;
               dthisto->setSamples(dtx, dty, 101);
               dtPlot->replot();

               // update trigger rate graph
               if(tratex.size() > 20) {

                  tratex.remove(0);
                  acqratex.remove(0);
                  trPlot->replot();
               }

               if(tratey.size() > 20) {

                  tratey.remove(0);
                  acqratey.remove(0);
                  trPlot->replot();
               }

               QDateTime time = QDateTime::currentDateTime();

               tratex.append(QwtDate::toDouble( time ));
               tratey.append(delta_tot_tnum/delta_elap_time);
               // tratey.append(qrand());   // just for testing purposes...

               acqratex.append(QwtDate::toDouble( time ));
               acqratey.append(delta_acq_tnum/delta_elap_time);

               trgraph->setSamples(tratex, tratey);
               acqgraph->setSamples(acqratex, acqratey);

               trPlot->replot();

            }
         }        // if (!read_failed)

      }

      // Coincidence counters
      coincTable->clearContents();
      coincTable->setRowCount(0);

      QMap<unsigned int, QString> telmap = topo.telescopeMap();
      QMapIterator<unsigned int, QString> i(telmap);
      uint8_t row = 0;
      uint32_t data = 0;

      while(i.hasNext()) {

         i.next();
         unsigned int telindex = i.key();

         QList<QDetector> dlist = topo.detectorListOf(telindex, DSSD);

         for(int k = (dlist.size()-1); k >= 0; k--) {

            if(dlist[k].allocated()) {

               TSIport port = dlist[k].port();
               TSIcoinc coinc = dlist[k].coinc();

               coincTable->insertRow(row);
               coincTable->setRowHeight(row, 20);

               QTableWidgetItem *dname = new QTableWidgetItem();
               QTableWidgetItem *dcounter = new QTableWidgetItem();
               QTableWidgetItem *dport = new QTableWidgetItem();
               QTableWidgetItem *dline = new QTableWidgetItem();
               QTableWidgetItem *ddiv = new QTableWidgetItem();

               dname->setText(topo.telescopeName(telindex) + "." + dlist[k].name());
               dport->setText(TSIport_str[dlist[k].port()]);
               dline->setText(TSIline_str[dlist[k].line()] + " - " + TSIline_str[dlist[k].line()+4]);

               uint32_t freq, freqof;
               freq = freqof = 0;

               if(topo.VMEbus.read(topo.tsiregset.getregaddr("ContatoreFreCoinc", port, coinc), &data)) {

                  topo.tsiregset.setregvalue("ContatoreFreCoinc", port, data, coinc);
                  freq = topo.tsiregset.read_coincfreq(port, coinc);

               } else {

                  dcounter->setText("n/a");
               }


               if(topo.VMEbus.read(topo.tsiregset.getregaddr("ContOverFlowFreCoinc", port, coinc/4), &data)) {

                  topo.tsiregset.setregvalue("ContOverFlowFreCoinc", port, data, coinc/4);
                  freqof = topo.tsiregset.read_coincfreqof(port, coinc);

                  freq = (freqof << 16) + freq;

                  dcounter->setText(QString::number(freq));

               } else {

                  dcounter->setText("n/a");
               }

               if(topo.VMEbus.read(topo.tsiregset.getregaddr("DivisoreFreCoinc", port), &data)) {

                  uint8_t freqdiv = topo.tsiregset.parse_coincfreqdiv(coinc, data);
                  ddiv->setText(QString::number(freqdiv));

               } else {

                  ddiv->setText("n/a");
               }

               coincTable->setItem(row, 0, dname);
               coincTable->setItem(row, 1, dcounter);
               coincTable->setItem(row, 2, dport);
               coincTable->setItem(row, 3, dline);
               coincTable->setItem(row, 4, ddiv);

               row++;

            }  // end if(allocated)
         }
      }

      // WARNING: do not use disconnect()

   } else  {      // offline

      numfails++;

      vmestatusLabel->setPixmap(QPixmap(QString::fromUtf8(":/icon/icons/cancel-large.png")));

      tsistatusLabel->setText("<font color=\"grey\"><b> n/a </b>");
      trigtot->setText("<font color=\"grey\"><b> n/a </b>");
      trigacqtot->setText("<font color=\"grey\"><b> n/a </b>");
      dtimeBar->setValue(0);
      tsiuptime->setText("<font color=\"grey\"><b> n/a </b>");
   }
}

void CPForm::send_stop(void) {

   timer->stop();

   topo.VMEbus.write(topo.tsiregset.getregaddr("ControlWord", XF), STOP);
   update_tsistatus();
}

void CPForm::send_run(void) {

   timer->start();

   topo.VMEbus.write(topo.tsiregset.getregaddr("ControlWord", XF), RUN);
   update_tsistatus();
}

void CPForm::send_reset(void) {

   int r = QMessageBox::warning(this, "Message",
                                "Reset command will erase current Trigger Supervisor configuration and reset all counters. Are you sure ?",
                                QMessageBox::Yes, QMessageBox::No);

   if(r == QMessageBox::Yes) {

      init();

      // RESET = 0x0010
      topo.VMEbus.write(topo.tsiregset.getregaddr("ControlWord", XF), 0x10);
      update_tsistatus();
   }
}

void CPForm::update_reftime(void) {

   timer->start(refreshBox->value() * 1000); // in msec
}

void CPForm::update_tsistatus(void) {

   uint32_t data;

   // TSI status
   if(topo.VMEbus.read(topo.tsiregset.getregaddr("ControlWord", XF), &data)) {

      tsistatusLabel->setText("<font color=\"blue\"><b>" + topo.tsiregset.parse_status((TSIstatus) data) + "</b>");

      topo.tsiregset.set((TSIstatus) data);

      // disable simulation tab during run
      if(topo.tsiregset.parse_status() == RUN) {

         tabSet->setTabEnabled(1, false);

         stopButton->setEnabled(true);
         runButton->setEnabled(false);
         resetButton->setEnabled(false);

      } else {

         tabSet->setTabEnabled(1, true);

         if(topo.tsiregset.parse_status() == STOP) {

            stopButton->setEnabled(false);
            runButton->setEnabled(true);
            resetButton->setEnabled(true);

         } else {    // status == RESET

            stopButton->setEnabled(false);
            runButton->setEnabled(true);
            resetButton->setEnabled(false);

            clear_plots();
            trigacqtot->setText(QString::number(0));
            trigtot->setText(QString::number(0));
            dtimeBar->setValue(0);

            refreshSim();
            refreshDiv();
         }
      }

   } else {

      tsistatusLabel->setText("<font color=\"grey\"><b> n/a </b>");
   }
}

void CPForm::refreshSim(void) {

   QMap<unsigned int, QString> telmap = topo.telescopeMap();
   QMapIterator<unsigned int, QString> i(telmap);
   uint8_t row = 0;

   // clear table
   simTable->clearContents();
   simTable->setRowCount(0);

   // fill table
   while(i.hasNext()) {

      i.next();
      unsigned int telindex = i.key();
      QString telname = topo.telescopeName(telindex);

      QList<QDetector> dlist = topo.detectorList(telindex);

      for(int k = (dlist.size()-1); k >= 0; k--) {

         if(dlist[k].allocated()) {

            QString telname = topo.telescopeName(telindex);
            QString detname = dlist[k].name();
            TSIport port = dlist[k].port();
            TSIline line = dlist[k].line();
            QString type = DetectorType_str[dlist[k].type()];

            simTable->insertRow(row);
            simTable->setRowHeight(row, 20);

            QTableWidgetItem *simname = new QTableWidgetItem();
            QTableWidgetItem *simtype = new QTableWidgetItem();
            QTableWidgetItem *simport = new QTableWidgetItem();
            QTableWidgetItem *simline = new QTableWidgetItem();

            simname->setText(telname + "." + detname);
            simtype->setText(type);

            simport->setText(TSIport_str[port]);
            simport->setData(Qt::UserRole, port);
            simline->setText(TSIline_str[line]);
            simline->setData(Qt::UserRole, line);

            QCheckBox *check = new QCheckBox(this);

            simTable->setItem(row, 0, simname);
            simTable->setItem(row, 1, simtype);
            simTable->setItem(row, 2, simport);
            simTable->setItem(row, 3, simline);
            simTable->setCellWidget(row, 4, check);

            row++;

            if(dlist[k].type() == DSSD) {

               simTable->insertRow(row);
               simTable->setRowHeight(row, 20);

               QTableWidgetItem *simname = new QTableWidgetItem();
               QTableWidgetItem *simtype = new QTableWidgetItem();
               QTableWidgetItem *simport = new QTableWidgetItem();
               QTableWidgetItem *simline = new QTableWidgetItem();

               simname->setText(telname + "." + detname);
               simtype->setText(type);

               simport->setText(TSIport_str[port]);
               simport->setData(Qt::UserRole, port);
               simline->setText(TSIline_str[line+4]);
               simline->setData(Qt::UserRole, line+4);

               simTable->setItem(row, 0, simname);
               simTable->setItem(row, 1, simtype);
               simTable->setItem(row, 2, simport);
               simTable->setItem(row, 3, simline);
               QCheckBox *check = new QCheckBox(this);
               simTable->setCellWidget(row, 4, check);

               row++;
            }

         }

      }  // end for

   }  // end while

}

void CPForm::fire_sim(void) {

//   uint32_t data;

   for(int row=0; row<simTable->rowCount(); row++) {

      QTableWidgetItem *simport = new QTableWidgetItem();
      QTableWidgetItem *simline = new QTableWidgetItem();
      QCheckBox *check = new QCheckBox(this);

      simport = simTable->item(row, 2);
      simline = simTable->item(row, 3);
      check = (QCheckBox *) simTable->cellWidget(row, 4);

      if(check->isChecked()) {

         TSIport port = (TSIport) simport->data(Qt::UserRole).toInt();
         TSIline line = (TSIline) simline->data(Qt::UserRole).toInt();

         topo.tsiregset.setsim(port, line, ENABLED);
         topo.tsiregset.set(port);     // enable trigger generation
      }

   } // end for
   
   // VME command send
   topo.VMEbus.write(topo.tsiregset.getregaddr("AbilitazioneSimTrg", XA), topo.tsiregset.getregvalue("AbilitazioneSimTrg", XA));
   topo.VMEbus.write(topo.tsiregset.getregaddr("AbilitazioneSimTrg", XB), topo.tsiregset.getregvalue("AbilitazioneSimTrg", XB));
   topo.VMEbus.write(topo.tsiregset.getregaddr("AbilitazioneSimTrg", XC), topo.tsiregset.getregvalue("AbilitazioneSimTrg", XC));
   topo.VMEbus.write(topo.tsiregset.getregaddr("AbilitazioneSimTrg", X4), topo.tsiregset.getregvalue("AbilitazioneSimTrg", X4));
   
   topo.VMEbus.write(topo.tsiregset.getregaddr("GenSimTrigger", XA), topo.tsiregset.getregvalue("GenSimTrigger", XA));
   topo.VMEbus.write(topo.tsiregset.getregaddr("GenSimTrigger", XB), topo.tsiregset.getregvalue("GenSimTrigger", XB));
   topo.VMEbus.write(topo.tsiregset.getregaddr("GenSimTrigger", XC), topo.tsiregset.getregvalue("GenSimTrigger", XC));
   topo.VMEbus.write(topo.tsiregset.getregaddr("GenSimTrigger", X4), topo.tsiregset.getregvalue("GenSimTrigger", X4));
   
   // VME reset simulation setup
   topo.VMEbus.write(topo.tsiregset.getregaddr("AbilitazioneSimTrg", XA), 0);
   topo.VMEbus.write(topo.tsiregset.getregaddr("AbilitazioneSimTrg", XB), 0);
   topo.VMEbus.write(topo.tsiregset.getregaddr("AbilitazioneSimTrg", XC), 0);
   topo.VMEbus.write(topo.tsiregset.getregaddr("AbilitazioneSimTrg", X4), 0);

   topo.VMEbus.write(topo.tsiregset.getregaddr("GenSimTrigger", XA), 0);
   topo.VMEbus.write(topo.tsiregset.getregaddr("GenSimTrigger", XB), 0);
   topo.VMEbus.write(topo.tsiregset.getregaddr("GenSimTrigger", XC), 0);
   topo.VMEbus.write(topo.tsiregset.getregaddr("GenSimTrigger", X4), 0);

   refresh();
}

void CPForm::refreshDiv(void) {

   QMap<unsigned int, QString> telmap = topo.telescopeMap();
   QMapIterator<unsigned int, QString> i(telmap);
   uint8_t row = 0;

   // clear table
   divTable->clearContents();
   divTable->setRowCount(0);

   // fill table
   while(i.hasNext()) {

      i.next();
      unsigned int telindex = i.key();
      QString telname = topo.telescopeName(telindex);

      QList<QDetector> dlist = topo.detectorList(telindex);

      for(int k = (dlist.size()-1); k >= 0; k--) {

         if( dlist[k].allocated() && (dlist[k].type() == DSSD) ) {

            QString telname = topo.telescopeName(telindex);
            QString detname = dlist[k].name();
            TSIport port = dlist[k].port();
            TSIcoinc coinc = dlist[k].coinc();
            QString type = DetectorType_str[dlist[k].type()];

            divTable->insertRow(row);
            divTable->setRowHeight(row, 20);

            QTableWidgetItem *dname = new QTableWidgetItem();
            QTableWidgetItem *dtype = new QTableWidgetItem();
            QMap <QString, QVariant> tmpmap;

            dname->setText(telname + "." + detname);
            dtype->setText(type);

            tmpmap["port"] = (TSIport) port;
            tmpmap["coinc"] = (TSIcoinc) coinc;
            dname->setData(Qt::UserRole, tmpmap);

            QComboBox *divlist = new QComboBox(this);
            divlist->addItems(CoincFreqDiv_str);

            divTable->setItem(row, 0, dname);
            divTable->setItem(row, 1, dtype);
            divTable->setCellWidget(row, 2, divlist);

            row++;
         }

      }  // end for

   }  // end while
}

void CPForm::fire_div(void) {

   topo.tsiregset.setregvalue("DivisoreFreCoinc", XA, 0);
   topo.tsiregset.setregvalue("DivisoreFreCoinc", XB, 0);
   topo.tsiregset.setregvalue("DivisoreFreCoinc", XC, 0);

   for(int row=0; row<divTable->rowCount(); row++) {

      QTableWidgetItem *dname = new QTableWidgetItem();
      QComboBox *divlist = new QComboBox(this);
      QMap <QString, QVariant> tmpmap;

      dname = divTable->item(row, 0);
      divlist = (QComboBox *) divTable->cellWidget(row, 2);
      tmpmap = dname->data(Qt::UserRole).toMap();

      TSIport port = (TSIport) tmpmap["port"].toUInt();
      TSIcoinc coinc = (TSIcoinc) tmpmap["coinc"].toUInt();

      topo.tsiregset.set(port, coinc, (CoincFreqDiv) divlist->currentIndex());

   } // end for

   // VME command send
   topo.VMEbus.write(topo.tsiregset.getregaddr("DivisoreFreCoinc", XA), topo.tsiregset.getregvalue("DivisoreFreCoinc", XA));
   topo.VMEbus.write(topo.tsiregset.getregaddr("DivisoreFreCoinc", XB), topo.tsiregset.getregvalue("DivisoreFreCoinc", XB));
   topo.VMEbus.write(topo.tsiregset.getregaddr("DivisoreFreCoinc", XC), topo.tsiregset.getregvalue("DivisoreFreCoinc", XC));
}
