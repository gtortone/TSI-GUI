#include <QList>
#include <QMessageBox>

#include "detwireset.h"

DetWireSetForm::DetWireSetForm(QTopology &tpl, QWidget *parent) : QWidget(parent), topo(tpl) {

   setupUi(this);

   connect(editWiringButton, SIGNAL(pressed()), this, SLOT(setWireEditMode()));
   connect(editSetupButton, SIGNAL(pressed()), this, SLOT(setSetupEditMode()));
   connect(okWiringButton, SIGNAL(pressed()), this, SLOT(confirmWireChanges()));
   connect(okSetupButton, SIGNAL(pressed()), this, SLOT(confirmSetupChanges()));
   connect(cancelWiringButton, SIGNAL(pressed()), this, SLOT(setViewMode()));
   connect(cancelSetupButton, SIGNAL(pressed()), this, SLOT(setViewMode()));
   connect(portComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(refreshLineList()));
   connect(delaySlider, SIGNAL(valueChanged(int)), this, SLOT(updateDelayVal(int)));
   connect(periodSlider, SIGNAL(valueChanged(int)), this, SLOT(updatePeriodVal(int)));
}

void DetWireSetForm::init(void) {

   helpLabel->show();

   editWiringButton->hide();
   editSetupButton->hide();
   descBox->hide();
   wiringBox->hide();
   setupBox->hide();
   okWiringButton->hide();;
   cancelWiringButton->hide();
   okSetupButton->hide();
   cancelSetupButton->hide();
   wiringStatusIcon->hide();
   statusLabel->hide();
   statusText->hide();
}

void DetWireSetForm::setViewMode(void) {

   QMap <QString, QVariant> tmpmap = curritem->data(Qt::UserRole).toMap();
   unsigned int telindex = tmpmap["telindex"].toUInt();
   unsigned int detindex = tmpmap["detindex"].toUInt();
   QDetector det = topo.detector(telindex, detindex);

   helpLabel->hide();
   descBox->show();
   wiringBox->show();
   setupBox->show();

   editWiringButton->setEnabled(true);
   editSetupButton->setEnabled(true);
   editWiringButton->show();
   editSetupButton->show();
   okWiringButton->hide();;
   cancelWiringButton->hide();
   okSetupButton->hide();
   cancelSetupButton->hide();

   portComboBox->hide();
   lineComboBox->hide();

   enableBox->hide();
   coincLogicBox->hide();
   delaySlider->hide();
   periodSlider->hide();
   faceBox->hide();

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

   if(det.type() == DSSD) {

      logicLabel->show();
      coincLogicLabel->show();
      faceLabel->show();
      faceValue->show();

   } else {

      logicLabel->hide();
      coincLogicLabel->hide();
      faceLabel->hide();
      faceValue->hide();
   }

   if(det.coincperiod() == PER0) {

      setupStatusIcon->setPixmap(QPixmap(QString::fromUtf8(":/icon/icons/warning-small.png")));

   } else {

      setupStatusIcon->setPixmap(QPixmap(QString::fromUtf8(":/icon/icons/allocated-small.png")));
   }

   setupStatusIcon->show();

   if(det.type() == DSSD) {

      coincLogicLabel->setText( CoincOp_str[det.coincop()] );
      faceValue->setText(DetectorFace_str[det.activefaces()] );
   }

   enableLabel->setText( DetEnabled_str[det.enabled()] );
   delayValue->setText( CoincDelay_strval[det.coincdelay()] );
   periodValue->setText( CoincPeriod_strval[det.coincperiod()] );

   refreshStatus();
}

void DetWireSetForm::setWireEditMode(void) {


   okWiringButton->show();
   cancelWiringButton->show();
   editWiringButton->setEnabled(false);
   editSetupButton->setEnabled(false);
   portComboBox->show();
   lineComboBox->show();

   refreshPortList();
}

void DetWireSetForm::setSetupEditMode(void) {

   QMap <QString, QVariant> tmpmap = curritem->data(Qt::UserRole).toMap();
   unsigned int telindex = tmpmap["telindex"].toUInt();
   unsigned int detindex = tmpmap["detindex"].toUInt();
   QDetector det = topo.detector(telindex, detindex);

   okSetupButton->show();
   cancelSetupButton->show();
   editWiringButton->setEnabled(false);
   editSetupButton->setEnabled(false);

   enableBox->setCurrentIndex( (int)(det.enabled()));
   coincLogicBox->setCurrentIndex( (int)(det.coincop()) );
   delaySlider->setValue( (int)(det.coincdelay()) );
   periodSlider->setValue( (int)(det.coincperiod()) );
   faceBox->setCurrentIndex( (int) (det.activefaces()) );

   enableBox->show();

   if(det.type() == DSSD) {

      coincLogicBox->show();
      faceBox->show();

   } else {

      coincLogicBox->hide();
      faceBox->hide();
   }

   delaySlider->show();
   periodSlider->show();
}

