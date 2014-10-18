#include "sumdialog.h"
#include "QMetrics.h"

#include <QPrinter>
#include <QPrintDialog>
#include <QDateTime>
#include <QMessageBox>

SummaryDialog::SummaryDialog(QTopology &tpl, QWidget *parent) : QDialog(parent), topo(tpl) {

   setupUi(this);

   connect(printButton, SIGNAL(clicked()), this, SLOT(print()));
   connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
   connect(sendButton, SIGNAL(clicked()), this, SLOT(sendVMEConfig()));
}

void SummaryDialog::init(void) {

   textBrowser->clear();
}

void SummaryDialog::showVMEConfig(void) {

   QStringList tmplist = topo.dumpVMEConfig(true);

   init();

   for(int i=0; i<tmplist.size(); i++) {
      if(tmplist[i] != "")
         textBrowser->append(tmplist[i]);
   }
}

void SummaryDialog::sendVMEConfig(void) {

   int r = QMessageBox::warning(this, "Message",
                                "Reset command will erase current Trigger Supervisor configuration and reset all counters. Are you sure ?",
                                QMessageBox::Yes, QMessageBox::No);

   if(r == QMessageBox::Yes) {

      bool retval = topo.sendVMEConfig();

      if(!retval)
         QMessageBox::warning(this, "Warning", "VME2NET connection error. Verify connection setup", QMessageBox::Ok);
   }
}

void SummaryDialog::showSummary(void) {

   init();

   QString page;

   unsigned int telindex;
   unsigned int ndet = 0;

   QString pagehdr = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\""
                     "\"http://www.w3.org/TR/REC-html40/strict.dtd\"><html><head><meta name=\"qrichtext\" content=\"1\"/>"
                     "<style type=\"text/css\"> p, li { white-space: pre-wrap; }</style></head>";

   QString mainhdr = "<body><p align=\"center\"><h2>EXOTIC Trigger Supervisor configuration summary</h2><p>";

   QString datehdr = "<p align=\"center\"><h3>%1</h3><p><br>";

   QString sechdr1 = "<p align=\"center\"><h3><font color=\"blue\">Configuration report for telescopes</font></h3><p>";

   QString telhdr = "<span style=\"font-weight: bold;\">Telescope</span> %1<br>";

   QString telftr = "</div>";

   QString sechdr2 = "<p align=\"center\"><h3><font color=\"blue\">Configuration report for TSI ports</font></h3><p>";

   QString porthdr = "<div style=\"margin-left: 10px;\"> <table style=\"text-align:center; width:300px;\" border=\"0\""
                     "cellpadding=\"1\" cellspacing=\"1\"><tbody>";

   QString portrow = "<tr><td width=150><p align=\"center\">%1</p></td>"
                     "<td width=150><p align=\"center\">%2</p></td></tr>";

   QString portftr = "</tbody></table><br></div>";

   QString sechdr3 = "<p align=\"center\"><h3><font color=\"blue\">Configuration report for monitoring lines</font></h3><p>";

   QString sechdr4 = "<p align=\"center\"><h3><font color=\"blue\">Configuration report for Trigger Level I</font></h3><p>";

   QString sechdr5 = "<p align=\"center\"><h3><font color=\"blue\">Configuration report for Trigger Level II</font></h3><p>";

   QString mainftr = "</body></html>";

   page += pagehdr;
   page += mainhdr;

   QDateTime now = QDateTime::currentDateTime();
   page += datehdr.arg(now.toString());

   page += sechdr1;

   // TELESCOPE status

   QMap<unsigned int, QString> telmap = topo.telescopeMap();
   QMapIterator<unsigned int, QString> i(telmap);

   while(i.hasNext()) {

      i.next();
      telindex = i.key();

      page += telhdr.arg(topo.telescopeName(telindex));

      QList<QDetector> dlist = topo.detectorList(telindex);

      for(int k = (dlist.size()-1); k >= 0; k--) {

         ndet++;

         page += htmlDetector(dlist[k]);

         if( (ndet % 3) == 0 )
            page += "<p style=\"page-break-before: always\">";
      }

      page += telftr;
      page += "<p style=\"page-break-before: always\">";
   }

   page += "<p style=\"page-break-before: always\">";

   // TSI PORT status

   page += sechdr2;

   page += porthdr;
   page += portrow.arg(htmlPort(XA), htmlPort(XB));
   page += portftr;
   page += porthdr;
   page += portrow.arg(htmlPort(XC), htmlPort(X4));
   page += portftr;

   page += "<p style=\"page-break-before: always\">";

   // MON LINES status

   page += sechdr3;

   page += htmlMonlines(XA);
   page += "<br>";
   page += htmlMonlines(XB);
   page += "<br>";
   page += htmlMonlines(XC);
   page += "<br>";
   page += htmlMonlines(X4);
   page += "<br>";
   page += htmlMonlines(XF);
   page += "<br>";

   page += "<p style=\"page-break-before: always\">";

   // TRIGGER LEVEL I configuration

   page += sechdr4;
   page += htmlTL1config();

   // TRIGGER LEVEL II configuration

   page += sechdr5;
   page += htmlTL2config();

   page += mainftr;

   textBrowser->setHtml(page);
}

