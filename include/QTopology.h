#ifndef QTOPOLOGY_H
#define QTOPOLOGY_H

#include <stdint.h>
#include <QVector>
#include <QMap>
#include <QList>
#include <QString>
#include <QStringList>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>

#include "QMetrics.h"
#include "QMonitor.h"
#include "QRegisterSet.h"
#include "QTL1logic.h"
#include "QTL2logic.h"
#include "v2n-client.h"

#define MAX_TELESCOPE_NUM	8

class TopologyException {

public:

   enum Type { TELINDEX_OUT_OF_BOUND = 1, DETINDEX_OUT_OF_BOUND, TELESCOPE_SET_FULL };

   QStringList type_str;
   Type _type;

   TopologyException(Type type) {

      type_str = (QStringList() << "" << "index out of bound" << "telescope set full");
      _type = type;
   } 

   std::string what(void) { return type_str[_type].toStdString(); };
};

class QDetector {

public:

   QDetector();
   QDetector(unsigned int detuid, QString str, DetectorType t);
   void init(void);

   bool allocated(void) const;
   bool enabled(void) const;

   // get methods
   unsigned int detuid(void) const;
   QString name(void) const;
   DetectorType type(void) const;
   TSIport port(void) const;
   TSIline line(void) const;
   TSIcoinc coinc(void) const;
   CoincOp coincop(void) const;
   CoincFreqDiv coincfreqdiv(void) const;
   CoincDelay coincdelay(void) const;
   CoincPeriod coincperiod(void) const;
   DetectorFace activefaces(void) const;

   // set methods
   void setName(QString str);
   void setAllocated(bool val);
   void setEnabled(bool val);
   void setPort(TSIport p);
   void setLine(TSIline l);
   void setCoincOp(CoincOp op);
   void setCoincFreqDiv(CoincFreqDiv div);
   void setCoincDelay(CoincDelay del);
   void setCoincPeriod(CoincPeriod per);
   void setActiveFaces(DetectorFace f);

   // utility methods
   static QString type_str(DetectorType t);
   static QString port_str(TSIport p);
   static QString line_str(TSIline l);
   static QString mon_str(TSImon m);
   static QString coinc_str(TSIcoinc c);
   static QString coincop_str(CoincOp op);
   static QString coincfreqdiv_str(CoincFreqDiv div);
   static QString coindelay_str(CoincDelay del);
   static QString coincperiod_str(CoincPeriod per);

   bool operator==(const QDetector &other) const {
      return( (_name == other._name) && (_type == other._type) );
   }

private:

   unsigned int _detuid;

   QString _name;
   DetectorType _type;

   TSIport _port;
   TSIline _line;
   TSIcoinc _coinc;

   CoincOp _coincop;
   CoincFreqDiv _coincfreqdiv;
   CoincPeriod _coincperiod;
   CoincDelay _coincdelay; 

   DetectorFace _activeface;
   bool _allocated;
   bool _enabled;
};

class QTopology {

private:

   QMap<unsigned int, QString> descTel;         // KEY: teluid - VALUE: telescope name
   QMultiMap<unsigned int, QDetector> hashDet;  // KEY: teluid - VALUE: <QDetector>

   unsigned int teluid;
   unsigned int detuid;
   unsigned int entrynum;

   QMap<TSIline, bool> avbInput[TSIPORT_NUM-1];    // Xa, Xb, Xc, X4
   QMap<TSImon, bool>  avbOutput[TSIPORT_NUM];     // Xa, Xb, Xc, X4, XF  (monitoring lines)
   QList<QMonitorParamInfo> monLines;

   QMap<QPair<TSIport, TSIline>, QPair<unsigned int, unsigned int> > bindLineDetector;


   void setInputBusy(TSIport p, TSIline l);
   void setInputFree(TSIport p, TSIline l);
   void setMonitorBusy(TSIport p, TSImon m);
   void setMonitorFree(TSIport p, TSImon m);

public:

