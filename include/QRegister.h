#ifndef QREGISTER_H
#define QREGISTER_H

#include "QMetrics.h"

#include <stdint.h>
#include <QMap>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#define REGISTER_CONF_FILE    "conf/regparams.conf"

// ################################### Generic base-class Register ###################################

template <typename T>
class QRegister {

protected:

   QString prefix;
   QString name;
   uint32_t address;
   uint32_t value;

   QMap<T, uint16_t> mask;
   QMap<T, uint8_t> mask_width;
   QMap<T, uint8_t> mask_pos;

public:

   void init(TSIport p, QString pfx, QString type, uint8_t id);

   void clear(void);

   QString getname(void);
   uint32_t getaddr(void);
   uint32_t getvalue(void);
   void setvalue(uint32_t);
};

template <typename T>
void QRegister<T>::init(TSIport p, QString pfx, QString type = "", uint8_t id = 255) {

   uint16_t row = 0;

   QFile file(REGISTER_CONF_FILE);
   QTextStream in(&file);

   QString line;
   QStringList fields, tmplist;

   prefix = pfx;

   if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
      qDebug() << "ERROR opening file " << REGISTER_CONF_FILE;

   while (!in.atEnd()) {

      row++;
      line = in.readLine();
      fields = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

      if(fields.size() > 0) {

         if(fields[0].left(2) == "//")
            continue;

         if(fields.size() < 10) {
            qDebug() << "ERROR: line not complete at line: " << row << endl;
            continue;
         }

         // [0] name - [1] port num - [2] id num
         if( (fields[0] == prefix) && (QString(fields[1]).toInt() == p) && ( (QString(fields[2]).toInt() == id) || (id == 255)) ) {

            // [3] portname - [4] idname
            if( (id == 255) || (fields[4] == "-") )
               name = prefix + fields[3];
            else
               name = prefix + fields[3] + fields[4];   // regname + portname + idname

            bool ok;
            uint32_t tmpaddr;
            uint16_t tmpmask;
            uint16_t index;

            // [5] address
            tmpaddr = QString(fields[5]).toUInt(&ok, 16);
            if(!ok) {
               qDebug() << "ERROR: address not valid at line: " << row << endl;
            } else address = tmpaddr;

            // [7] mask
            tmpmask = QString(fields[7]).toUInt(&ok, 16);
            if(!ok) {

               qDebug() << "ERROR: mask not valid at line: " << row << endl;

            } else {

               if(type == "single") {

                  index = 0;
                  ok = true;

               } else {

                  if(pfx == "ControlWord" || pfx == "Multiplexer" || pfx == "Multiplexer19In" || \
                     pfx == "Multiplexer30Dl" || pfx == "Multiplexer8ptrg" || pfx == "Multiplexer16Dl" ) {

                     // [11] seqid
                     index = QString(fields[11]).toUInt(&ok);

                  } else {

                     // [6] line
                     index = QString(fields[6]).toUInt(&ok);
                  }
               }

               if (!ok) {
                  qDebug() << "ERROR: line not valid at line: " << row << endl;

               } else {

                  if(type != "") {

                     if(fields[10] == type) {

                        mask[(T)index] = tmpmask;
                        mask_width[(T)index] = QString(fields[8]).toUInt(); // [8] mask width
                        mask_pos[(T)index] = QString(fields[9]).toUInt();   // [9] mask position
                     }

                  } else {

                     mask[(T)index] = tmpmask;
                     mask_width[(T)index] = QString(fields[8]).toUInt(); // [8] mask width
                     mask_pos[(T)index] = QString(fields[9]).toUInt();   // [9] mask position
                  }
               }
            }
         }
      }	// end if
   }  // end while

   file.close();
   clear();
}

template <typename T>
void QRegister<T>::clear(void) {

   value = 0;
}

template <typename T>
QString QRegister<T>::getname(void) {

   return name;
}

template <typename T>
uint32_t QRegister<T>::getaddr(void) {

   return address;
}

template <typename T>
uint32_t QRegister<T>::getvalue(void) {

   return value;
}

template <typename T>
void QRegister<T>::setvalue(uint32_t v) {

   value = v;
}

// ################################### VME REGISTER ControlWord ###################################

class QRegTSIStatus : public QRegister<uint8_t> {

public:

   void set(enum TSIstatus s);
   enum TSIstatus get(void);
};

// ################################### VME REGISTER MascheraIngressi{Xa,Xb,Xc,X4} ###################################

class QRegTSIInput : public QRegister<TSIline> {

public:

   void set(TSIline l, enum LineEnable e);
};

