#include "QTopology.h"

QDetector::QDetector(unsigned int detuid, QString str, DetectorType t) {

   init();

   _detuid = detuid;
   _name = str;
   _type = t;

}
 
QDetector::QDetector() {

   init();
}

void QDetector::init(void) {

   _port = UNDEF_PORT;
   _line = UNDEF_LINE;
   _coinc = UNDEF_COINC;

   _coincop = (CoincOp) 0;             // AND
   _coincfreqdiv = (CoincFreqDiv) 0;   // DIV0
   _coincperiod = (CoincPeriod) 0;     // PER0
   _coincdelay = (CoincDelay) 0;       // DEL0
   _activeface = (DetectorFace) FACE_BOTH;

   _allocated = false;
   _enabled = false;
}

void QDetector::setName(QString str) {

   _name = str;
}

QString QDetector::name(void) const {
   
   return _name;
} 

DetectorType QDetector::type(void) const {

   return _type;
}

bool QDetector::allocated(void) const {

   return _allocated;
}

bool QDetector::enabled(void) const {

   return _enabled;
}

unsigned int QDetector::detuid(void) const {

   return _detuid;
}

void QDetector::setAllocated(bool val) {

   _allocated = val;
}

void QDetector::setEnabled(bool val) {

   _enabled = val;
}

void QDetector::setPort(TSIport p) {

   _port = p;
}

void QDetector::setLine(TSIline l) {

   _line = l;
   if(_type == DSSD) {

      if(_line <= TRG4)
         _coinc = (TSIcoinc) (_line);
      else
         _coinc = (TSIcoinc)(_line - 4);
   }
}

void QDetector::setCoincOp(CoincOp op) {

   _coincop = op;
}

void QDetector::setCoincFreqDiv(CoincFreqDiv div) {

   _coincfreqdiv = div;
}

void QDetector::setCoincDelay(CoincDelay del) {

   _coincdelay = del;
}

void QDetector::setCoincPeriod(CoincPeriod per) {

   _coincperiod = per;
}

void QDetector::setActiveFaces(DetectorFace f) {

   _activeface = f;
}

TSIport QDetector::port(void) const {

   return _port;
}

TSIline QDetector::line(void) const {

   return _line;
}

TSIcoinc QDetector::coinc(void) const {

   return _coinc;
}

CoincOp QDetector::coincop(void) const {

   return _coincop;
}

CoincFreqDiv QDetector::coincfreqdiv(void) const {

   return _coincfreqdiv;
}

CoincDelay QDetector::coincdelay(void) const {

   return _coincdelay;
}

CoincPeriod QDetector::coincperiod(void) const {

   return _coincperiod;
}

DetectorFace QDetector::activefaces(void) const {

   return _activeface;
}

QString QDetector::type_str(DetectorType t) {

   if ( (t >= 0) && (t < DTYPE_NUM) )
      return(DetectorType_str[t]);
   else
      return "UNDEF_DTYPE";
}

QString QDetector::port_str(TSIport p) {
  
   if( (p >= 0) && (p < TSIPORT_NUM) ) 
      return(TSIport_str[p]);
   else
      return("UNDEF_PORT");
}

QString QDetector::line_str(TSIline l) {

   if ( (l >= 0) && (l < PORTLINE_NUM) )
      return(TSIline_str[l]);
   else
      return("UNDEF_LINE");
}

QString QDetector::mon_str(TSImon m) {

   if ( (m >= 0) && (m < PORTOUTLINE_NUM) )
      return(TSImon_str[m]);
   else
      return("UNDEF_MON");
}

QString QDetector::coinc_str(TSIcoinc c) {

   if( (c >= 0) && (c < PORTCOINC_NUM) )
      return(TSIcoinc_str[c]);
   else
      return("UNDEF_COINC");
}

QString QDetector::coincop_str(CoincOp op) {

   if( (op >= 0) && (op < COINCOP_NUM) ) 
      return(CoincOp_str[op]);
   else
      return("UNDEF_COINCOP");
}

QString QDetector::coincfreqdiv_str(CoincFreqDiv div) {

   if( (div >= 0) && (div < COINCFREQDIV_NUM) ) 
      return(CoincFreqDiv_str[div]);
   else
      return("UNDEV_COINCFREQDIV");
}

QString QDetector::coindelay_str(CoincDelay del) {

   if( (del >= 0) && (del < COINCDELAY_NUM) )
      return(CoincDelay_str[del]);
   else
      return("UNDEF_COINCDELAY");
}

QString QDetector::coincperiod_str(CoincPeriod per) {

   if( (per >= 0) && (per < COINCPERIOD_NUM) )
      return(CoincPeriod_str[per]);
   else
      return("UNDEF_COINCPERIOD");
}

QTopology::QTopology(void) { 

   teluid = detuid = 1;
   entrynum = 0;

   tsiregset.init();
   mpset.init();

   TL1logic.init();
   TL2logic.init();

   Init();
}

void QTopology::Init(void) {

   for(int i=0; i<(TSIPORT_NUM-1); i++) {    // Xa, Xb, Xc, X4

      for(int j=0; j<PORTINLINE_NUM; j++)
         avbInput[i][(TSIline)j] = true;
   }

   for(int i=0; i<TSIPORT_NUM; i++) {    // Xa, Xb, Xc, X4, XF

      for(int j=0; j<TSImon_max[i]; j++)
         avbOutput[i][(TSImon)j] = true;
   }

   monLines.clear();
   tsiregset.clearall();

   TL1logic.clear();
   TL2logic.clear();

   tsiregset.setbaddr(0xF0910000);
   VMEbus.setup("exodaq01.na.infn.it", 5555);

}

void QTopology::Clear(void) {

   QMapIterator<unsigned int, QString> i(descTel);
   QMapIterator<unsigned int, QDetector> k(hashDet);

   while (i.hasNext()) {

      i.next();
      descTel.remove(i.key());
   }
   
   while (k.hasNext()) {

      k.next();
      hashDet.remove(k.key());
   }

   teluid = detuid = 1;
   entrynum = 0;

   Init();
}

bool QTopology::empty(void) {

   return (entrynum == 0);
}

QList<TSIport> QTopology::freePorts(void) {

   QList<TSIport> tmplist;

   for(int i=0; i < TSIPORT_NUM-1; i++) {    // Xa, Xb, Xc, X4

      for(int j=0; j < PORTLINE_NUM; j++) {

         if(avbInput[i].value((TSIline)j)) {
            tmplist << (TSIport) i;
            break;
         }
      }
   }

   return tmplist;
}

QList<TSIport> QTopology::busyPorts(void) {

   QList<TSIport> tmplist;

   return tmplist;
}

QList<TSIline> QTopology::freeInputLines(TSIport p, DetectorType dtype) {

   QList<TSIline> tmplist;

   if(dtype == DSSD) {

      for(int i=0; i <= 3; i++) {      // TRG0, TRG1, TRG2, TRG3

         if(avbInput[p].value((TSIline) i))           // line is available
            tmplist << (TSIline) i;
      }

      for(int i=8; i <= 11; i++) {     // TRG8, TRG9, TRG10, TRG11

         if(avbInput[p].value((TSIline) i))           // line is available
            tmplist << (TSIline) i;
      }

   } else {

      for(int i=0; i < PORTINLINE_NUM; i++) {

         if(avbInput[p].value((TSIline) i))           // line is available
            tmplist << (TSIline) i;
      }
   }

   return tmplist;
}