   QTopology(void);
   void Init(void);
   void Clear(void);
   bool empty(void);

   QList<TSIport> freePorts(void);
   QList<TSIport> busyPorts(void);

   QList<TSIline> freeInputLines(TSIport p, DetectorType dtype);
   QList<TSIline> busyInputLines(TSIport p);

   QMonitorParamSet mpset;

   QTL1logic TL1logic;
   QTL2logic TL2logic;

   QList<TSImon> freeMonitorLines(TSIport p);
   QList<TSImon> busyMonitorLines(TSIport p);
   bool MonitorBusy(TSIport p, TSImon m);
   bool MonitorFree(TSIport p, TSImon m);

   QRegisterSet tsiregset;
   V2NClient VMEbus;

   // Telescope handling   
   
   unsigned int addTelescope(QString name);
   void delTelescope(unsigned int index);
   
   void setTelescopeName(unsigned int index, QString str);
   QString telescopeName(unsigned int index);
   bool lookupTelescopeName(QString str);

   QMap<unsigned int, QString> telescopeMap(void);
   unsigned int sizeTelescopeList(void);
   
   // Detector lifecycle handling

   unsigned int addDetector(unsigned int telindex, QString str, DetectorType type);
   void setDetectorName(unsigned int telindex, unsigned int detindex, QString str);
   bool lookupDetectorName(unsigned int telindex, QString str);
   void delDetector(unsigned int telindex, unsigned int detindex);
   DetectorType getDetectorType(unsigned int telindex, unsigned int detindex);

   // Detector properties handling

   // setters
   void setDetectorWire(unsigned int telindex, unsigned int detindex, TSIport p, TSIline l);
   void setDetectorEnabled(unsigned int telindex, unsigned int detindex, bool val);
   void setDetectorCoincOp(unsigned int telindex, unsigned int detindex, CoincOp op);
   void setDetectorCoincFreqDiv(unsigned int telindex, unsigned int detindex, CoincFreqDiv div);
   void setDetectorCoincDelay(unsigned int telindex, unsigned int detindex, CoincDelay del);
   void setDetectorCoincPeriod(unsigned int telindex, unsigned int detindex, CoincPeriod per);
   void setDetectorActiveFaces(unsigned int telindex, unsigned int detindex, DetectorFace f);

   unsigned int sizeDetectorList(unsigned int telindex);
   QDetector detector(unsigned int telindex, unsigned int detindex);
   QList<QDetector> detectorList(unsigned int telindex);
   QList<QDetector> detectorListOf(unsigned int telindex, DetectorType type);
   unsigned int detectorNumOf(unsigned int telindex, DetectorType type);
   unsigned int getDetectorMaxAllowed(DetectorType type);

   // Detector topology info handling

   void addDetectorTopoInfo(TSIport p, TSIline l, unsigned int telindex, unsigned int detindex);
   void delDetectorTopoInfo(TSIport p, TSIline l);
   QPair<unsigned int, unsigned int> detectorTopoInfo(TSIport p, TSIline l);

   // Monitor lines handling

   void addMonline(TSIport p, TSImon ml, TSIline l, int16_t group);
   void addMonline(TSIport p, TSImon ml, uint16_t seqid);
   void delMonline(TSIport p, TSImon ml, TSIline l, int16_t group);
   void delMonline(TSIport p, TSImon ml);
   void delMonlines(TSIport p, TSIline l);
   TSImon monline(TSIport p, TSIline l, int16_t group);
   QList<TSImon> monlineList(TSIport p, TSIline l);
   QList<QMonitorParamInfo> monparamList(TSIport p);

   // file Serialization and De-Serialization
   QString serializeToXML(void);
   QPair<QXmlStreamReader::Error, QString> deserializeFromXML(QString xmlcontent);

   // VME register config dump
   QStringList dumpVMEConfig(bool zerosupp);
   bool sendVMEConfig(void);
   bool verifyVMEConfig(void);
};

#endif
