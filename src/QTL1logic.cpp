#include "QTL1logic.h"

#include <QVector>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDebug>

// private methods
void QTL1logic::fillcol(uint8_t lineidx, enum TL1SignalIn sig) {

   for(int i=0; i<TL1GATE_NUM; i++) {

      if(SigInMatrix[i][lineidx] != UNDEF_TL1IN)
         SigInMatrix[i][lineidx] = sig;
   }
}

// public methods
void QTL1logic::init(void) {

   uint16_t row, gateidx;
   QVector<uint8_t> vect_in, vect_pt;

   TL1Gate.resize(TL1GATE_NUM);

   QFile file("conf/TL1params.conf");
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

         if(fields.size() < 4) {
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

         // [3] passthrough
         tmplist = QString(fields[3]).split(",");
         vect_pt.clear();
         for(int i=0; i<tmplist.size(); i++)
            vect_pt.push_back(tmplist[i].toInt());
         g.setLinePtIndex(vect_pt);

         if(tmplist.size() > 0) {

            desc += ", ";
            desc += QString::number(tmplist.size()) + "xPT";
         }

         // set description
         g.setDesc(desc);
         g.setShortDesc(shortdesc);

         TL1Gate[gateidx++].copy(g);

      }	// end if
   }  // end while

   // init data structures
   clear();

}

void QTL1logic::clear(void) {

   // init TL1 SigIn matrix
   for(int i=0; i<TL1GATE_NUM; i++) {

      QGate g = getGate(i);
      QVector<uint8_t> avb_input = g.getLineInIndex();

      for(int j=0; j<TL1SIGIN_NUM; j++) {

         if(avb_input.contains(j))
            SigInMatrix[i][j] = AVAIL_TL1IN;
         else
            SigInMatrix[i][j] = UNDEF_TL1IN;
      }
   }

   // init TL1 SigOut vector
   for(int i=0; i<TL1SIGOUT_NUM; i++)
      SigOutVector[i] = AVAIL_TL1OUT;
}

QGate QTL1logic::getGate(uint8_t gateidx) {

   return TL1Gate[gateidx];
}

QStringList QTL1logic::getGateDescList(void) {

   QList<QString> tmplist;

   for(int i=0; i<TL1Gate.size(); i++)
      tmplist << TL1Gate[i].getDesc();

   return(tmplist);
}

QStringList QTL1logic::getGateShortDescList(void) {

   QList<QString> tmplist;

   for(int i=0; i<TL1Gate.size(); i++)
      tmplist << TL1Gate[i].getShortDesc();

   return(tmplist);
}

QString QTL1logic::getGateDesc(uint8_t gateidx) {

   return(TL1Gate[gateidx].getDesc());
}

QString QTL1logic::getGateShortDesc(uint8_t gateidx) {

   return(TL1Gate[gateidx].getShortDesc());
}

enum TL1SignalIn QTL1logic::getInput(uint8_t gateidx, uint8_t lineidx) {

   return(SigInMatrix[gateidx][lineidx]);
}

void QTL1logic::assignInput(uint8_t gateidx, uint8_t lineidx, enum TL1SignalIn sig) {

   if(SigInMatrix[gateidx][lineidx] != UNDEF_TL1IN)
      SigInMatrix[gateidx][lineidx] = sig;

   fillcol(lineidx, sig);
}

void QTL1logic::unassignInput(uint8_t gateidx, uint8_t lineidx) {

   assignInput(gateidx, lineidx, AVAIL_TL1IN);
}

bool QTL1logic::allocatedGate(uint8_t gateidx) {

   QVector<uint8_t> tmpvect = TL1Gate[gateidx].getLineInIndex();

   for(int i=0; i<tmpvect.size(); i++) {

      if(SigInMatrix[gateidx][tmpvect[i]] == AVAIL_TL1IN)
         return false;
   }

   return true;
}

enum TL1SignalOut QTL1logic::getOutput(uint8_t gateidx) {

   for(int i=0; i<TL1SIGOUT_NUM; i++) {

      if(SigOutVector[i] == gateidx)
         return((enum TL1SignalOut)i);
   }

   return(AVAIL_TL1OUT);
}

void QTL1logic::assignOutput(uint8_t gateidx, enum TL1SignalOut sig) {

   SigOutVector[sig] = gateidx;
}

void QTL1logic::unassignOutput(enum TL1SignalOut sig) {

   SigOutVector[sig] = AVAIL_TL1OUT;
}

bool QTL1logic::allocatedOutput(enum TL1SignalOut sig) {

   return(SigOutVector[sig] != AVAIL_TL1OUT);
}

uint8_t QTL1logic::getAssignedGate(enum TL1SignalOut sig) {

   return(SigOutVector[sig]);
}

QVector<uint8_t> QTL1logic::getGateInputIndex(uint8_t gateidx) {

   return TL1Gate[gateidx].getLineInIndex();
}

QVector<uint8_t> QTL1logic::getGatePtIndex(uint8_t gateidx) {

   return TL1Gate[gateidx].getLinePtIndex();
}