QString SummaryDialog::htmlDetector(QDetector det) {

   QString str;

   QString dethdr = "<div style=\"margin-left: 40px;\"><span style=\"font-weight: bold;\">Detector</span> %1 - %2";

   QString detwire = "<div style=\"margin-left: 40px;\"> <table style=\"text-align:left; width:300px;\" border=\"1\""
                     "cellpadding=\"1\" cellspacing=\"1\"><tbody>"
                     "<tr><td width=150><p align=\"center\"><b>TSI port</b></p></td>"
                     "<td width=150><p align=\"center\"><b>TSI line</b></p></td></tr>"
                     "<tr><td width=150><p align=\"center\">%1</p></td>"
                     "<td width=150><p align=\"center\">%2</p></td></tr>"
                     "</tbody></table><br></div>";

   QString detsetup = "<div style=\"margin-left: 40px;\"> <table style=\"text-align:left; width:500px;\" border=\"1\""
                      "cellpadding=\"1\" cellspacing=\"1\"><tbody>"
                      "<tr><td width=125><p align=\"center\"><b>enabled</b></p></td>"
                      "<td width=125><p align=\"center\"><b>coinc. logic</b></p></td>"
                      "<td width=125><p align=\"center\"><b>signal delay</b></p></td>"
                      "<td width=125><p align=\"center\"><b>signal width</b></p></td></tr>"
                      "<tr><td width=125><p align=\"center\">%1</p></td>"
                      "<td width=125><p align=\"center\">%2</p></td>"
                      "<td width=125><p align=\"center\">%3</p></td>"
                      "<td width=125><p align=\"center\">%4</p></td></tr>"
                      "</tbody></table><br></div>";

   QString monsetup = "<div style=\"margin-left: 40px;\"><span style=\"font-weight: bold;\">"
                      "<font color=\"blue\">Monitoring is enabled for \"%1\" on line %2.%3</font></span><br></div>";

   QString detstathdr = "<ul type=\"square\">";

   QString detstatw1 = "<li><font color=\"red\"><b>Wiring warning</b>"
                       " - detector input line is not specified and it will not included in trigger logic</font></li>";

   QString detstatw2 = "<li><font color=\"red\"><b>Setup warning</b>"
                       " - signal width equal to zero means that input is disabled for this detector</li>";

   QString detstatok = "<li><font color=\"green\"><b>Configuration of wiring and setup is correct !</b></font></li>";

   QString detstatftr = "</ul>";

   QString detftr = "</div><br>";

   str += dethdr.arg(det.name(), QDetector::type_str(det.type()));

   QString port, line;
   QString enabled, clogic, width, delay;

   if(det.allocated()) {

      port = QDetector::port_str(det.port());

      if(det.type() == DSSD)
         line += QDetector::line_str(det.line()) + " - " + QDetector::line_str( (TSIline) (det.line()+4) );
      else
         line = QDetector::line_str(det.line());

   } else {

      port = " - ";
      line = " - ";
   }

   if(det.enabled())
      enabled = "YES";
   else
      enabled = "<b>NO</b>";

   if(det.type() == DSSD)
      clogic = QDetector::coincop_str(det.coincop());
   else
      clogic = " - ";
   delay = CoincDelay_strval[det.coincdelay()];
   width = CoincPeriod_strval[det.coincperiod()];

   str += detwire.arg(port, line);
   str += detsetup.arg(enabled, clogic, delay, width);

   if(det.allocated()) {

      QList<QMonitorParamInfo> mlist = topo.monparamList(det.port());

      for(int i=0; i<mlist.size(); i++) {

         if((mlist[i].bindline == det.line()) || (mlist[i].bindline == (TSIline) (det.line()+4)))
            str += monsetup.arg(mlist[i].desc, port, QDetector::mon_str(mlist[i].mline));
      }
   }

   str += detstathdr;

   bool ok = true;

   if(!det.allocated()) {

      str += detstatw1;
      ok = false;
   }

   if(det.coincperiod() == PER0) {

      str += detstatw2;
      ok = false;
   }

   if(ok)
      str += detstatok;

   str += detstatftr;

   str += detftr;

   return str;
}

