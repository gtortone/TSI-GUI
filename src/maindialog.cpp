#include <QtGui>

#include "maindialog.h"
#include <stdint.h>
#include <QStringList>

MainDialog::MainDialog(void) {

   setupUi(this);

   dlgtitle = this->windowTitle();

   splitter = new QSplitter(centralwidget);
   splitter->setOrientation(Qt::Horizontal);
   splitter->setStretchFactor(1,1);

   logoform = new LogoForm(this);
   splitter->addWidget(logoform);

   topoform = new TopologyForm(topo, this);
   topoform->init();
   splitter->addWidget(topoform);

   dwiresetform = new DetWireSetForm(topo, this);
   dwiresetform->init();
   splitter->addWidget(dwiresetform);

   dmonform = new DssdMonForm(topo, this);
   dmonform->init();
   splitter->addWidget(dmonform);

   omonform = new OdetMonForm(topo, this);
   omonform->init();
   splitter->addWidget(omonform);

   pmonform = new PortMonForm(topo, this);
   pmonform->init();
   splitter->addWidget(pmonform);

   sumdialog = new SummaryDialog(topo, this);
   sumdialog->init();

   tl1form = new TL1Form(topo, this);
   tl1form->init();
   splitter->addWidget(tl1form);

   tl2form = new TL2Form(topo, this);
   tl2form->init();
   splitter->addWidget(tl2form);

   cpform = new CPForm(topo, this);
   splitter->addWidget(cpform);

   helpform01 = new HelpDialog01(this);

   setCentralWidget(splitter);

   // set menu actions
   connect(actionNewWorkspace, SIGNAL(triggered()), this, SLOT(newWorkspace()));
   connect(actionOpenWorkspace, SIGNAL(triggered()), this, SLOT(openWorkspace()));
   connect(actionSaveWorkspace, SIGNAL(triggered()), this, SLOT(saveWorkspace()));
   connect(actionSaveWorkspaceAs, SIGNAL(triggered()), this, SLOT(saveWorkspaceAs()));
   connect(actionCloseWorkspace, SIGNAL(triggered()), this, SLOT(closeWorkspace()));
   connect(actionExit, SIGNAL(triggered()), this, SLOT(exit()));

   connect(actionNewTopology, SIGNAL(triggered()), this, SLOT(newTopology()));
   connect(actionEditTopology, SIGNAL(triggered()), this, SLOT(editTopology()));

   connect(actionDetWireSetup, SIGNAL(triggered()), this, SLOT(detWiringSetup()));
   connect(actionMonDet, SIGNAL(triggered()), this, SLOT(assignMonDet()));
   connect(actionMonPort, SIGNAL(triggered()), this, SLOT(assignMonPort()));
   connect(actionConfigSummary, SIGNAL(triggered()), this, SLOT(showSummary()));

   connect(actionTL1config, SIGNAL(triggered()), this, SLOT(TL1Config()));
   connect(actionTL2config, SIGNAL(triggered()), this, SLOT(TL2Config()));
   connect(actionTL3config, SIGNAL(triggered()), this, SLOT(TL3Config()));

   connect(actionSetBaddr, SIGNAL(triggered()), this, SLOT(setvmebaddr()));
   connect(actionSetVMEConn, SIGNAL(triggered()), this, SLOT(setvmeconn()));
   connect(actionTSIcp, SIGNAL(triggered()), this, SLOT(TSIpanel()));
   connect(actionGenConfig, SIGNAL(triggered()), this, SLOT(dumpConfig()));

   connect(actionTSIdiagram, SIGNAL(triggered()), this, SLOT(help01()));
   connect(actionAboutGui, SIGNAL(triggered()), this, SLOT(aboutGui()));
   connect(actionAboutQt, SIGNAL(triggered()), this, SLOT(aboutQt()));

   actionSaveWorkspace->setEnabled(false);
   actionSaveWorkspaceAs->setEnabled(false);
   actionCloseWorkspace->setEnabled(false);

   wsactive = false;
   init();
}

void MainDialog::closeEvent(QCloseEvent *event) {

   int r = QMessageBox::warning(this, tr("Message"), tr("Exit from TSI GUI\n" "Are you sure ?"), QMessageBox::Yes,
                                QMessageBox::No);

   if (r == QMessageBox::Yes) {
      closeWorkspace();
      event->accept();

   } else {

      event->ignore();
   }
 }

void MainDialog::init(void) {

   topoform->hide();
   dwiresetform->hide();
   dmonform->hide();
   omonform->hide();
   pmonform->hide();
   sumdialog->hide();
   tl1form->hide();
   tl2form->hide();
   cpform->hide();
   logoform->show();

   refreshMenubar();
}

