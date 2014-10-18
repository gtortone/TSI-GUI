#include "editdetdialog.h"

EditDetDialog::EditDetDialog(QWidget *parent) : QDialog(parent) {

   setupUi(this);

   init();

   connect(buttonBox, SIGNAL(accepted()), this, SLOT(updateValues()));
}

void EditDetDialog::init(void) {

   detname = "";
   dettype = DSSD;

   dnameLineEdit->setText(detname);
   dtypeComboBox->setCurrentIndex(0);

   dnameLineEdit->setFocus();
}

QString EditDetDialog::detName(void) {

   return detname;
}

DetectorType EditDetDialog::detType(void) {

   return dettype;
}

void EditDetDialog::setdetName(QString str) {

   detname = str;
   dnameLineEdit->setText(str);
}

void EditDetDialog::setdetType(DetectorType type) {

   dettype = type;
   dtypeComboBox->setCurrentIndex(type);
}

void EditDetDialog::updateValues(void) {

   detname = dnameLineEdit->text();
   dettype = (DetectorType) dtypeComboBox->currentIndex();
}