// ################################### VME REGISTER AbilitazioneSimTrg{Xa,Xb,Xc,X4} ###################################

class QRegSimInput : public QRegister<TSIline> {

public:

   void set(TSIline l, enum LineEnable e);
};

// ################################### VME REGISTER GenSimTrigger{Xa,Xb,Xc,X4} ###################################

class QRegSimGen : public QRegister<TSIport> {

public:

   void set(void);
};

// ################################### VME REGISTER LogicaCoinc{Xa,Xb,Xc} ###################################

class QRegCoincLogic : public QRegister<TSIcoinc> {

public:
   
   void set(TSIcoinc c, enum CoincOp op);
};

// ################################### VME REGISTER ContatoreFreCoinc{Xa,Xb,Xc} ###################################

class QRegCoincFreq : public QRegister<uint8_t> {

public:

   void set(uint32_t val);
   uint32_t get(void);
};

// ################################### VME REGISTER ContatoreOverFlowFreCoinc{Xa,Xb,Xc} ###################################

class QRegCoincFreqOF : public QRegister<TSIcoinc> {

public:

   void set(TSIcoinc c, uint8_t val);
   uint8_t get(TSIcoinc c);
};

// ################################### VME REGISTER DivisoreFreCoinc{Xa,Xb,Xc} ###################################

class QRegCoincFreqdiv : public QRegister<TSIcoinc> {

public:
   
   void set(TSIcoinc c, enum CoincFreqDiv div);
   uint8_t get(TSIcoinc c);
   uint8_t parse(TSIcoinc c, uint16_t data);
};

// ################################### VME REGISTER DurataRitardoCoinc{Xa0...3, Xb0...3, Xc0...3} ###################################
// == W I D T H ==

class QRegCoincWidth : public QRegister<TSIcoinc> {

public:

   void set(TSIcoinc c, enum CoincPeriod w);
};

// ################################### VME REGISTER DurataRitardoCoinc{Xa0...3, Xb0...3, Xc0...3} ###################################
// == D E L A Y ==

class QRegCoincDelay : public QRegister<TSIcoinc> {

public:

   void set(TSIcoinc c, enum CoincDelay d);
};

// ################################### VME REGISTER DurataRitardo{X40...7} ###################################
// == W I D T H ==

class QRegLineWidth : public QRegister<TSIline> {

public:

   void set(TSIline l, enum CoincPeriod w);
};

// ################################### VME REGISTER DurataRitardo{X40...7} ###################################
// == D E L A Y ==

class QRegLineDelay : public QRegister<TSIline> {

public:

   void set(TSIline l, enum CoincDelay d);
};

// ################################### VME REGISTER EnableRitardoCoinc{Xa,Xb,Xc} ###################################

class QRegCoincEnabledelay : public QRegister<TSIcoinc> {

public:
   
   void set(TSIcoinc c, enum DelayEnable e);
};

// ################################### VME REGISTER EnableRitardo{X4} ###################################

class QRegLineEnabledelay : public QRegister<TSIline> {

public:

   void set(TSIline l, enum DelayEnable e);
};

// #################### VME REGISTER Multiplexer{XaUno....Quattro, XbUno...Quattro, XcUno...Quattro, X4Uno...Tre, Xf} ####################

class QRegMultiplexer : public QRegister<uint8_t> {

public:

   void set(uint8_t index);   // copy mask[index] to value
};

// ################################### VME REGISTER Multiplexer19In{X40...9} ###################################

class QRegTL1SigIn : public QRegister<uint8_t> {

public:

   void set(TL1SignalIn line, uint8_t input);      // line {X4-TRG0...TRGXc} - input {0...18}
};

// ################################### VME REGISTER Multiplexer30Dl{X40...3} ###################################

class QRegTL1Logic : public QRegister<uint8_t> {

public:

   void set(TL1SignalOut line, uint8_t logic);      // line {PTRG0...PTRG7} - input {0...31}
};

// ################################### VME REGISTER Multiplexer8ptrg{Xf0...3} ###################################

class QRegTL2SigIn : public QRegister<uint8_t> {

public:

   void set(TL2SignalIn line, uint8_t input);      // line {PTRG0...PTRG7} - input {0...7}
};

// ################################### VME REGISTER Multiplexer16Dl{Xf0...1} ###################################

class QRegTL2Logic : public QRegister<uint8_t> {

public:

   void set(TL2SignalOut line, uint8_t logic);      // line {PTRIN0...PTRIN3} - input {0...3}
};

// ################################### VME REGISTER read only generic for TSI statistics ###################################

class QRegTSIStats : public QRegister<uint8_t> {

};

#endif // QREGISTER_H