QList<TSIline> QTopology::busyInputLines(TSIport p) {

   QList<TSIline> tmplist;

   for(int i=0; i < PORTINLINE_NUM; i++) {

      if(!avbInput[p].value((TSIline) i))           // line is NOT available
         tmplist << (TSIline) i;
   }

   return tmplist;
}

void QTopology::setInputBusy(TSIport p, TSIline l) {

   avbInput[p][l] = false;
}

void QTopology::setInputFree(TSIport p, TSIline l) {

   avbInput[p][l] = true;
}

QList<TSImon> QTopology::freeMonitorLines(TSIport p) {

   QList<TSImon> tmplist;

   for(int i=0; i < TSImon_max[p]; i++) {

      if(avbOutput[p].value((TSImon) i))           // mon is available
         tmplist << (TSImon) i;
   }

   return tmplist;
}

QList<TSImon> QTopology::busyMonitorLines(TSIport p) {

   QList<TSImon> tmplist;

   for(int i=0; i < TSImon_max[p]; i++) {

      if(!avbOutput[p].value((TSImon) i))           // mon is NOT available
         tmplist << (TSImon) i;
   }

   return tmplist;
}

bool QTopology::MonitorBusy(TSIport p, TSImon m) {

   return(!avbOutput[p].value((TSImon) m));
}

bool QTopology::MonitorFree(TSIport p, TSImon m) {

   return(avbOutput[p].value((TSImon) m));
}

void QTopology::setMonitorBusy(TSIport p, TSImon m) {

   avbOutput[p][m] = false;
}

void QTopology::setMonitorFree(TSIport p, TSImon m) {

   avbOutput[p][m] = true;
}

unsigned int QTopology::addTelescope(QString name) {

   if(entrynum == MAX_TELESCOPE_NUM)
      throw TopologyException(TopologyException::TELESCOPE_SET_FULL);

   unsigned int currteluid = teluid;

   descTel[teluid++] = name;
   entrynum++;

   return (currteluid);
}
   
void QTopology::delTelescope(unsigned int index) {

   if(!descTel.contains(index))
      throw TopologyException(TopologyException::TELINDEX_OUT_OF_BOUND);

   QMutableMapIterator<unsigned int, QDetector> i(hashDet);

   while (i.hasNext()) {

      i.next();
      if (i.key() == index) {

         if(i.value().allocated()) {	// free lines

            TSIport p = i.value().port();
            TSIline l = i.value().line();

            avbInput[p][l] = true;
            delDetectorTopoInfo(p, l);
            delMonlines(p, l);      // free also MON lines

            if(i.value().type() == DSSD) {

               avbInput[p][(TSIline)(l+4)] = true;
               delDetectorTopoInfo(p, (TSIline)(l+4));
               delMonlines(p, (TSIline)(l+4));
            }
         }

         i.remove();
      }
   }

   descTel.remove(index);

   entrynum--;
}

void QTopology::setTelescopeName(unsigned int index, QString str) {

   if(!descTel.contains(index))
      throw TopologyException(TopologyException::TELINDEX_OUT_OF_BOUND);

   descTel[index] = str;
}

QString QTopology::telescopeName(unsigned int index) {

   if(!descTel.contains(index))
      throw TopologyException(TopologyException::TELINDEX_OUT_OF_BOUND);

   return descTel[index];
}

bool QTopology::lookupTelescopeName(QString str) {

   QMapIterator<unsigned int, QString> i(descTel);

   while (i.hasNext()) {
      i.next();
      if(i.value() == str)
         return true;
   }

   return false;
}

QMap<unsigned int, QString> QTopology::telescopeMap(void) {

   return descTel;
}

unsigned int QTopology::sizeTelescopeList(void) {

   return entrynum;
}

unsigned int QTopology::addDetector(unsigned int telindex, QString str, DetectorType type) {

   if(!descTel.contains(telindex))
      throw TopologyException(TopologyException::TELINDEX_OUT_OF_BOUND);

   unsigned int currdetuid = detuid;

   hashDet.insert(telindex, QDetector(detuid++, str, type));

   return currdetuid;
}

void QTopology::setDetectorName(unsigned int telindex, unsigned int detindex, QString str) {

   if(!descTel.contains(telindex))
      throw TopologyException(TopologyException::TELINDEX_OUT_OF_BOUND);

   QMutableMapIterator<unsigned int, QDetector> i(hashDet);
   bool found = false;

   while (i.hasNext() && !found) {

      i.next();
      if( (i.key() == telindex) && (i.value().detuid() == detindex )) {

         i.value().setName(str);
         found = true;
      }
   }

   if(!found)
      throw TopologyException(TopologyException::DETINDEX_OUT_OF_BOUND);
}

bool QTopology::lookupDetectorName(unsigned int telindex, QString str) {

   if(!descTel.contains(telindex))
      throw TopologyException(TopologyException::TELINDEX_OUT_OF_BOUND);

   QMapIterator<unsigned int, QDetector> i(hashDet);

   while (i.hasNext()) {

      i.next();
      if( (i.key() == telindex) && (i.value().name() == str) )
         return true;
   }

   return false;
}

void QTopology::delDetector(unsigned int telindex, unsigned int detindex) {

   if(!descTel.contains(telindex))
      throw TopologyException(TopologyException::TELINDEX_OUT_OF_BOUND);
   
   QMutableMapIterator<unsigned int, QDetector> i(hashDet);
   bool found = false;

   while (i.hasNext() && !found) {

      i.next();
      if( (i.key() == telindex) && (i.value().detuid() == detindex )) {

         found = true;
         if(i.value().allocated()) {	// free lines

            TSIport p = i.value().port();
            TSIline l = i.value().line();

            avbInput[p][l] = true;
            delDetectorTopoInfo(p, l);
            delMonlines(p, l);      // free also MON lines

            if(i.value().type() == DSSD) {

               avbInput[p][(TSIline)(l+4)] = true;
               delDetectorTopoInfo(p, (TSIline)(l+4));
               delMonlines(p, (TSIline)(l+4));
            }
         }

         i.remove();
      }
   }

   if(!found)
      throw TopologyException(TopologyException::DETINDEX_OUT_OF_BOUND);
}

DetectorType QTopology::getDetectorType(unsigned int telindex, unsigned int detindex) {

   if(!descTel.contains(telindex))
      throw TopologyException(TopologyException::TELINDEX_OUT_OF_BOUND);

   QMutableMapIterator<unsigned int, QDetector> i(hashDet);
   bool found = false;

   while (i.hasNext() && !found) {

      i.next();
      if( (i.key() == telindex) && (i.value().detuid() == detindex )) {

         found = true;
         return(i.value().type());
      }
   }

   return(UNDEF_DTYPE);
}

