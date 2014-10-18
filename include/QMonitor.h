#ifndef QMONITOR_H
#define QMONITOR_H

#include <QMultiHash>
#include <QFile>
#include <QTextStream> 
#include <QStringList>
#include <QtAlgorithms>
#include <QDebug>

#include <stdint.h>
#include "QMetrics.h"

#define REGISTER_CONF_FILE    "conf/regparams.conf"

struct QMonitorParamInfo {

   TSIport port;
   TSImon mline;
   QString portname;
   QString idname;

   uint32_t address;
   uint16_t seqid;
   uint16_t tag;

   TSIline bindline;
   int16_t group;
   QString desc;
  
   void init(void) {

      port = UNDEF_PORT;
      mline = UNDEF_MON;
      portname = idname = "";
     
      address = tag = 0;
      bindline = UNDEF_LINE;
      desc = "";
   }
};

class QMonitorParamSet {

private:

   bool _init;

   QMultiHash<TSIport, QMonitorParamInfo> mset_byport;		// key: TSIport  / value: QMonitorLineInfo

public:

   QMonitorParamSet(void);

   QMonitorParamInfo undef_mpinfo;

   void init(void);

   QList<QMonitorParamInfo> monparamList(TSIport p);
   QList<QMonitorParamInfo> monparamList(TSIport p, TSImon ml);
   QMonitorParamInfo monparam(TSIport p, TSImon ml, TSIline l, int16_t group);
   QMonitorParamInfo monparam(TSIport p, TSImon ml, uint16_t seqid);
   QList<int16_t> monlineList(TSIport p, TSIline l, int16_t group);
};

#endif