void MainDialog::refreshMenubar(void) {

   if(!wsactive) {

      actionNewTopology->setEnabled(false);
      actionEditTopology->setEnabled(false);
      actionDetWireSetup->setEnabled(false);
      menuMonAssign->setEnabled(false);
      actionConfigSummary->setEnabled(false);
      actionTL1config->setEnabled(false);
      actionTL2config->setEnabled(false);
      actionTL3config->setEnabled(false);
      actionSetBaddr->setEnabled(false);
      actionSetVMEConn->setEnabled(false);
      actionTSIcp->setEnabled(false);
      actionGenConfig->setEnabled(false);

   } else {

      if(topo.empty()) {

         actionNewTopology->setEnabled(true);
         actionEditTopology->setEnabled(false);
         actionDetWireSetup->setEnabled(false);
         menuMonAssign->setEnabled(false);
         actionConfigSummary->setEnabled(false);
         actionTL1config->setEnabled(false);
         actionTL2config->setEnabled(false);
         actionTL3config->setEnabled(false);
         actionSetBaddr->setEnabled(false);
         actionSetVMEConn->setEnabled(false);
         actionTSIcp->setEnabled(false);
         actionGenConfig->setEnabled(false);

      } else {

         actionNewTopology->setEnabled(true);
         actionEditTopology->setEnabled(true);
         actionDetWireSetup->setEnabled(true);
         menuMonAssign->setEnabled(true);
         actionConfigSummary->setEnabled(true);
         actionTL1config->setEnabled(true);
         actionTL2config->setEnabled(true);
         actionTL3config->setEnabled(true);
         actionSetBaddr->setEnabled(true);
         actionSetVMEConn->setEnabled(true);
         actionTSIcp->setEnabled(true);
         actionGenConfig->setEnabled(true);
      }

      actionSaveWorkspace->setEnabled(true);
      actionSaveWorkspaceAs->setEnabled(true);
      actionCloseWorkspace->setEnabled(true);
   }
}

void MainDialog::newWorkspace(void) {

   if(wsactive) {

      int r = QMessageBox::warning(this, tr("Message"), tr("Do you want to save your changes on current workspace ?"), QMessageBox::Yes,
                                   QMessageBox::No, QMessageBox::Cancel);

      if(r != QMessageBox::Cancel) {

         if(r == QMessageBox::Yes)
            saveWorkspace();

         filename.clear();
         topoform->init();
      }
   }

   wsactive = true;

   this->setWindowTitle( dlgtitle + " - untitled-ws");

   init();
}

void MainDialog::openWorkspace(void) {

   QString xmlin;
   QString filein;
   QPair<QXmlStreamReader::Error, QString> xmlerr;

   closeWorkspace();

   filein = QFileDialog::getOpenFileName(this, "Open Topology file", ".", "XML files (*.xml)");

   if(filein.isEmpty()) {

      QMessageBox::warning(this, "Message", "File name not specified - topology not opened !", QMessageBox::Ok);
      return;
   }

   QFile file(filein);

   if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {

      QMessageBox::warning(this, "Message", "Error reading from " + filename, QMessageBox::Ok);

   } else {

      while (!file.atEnd()) {
         QByteArray line = file.readLine();
         xmlin += line;
      }
      file.close();

      xmlerr = topo.deserializeFromXML(xmlin);

      // if no errors...
      if(xmlerr.first == QXmlStreamReader::NoError) {

         wsactive = true;

         refreshMenubar();
         topoform->refresh();

         filename = filein;
         QMessageBox::information(this, "Message", "Topology successfully opened from " + filename, QMessageBox::Ok);
         this->setWindowTitle( dlgtitle + " - " + filename);

      } else {

         QMessageBox::critical(this, "Message", "Error detected in topology file: " + xmlerr.second, QMessageBox::Ok);
      }
   }
}

void MainDialog::saveWorkspace(void) {

   QString xmlout;
   QString fileout;

   xmlout = topo.serializeToXML();

   if(filename.isEmpty()) {

      fileout = QFileDialog::getSaveFileName(this, "Save Topology file", ".", "XML files (*.xml)");

      if(fileout.isEmpty()) {

         QMessageBox::warning(this, "Message", "File name not specified - topology not saved !", QMessageBox::Ok);
         return;

      } else {

         if (!fileout.endsWith(".xml", Qt::CaseInsensitive))
            filename = fileout + ".xml";
         else
            filename = fileout;
      }
   }

   QFile file(filename);

   if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {

      QMessageBox::warning(this, "Message", "Error writing on " + filename, QMessageBox::Ok);

   } else {

      file.write(xmlout.toStdString().c_str());
      file.close();
      QMessageBox::information(this, "Message", "Topology saved in " + filename, QMessageBox::Ok);
      this->setWindowTitle( dlgtitle + " - " + filename);
   }

}