void QTopology::setDetectorWire(unsigned int telindex, unsigned int detindex, TSIport p, TSIline l) {

   if(!descTel.contains(telindex))
      throw TopologyException(TopologyException::TELINDEX_OUT_OF_BOUND);

   QMutableMapIterator<unsigned int, QDetector> i(hashDet);
   bool found = false;

   while (i.hasNext() && !found) {

      i.next();
      if( (i.key() == telindex) && (i.value().detuid() == detindex )) {

         found = true;
         if(i.value().allocated()) {      // free previous port, line

            TSIport oldp = i.value().port();
            TSIline oldl = i.value().line();

            setInputFree(oldp, oldl);
            delDetectorTopoInfo(oldp, oldl);
            delMonlines(oldp, oldl);      // free also MON line

            if(i.value().type() == DSSD)     // for DSSD free also coincidence line

               setInputFree(oldp, (TSIline)(oldl+4));
               delDetectorTopoInfo(oldp, (TSIline)(oldl+4));
               delMonlines(oldp, (TSIline)(oldl+4));           // free also MON line
         }

         i.value().setPort(p);
         i.value().setLine(l);

         addDetectorTopoInfo(p, l, telindex, detindex);

         setInputBusy(p,l);

         if(i.value().type() == DSSD) {

            setInputBusy(p, (TSIline)(l+4));
            addDetectorTopoInfo(p, (TSIline)(l+4), telindex, detindex);
         }

         i.value().setAllocated(true);
      }
   }

   if(!found)
      throw TopologyException(TopologyException::DETINDEX_OUT_OF_BOUND);
}

void QTopology::setDetectorEnabled(unsigned int telindex, unsigned int detindex, bool val) {

   if(!descTel.contains(telindex))
      throw TopologyException(TopologyException::TELINDEX_OUT_OF_BOUND);

   QMutableMapIterator<unsigned int, QDetector> i(hashDet);
   bool found = false;

   while (i.hasNext() && !found) {

      i.next();
      if( (i.key() == telindex) && (i.value().detuid() == detindex )) {

         found = true;
         i.value().setEnabled(val);
      }
   }

   if(!found)
      throw TopologyException(TopologyException::DETINDEX_OUT_OF_BOUND);
}

void QTopology::setDetectorCoincOp(unsigned int telindex, unsigned int detindex, CoincOp op) {

   if(!descTel.contains(telindex))
      throw TopologyException(TopologyException::TELINDEX_OUT_OF_BOUND);

   QMutableMapIterator<unsigned int, QDetector> i(hashDet);
   bool found = false;

   while (i.hasNext() && !found) {

      i.next();
      if( (i.key() == telindex) && (i.value().detuid() == detindex )) {

         found = true;
         i.value().setCoincOp(op);
      }
   }

   if(!found)
      throw TopologyException(TopologyException::DETINDEX_OUT_OF_BOUND);
}

void QTopology::setDetectorCoincFreqDiv(unsigned int telindex, unsigned int detindex, CoincFreqDiv div) {

   if(!descTel.contains(telindex))
      throw TopologyException(TopologyException::TELINDEX_OUT_OF_BOUND);

   QMutableMapIterator<unsigned int, QDetector> i(hashDet);
   bool found = false;

   while (i.hasNext() && !found) {

      i.next();
      if( (i.key() == telindex) && (i.value().detuid() == detindex )) {

         found = true;
         i.value().setCoincFreqDiv(div);
      }
   }

   if(!found)
      throw TopologyException(TopologyException::DETINDEX_OUT_OF_BOUND);
}

void QTopology::setDetectorCoincDelay(unsigned int telindex, unsigned int detindex, CoincDelay del) {

   if(!descTel.contains(telindex))
      throw TopologyException(TopologyException::TELINDEX_OUT_OF_BOUND);

   QMutableMapIterator<unsigned int, QDetector> i(hashDet);
   bool found = false;

   while (i.hasNext() && !found) {

      i.next();
      if( (i.key() == telindex) && (i.value().detuid() == detindex )) {

         found = true;
         i.value().setCoincDelay(del);
      }
   }

   if(!found)
      throw TopologyException(TopologyException::DETINDEX_OUT_OF_BOUND);
}

void QTopology::setDetectorCoincPeriod(unsigned int telindex, unsigned int detindex, CoincPeriod per) {

   if(!descTel.contains(telindex))
      throw TopologyException(TopologyException::TELINDEX_OUT_OF_BOUND);

   QMutableMapIterator<unsigned int, QDetector> i(hashDet);
   bool found = false;

   while (i.hasNext() && !found) {

      i.next();
      if( (i.key() == telindex) && (i.value().detuid() == detindex )) {

         found = true;
         i.value().setCoincPeriod(per);
      }
   }

   if(!found)
      throw TopologyException(TopologyException::DETINDEX_OUT_OF_BOUND);
}

void QTopology::setDetectorActiveFaces(unsigned int telindex, unsigned int detindex, DetectorFace f) {

   if(!descTel.contains(telindex))
      throw TopologyException(TopologyException::TELINDEX_OUT_OF_BOUND);

   QMutableMapIterator<unsigned int, QDetector> i(hashDet);
   bool found = false;

   while (i.hasNext() && !found) {

      i.next();
      if( (i.key() == telindex) && (i.value().detuid() == detindex )) {

         found = true;
         i.value().setActiveFaces(f);
      }
   }

   if(!found)
      throw TopologyException(TopologyException::DETINDEX_OUT_OF_BOUND);
}

unsigned int QTopology::sizeDetectorList(unsigned int telindex) {

   return hashDet.count(telindex);
}

QDetector QTopology::detector(unsigned int telindex, unsigned int detindex) {

   QDetector undefdet(0, "", UNDEF_DTYPE);

   if(!descTel.contains(telindex))
      throw TopologyException(TopologyException::TELINDEX_OUT_OF_BOUND);

   QMapIterator<unsigned int, QDetector> i(hashDet);

   while (i.hasNext()) {

      i.next();
      if( (i.key() == telindex) && (i.value().detuid() == detindex ))
         return i.value();
   }

   return undefdet;
}

QList<QDetector> QTopology::detectorList(unsigned int telindex) {

   return hashDet.values(telindex);
}

QList<QDetector> QTopology::detectorListOf(unsigned int telindex, DetectorType type) {

   if(!descTel.contains(telindex))
      throw TopologyException(TopologyException::TELINDEX_OUT_OF_BOUND);

   QList<QDetector> tmplist;
   QMapIterator<unsigned int, QDetector> i(hashDet);

   while (i.hasNext()) {

      i.next();
      if( (i.key() == telindex) && (i.value().type() == type ))
         tmplist << i.value();
   }

   return tmplist;
}

unsigned int QTopology::detectorNumOf(unsigned int telindex, DetectorType type) {

   if(!descTel.contains(telindex))
      throw TopologyException(TopologyException::TELINDEX_OUT_OF_BOUND);

   QList<QDetector> tmplist;
   
   tmplist  = detectorListOf(telindex, type);
 
   return tmplist.size();
}

unsigned int QTopology::getDetectorMaxAllowed(DetectorType type) {

   return DetectorType_max[type];
}

void QTopology::addMonline(TSIport p, TSImon ml, TSIline l, int16_t group) {

   QMonitorParamInfo mpinfo;

   mpinfo = mpset.monparam(p, ml, l, group);

   if(mpinfo.port != UNDEF_PORT) {

      monLines.append(mpinfo);
      setMonitorBusy(p, ml);
   }
}

void QTopology::addMonline(TSIport p, TSImon ml, uint16_t seqid) {

   QMonitorParamInfo mpinfo;

   mpinfo = mpset.monparam(p, ml, seqid);

   if(mpinfo.port != UNDEF_PORT) {

      monLines.append(mpinfo);
      setMonitorBusy(p, ml);
   }
}