QString SummaryDialog::htmlPort(TSIport p) {

   QString str;

   QString porthdr =  "<span style=\"font-weight: bold;\">Port</span> %1<br>"
                      "<table style=\"text-align:center; width:300px;\" border=\"1\""
                      "cellpadding=\"1\" cellspacing=\"1\"><tbody>"
                      "<tr><td width=150><p align=\"center\"><b>line</b></p></td>"
                      "<td width=150><p align=\"center\"><b>telescope.detector</b></p></td></tr>";

   QString linerow = "<tr><td width=150><p align=\"center\">%1</p></td>"
                     "<td width=150><p align=\"center\">%2</p></td></tr>";

   QString portftr = "</tbody></table><br>";

   QList<TSIline> busyl;
   QList<TSImon> busym;
   QList<QMonitorParamInfo> mlist;

   busym = topo.busyMonitorLines(p);
   busyl = topo.busyInputLines(p);
   mlist = topo.monparamList(p);

   str += porthdr.arg(QDetector::port_str(p));

   // MON-LINES

   for(TSImon m = (TSImon) (TSImon_max[p] - 1); m >= 0; m = (TSImon)(m - 1)) {

      bool found = false;
      for(int i=0; i<mlist.size(); i++) {

         if(mlist[i].mline == m) {

            found = true;

            if(mlist[i].bindline != UNDEF_LINE) {

               QPair<unsigned int, unsigned int> detloc = topo.detectorTopoInfo(p, mlist[i].bindline);

               if( (detloc.first != 0) && (detloc.second != 0) ) {

                  QDetector det = topo.detector(detloc.first, detloc.second);
                  str += linerow.arg(QDetector::mon_str(m), topo.telescopeName(detloc.first) + "." + det.name());

               } else str += linerow.arg(QDetector::mon_str(m), "<i>[see below]</i>");

            } else str += linerow.arg(QDetector::mon_str(m), "<i>[see below]</i>");
         }
      }

      if(!found)
         str += linerow.arg(QDetector::mon_str(m), " ");
   }

   // TRG-LINES
   for(TSIline l = TRG15; l >= 0; l = (TSIline) (l - 1)) {

      int ix = busyl.indexOf(l);

      if(ix != -1) {

         QPair<unsigned int, unsigned int> detloc = topo.detectorTopoInfo(p, l);
         QDetector det = topo.detector(detloc.first, detloc.second);

         str += linerow.arg(QDetector::line_str(l), topo.telescopeName(detloc.first) + "." + det.name());

      } else str += linerow.arg(QDetector::line_str(l), " ");
   }

   str += portftr;

   return str;
}

