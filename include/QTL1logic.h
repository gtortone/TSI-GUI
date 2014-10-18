#ifndef QTL1LOGIC_H
#define QTL1LOGIC_H

#include <QVector>
#include <QString>
#include <QStringList>
#include <QList>

#include <stdint.h>
#include "QMetrics.h"
#include "QGate.h"

class QTL1logic {

private:

   QVector<QGate> TL1Gate;
   enum TL1SignalIn SigInMatrix[TL1GATE_NUM][TL1SIGIN_NUM];
   int SigOutVector[TL1SIGOUT_NUM];    // element i contains index of TL1Gate assigned to PTRG(i) (or UNDEF_OUT if not allocated)
   void fillcol(uint8_t lineidx, enum TL1SignalIn sig);

public:

   void init(void);
   void clear(void);

   QGate getGate(uint8_t gateidx);
   QStringList getGateDescList(void);
   QStringList getGateShortDescList(void);
   QString getGateDesc(uint8_t gateidx);
   QString getGateShortDesc(uint8_t gateidx);

   enum TL1SignalIn getInput(uint8_t gateidx, uint8_t lineidx);
   void assignInput(uint8_t gateidx, uint8_t lineidx, enum TL1SignalIn sig);
   void unassignInput(uint8_t gateidx, uint8_t lineidx);
   bool allocatedGate(uint8_t gateidx);

   enum TL1SignalOut getOutput(uint8_t gateidx);
   void assignOutput(uint8_t gateidx, enum TL1SignalOut sig);
   void unassignOutput(enum TL1SignalOut sig);
   bool allocatedOutput(enum TL1SignalOut sig);
   uint8_t getAssignedGate(enum TL1SignalOut sig);

   QVector<uint8_t> getGateInputIndex(uint8_t gateidx);
   QVector<uint8_t> getGatePtIndex(uint8_t gateidx);
};

#endif // QTL1LOGIC_H