void QTopology::delMonline(TSIport p, TSImon ml, TSIline l, int16_t group) {

   for(int i=0; i<monLines.size(); i++) {

      if( (monLines[i].port == p) && (monLines[i].mline == ml) && (monLines[i].bindline == l) && (monLines[i].group == group)) {

         setMonitorFree(p, ml);
         monLines.removeAt(i);
      }
   }
}

void QTopology::delMonline(TSIport p, TSImon ml) {

   for(int i=0; i<monLines.size(); i++) {

      if( (monLines[i].port == p) && (monLines[i].mline == ml) ) {

         setMonitorFree(p, ml);
         monLines.removeAt(i);
      }
   }
}

void QTopology::delMonlines(TSIport p, TSIline l) {

   QList<TSImon> idlist;

   for(int i=0; i<monLines.size(); i++) {

      if( (monLines[i].port == p) && (monLines[i].bindline == l) ) {

         idlist = monlineList(p, l);

         for(int ix=0; ix<idlist.size(); ix++)
            setMonitorFree(p, idlist[ix]);

         monLines.removeAt(i);
      }
   }
}

TSImon QTopology::monline(TSIport p, TSIline l, int16_t group) {

   for(int i=0; i<monLines.size(); i++) {

      if( (monLines[i].port == p) && (monLines[i].bindline == l) && (monLines[i].group == group))
         return monLines[i].mline;
   }

   return UNDEF_MON;
}

QList<TSImon> QTopology::monlineList(TSIport p, TSIline l) {

   QList<TSImon> tmplist;

   for(int i=0; i<monLines.size(); i++) {

      if( (monLines[i].port == p) && (monLines[i].bindline == l) )
         tmplist <<  monLines[i].mline;
   }

   return tmplist;
}

QList<QMonitorParamInfo> QTopology::monparamList(TSIport p) {

   QList<QMonitorParamInfo> tmplist;

   for(int i=0; i<monLines.size(); i++) {

      if(monLines[i].port == p)
         tmplist << monLines[i];
   }

   return tmplist;
}

void QTopology::addDetectorTopoInfo(TSIport p, TSIline l, unsigned int telindex, unsigned int detindex) {

   if(!descTel.contains(telindex))
      throw TopologyException(TopologyException::TELINDEX_OUT_OF_BOUND);

   QPair<TSIport, TSIline> loc(p, l);
   QPair<unsigned int, unsigned int> detloc(telindex, detindex);

   bindLineDetector[loc] = detloc;
}

void QTopology::delDetectorTopoInfo(TSIport p, TSIline l) {

   QPair<TSIport, TSIline> loc(p, l);

   bindLineDetector.remove(loc);
}

QPair<unsigned int, unsigned int> QTopology::detectorTopoInfo(TSIport p, TSIline l) {

   QPair<TSIport, TSIline> loc(p, l);

   if(bindLineDetector.contains(loc)) {

      return bindLineDetector.value(loc);

   } else {

      return (QPair<unsigned int, unsigned int> (0, 0));
   }
}

QString QTopology::serializeToXML(void) {

   QString output;
   QMapIterator<unsigned int, QString> i(descTel);

   QXmlStreamWriter stream(&output);
   stream.setAutoFormatting(true);
   stream.writeStartDocument();

   stream.writeStartElement("topology");
   stream.writeAttribute("entrynum", QString::number(entrynum));
   stream.writeAttribute("teluid", QString::number(teluid));
   stream.writeAttribute("detuid", QString::number(detuid));

   // telescopes
   while (i.hasNext()) {

      i.next();
      stream.writeStartElement("telescope");
      stream.writeAttribute("id", QString::number(i.key()));
      stream.writeAttribute("name", i.value());
      stream.writeEndElement();  // telescope
   }

   // detectors
   i.toFront();   // rewind...
   while (i.hasNext()) {

      i.next();
      QList<QDetector> dlist = detectorList(i.key());

      for(int ix = (dlist.size()-1); ix >= 0; ix--) {

         stream.writeStartElement("detector");
         stream.writeAttribute("parentid", QString::number(i.key()));

         QDetector det = dlist[ix];
         stream.writeAttribute("id", QString::number(det.detuid()));
         stream.writeAttribute("name", det.name());
         stream.writeAttribute("type", QString::number(det.type()));
         stream.writeAttribute("port", QString::number(det.port()));
         stream.writeAttribute("line", QString::number(det.line()));

         stream.writeAttribute("coincop", QString::number(det.coincop()));
         stream.writeAttribute("coincfreqdiv", QString::number(det.coincfreqdiv()));
         stream.writeAttribute("coincperiod", QString::number(det.coincperiod()));
         stream.writeAttribute("coincdelay", QString::number(det.coincdelay()));

         stream.writeAttribute("enabled", QString::number(det.enabled()));

         stream.writeEndElement();  // detector
      }

   }

   if(monLines.size() > 0) {

      for(int k=0; k<monLines.size(); k++) {

         stream.writeStartElement("monline");

         stream.writeAttribute("port", QString::number(monLines[k].port));
         stream.writeAttribute("mline", QString::number(monLines[k].mline));
         stream.writeAttribute("seqid", QString::number(monLines[k].seqid));

         stream.writeEndElement();
      }
   }

   // TRIGGER level I config

   for(int s=0; s<TL1SIGOUT_NUM; s++) {

      TL1SignalOut sigout = (enum TL1SignalOut) s;

      if(TL1logic.allocatedOutput(sigout)) {

         stream.writeStartElement("tl1conf");

         stream.writeAttribute("output", QString::number(s));

         uint8_t row = TL1logic.getAssignedGate(sigout);

         stream.writeAttribute("gate", QString::number(row));

         QGate g = TL1logic.getGate(TL1logic.getAssignedGate(sigout));
         QVector<uint8_t> linein = g.getLineInIndex();

         QString lines;

         for(int i=0; i<linein.size(); i++) {

            lines += QString::number(TL1logic.getInput(row, linein[i]));

            if( (i+1) < linein.size())
               lines += ",";
         }

         stream.writeAttribute("lines", lines);

         stream.writeEndElement();
      }
   }

   // TRIGGER level II config

   for(int s=0; s<TL2SIGOUT_NUM; s++) {

      TL2SignalOut sigout = (enum TL2SignalOut) s;

      if(TL2logic.allocatedOutput(sigout)) {

         stream.writeStartElement("tl2conf");

         stream.writeAttribute("output", QString::number(s));

         uint8_t row = TL2logic.getAssignedGate(sigout);

         stream.writeAttribute("gate", QString::number(row));

         QGate g = TL2logic.getGate(TL2logic.getAssignedGate(sigout));
         QVector<uint8_t> linein = g.getLineInIndex();

         QString lines;

         for(int i=0; i<linein.size(); i++) {

            lines += QString::number(TL2logic.getInput(row, linein[i]));

            if( (i+1) < linein.size())
               lines += ",";
         }

         stream.writeAttribute("lines", lines);

         stream.writeEndElement();
      }
   }

   // VME config

   stream.writeStartElement("vme");

   stream.writeAttribute("baseaddr", "0x" + QString::number(tsiregset.getbaddr(), 16).toUpper());
   stream.writeAttribute("host", VMEbus.gethostname());
   stream.writeAttribute("port", QString::number(VMEbus.getport()));

   stream.writeEndElement();

   stream.writeEndElement();  // topology
   stream.writeEndDocument();

   return output;
}