void DetWireSetForm::confirmWireChanges(void) {

   QMap <QString, QVariant> tmpmap = curritem->data(Qt::UserRole).toMap();
   unsigned int telindex = tmpmap["telindex"].toUInt();
   unsigned int detindex = tmpmap["detindex"].toUInt();
   QDetector det = topo.detector(telindex, detindex);
   int r = QMessageBox::Yes;

   if(det.allocated()) {
      r = QMessageBox::warning(this, tr("Message"),
                               tr("Replace previous wiring with new one !\nRelated monitoring lines will be freed !" "\nAre you sure ?"),
                               QMessageBox::Yes, QMessageBox::No);
   }

   if(r == QMessageBox::Yes) {

      TSIport p = (TSIport) portComboBox->itemData(portComboBox->currentIndex()).toInt();
      TSIline l = (TSIline) lineComboBox->itemData(lineComboBox->currentIndex()).toInt();

      topo.setDetectorWire(telindex, detindex, p, l);
   }

   setViewMode();
}

void DetWireSetForm::confirmSetupChanges(void) {

   QMap <QString, QVariant> tmpmap = curritem->data(Qt::UserRole).toMap();
   unsigned int telindex = tmpmap["telindex"].toUInt();
   unsigned int detindex = tmpmap["detindex"].toUInt();

   if(topo.getDetectorType(telindex, detindex) == DSSD) {

      topo.setDetectorCoincOp(telindex, detindex, (CoincOp)(coincLogicBox->currentIndex()));
      topo.setDetectorActiveFaces(telindex, detindex, (DetectorFace)(faceBox->currentIndex()));
   }

   topo.setDetectorEnabled(telindex, detindex, (bool)(enableBox->currentIndex()));
   topo.setDetectorCoincDelay(telindex, detindex, (CoincDelay)(delaySlider->value()));
   topo.setDetectorCoincPeriod(telindex, detindex, (CoincPeriod)(periodSlider->value()));

   setViewMode();
}

void DetWireSetForm::refreshPortList(void) {

   QMap <QString, QVariant> tmpmap = curritem->data(Qt::UserRole).toMap();
   unsigned int telindex = tmpmap["telindex"].toUInt();
   unsigned int detindex = tmpmap["detindex"].toUInt();
   QDetector det = topo.detector(telindex, detindex);
   QList<TSIport> freep;

   portComboBox->clear();

   // create list of free ports
   freep = topo.freePorts();
   for(int i=0; i < freep.size(); i++) {

      if((det.type() == DSSD) && freep[i] == X4)
         continue;

      if((det.type() != DSSD) && freep[i] != X4)
         continue;

      portComboBox->addItem(TSIport_str[freep[i]], (TSIport)freep[i]);
   }
}

void DetWireSetForm::refreshLineList(void) {

   QMap <QString, QVariant> tmpmap = curritem->data(Qt::UserRole).toMap();
   unsigned int telindex = tmpmap["telindex"].toUInt();
   unsigned int detindex = tmpmap["detindex"].toUInt();
   QDetector det = topo.detector(telindex, detindex);
   TSIport p = (TSIport) portComboBox->itemData(portComboBox->currentIndex()).toInt();
   QList<TSIline> freel;

   lineComboBox->clear();

   // create list of free lines
   freel = topo.freeInputLines(p, det.type());
   for(int i=0; i < freel.size(); i++) {

      lineComboBox->addItem(TSIline_str[freel[i]], (TSIline)freel[i]);
   }
}

void DetWireSetForm::showDetector(QTableWidgetItem *item) {

   curritem = item;

   QMap <QString, QVariant> tmpmap = curritem->data(Qt::UserRole).toMap();
   unsigned int telindex = tmpmap["telindex"].toUInt();
   unsigned int detindex = tmpmap["detindex"].toUInt();
   QDetector det = topo.detector(telindex, detindex);

   // detector header info
   telNameLabel->setText(topo.telescopeName(telindex));
   detNameLabel->setText(det.name());
   detTypeLabel->setText(QDetector::type_str(det.type()));

   setViewMode();
}

void DetWireSetForm::updateDelayVal(int val) {

   delayValue->setText( CoincDelay_strval[val] );
}

void DetWireSetForm::updatePeriodVal(int val) {

   periodValue->setText( CoincPeriod_strval[val] );
}

void DetWireSetForm::refreshStatus(void) {

   QMap <QString, QVariant> tmpmap = curritem->data(Qt::UserRole).toMap();
   unsigned int telindex = tmpmap["telindex"].toUInt();
   unsigned int detindex = tmpmap["detindex"].toUInt();
   QDetector det = topo.detector(telindex, detindex);
   QString str;
   bool ok = true;

   statusLabel->show();
   statusText->show();

   str = "<ul type=\"square\">";

   if(!det.allocated()) {
      str += "<li><font color=\"red\"><b>Wiring warning</b> - detector input line is not specified and it will not included in trigger logic dialogs</font><li>";
      ok = false;
   }

   if(det.coincperiod() == PER0) {
      str += "<li><font color=\"red\"><b>Setup warning</b> - signal width equal to zero means that input is disabled for this detector<li>";
      ok = false;
   }

   if(det.enabled() == false) {
      str += "<li><font color=\"red\"><b>Setup warning</b> - detector will not be available in trigger logic dialogs<li>";
      ok = false;
   }

   if(ok)
      str += "<li><font color=\"green\"><b>Configuration of wiring and setup is correct and detector will be available in trigger logic dialogs !</b></font><li>";

   if( (det.type() == DSSD) && (det.activefaces() != FACE_BOTH) )
      str += "<li><font color=\"blue\"><b>note: DSSD has one face enabled</b></font><li>";

   str += "</ul>";

   statusText->setHtml(str);
}