void MainDialog::saveWorkspaceAs(void) {

   QString xmlout;
   QString fileout;

   xmlout = topo.serializeToXML();

   fileout = QFileDialog::getSaveFileName(this, "Save Topology file as", ".", "XML files (*.xml)");

   if(fileout.isEmpty()) {

      QMessageBox::warning(this, "Message", "File name not specified - topology not saved !", QMessageBox::Ok);
      return;

   } else {

      if (!fileout.endsWith(".xml", Qt::CaseInsensitive))
         filename = fileout + ".xml";
      else
         filename = fileout;
   }


   QFile file(filename);

   if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {

      QMessageBox::warning(this, "Message", "Error writing on " + filename, QMessageBox::Ok);

   } else {

      file.write(xmlout.toStdString().c_str());
      file.close();
      QMessageBox::information(this, "Message", "Topology saved in " + filename, QMessageBox::Ok);
      this->setWindowTitle( dlgtitle + " - " + filename);
   }
}

void MainDialog::closeWorkspace(void) {

   if(wsactive) {

      int r = QMessageBox::warning(this, tr("Message"), tr("Do you want to save your changes on current workspace ?"), QMessageBox::Yes,
                                   QMessageBox::No, QMessageBox::Cancel);

      if(r != QMessageBox::Cancel) {

         if(r == QMessageBox::Yes)
            saveWorkspace();

         filename.clear();
         topoform->init();
      }
   }

   wsactive = false;

   actionSaveWorkspace->setEnabled(false);
   actionSaveWorkspaceAs->setEnabled(false);
   actionCloseWorkspace->setEnabled(false);

   this->setWindowTitle(dlgtitle);

   init();
}

void MainDialog::exit(void) {

   closeWorkspace();
   qApp->exit(0);
}

void MainDialog::newTopology(void) {

   if(!topo.empty()) {

      int r = QMessageBox::warning(this, tr("Message"), tr("New Topology will clear current topology !\n" "Are you sure ?"), QMessageBox::Yes,
                                   QMessageBox::No);

      if (r == QMessageBox::Yes)
         topoform->init();
   }

   editTopology();
   refreshMenubar();
}

void MainDialog::editTopology(void) {

   topoform->setMode(TopologyForm::MODE_EDIT);
   topoform->show();

   // set form actions
   disconnect(topoform->detTable, SIGNAL(itemClicked(QTableWidgetItem *)), dwiresetform, SLOT(showDetector(QTableWidgetItem *)));
   disconnect(topoform->detTable, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(selectMonForm(QTableWidgetItem*)));
   disconnect(topoform->telComboBox, SIGNAL(currentIndexChanged(int)), dwiresetform, SLOT(init()));

   logoform->hide();
   dwiresetform->hide();
   dmonform->hide();
   omonform->hide();
   pmonform->hide();
   tl1form->hide();
   tl2form->hide();
   cpform->hide();
}

void MainDialog::detWiringSetup(void) {
   
   topoform->setMode(TopologyForm::MODE_SELECTION);
   topoform->show();

   // set form actions
   connect(topoform->detTable, SIGNAL(itemClicked(QTableWidgetItem *)), dwiresetform, SLOT(showDetector(QTableWidgetItem *)));
   disconnect(topoform->detTable, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(selectMonForm(QTableWidgetItem*)));
   connect(topoform->telComboBox, SIGNAL(currentIndexChanged(int)), dwiresetform, SLOT(init()));

   logoform->hide();
   dmonform->hide();
   omonform->hide();
   pmonform->hide();
   dwiresetform->show();
   dwiresetform->init();
   tl1form->hide();
   tl2form->hide();
   cpform->hide();
}

void MainDialog::assignMonDet(void) {

   topoform->setMode(TopologyForm::MODE_SELECTION);
   topoform->show();

   // set form actions
   disconnect(topoform->detTable, SIGNAL(itemClicked(QTableWidgetItem *)), dwiresetform, SLOT(showDetector(QTableWidgetItem *)));
   connect(topoform->detTable, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(selectMonForm(QTableWidgetItem*)));
   disconnect(topoform->telComboBox, SIGNAL(currentIndexChanged(int)), dwiresetform, SLOT(init()));

   logoform->hide();
   dwiresetform->hide();
   pmonform->hide();
   tl1form->hide();
   tl2form->hide();
   cpform->hide();
}