QPair<QXmlStreamReader::Error, QString> QTopology::deserializeFromXML(QString xmlcontent) {

   QXmlStreamReader xml;
   QPair<QXmlStreamReader::Error, QString> xmlerr;
   QXmlStreamAttributes attrs;
   QString value;

   // just to be sure topology is cleared
   Clear();

   xml.addData(xmlcontent);

   while(!xml.atEnd()) {

      xml.readNext();

      if(xml.hasError()) {

         qDebug() << "error - " << xml.errorString();
         xmlerr.first = xml.error();
         xmlerr.second = xml.errorString();
         return xmlerr;

      } else if(xml.isStartElement()) {

         attrs = xml.attributes();

         if(xml.name() == "topology") {

            value.clear();
            value.append(attrs.value("entrynum"));
            entrynum = value.toUInt();

            value.clear();
            value.append(attrs.value("teluid"));
            teluid = value.toUInt();

            value.clear();
            value.append(attrs.value("detuid"));
            detuid = value.toUInt();

         } else if(xml.name() == "telescope") {

            value.clear();
            value.append(attrs.value("id"));
            unsigned int telindex = value.toUInt();

            value.clear();
            value.append(attrs.value("name"));
            QString telname = value;

            descTel.insert(telindex, telname);

         } else if(xml.name() == "detector") {

            value.clear();
            value.append(attrs.value("parentid"));
            unsigned int telindex = value.toUInt();

            value.clear();
            value.append(attrs.value("id"));
            unsigned int detindex = value.toUInt();

            value.clear();
            value.append(attrs.value("name"));
            QString name = value;

            value.clear();
            value.append(attrs.value("type"));
            DetectorType type = (DetectorType) value.toInt();

            value.clear();
            value.append(attrs.value("port"));
            TSIport port = (TSIport) value.toInt();

            value.clear();
            value.append(attrs.value("line"));
            TSIline line = (TSIline) value.toInt();

            value.clear();
            value.append(attrs.value("coincop"));
            CoincOp coincop = (CoincOp) value.toInt();

            value.clear();
            value.append(attrs.value("coincfreqdiv"));
            CoincFreqDiv coincfreqdiv = (CoincFreqDiv) value.toInt();

            value.clear();
            value.append(attrs.value("coincperiod"));
            CoincPeriod coincperiod = (CoincPeriod) value.toInt();

            value.clear();
            value.append(attrs.value("coincdelay"));
            CoincDelay coincdelay = (CoincDelay) value.toInt();

            value.clear();
            value.append(attrs.value("enabled"));
            bool enabled = (bool) value.toUInt();

            QDetector det(detindex, name, type);
            hashDet.insert(telindex, det);

            if( (port != UNDEF_PORT) && (line != UNDEF_LINE) ) {

               setDetectorWire(telindex, detindex, port, line);
               setDetectorCoincOp(telindex, detindex, coincop);
               setDetectorCoincFreqDiv(telindex, detindex, coincfreqdiv);
               setDetectorCoincPeriod(telindex, detindex, coincperiod);
               setDetectorCoincDelay(telindex, detindex, coincdelay);
               setDetectorEnabled(telindex, detindex, enabled);
            }            

         } else if(xml.name() == "monline") {

            value.clear();
            value.append(attrs.value("port"));
            TSIport port = (TSIport) value.toInt();

            value.clear();
            value.append(attrs.value("mline"));
            TSImon mline = (TSImon) value.toInt();

            value.clear();
            value.append(attrs.value("seqid"));
            uint16_t seqid = value.toUInt();

            addMonline(port, mline, seqid);

         } else if(xml.name() == "tl1conf") {

            value.clear();
            value.append(attrs.value("output"));
            TL1SignalOut sigout = (TL1SignalOut) value.toInt();

            value.clear();
            value.append(attrs.value("gate"));
            uint8_t gateidx = (uint8_t) value.toInt();

            value.clear();
            value.append(attrs.value("lines"));
            QString tmp = value.toStdString().c_str();
            QStringList sigin = tmp.split(",");

            QVector<uint8_t> inlist = TL1logic.getGateInputIndex(gateidx);

            for(int i=0; i<sigin.size(); i++)
               TL1logic.assignInput(gateidx, inlist[i], (TL1SignalIn) sigin[i].toInt());

            TL1logic.assignOutput(gateidx, sigout);


         } else if(xml.name() == "tl2conf") {

            value.clear();
            value.append(attrs.value("output"));
            TL2SignalOut sigout = (TL2SignalOut) value.toInt();

            value.clear();
            value.append(attrs.value("gate"));
            uint8_t gateidx = (uint8_t) value.toInt();

            value.clear();
            value.append(attrs.value("lines"));
            QString tmp = value.toStdString().c_str();
            QStringList sigin = tmp.split(",");

            QVector<uint8_t> inlist = TL2logic.getGateInputIndex(gateidx);

            for(int i=0; i<sigin.size(); i++)
               TL2logic.assignInput(gateidx, inlist[i], (TL2SignalIn) sigin[i].toInt());

            TL2logic.assignOutput(gateidx, sigout);

         } else if(xml.name() == "vme") {

            bool ok;

            value.clear();
            value.append(attrs.value("baseaddr"));
            tsiregset.setbaddr(value.toUInt(&ok, 16));

            value.clear();
            value.append(attrs.value("host"));
            QString host = value;

            value.clear();
            value.append(attrs.value("port"));
            uint16_t port = value.toUInt();

            VMEbus.setup(host, port);
         }

      }

   }  // end while

   xmlerr.first = xml.error();
   xmlerr.second = xml.errorString();
   return xmlerr;
}

