#ifndef QTL2LOGIC_H
#define QTL2LOGIC_H

#include <QVector>
#include <QString>
#include <QStringList>
#include <QList>

#include <stdint.h>
#include "QMetrics.h"
#include "QGate.h"

class QTL2logic {

private:

   QVector<QGate> TL2Gate;
   enum TL2SignalIn SigInMatrix[TL2GATE_NUM][TL2SIGIN_NUM];
   int SigOutVector[TL2SIGOUT_NUM];    // element i contains index of TL2Gate assigned to PTRIN(i) (or UNDEF_OUT if not allocated)
   void fillcol(uint8_t lineidx, enum TL2SignalIn sig);

public:

   void init(void);
   void clear(void);

   QGate getGate(uint8_t gateidx);
   QStringList getGateDescList(void);
   QStringList getGateShortDescList(void);
   QString getGateDesc(uint8_t gateidx);
   QString getGateShortDesc(uint8_t gateidx);

   enum TL2SignalIn getInput(uint8_t gateidx, uint8_t lineidx);
   void assignInput(uint8_t gateidx, uint8_t lineidx, enum TL2SignalIn sig);
   void unassignInput(uint8_t gateidx, uint8_t lineidx);
   bool allocatedGate(uint8_t gateidx);

   enum TL2SignalOut getOutput(uint8_t gateidx);
   void assignOutput(uint8_t gateidx, enum TL2SignalOut sig);
   void unassignOutput(enum TL2SignalOut sig);
   bool allocatedOutput(enum TL2SignalOut sig);
   uint8_t getAssignedGate(enum TL2SignalOut sig);

   QVector<uint8_t> getGateInputIndex(uint8_t gateidx);
};

#endif // QTL2LOGIC_H
