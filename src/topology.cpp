#include "topology.h"

TopologyForm::TopologyForm(QTopology &tpl, QWidget *parent) : QWidget(parent), topo(tpl) {

   setupUi(this);

   connect(newTelButton, SIGNAL(clicked()), this, SLOT(newTelescope()));
   connect(newTelButton, SIGNAL(clicked()), parent, SLOT(refreshMenubar()));
   connect(editTelButton, SIGNAL(clicked()), this, SLOT(editTelescope()));
   connect(delTelButton, SIGNAL(clicked()), this, SLOT(delTelescope()));
   connect(delTelButton, SIGNAL(clicked()), parent, SLOT(refreshMenubar()));
   connect(telComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(refreshDetList()));

   connect(newDetButton, SIGNAL(clicked()), this, SLOT(newDetector()));
   connect(editDetButton, SIGNAL(clicked()), this, SLOT(editDetector()));
   connect(delDetButton, SIGNAL(clicked()), this, SLOT(delDetector()));

}

void TopologyForm::init(void) {

   detdialog = new EditDetDialog(this);
   detdialog->init();

   // telescope section
   telComboBox->clear();
   newTelButton->setEnabled(true);
   editTelButton->setEnabled(false);
   delTelButton->setEnabled(false);

   // detector section
   detTable->clearContents();
   detTable->setRowCount(0);
   detTable->setEnabled(false);
   detTable->setColumnWidth(0, 140);
   detTable->setColumnWidth(1, 90);
   newDetButton->setEnabled(false);
   editDetButton->setEnabled(false);
   delDetButton->setEnabled(false);

   topo.Clear();
}

void TopologyForm::setMode(mode m) {

   dlgmode = m;

   if(m == MODE_SELECTION) {

      newTelButton->hide();
      editTelButton->hide();
      delTelButton->hide();

      newDetButton->hide();
      editDetButton->hide();
      delDetButton->hide();
   }

   if (m == MODE_EDIT) {

      newTelButton->show();
      editTelButton->show();
      delTelButton->show();

      newDetButton->show();
      editDetButton->show();
      delDetButton->show();

   }
}

void TopologyForm::newTelescope(void) {

   if(topo.sizeTelescopeList() == MAX_TELESCOPE_NUM) {

      QMessageBox::warning(this, tr("Message"), tr("Maximum number of telescope reached !"), QMessageBox::Ok);

   } else {

      bool ok;
      QString telname;
      QString text = QInputDialog::getText(this, tr("New telescope"),
                                           tr("Telescope logical name:"), QLineEdit::Normal,
                                           telname, &ok);
      if (ok && !text.isEmpty()) {

         if(!topo.lookupTelescopeName(text)) {

            topo.addTelescope(text);
            refreshTelList();
            refreshDetList();

         } else {

            QMessageBox::warning(this, tr("Message"), tr("Telescope name already present !"), QMessageBox::Ok);
         }
      }
   }
}

void TopologyForm::editTelescope(void) {

   bool ok;
   int currindex = telComboBox->currentIndex();
   QString telname = telComboBox->currentText();
   QString text = QInputDialog::getText(this, tr("New telescope"),
                                        tr("Telescope logical name:"), QLineEdit::Normal,
                                        telname, &ok);

   if (ok && !text.isEmpty()) {

      if(!topo.lookupTelescopeName(text)) {

         unsigned int telindex = telComboBox->itemData(telComboBox->currentIndex()).toInt();

         topo.setTelescopeName(telindex, text);

         refreshTelList();
         refreshDetList();

         telComboBox->setCurrentIndex(currindex);
      }

      else {

         QMessageBox::warning(this, tr("Message"), tr("Telescope name already present !"), QMessageBox::Ok);
      }
   }
}

void TopologyForm::delTelescope(void) {

   int r = QMessageBox::warning(this, tr("Message"),
                                tr("Telescope will be deleted !\nRelated monitoring lines will be freed !" "\nAre you sure ?"),
                                QMessageBox::Yes, QMessageBox::No);

   if(r == QMessageBox::Yes) {

      unsigned int telindex = telComboBox->itemData(telComboBox->currentIndex()).toInt();

      topo.delTelescope(telindex);;

      refreshTelList();
      refreshDetList();
   }
}