QStringList QTopology::dumpVMEConfig(bool zerosupp) {

   uint8_t p, id;
   QStringList tmplist;
   QMapIterator<unsigned int, QString> i(descTel);

   tsiregset.clearall();

   // detectors
   while (i.hasNext()) {

      i.next();
      QList<QDetector> dlist = detectorList(i.key());

      for(int ix = (dlist.size()-1); ix >= 0; ix--) {

         QDetector det = dlist[ix];
         TSIport port = det.port();
         TSIline line = det.line();

         if(det.enabled()) {

            if(det.type() == DSSD) {

               TSIcoinc coinc = det.coinc();

               tsiregset.set(port, (TSIline)(line), ENABLED);
               tsiregset.set(port, (TSIline)(line+4), ENABLED);
               tsiregset.set(port, coinc, det.coincop());
               tsiregset.set(port, coinc, det.coincfreqdiv());
               tsiregset.set(port, coinc, det.coincperiod());
               tsiregset.set(port, coinc, det.coincdelay());

               if(det.coincdelay() != DEL0)
                  tsiregset.set(port, coinc, DELAYON);
               else
                  tsiregset.set(port, coinc, DELAYOFF);

            } else {

               tsiregset.set(port, line, ENABLED);
               tsiregset.set(line, det.coincperiod());
               tsiregset.set(line, det.coincdelay());

               if(det.coincdelay() != DEL0)
                  tsiregset.set(line, DELAYON);
               else
                  tsiregset.set(line, DELAYOFF);
            }
         }

      }  // end for

   }  // end while

   // trigger level I
   for(int ix=0; ix<TL1SIGOUT_NUM; ix++) {

      if(TL1logic.allocatedOutput((enum TL1SignalOut) ix)) {

         uint8_t gateidx = TL1logic.getAssignedGate((enum TL1SignalOut) ix);

         tsiregset.set(((enum TL1SignalOut) ix), ix / 2, (gateidx + 1));

         QGate g = TL1logic.getGate(gateidx);
         QVector<uint8_t> gin = g.getLineInIndex();

         for(int l=0; l<gin.size(); l++) {

            TL1SignalIn sigin = TL1logic.getInput(gateidx, gin[l]);
            tsiregset.set((TL1SignalIn)(sigin+1), gin[l]/2, (uint8_t)(gin[l]));     // to check
         }
      }
   }

   // trigger level II
   for(int ix=0; ix<TL2SIGOUT_NUM; ix++) {

      if(TL2logic.allocatedOutput((enum TL2SignalOut) ix)) {

         uint8_t gateidx = TL2logic.getAssignedGate((enum TL2SignalOut) ix);

         tsiregset.set(((enum TL2SignalOut) ix), ix / 2, (gateidx + 1));

         QGate g = TL2logic.getGate(gateidx);
         QVector<uint8_t> gin = g.getLineInIndex();

         for(int l=0; l<gin.size(); l++) {

            TL2SignalIn sigin = TL2logic.getInput(gateidx, gin[l]);
            tsiregset.set((TL2SignalIn)(sigin+1), gin[l]/2, (uint8_t)(gin[l]));     // to check
         }
      }
   }

   // monitoring lines
   if(monLines.size() > 0) {

      for(int k=0; k<monLines.size(); k++)
         tsiregset.set(monLines[k].port, monLines[k].mline, monLines[k].seqid);
   }

   // dump VME registers
   for(p=0; p<TSIPORT_NUM-1; p++) {    // Xa, Xb, Xc, X4

      tmplist.append((zerosupp && !tsiregset.getregvalue("MascheraIngressi", (TSIport)p)) ? "" : tsiregset.output("MascheraIngressi", (TSIport)p));

      if(p != X4) {
         tmplist.append((zerosupp && !tsiregset.getregvalue("LogicaCoinc", (TSIport)p)) ? "" : tsiregset.output("LogicaCoinc", (TSIport)p));
//         tmplist.append((zerosupp && !tsiregset.getregvalue("DivisoreFreCoinc", (TSIport)p)) ? "" : tsiregset.output("DivisoreFreCoinc", (TSIport)p));

         for(id=0; id<TSIPORTID_NUM; id++)
            tmplist.append((zerosupp && !tsiregset.getregvalue("DurataRitatdoCoinc", (TSIport)p, id)) ? "" : tsiregset.output("DurataRitatdoCoinc", (TSIport)p, id));

         tmplist.append((zerosupp && !tsiregset.getregvalue("EnableRitardoCoinc", (TSIport)p)) ? "" : tsiregset.output("EnableRitardoCoinc", (TSIport)p));
      }
   }

   for(id=0; id<(PORTINLINE_NUM / 2); id++)
      tmplist.append((zerosupp && !tsiregset.getregvalue("DurataRitatdoCoinc", X4, id)) ? "" : tsiregset.output("DurataRitatdoCoinc", X4, id));

   tmplist.append((zerosupp && !tsiregset.getregvalue("EnableRitardoCoinc", X4)) ? "" : tsiregset.output("EnableRitardoCoinc", X4));

   for(id=0; id<TL1_LINE_IDNUM; id++)
      tmplist.append((zerosupp && !tsiregset.getregvalue("Multiplexer19In", X4, id)) ? "" : tsiregset.output("Multiplexer19In", X4, id));

   for(id=0; id<TL1_LOGIC_IDNUM; id++)
      tmplist.append((zerosupp && !tsiregset.getregvalue("Multiplexer30Dl", X4, id)) ? "" : tsiregset.output("Multiplexer30Dl", X4, id));

   for(id=0; id<TL2_LINE_IDNUM; id++)
      tmplist.append((zerosupp && !tsiregset.getregvalue("Multiplexer8ptrg", XF, id)) ? "" : tsiregset.output("Multiplexer8ptrg", XF, id));

   for(id=0; id<TL2_LOGIC_IDNUM; id++)
      tmplist.append((zerosupp && !tsiregset.getregvalue("Multiplexer16Dl", XF, id)) ? "" : tsiregset.output("Multiplexer16Dl", XF, id));

   for(id=0; id<MUX_XA_IDNUM; id++)
      tmplist.append((zerosupp && !tsiregset.getregvalue("Multiplexer", XA, id)) ? "" : tsiregset.output("Multiplexer", XA, id));

   for(id=0; id<MUX_XB_IDNUM; id++)
      tmplist.append((zerosupp && !tsiregset.getregvalue("Multiplexer", XB, id)) ? "" : tsiregset.output("Multiplexer", XB, id));

   for(id=0; id<MUX_XC_IDNUM; id++)
      tmplist.append((zerosupp && !tsiregset.getregvalue("Multiplexer", XC, id)) ? "" : tsiregset.output("Multiplexer", XC, id));

   for(id=0; id<MUX_X4_IDNUM; id++)
      tmplist.append((zerosupp && !tsiregset.getregvalue("Multiplexer", X4, id)) ? "" : tsiregset.output("Multiplexer", X4, id));

   tmplist.append((zerosupp && !tsiregset.getregvalue("Multiplexer", XF)) ? "" : tsiregset.output("Multiplexer", XF));

   return tmplist;
}

