#include "QTL2logic.h"

#include <QVector>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDebug>

// private methods
void QTL2logic::fillcol(uint8_t lineidx, enum TL2SignalIn sig) {

   for(int i=0; i<TL2GATE_NUM; i++) {

      if(SigInMatrix[i][lineidx] != UNDEF_TL2IN)
         SigInMatrix[i][lineidx] = sig;
   }
}

// public methods
void QTL2logic::init(void) {

   uint16_t row, gateidx;
   QVector<uint8_t> vect_in, vect_pt;

   TL2Gate.resize(TL2GATE_NUM);

   QFile file("conf/TL2params.conf");
   QTextStream in(&file);

   QString line;
   QStringList fields, tmplist;

   if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
      qDebug() << "ERROR opening file !";

   row = gateidx = 0;
   while (!in.atEnd()) {

      row++;
      line = in.readLine();
      fields = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

      if(fields.size() > 0) {

         if(fields[0].left(2) == "//")
            continue;

         if(fields.size() < 3) {
            qDebug() << "ERROR: line not complete at line: " << row << endl;
            continue;
         }

         QGate g;
         QString desc, shortdesc;

         // [0] seqid (confindex)
         g.setConfindex(fields[0].toInt());;

         // [1] operator
         if(fields[1] == "AND") {

            g.setType(AND_G);
            desc += "AND gate - ";
            shortdesc += "AND ";

         } else if(fields[1] == "OR") {

            g.setType(OR_G);
            desc += "OR gate - ";
            shortdesc += "OR ";

         } else {
            qDebug() << "ERROR: gate type not valid at line: " << row << endl;
            continue;
         }

         // [2] input
         tmplist = QString(fields[2]).split(",");
         vect_in.clear();
         for(int i=0; i<tmplist.size(); i++)
            vect_in.push_back(tmplist[i].toInt());
         g.setLineInIndex(vect_in);

         desc += QString::number(tmplist.size()) + "xIN";
         shortdesc += QString::number(tmplist.size()) + "IN";

         // set description
         g.setDesc(desc);
         g.setShortDesc(shortdesc);

         TL2Gate[gateidx++].copy(g);

      }	// end if
   }  // end while

   // init data structures
   clear();
}

void QTL2logic::clear(void) {

   // init TL2 SigIn matrix
   for(int i=0; i<TL2GATE_NUM; i++) {

      QGate g = getGate(i);
      QVector<uint8_t> avb_input = g.getLineInIndex();

      for(int j=0; j<TL2SIGIN_NUM; j++) {

         if(avb_input.contains(j))
            SigInMatrix[i][j] = AVAIL_TL2IN;
         else
            SigInMatrix[i][j] = UNDEF_TL2IN;
      }
   }

   // init TL2 SigOut vector
   for(int i=0; i<TL2SIGOUT_NUM; i++)
      SigOutVector[i] = AVAIL_TL2OUT;
}

QGate QTL2logic::getGate(uint8_t gateidx) {

   return TL2Gate[gateidx];
}

QStringList QTL2logic::getGateDescList(void) {

   QList<QString> tmplist;

   for(int i=0; i<TL2Gate.size(); i++)
      tmplist << TL2Gate[i].getDesc();

   return(tmplist);
}

QStringList QTL2logic::getGateShortDescList(void) {

   QList<QString> tmplist;

   for(int i=0; i<TL2Gate.size(); i++)
      tmplist << TL2Gate[i].getShortDesc();

   return(tmplist);
}

QString QTL2logic::getGateDesc(uint8_t gateidx) {

   return(TL2Gate[gateidx].getDesc());
}

QString QTL2logic::getGateShortDesc(uint8_t gateidx) {

   return(TL2Gate[gateidx].getShortDesc());
}

enum TL2SignalIn QTL2logic::getInput(uint8_t gateidx, uint8_t lineidx) {

   return(SigInMatrix[gateidx][lineidx]);
}

void QTL2logic::assignInput(uint8_t gateidx, uint8_t lineidx, enum TL2SignalIn sig) {

   if(SigInMatrix[gateidx][lineidx] != UNDEF_TL2IN)
      SigInMatrix[gateidx][lineidx] = sig;

   fillcol(lineidx, sig);
}

void QTL2logic::unassignInput(uint8_t gateidx, uint8_t lineidx) {

   assignInput(gateidx, lineidx, AVAIL_TL2IN);
}

bool QTL2logic::allocatedGate(uint8_t gateidx) {

   QVector<uint8_t> tmpvect = TL2Gate[gateidx].getLineInIndex();

   for(int i=0; i<tmpvect.size(); i++) {

      if(SigInMatrix[gateidx][tmpvect[i]] == AVAIL_TL2IN)
         return false;
   }

   return true;
}

enum TL2SignalOut QTL2logic::getOutput(uint8_t gateidx) {

   for(int i=0; i<TL2SIGOUT_NUM; i++) {

      if(SigOutVector[i] == gateidx)
         return((enum TL2SignalOut)i);
   }

   return(AVAIL_TL2OUT);
}

void QTL2logic::assignOutput(uint8_t gateidx, enum TL2SignalOut sig) {

   SigOutVector[sig] = gateidx;
}

void QTL2logic::unassignOutput(enum TL2SignalOut sig) {

   SigOutVector[sig] = AVAIL_TL2OUT;
}

bool QTL2logic::allocatedOutput(enum TL2SignalOut sig) {

   return(SigOutVector[sig] != AVAIL_TL2OUT);
}

uint8_t QTL2logic::getAssignedGate(enum TL2SignalOut sig) {

   return(SigOutVector[sig]);
}

QVector<uint8_t> QTL2logic::getGateInputIndex(uint8_t gateidx) {

   return TL2Gate[gateidx].getLineInIndex();
}