QString SummaryDialog::htmlMonlines(TSIport p) {

   QString str;

   QString monhdr =  "<p align=\"center\"><span style=\"font-weight: bold;\">Port</span> %1</p>"
                      "<p align=\"center\"><table style=\"text-align:center; width:600px;\" border=\"1\""
                      "cellpadding=\"1\" cellspacing=\"1\"><tbody>"
                      "<tr><td width=150><p align=\"center\"><b>monitor line</b></p></td>"
                      "<td width=150><p align=\"center\"><b>description</b></p></td></tr>";

   QString monrow = "<tr><td width=150><p align=\"center\">%1</p></td>"
                     "<td width=150><p align=\"center\">%2</p></td></tr>";

   QString monftr = "</tbody></table></p><br>";

   QList<QMonitorParamInfo> mlist = topo.monparamList(p);

   str += monhdr.arg(QDetector::port_str(p));

   // MON-LINES

   for(TSImon m = MON0; m < (TSImon)(TSImon_max[p]); m = (TSImon)(m + 1)) {

      bool found = false;
      for(int i=0; i<mlist.size(); i++) {

         if(mlist[i].mline == m) {

            found = true;

            str += monrow.arg(QDetector::mon_str(m), mlist[i].desc);
         }
      }

      if(!found)
         str += monrow.arg(QDetector::mon_str(m), " ");
   }

   str += monftr;

   return str;
}

QString SummaryDialog::htmlTL1config(void) {

   QString str;

   for(int s=0; s<TL1SIGOUT_NUM; s++) {

      TL1SignalOut sigout = (enum TL1SignalOut) s;

      if(topo.TL1logic.allocatedOutput(sigout)) {

         str += "<b>" + TL1SignalOut_str[sigout] + "</b>" + " = ";

         uint8_t row = topo.TL1logic.getAssignedGate(sigout);

         QGate g = topo.TL1logic.getGate(topo.TL1logic.getAssignedGate(sigout));
         QVector<uint8_t> linein = g.getLineInIndex();

         for(int i=0; i<linein.size(); i++) {

            if(topo.TL1logic.getInput(row, linein[i]) > X4_TRG15) {

               str += TL1SignalIn_str[topo.TL1logic.getInput(row, linein[i])] + "  ";

            } else {

               QPair<unsigned int, unsigned int> detloc = topo.detectorTopoInfo(X4, (TSIline)(topo.TL1logic.getInput(row, linein[i])));
               QDetector det = topo.detector(detloc.first, detloc.second);

               str += topo.telescopeName(detloc.first) + "." + det.name() + " (" + TL1SignalIn_str[topo.TL1logic.getInput(row, linein[i])] + ")  ";
            }

            if((i + 1) <= (linein.size() - 1))
               str += "<font color=\"red\">" + QGateType_str[g.getType()] + "  " + "</font>";
         }

         str += "<br><br>";
      }
   }

   return str;
}

QString SummaryDialog::htmlTL2config(void) {

   QString str;

   for(int s=0; s<TL2SIGOUT_NUM; s++) {

      TL2SignalOut sigout = (enum TL2SignalOut) s;

      if(topo.TL2logic.allocatedOutput(sigout)) {

         str += "<b>" + TL2SignalOut_str[sigout] + "</b>" + " = ";

         uint8_t row = topo.TL2logic.getAssignedGate(sigout);

         QGate g = topo.TL2logic.getGate(topo.TL2logic.getAssignedGate(sigout));
         QVector<uint8_t> linein = g.getLineInIndex();

         for(int i=0; i<linein.size(); i++) {

               str += TL2SignalIn_str[topo.TL2logic.getInput(row, linein[i])] + "  ";

            if((i + 1) <= (linein.size() - 1))
               str += "<font color=\"red\">" +  QGateType_str[g.getType()] + "  " + "</font>";
         }

         str += "<br><br>";
      }
   }

   return str;
}

void SummaryDialog::print(void) {

   QPrinter *printer = new QPrinter();
   QPrintDialog *printdialog = new QPrintDialog(printer, this);

   if (printdialog->exec() == QDialog::Accepted) {

      textBrowser->print(printer);
   }
}