bool QTopology::sendVMEConfig(void) {

   // connect to VME2NET
   VMEbus.connect();

   // RESET = 0x0010
   VMEbus.write(tsiregset.getregaddr("ControlWord", XF), 0x10);
   VMEbus.write(tsiregset.getregaddr("ControlWord", XF), STOP);

   if(VMEbus.online()) {

      dumpVMEConfig(false);    // update VME register class tsiregset

      VMEbus.write(tsiregset.getregaddr("MascheraIngressi", XA), tsiregset.getregvalue("MascheraIngressi", XA));
      VMEbus.write(tsiregset.getregaddr("MascheraIngressi", XB), tsiregset.getregvalue("MascheraIngressi", XB));
      VMEbus.write(tsiregset.getregaddr("MascheraIngressi", XC), tsiregset.getregvalue("MascheraIngressi", XC));
      VMEbus.write(tsiregset.getregaddr("MascheraIngressi", X4), tsiregset.getregvalue("MascheraIngressi", X4));

      VMEbus.write(tsiregset.getregaddr("AbilitazioneSimTrg", XA), 0);
      VMEbus.write(tsiregset.getregaddr("AbilitazioneSimTrg", XB), 0);
      VMEbus.write(tsiregset.getregaddr("AbilitazioneSimTrg", XC), 0);
      VMEbus.write(tsiregset.getregaddr("AbilitazioneSimTrg", X4), 0);

      VMEbus.write(tsiregset.getregaddr("LogicaCoinc", XA), tsiregset.getregvalue("LogicaCoinc", XA));
      VMEbus.write(tsiregset.getregaddr("LogicaCoinc", XB), tsiregset.getregvalue("LogicaCoinc", XB));
      VMEbus.write(tsiregset.getregaddr("LogicaCoinc", XC), tsiregset.getregvalue("LogicaCoinc", XC));

      VMEbus.write(tsiregset.getregaddr("DivisoreFreCoinc", XA), 0);
      VMEbus.write(tsiregset.getregaddr("DivisoreFreCoinc", XB), 0);
      VMEbus.write(tsiregset.getregaddr("DivisoreFreCoinc", XC), 0);

      VMEbus.write(tsiregset.getregaddr("DurataRitatdoCoinc", XA, 0), tsiregset.getregvalue("DurataRitatdoCoinc", XA, 0));
      VMEbus.write(tsiregset.getregaddr("DurataRitatdoCoinc", XA, 1), tsiregset.getregvalue("DurataRitatdoCoinc", XA, 1));
      VMEbus.write(tsiregset.getregaddr("DurataRitatdoCoinc", XA, 2), tsiregset.getregvalue("DurataRitatdoCoinc", XA, 2));
      VMEbus.write(tsiregset.getregaddr("DurataRitatdoCoinc", XA, 3), tsiregset.getregvalue("DurataRitatdoCoinc", XA, 3));

      VMEbus.write(tsiregset.getregaddr("DurataRitatdoCoinc", XB, 0), tsiregset.getregvalue("DurataRitatdoCoinc", XB, 0));
      VMEbus.write(tsiregset.getregaddr("DurataRitatdoCoinc", XB, 1), tsiregset.getregvalue("DurataRitatdoCoinc", XB, 1));
      VMEbus.write(tsiregset.getregaddr("DurataRitatdoCoinc", XB, 2), tsiregset.getregvalue("DurataRitatdoCoinc", XB, 2));
      VMEbus.write(tsiregset.getregaddr("DurataRitatdoCoinc", XB, 3), tsiregset.getregvalue("DurataRitatdoCoinc", XB, 3));

      VMEbus.write(tsiregset.getregaddr("DurataRitatdoCoinc", XC, 0), tsiregset.getregvalue("DurataRitatdoCoinc", XC, 0));
      VMEbus.write(tsiregset.getregaddr("DurataRitatdoCoinc", XC, 1), tsiregset.getregvalue("DurataRitatdoCoinc", XC, 1));
      VMEbus.write(tsiregset.getregaddr("DurataRitatdoCoinc", XC, 2), tsiregset.getregvalue("DurataRitatdoCoinc", XC, 2));
      VMEbus.write(tsiregset.getregaddr("DurataRitatdoCoinc", XC, 3), tsiregset.getregvalue("DurataRitatdoCoinc", XC, 3));

      VMEbus.write(tsiregset.getregaddr("DurataRitatdoCoinc", X4, 0), tsiregset.getregvalue("DurataRitatdoCoinc", X4, 0));
      VMEbus.write(tsiregset.getregaddr("DurataRitatdoCoinc", X4, 1), tsiregset.getregvalue("DurataRitatdoCoinc", X4, 1));
      VMEbus.write(tsiregset.getregaddr("DurataRitatdoCoinc", X4, 2), tsiregset.getregvalue("DurataRitatdoCoinc", X4, 2));
      VMEbus.write(tsiregset.getregaddr("DurataRitatdoCoinc", X4, 3), tsiregset.getregvalue("DurataRitatdoCoinc", X4, 3));
      VMEbus.write(tsiregset.getregaddr("DurataRitatdoCoinc", X4, 4), tsiregset.getregvalue("DurataRitatdoCoinc", X4, 4));
      VMEbus.write(tsiregset.getregaddr("DurataRitatdoCoinc", X4, 5), tsiregset.getregvalue("DurataRitatdoCoinc", X4, 5));
      VMEbus.write(tsiregset.getregaddr("DurataRitatdoCoinc", X4, 6), tsiregset.getregvalue("DurataRitatdoCoinc", X4, 6));
      VMEbus.write(tsiregset.getregaddr("DurataRitatdoCoinc", X4, 7), tsiregset.getregvalue("DurataRitatdoCoinc", X4, 7));

      VMEbus.write(tsiregset.getregaddr("EnableRitardoCoinc", XA), tsiregset.getregvalue("EnableRitardoCoinc", XA));
      VMEbus.write(tsiregset.getregaddr("EnableRitardoCoinc", XB), tsiregset.getregvalue("EnableRitardoCoinc", XB));
      VMEbus.write(tsiregset.getregaddr("EnableRitardoCoinc", XC), tsiregset.getregvalue("EnableRitardoCoinc", XC));
      VMEbus.write(tsiregset.getregaddr("EnableRitardoCoinc", X4), tsiregset.getregvalue("EnableRitardoCoinc", X4));

      VMEbus.write(tsiregset.getregaddr("Multiplexer", XA, 0), tsiregset.getregvalue("Multiplexer", XA, 0));
      VMEbus.write(tsiregset.getregaddr("Multiplexer", XA, 1), tsiregset.getregvalue("Multiplexer", XA, 1));
      VMEbus.write(tsiregset.getregaddr("Multiplexer", XA, 2), tsiregset.getregvalue("Multiplexer", XA, 2));
      VMEbus.write(tsiregset.getregaddr("Multiplexer", XA, 3), tsiregset.getregvalue("Multiplexer", XA, 3));

      VMEbus.write(tsiregset.getregaddr("Multiplexer", XB, 0), tsiregset.getregvalue("Multiplexer", XB, 0));
      VMEbus.write(tsiregset.getregaddr("Multiplexer", XB, 1), tsiregset.getregvalue("Multiplexer", XB, 1));
      VMEbus.write(tsiregset.getregaddr("Multiplexer", XB, 2), tsiregset.getregvalue("Multiplexer", XB, 2));
      VMEbus.write(tsiregset.getregaddr("Multiplexer", XB, 3), tsiregset.getregvalue("Multiplexer", XB, 3));

      VMEbus.write(tsiregset.getregaddr("Multiplexer", XC, 0), tsiregset.getregvalue("Multiplexer", XC, 0));
      VMEbus.write(tsiregset.getregaddr("Multiplexer", XC, 1), tsiregset.getregvalue("Multiplexer", XC, 1));
      VMEbus.write(tsiregset.getregaddr("Multiplexer", XC, 2), tsiregset.getregvalue("Multiplexer", XC, 2));
      VMEbus.write(tsiregset.getregaddr("Multiplexer", XC, 3), tsiregset.getregvalue("Multiplexer", XC, 3));

      VMEbus.write(tsiregset.getregaddr("Multiplexer", X4, 0), tsiregset.getregvalue("Multiplexer", X4, 0));
      VMEbus.write(tsiregset.getregaddr("Multiplexer", X4, 1), tsiregset.getregvalue("Multiplexer", X4, 1));
      VMEbus.write(tsiregset.getregaddr("Multiplexer", X4, 2), tsiregset.getregvalue("Multiplexer", X4, 2));
      VMEbus.write(tsiregset.getregaddr("Multiplexer", XF), tsiregset.getregvalue("Multiplexer", XF));

      VMEbus.write(tsiregset.getregaddr("Multiplexer19In", X4, 0), tsiregset.getregvalue("Multiplexer19In", X4, 0));
      VMEbus.write(tsiregset.getregaddr("Multiplexer19In", X4, 1), tsiregset.getregvalue("Multiplexer19In", X4, 1));
      VMEbus.write(tsiregset.getregaddr("Multiplexer19In", X4, 2), tsiregset.getregvalue("Multiplexer19In", X4, 2));
      VMEbus.write(tsiregset.getregaddr("Multiplexer19In", X4, 3), tsiregset.getregvalue("Multiplexer19In", X4, 3));
      VMEbus.write(tsiregset.getregaddr("Multiplexer19In", X4, 4), tsiregset.getregvalue("Multiplexer19In", X4, 4));
      VMEbus.write(tsiregset.getregaddr("Multiplexer19In", X4, 5), tsiregset.getregvalue("Multiplexer19In", X4, 5));
      VMEbus.write(tsiregset.getregaddr("Multiplexer19In", X4, 6), tsiregset.getregvalue("Multiplexer19In", X4, 6));
      VMEbus.write(tsiregset.getregaddr("Multiplexer19In", X4, 7), tsiregset.getregvalue("Multiplexer19In", X4, 7));
      VMEbus.write(tsiregset.getregaddr("Multiplexer19In", X4, 8), tsiregset.getregvalue("Multiplexer19In", X4, 8));
      VMEbus.write(tsiregset.getregaddr("Multiplexer19In", X4, 9), tsiregset.getregvalue("Multiplexer19In", X4, 9));

      VMEbus.write(tsiregset.getregaddr("Multiplexer30Dl", X4, 0), tsiregset.getregvalue("Multiplexer30Dl", X4, 0));
      VMEbus.write(tsiregset.getregaddr("Multiplexer30Dl", X4, 1), tsiregset.getregvalue("Multiplexer30Dl", X4, 1));
      VMEbus.write(tsiregset.getregaddr("Multiplexer30Dl", X4, 2), tsiregset.getregvalue("Multiplexer30Dl", X4, 2));
      VMEbus.write(tsiregset.getregaddr("Multiplexer30Dl", X4, 3), tsiregset.getregvalue("Multiplexer30Dl", X4, 3));

      VMEbus.write(tsiregset.getregaddr("Multiplexer8ptrg", XF, 0), tsiregset.getregvalue("Multiplexer8ptrg", XF, 0));
      VMEbus.write(tsiregset.getregaddr("Multiplexer8ptrg", XF, 1), tsiregset.getregvalue("Multiplexer8ptrg", XF, 1));
      VMEbus.write(tsiregset.getregaddr("Multiplexer8ptrg", XF, 2), tsiregset.getregvalue("Multiplexer8ptrg", XF, 2));
      VMEbus.write(tsiregset.getregaddr("Multiplexer8ptrg", XF, 3), tsiregset.getregvalue("Multiplexer8ptrg", XF, 3));

      VMEbus.write(tsiregset.getregaddr("Multiplexer16Dl", XF, 0), tsiregset.getregvalue("Multiplexer16Dl", XF, 0));
      VMEbus.write(tsiregset.getregaddr("Multiplexer16Dl", XF, 1), tsiregset.getregvalue("Multiplexer16Dl", XF, 1));

      return(true);

   } else {    // VME is offline

      return(false);
   }
}

