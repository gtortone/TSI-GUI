#ifndef QGATE_H
#define QGATE_H

#include <QVector>
#include <QString>

#include <stdint.h>

class QGate {

private:

   bool selected;

   // properties from config file
   uint8_t confindex;
   enum QGateType type;
   QString desc;
   QString shortdesc;

   QVector<uint8_t> lineInIndex;
   QVector<uint8_t> linePtIndex;

public:

   enum QGateType getType(void) { return type; }
   QString getDesc(void) { return desc; };
   QString getShortDesc(void) { return shortdesc; };
   QVector<uint8_t> getLineInIndex(void) { return lineInIndex; };
   QVector<uint8_t> getLinePtIndex(void) { return linePtIndex; };

   void copy(QGate other) {
      confindex = other.confindex;
      type = other.type;
      desc = other.desc;
      shortdesc = other.shortdesc;
      lineInIndex = other.lineInIndex;
      linePtIndex = other.linePtIndex;
   };

   void setConfindex(uint8_t val) { confindex = val; };
   void setType(enum QGateType val) { type = val; };
   void setDesc(QString val) { desc = val; };
   void setShortDesc(QString val) { shortdesc = val; };
   void setLineInIndex(QVector<uint8_t> vect) { lineInIndex = vect; };
   void setLinePtIndex(QVector<uint8_t> vect) { linePtIndex = vect; };
};

#endif // QGATE_H