void TopologyForm::newDetector(void) {

   unsigned int telindex = telComboBox->itemData(telComboBox->currentIndex()).toInt();
   QString detname;

   detdialog->init();
   detdialog->setdetName(detname);

   QDialog::DialogCode ok = (QDialog::DialogCode) detdialog->exec();

   if ( ok && (detdialog->detName() != "") ) {

      if (!topo.lookupDetectorName(telindex, detdialog->detName())) {

         if(topo.detectorNumOf(telindex, detdialog->detType()) + 1 <= topo.getDetectorMaxAllowed(detdialog->detType())) {

            topo.addDetector(telindex, detdialog->detName(), detdialog->detType());
            refreshDetList();

         } else {

            QMessageBox::warning(this, tr("Message"), tr("Max multiplicity of this detector type reached !"), QMessageBox::Ok);
         }

      } else {

         QMessageBox::warning(this, tr("Message"), tr("Detector name already present !"), QMessageBox::Ok);
      }
   }
}

void TopologyForm::editDetector(void) {

   if(detTable->currentRow() != -1) {

      QMap <QString, QVariant> tmpmap = detTable->currentItem()->data(Qt::UserRole).toMap();
      unsigned int telindex = tmpmap["telindex"].toUInt();
      unsigned int detindex = tmpmap["detindex"].toUInt();
      QDetector det = topo.detector(telindex, detindex);

      QString detname = det.name();
      DetectorType dettype = det.type();

      detdialog->init();
      detdialog->setdetName(detname);
      detdialog->setdetType(dettype);

      detdialog->dtypeComboBox->setEnabled(false);
      QDialog::DialogCode ok = (QDialog::DialogCode) detdialog->exec();
      detdialog->dtypeComboBox->setEnabled(true);

      if ( ok && (detdialog->detName() != "")) {

         if (!topo.lookupDetectorName(telindex, detdialog->detName())) {

            topo.setDetectorName(telindex, detindex, detdialog->detName());
            refreshDetList();

         } else {

            QMessageBox::warning(this, tr("Message"), tr("Detector name already present !"), QMessageBox::Ok);
         }
      }
   }
}

void TopologyForm::delDetector(void) {

   if(detTable->currentRow() != -1) {

      int r = QMessageBox::warning(this, tr("Message"),
                                   tr("Detector will be deleted !\nRelated monitoring lines will be freed !" "\nAre you sure ?"),
                                   QMessageBox::Yes, QMessageBox::No);

      if(r == QMessageBox::Yes) {

         QMap <QString, QVariant> tmpmap = detTable->currentItem()->data(Qt::UserRole).toMap();
         unsigned int telindex = tmpmap["telindex"].toUInt();
         unsigned int detindex = tmpmap["detindex"].toUInt();

         topo.delDetector(telindex,detindex);
         refreshDetList();
      }
   }
}

void TopologyForm::refreshTelList() {

   telComboBox->clear();

   if(topo.sizeTelescopeList() == 0) {

      editTelButton->setEnabled(false);
      delTelButton->setEnabled(false);
      newDetButton->setEnabled(false);
      delDetButton->setEnabled(false);
      detTable->setEnabled(false);
   }

   else {

      QMap<unsigned int, QString> telmap = topo.telescopeMap();
      QMapIterator<unsigned int, QString> i(telmap);

      while (i.hasNext()) {
         i.next();
         telComboBox->addItem(i.value(), i.key());
      }

      telComboBox->setCurrentIndex( telComboBox->count() - 1 );

      editTelButton->setEnabled(true);
      delTelButton->setEnabled(true);
      newDetButton->setEnabled(true);
      detTable->setEnabled(true);
   }
}

void TopologyForm::refreshDetList() {

   unsigned int telindex = telComboBox->itemData(telComboBox->currentIndex()).toInt();

   detTable->clearContents();
   detTable->setRowCount(0);

   if(topo.sizeDetectorList(telindex) == 0) {

      editDetButton->setEnabled(false);
      delDetButton->setEnabled(false);
      detTable->setEnabled(false);

   } else {

      QList<QDetector> dlist = topo.detectorList(telindex);

      editDetButton->setEnabled(true);
      delDetButton->setEnabled(true);
      detTable->setEnabled(true);

      detTable->setRowCount(topo.sizeDetectorList(telindex));

      int row = 0;
      for(int i = (dlist.size()-1); i >= 0; i--) {

         QMap <QString, QVariant> tmpmap;
         tmpmap["telindex"] = telindex;
         tmpmap["detindex"] = dlist[i].detuid();

         QTableWidgetItem *dname = new QTableWidgetItem();
         QTableWidgetItem *dtype = new QTableWidgetItem();

         dname->setText(dlist[i].name());
         dname->setData(Qt::UserRole, tmpmap);
         dtype->setText(QDetector::type_str(dlist[i].type()));
         dtype->setData(Qt::UserRole, tmpmap);

         detTable->setItem(row, 0, dname);
         detTable->setItem(row, 1, dtype);

         row++;
      }
   }
}

void TopologyForm::refresh(void) {

   refreshTelList();
   refreshDetList();

   telComboBox->setCurrentIndex(0);
   detTable->setCurrentCell(0,0);
}