bool QTopology::verifyVMEConfig(void) {

   uint32_t data = 0;
   int i, j;

   // connect to VME2NET
   VMEbus.connect();

   dumpVMEConfig(false);    // update VME register class tsiregset

   // get TSI status - if TSI is in STEADY RESET go to STOP state
   VMEbus.read(tsiregset.getregaddr("ControlWord", XF), &data);

   if(data == RESET)
      VMEbus.write(tsiregset.getregaddr("ControlWord", XF), STOP);

   for(i=0; i<(TSIPORT_NUM-1); i++) {
      VMEbus.read(tsiregset.getregaddr("MascheraIngressi", (TSIport)i), &data);
      if( data != tsiregset.getregvalue("MascheraIngressi", (TSIport)i) )
         return(false);
   }

   for(i=0; i<(TSIPORT_NUM-2); i++) {
      VMEbus.read(tsiregset.getregaddr("LogicaCoinc", (TSIport)i), &data);
      if( data != tsiregset.getregvalue("LogicaCoinc", (TSIport)i) )
         return(false);
   }

   for(i=0; i<(TSIPORT_NUM-2); i++) {
      for(j=0; j<4; j++) {
         VMEbus.read(tsiregset.getregaddr("DurataRitatdoCoinc", (TSIport)i, j), &data);
         if( data != tsiregset.getregvalue("DurataRitatdoCoinc", (TSIport)i, j) )
            return(false);
      }
   }

   for(j=0; j<8; j++) {
      VMEbus.read(tsiregset.getregaddr("DurataRitatdoCoinc", X4, j), &data);
      if( data != tsiregset.getregvalue("DurataRitatdoCoinc", X4, j) )
         return(false);
   }

   for(i=0; i<(TSIPORT_NUM-1); i++) {
      VMEbus.read(tsiregset.getregaddr("EnableRitardoCoinc", (TSIport)i), &data);
      if( data != tsiregset.getregvalue("EnableRitardoCoinc", (TSIport)i) )
         return(false);
   }

   for(j=0; j<MUX_XA_IDNUM; j++) {
      VMEbus.read(tsiregset.getregaddr("Multiplexer", XA, j), &data);
      if( data != tsiregset.getregvalue("Multiplexer", XA, j) )
         return(false);
   }

   for(j=0; j<MUX_XB_IDNUM; j++) {
      VMEbus.read(tsiregset.getregaddr("Multiplexer", XB, j), &data);
      if( data != tsiregset.getregvalue("Multiplexer", XB, j) )
         return(false);
   }

   for(j=0; j<MUX_XC_IDNUM; j++) {
      VMEbus.read(tsiregset.getregaddr("Multiplexer", XC, j), &data);
      if( data != tsiregset.getregvalue("Multiplexer", XC, j) )
         return(false);
   }

   for(j=0; j<MUX_X4_IDNUM; j++) {
      VMEbus.read(tsiregset.getregaddr("Multiplexer", X4, j), &data);
      if( data != tsiregset.getregvalue("Multiplexer", X4, j) )
         return(false);
   }

   for(j=0; j<MUX_XF_IDNUM; j++) {
      VMEbus.read(tsiregset.getregaddr("Multiplexer", XF, j), &data);
      if( data != tsiregset.getregvalue("Multiplexer", XF, j) )
         return(false);
   }

   for(j=0; j<10; j++) {
      VMEbus.read(tsiregset.getregaddr("Multiplexer19In", XF, j), &data);
      if( data != tsiregset.getregvalue("Multiplexer19In", XF, j) )
         return(false);
   }

   for(j=0; j<4; j++) {
      VMEbus.read(tsiregset.getregaddr("Multiplexer30Dl", XF, j), &data);
      if( data != tsiregset.getregvalue("Multiplexer30Dl", XF, j) )
         return(false);
   }

   for(j=0; j<4; j++) {
      VMEbus.read(tsiregset.getregaddr("Multiplexer8ptrg", XF, j), &data);
      if( data != tsiregset.getregvalue("Multiplexer8ptrg", XF, j) )
         return(false);
   }

   for(j=0; j<2; j++) {
      VMEbus.read(tsiregset.getregaddr("Multiplexer16Dl", XF, j), &data);
      if( data != tsiregset.getregvalue("Multiplexer16Dl", XF, j) )
         return(false);
   }



   return(true);
}
