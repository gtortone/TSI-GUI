#ifndef QREGISTERSET_H
#define QREGISTERSET_H

#include "QRegister.h"
#include "QMetrics.h"
#include <QMap>

class QRegisterSet {

private:

   uint32_t baddr;   // TSI base address

   QRegTSIStatus status;
   QRegTSIInput input[TSIPORT_NUM-1];                          // Xa, Xb, Xc, X4
   QRegSimInput siminput[TSIPORT_NUM-1];                       // Xa, Xb, Xc, X4
   QRegSimGen simgen[TSIPORT_NUM-1];                           // Xa, Xb, Xc, X4
   QRegCoincLogic coinclogic[TSIPORT_NUM-2];                   // Xa, Xb, Xc
   QRegCoincFreq coincfreq[TSIPORT_NUM-2][PORTCOINC_NUM];      // Xa, Xb, Xc
   QRegCoincFreqOF coincfreqof[TSIPORT_NUM-2][2];              // Xa, Xb, Xc
   QRegCoincFreqdiv coincfreqdiv[TSIPORT_NUM-2];               // Xa, Xb, Xc
   QRegCoincWidth coincwidth[TSIPORT_NUM-2][TSIPORTID_NUM];    // Xa, Xb, Xc
   QRegCoincDelay coincdelay[TSIPORT_NUM-2][TSIPORTID_NUM];    // Xa, Xb, Xc
   QRegLineWidth linewidth[PORTINLINE_NUM / 2];                // X4
   QRegLineDelay linedelay[PORTINLINE_NUM / 2];                // X4
   QRegCoincEnabledelay coincenabledelay[TSIPORT_NUM-2];       // Xa, Xb, Xc
   QRegLineEnabledelay lineenabledelay;
   QRegTL1SigIn tl1sigin[TL1_LINE_IDNUM];
   QRegTL1Logic tl1logic[TL1_LOGIC_IDNUM];
   QRegTL2SigIn tl2sigin[TL2_LINE_IDNUM];
   QRegTL2Logic tl2logic[TL2_LOGIC_IDNUM];
   QRegTSIStats trigtot;
   QRegTSIStats trigacqtot;
   QRegTSIStats deadtime;
   QRegTSIStats trigrate;
   QRegTSIStats uptime;

   QRegMultiplexer muxXA[MUX_XA_IDNUM];
   QRegMultiplexer muxXB[MUX_XB_IDNUM];
   QRegMultiplexer muxXC[MUX_XC_IDNUM];
   QRegMultiplexer muxX4[MUX_X4_IDNUM];
   QRegMultiplexer muxXF;

public:

   void init(void);
   void clearall(void);

   void setbaddr(uint32_t value);
   uint32_t getbaddr(void);

   void set(TSIstatus s);
   void set(TSIport p, TSIline l, enum LineEnable e);
   void setsim(TSIport p, TSIline l, enum LineEnable e);
   void set(TSIport p);
   void set(TSIport p, TSIcoinc c, enum CoincOp op);
   void set(TSIport p, TSIcoinc c, enum CoincFreqDiv div);
   void set(TSIport p, TSIcoinc c, enum CoincPeriod w);
   void set(TSIport p, TSIcoinc c, enum CoincDelay d);
   void set(TSIport p, TSIcoinc c, enum DelayEnable e);
   void set(TSIline l, enum CoincPeriod w);
   void set(TSIline l, enum CoincDelay d);
   void set(TSIline l, enum DelayEnable e);
   void set(TSIport p, uint8_t id, uint8_t index);
   void set(TL1SignalIn line, uint8_t id, uint8_t input);
   void set(TL1SignalOut line, uint8_t id, uint8_t logic);
   void set(TL2SignalIn line, uint8_t id, uint8_t input);
   void set(TL2SignalOut line, uint8_t id, uint8_t logic);

   TSIstatus parse_status(void);
   QString parse_status(TSIstatus s);
   uint8_t parse_coincfreqdiv(TSIcoinc c, uint16_t data);

   uint32_t read_coincfreq(TSIport p, TSIcoinc c);
   uint8_t read_coincfreqof(TSIport p, TSIcoinc c);

   QString getregname(QString pfx, TSIport p, uint8_t id = 0);
   uint32_t getregaddr(QString pfx, TSIport p, uint8_t id = 0);

   uint32_t getregvalue(QString pfx, TSIport p, uint8_t id = 0);
   void setregvalue(QString pfx, TSIport p, uint32_t value, uint8_t id = 0);

   QString output(QString pfx, TSIport p, uint8_t id = 0);
};

#endif // QREGISTERSET_H