void MainDialog::assignMonPort(void) {

   topoform->setMode(TopologyForm::MODE_SELECTION);
   topoform->show();

   // set form actions
   disconnect(topoform->detTable, SIGNAL(itemClicked(QTableWidgetItem *)), dwiresetform, SLOT(showDetector(QTableWidgetItem *)));
   disconnect(topoform->detTable, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(selectMonForm(QTableWidgetItem*)));
   disconnect(topoform->telComboBox, SIGNAL(currentIndexChanged(int)), dwiresetform, SLOT(init()));

   logoform->hide();
   dwiresetform->hide();
   dmonform->hide();
   omonform->hide();
   tl1form->hide();
   tl2form->hide();
   cpform->hide();

   pmonform->init();
   pmonform->show();
}

void MainDialog::selectMonForm(QTableWidgetItem *item) {

   QMap <QString, QVariant> tmpmap = item->data(Qt::UserRole).toMap();
   unsigned int telindex = tmpmap["telindex"].toUInt();
   unsigned int detindex = tmpmap["detindex"].toUInt();
   QDetector det = topo.detector(telindex, detindex);

   if(det.type() == DSSD) {

      omonform->hide();
      pmonform->hide();
      dmonform->show();
      dmonform->init();
      dmonform->showDetector(item);

   } else {

      dmonform->hide();
      omonform->show();
      pmonform->hide();
      omonform->init();
      omonform->showDetector(item);
   }
}

void MainDialog::showSummary(void) {

   sumdialog->show();
   sumdialog->raise();
   sumdialog->activateWindow();

   sumdialog->showSummary();
}

void MainDialog::setvmebaddr(void) {

   bool okdialog, okconv;
   QString text = QInputDialog::getText(this, "Set TSI VME base address", "address (in hex)", \
                                        QLineEdit::Normal, QString::number(topo.tsiregset.getbaddr(), 16).toUpper(), &okdialog);

   if(okdialog) {

      uint32_t addr = text.toULong(&okconv, 16);

      if(okconv) {

         topo.tsiregset.setbaddr(addr);
         QMessageBox::information(this, tr("Message"), tr("TSI VME base address successfully changed !"), QMessageBox::Ok);

      } else QMessageBox::critical(this, tr("Message"), tr("TSI VME base address not valid !"), QMessageBox::Ok);
   }
}

void MainDialog::setvmeconn(void) {

   bool okdialog, okconv;
   QString url;

   if(topo.VMEbus.gethostname() != "")
      url = topo.VMEbus.gethostname() + ":" + QString::number(topo.VMEbus.getport());

   QString text = QInputDialog::getText(this, "Set VME2NET connection", "hostname:port", QLineEdit::Normal, url, &okdialog);

   if(okdialog) {

      okconv = true;

      QStringList sigin = text.split(":");

      if(sigin.size() == 2) {

         bool ok;
         sigin[1].toUInt(&ok, 10);

         if(!ok)
            okconv = false;

      } else okconv = false;

      if(okconv) {

         topo.VMEbus.setup(sigin[0], sigin[1].toUInt());
         QMessageBox::information(this, tr("Message"), tr("VME2NET host successfully configured !"), QMessageBox::Ok);

      } else QMessageBox::critical(this, tr("Message"), tr("VME2NET host:port not valid !"), QMessageBox::Ok);
   }
}

void MainDialog::TSIpanel(void) {

   logoform->hide();
   topoform->hide();
   dwiresetform->hide();
   dmonform->hide();
   omonform->hide();
   pmonform->hide();
   tl1form->hide();
   tl2form->hide();

   cpform->show();
}

void MainDialog::dumpConfig(void) {

   sumdialog->show();
   sumdialog->raise();
   sumdialog->activateWindow();

   sumdialog->showVMEConfig();
}

void MainDialog::TL1Config(void) {

   logoform->hide();
   topoform->hide();
   dwiresetform->hide();
   dmonform->hide();
   omonform->hide();
   pmonform->hide();
   tl2form->hide();
   cpform->hide();

   tl1form->init();
   tl1form->show();
}

void MainDialog::TL2Config(void) {

   logoform->hide();
   topoform->hide();
   dwiresetform->hide();
   dmonform->hide();
   omonform->hide();
   pmonform->hide();
   tl1form->hide();
   cpform->hide();

   tl2form->init();
   tl2form->show();
}

void MainDialog::TL3Config(void) {

}

void MainDialog::help01(void) {

   helpform01->show();
}

void MainDialog::aboutQt(void) {

   QMessageBox::aboutQt(this);
}

void MainDialog::aboutGui(void) {

   QMessageBox::about(this, tr("About Exotic TSI GUI"),
               tr("<h2>Exotic Trigger Supervisor GUI 1.0</h2>"
                  "<p>TSI-GUI enable simple and effective "
                  "configuration of Exotic Trigger Supervisor Interface"));
}
