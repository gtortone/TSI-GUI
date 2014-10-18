#include "QRegisterSet.h"

void QRegisterSet::init(void) {

   status.init(XF, "ControlWord", "index");

   input[XA].init(XA, "MascheraIngressi");
   input[XB].init(XB, "MascheraIngressi");
   input[XC].init(XC, "MascheraIngressi");
   input[X4].init(X4, "MascheraIngressi");

   siminput[XA].init(XA, "AbilitazioneSimTrg");
   siminput[XB].init(XB, "AbilitazioneSimTrg");
   siminput[XC].init(XC, "AbilitazioneSimTrg");
   siminput[X4].init(X4, "AbilitazioneSimTrg");

   simgen[XA].init(XA, "GenSimTrigger");
   simgen[XB].init(XB, "GenSimTrigger");
   simgen[XC].init(XC, "GenSimTrigger");
   simgen[X4].init(X4, "GenSimTrigger");

   coinclogic[XA].init(XA, "LogicaCoinc");
   coinclogic[XB].init(XB, "LogicaCoinc");
   coinclogic[XC].init(XC, "LogicaCoinc");

   for(int i=0; i<(TSIPORT_NUM-2); i++) { // Xa, Xb, Xc
      for(int j=0; j<PORTCOINC_NUM; j++) {

         coincfreq[i][j].init((TSIport)i, "ContatoreFreCoinc", "single", j);
      }
   }

   for(int i=0; i<(TSIPORT_NUM-2); i++) { // Xa, Xb, Xc
      for(int j=0; j<2; j++) {

         coincfreqof[i][j].init((TSIport)i, "ContOverFlowFreCoinc", "abs", j);
      }
   }

   coincfreqdiv[XA].init(XA, "DivisoreFreCoinc");
   coincfreqdiv[XB].init(XB, "DivisoreFreCoinc");
   coincfreqdiv[XC].init(XC, "DivisoreFreCoinc");

   for(int i=0; i<(TSIPORT_NUM - 2); i++) {  // Xa, Xb, Xc

      for(int j=0; j<TSIPORTID_NUM; j++) {

         coincwidth[i][j].init((TSIport)i, "DurataRitatdoCoinc", "width", j);
         coincdelay[i][j].init((TSIport)i, "DurataRitatdoCoinc", "delay", j);
      }
   }

   for(int i=0; i<(PORTINLINE_NUM / 2); i++) {  // X4

      linewidth[i].init(X4, "DurataRitatdoCoinc", "width", i);
      linedelay[i].init(X4, "DurataRitatdoCoinc", "delay", i);
   }

   coincenabledelay[XA].init(XA, "EnableRitardoCoinc");
   coincenabledelay[XB].init(XB, "EnableRitardoCoinc");
   coincenabledelay[XC].init(XC, "EnableRitardoCoinc");

   lineenabledelay.init(X4, "EnableRitardoCoinc");

   for(int i=0; i<MUX_XA_IDNUM; i++)
      muxXA[i].init(XA, "Multiplexer", "index", i);

   for(int i=0; i<MUX_XB_IDNUM; i++)
      muxXB[i].init(XB, "Multiplexer", "index", i);

   for(int i=0; i<MUX_XC_IDNUM; i++)
      muxXC[i].init(XC, "Multiplexer", "index", i);

   for(int i=0; i<MUX_X4_IDNUM; i++)
      muxX4[i].init(X4, "Multiplexer", "index", i);

   muxXF.init(XF, "Multiplexer");

   for(int i=0; i<TL1_LINE_IDNUM; i++)
      tl1sigin[i].init(X4, "Multiplexer19In", "tl1sigin", i);

   for(int i=0; i<TL1_LOGIC_IDNUM; i++)
      tl1logic[i].init(X4, "Multiplexer30Dl", "tl1logic", i);

   for(int i=0; i<TL2_LINE_IDNUM; i++)
      tl2sigin[i].init(XF, "Multiplexer8ptrg", "tl2sigin", i);

   for(int i=0; i<TL2_LOGIC_IDNUM; i++)
      tl2logic[i].init(XF, "Multiplexer16Dl", "tl2logic", i);

   trigtot.init(XF, "TriggerTotali", "single");
   trigacqtot.init(XF, "TriggerAcquisiti", "single");
   deadtime.init(XF, "TempoMorto", "single");
   trigrate.init(XF, "TempoRate", "single");
   uptime.init(XF, "RunTime", "single");
}

void QRegisterSet::clearall(void) {

   status.clear();

   for(int i=0; i<(TSIPORT_NUM-1); i++) {   // Xa, Xb, Xc, X4
      input[i].clear();
      siminput[i].clear();
      simgen[i].clear();
   }

   for(int i=0; i<(TSIPORT_NUM-2); i++) { // Xa, Xb, Xc

      coinclogic[i].clear();
      coincfreqdiv[i].clear();
      coincenabledelay[i].clear();
   }

   for(int i=0; i<(TSIPORT_NUM-2); i++) { // Xa, Xb, Xc
      for(int j=0; j<PORTCOINC_NUM; j++) {

         coincfreq[i][j].clear();
      }
   }

   for(int i=0; i<(TSIPORT_NUM-2); i++) { // Xa, Xb, Xc
      for(int j=0; j<2; j++) {

         coincfreqof[i][j].clear();
      }
   }

   for(int i=0; i<(TSIPORT_NUM-2); i++) { // Xa, Xb, Xc
      for(int j=0; j<TSIPORTID_NUM; j++) {

         coincwidth[i][j].clear();
         coincdelay[i][j].clear();
      }
   }

   for(int i=0; i<(PORTINLINE_NUM / 2); i++) {

      linewidth[i].clear();
      linedelay[i].clear();
   }

   lineenabledelay.clear();

   for(int i=0; i<MUX_XA_IDNUM; i++)
      muxXA[i].clear();

   for(int i=0; i<MUX_XB_IDNUM; i++)
      muxXB[i].clear();

   for(int i=0; i<MUX_XC_IDNUM; i++)
      muxXC[i].clear();

   for(int i=0; i<MUX_X4_IDNUM; i++)
      muxX4[i].clear();

   muxXF.clear();

   for(int i=0; i<TL1_LINE_IDNUM; i++)
      tl1sigin[i].clear();

   for(int i=0; i<TL1_LOGIC_IDNUM; i++)
      tl1logic[i].clear();

   for(int i=0; i<TL2_LINE_IDNUM; i++)
      tl2sigin[i].clear();

   for(int i=0; i<TL2_LOGIC_IDNUM; i++)
      tl2logic[i].clear();

   trigtot.clear();
   trigacqtot.clear();
   deadtime.clear();
   trigrate.clear();
   uptime.clear();
}

void QRegisterSet::setbaddr(uint32_t value) {

   baddr = value;
}

uint32_t QRegisterSet::getbaddr(void) {

   return baddr;
}

void QRegisterSet::set(TSIstatus s) {

   status.set(s);
}

void QRegisterSet::set(TSIport p, TSIline l, enum LineEnable e) {

   input[p].set(l, e);
}

void QRegisterSet::setsim(TSIport p, TSIline l, enum LineEnable e) {

   siminput[p].set(l, e);
}

void QRegisterSet::set(TSIport p) {

   simgen[p].set();
}

void QRegisterSet::set(TSIport p, TSIcoinc c, enum CoincOp op) {

   coinclogic[p].set(c, op);
}

void QRegisterSet::set(TSIport p, TSIcoinc c, enum CoincFreqDiv div) {

   coincfreqdiv[p].set(c, div);
}

void QRegisterSet::set(TSIport p, TSIcoinc c, enum CoincPeriod w) {

   coincwidth[p][c/2].set(c, w);
}

void QRegisterSet::set(TSIport p, TSIcoinc c, enum CoincDelay d) {

   coincdelay[p][c/2].set(c, d);
}

void QRegisterSet::set(TSIport p, TSIcoinc c, enum DelayEnable e) {

   coincenabledelay[p].set(c, e);
}

void QRegisterSet::set(TSIline l, enum DelayEnable e) {

   lineenabledelay.set(l, e);
}

void QRegisterSet::set(TSIline l, enum CoincPeriod w) {

   linewidth[l/2].set(l, w);
}

void QRegisterSet::set(TSIline l, enum CoincDelay d) {

   linedelay[l/2].set(l, d);
}

void QRegisterSet::set(TSIport p, uint8_t id, uint8_t index) {  // for multiplexer registers

   if(p == XA)
      muxXA[id].set(index);
   else if(p == XB)
      muxXB[id].set(index);
   else if(p == XC)
      muxXC[id].set(index);
   else if(p == X4)
      muxX4[id].set(index);
   else if(p == XF)
      muxXF.set(index);
}

void QRegisterSet::set(TL1SignalIn line, uint8_t id, uint8_t input) {

   tl1sigin[id].set(line, input);
}

void QRegisterSet::set(TL1SignalOut line, uint8_t id, uint8_t logic) {

   tl1logic[id].set(line, logic);
}

void QRegisterSet::set(TL2SignalIn line, uint8_t id, uint8_t input) {

   tl2sigin[id].set(line, input);
}

void QRegisterSet::set(TL2SignalOut line, uint8_t id, uint8_t logic) {

   tl2logic[id].set(line, logic);
}

TSIstatus QRegisterSet::parse_status(void) {

   return(status.get());
}

QString QRegisterSet::parse_status(TSIstatus s) {

   if(s == RESET)
      return(TSIstatus_str[2]);
   else
      return(TSIstatus_str[s]);
}

uint32_t QRegisterSet::read_coincfreq(TSIport p, TSIcoinc c) {

   return(coincfreq[p][c].get());
}

uint8_t QRegisterSet::read_coincfreqof(TSIport p, TSIcoinc c) {

   return(coincfreqof[p][c/4].get(c));
}

uint8_t QRegisterSet::parse_coincfreqdiv(TSIcoinc c, uint16_t data) {

   uint8_t div = coincfreqdiv[XA].parse(c, data);     // due to problem with class (static) method...

   if(div == 0)
      return(1);
   else if(div == 1)
      return(2);
   else if(div == 2)
      return(10);
   else if(div == 3)
      return(100);

   return div;
}

QString QRegisterSet::getregname(QString pfx, TSIport p, uint8_t id) {

   QString retval = "";

   if(pfx == "ControlWord") {

      retval = status.getname();

   } else if(pfx == "MascheraIngressi") {

      retval = input[p].getname();

   } else if(pfx == "AbilitazioneSimTrg") {

      retval = siminput[p].getname();

   } else if(pfx == "GenSimTrigger") {

      retval = simgen[p].getname();

   } else if(pfx == "LogicaCoinc") {

      retval = coinclogic[p].getname();

   } else if(pfx == "ContatoreFreCoinc") {

      retval = coincfreq[p][id].getname();

   } else if(pfx == "ContOverFlowFreCoinc") {

      retval = coincfreqof[p][id].getname();

   } else if(pfx == "DivisoreFreCoinc") {

      retval = coincfreqdiv[p].getname();

   } else if(pfx == "DurataRitatdoCoinc") {

      if(p == X4)
         retval = linewidth[id].getname();
      else
         retval = coincwidth[p][id].getname();

   } else if(pfx == "EnableRitardoCoinc") {

      if(p == X4)
         retval = lineenabledelay.getname();
      else
         retval = coincenabledelay[p].getname();

   } else if(pfx == "Multiplexer19In") {

      retval = tl1sigin[id].getname();

   } else if(pfx == "Multiplexer30Dl") {

      retval = tl1logic[id].getname();

   } else if(pfx == "Multiplexer8ptrg") {

      retval = tl2sigin[id].getname();

   } else if(pfx == "Multiplexer16Dl") {

      retval = tl2logic[id].getname();

   } else if(pfx == "Multiplexer") {

      if(p == XA)
         retval = muxXA[id].getname();
      else if(p == XB)
         retval = muxXB[id].getname();
      else if(p == XC)
         retval = muxXC[id].getname();
      else if(p == X4)
         retval = muxX4[id].getname();
      else if(p == XF)
         retval = muxXF.getname();

   } else if(pfx == "TriggerTotali") {

      retval = trigtot.getname();

   } else if(pfx == "TriggerAcquisiti") {

      retval = trigacqtot.getname();

   } else if(pfx == "TempoMorto") {

      retval = deadtime.getname();

   } else if(pfx == "TempoRate") {

      retval = trigrate.getname();

   } else if(pfx == "RunTime") {

      retval = uptime.getname();
   }

   return(retval);
}

uint32_t QRegisterSet::getregaddr(QString pfx, TSIport p, uint8_t id) {

   uint16_t retval = 0;

   if(pfx == "ControlWord") {

      retval = status.getaddr();

   } else if(pfx == "MascheraIngressi") {

      retval = input[p].getaddr();

   } else if(pfx == "AbilitazioneSimTrg") {

      retval = siminput[p].getaddr();

   } else if(pfx == "GenSimTrigger") {

      retval = simgen[p].getaddr();

   } else if(pfx == "LogicaCoinc") {

      retval = coinclogic[p].getaddr();

   } else if(pfx == "ContatoreFreCoinc") {

      retval = coincfreq[p][id].getaddr();

   } else if(pfx == "ContOverFlowFreCoinc") {

      retval = coincfreqof[p][id].getaddr();

   } else if(pfx == "DivisoreFreCoinc") {

      retval = coincfreqdiv[p].getaddr();

   } else if(pfx == "DurataRitatdoCoinc") {

      if(p == X4)
         retval = linewidth[id].getaddr();
      else
         retval = coincwidth[p][id].getaddr();

   } else if(pfx == "EnableRitardoCoinc") {

      if(p == X4)
         retval = lineenabledelay.getaddr();
      else
         retval = coincenabledelay[p].getaddr();

   } else if(pfx == "Multiplexer19In") {

      retval = tl1sigin[id].getaddr();

   } else if(pfx == "Multiplexer30Dl") {

      retval = tl1logic[id].getaddr();

   } else if(pfx == "Multiplexer8ptrg") {

      retval = tl2sigin[id].getaddr();

   } else if(pfx == "Multiplexer16Dl") {

      retval = tl2logic[id].getaddr();

   } else if(pfx == "Multiplexer") {

      if(p == XA)
         retval = muxXA[id].getaddr();
      else if(p == XB)
         retval = muxXB[id].getaddr();
      else if(p == XC)
         retval = muxXC[id].getaddr();
      else if(p == X4)
         retval = muxX4[id].getaddr();
      else if(p == XF)
         retval = muxXF.getaddr();

   } else if(pfx == "TriggerTotali") {

      retval = trigtot.getaddr();

   } else if(pfx == "TriggerAcquisiti") {

      retval = trigacqtot.getaddr();

   } else if(pfx == "TempoMorto") {

      retval = deadtime.getaddr();

   } else if(pfx == "TempoRate") {

      retval = trigrate.getaddr();

   } else if(pfx == "RunTime") {

      retval = uptime.getaddr();
   }

   return(baddr + retval);    // add VME base address
}

uint32_t QRegisterSet::getregvalue(QString pfx, TSIport p, uint8_t id) {

   uint32_t retval = 0;

   if(pfx == "ControlWord") {

      retval = status.getvalue();

   } else if(pfx == "MascheraIngressi") {

      retval = input[p].getvalue();

   } else if(pfx == "AbilitazioneSimTrg") {

      retval = siminput[p].getvalue();

   } else if(pfx == "GenSimTrigger") {

      retval = simgen[p].getvalue();

   } else if(pfx == "LogicaCoinc") {

      retval = coinclogic[p].getvalue();

   } else if(pfx == "ContatoreFreCoinc") {

      retval = coincfreq[p][id].getvalue();

   } else if(pfx == "ContOverFlowFreCoinc") {

      retval = coincfreqof[p][id].getvalue();

   } else if(pfx == "DivisoreFreCoinc") {

      retval = coincfreqdiv[p].getvalue();

   } else if(pfx == "DurataRitatdoCoinc") {

      if(p == X4)
         retval = linewidth[id].getvalue() | linedelay[id].getvalue();
      else
         retval = coincwidth[p][id].getvalue() | coincdelay[p][id].getvalue();

   } else if(pfx == "EnableRitardoCoinc") {

      if(p == X4)
         retval = lineenabledelay.getvalue();
      else
         retval = coincenabledelay[p].getvalue();

   } else if(pfx == "Multiplexer19In") {

      retval = tl1sigin[id].getvalue();

   } else if(pfx == "Multiplexer30Dl") {

      retval = tl1logic[id].getvalue();

   } else if(pfx == "Multiplexer8ptrg") {

      retval = tl2sigin[id].getvalue();

   } else if(pfx == "Multiplexer16Dl") {

      retval = tl2logic[id].getvalue();

   } else if(pfx == "Multiplexer") {

      if(p == XA)
         retval = muxXA[id].getvalue();
      else if(p == XB)
         retval = muxXB[id].getvalue();
      else if(p == XC)
         retval = muxXC[id].getvalue();
      else if(p == X4)
         retval = muxX4[id].getvalue();
      else if(p == XF)
         retval = muxXF.getvalue();

   } else if(pfx == "TriggerTotali") {

      retval = trigtot.getvalue();

   } else if(pfx == "TriggerAcquisiti") {

      retval = trigacqtot.getvalue();

   } else if(pfx == "TempoMorto") {

      retval = deadtime.getvalue();

   } else if(pfx == "TempoRate") {

      retval = trigrate.getvalue();

   } else if(pfx == "RunTime") {

      retval = uptime.getvalue();
   }

   return(retval);
}

void QRegisterSet::setregvalue(QString pfx, TSIport p, uint32_t value, uint8_t id) {

   if(pfx == "ControlWord") {

      status.setvalue(value);

   } else if(pfx == "MascheraIngressi") {

      input[p].setvalue(value);

   } else if(pfx == "AbilitazioneSimTrg") {

      siminput[p].setvalue(value);

   } else if(pfx == "GenSimTrigger") {

      simgen[p].setvalue(value);

   } else if(pfx == "LogicaCoinc") {

      coinclogic[p].setvalue(value);

   } else if(pfx == "ContatoreFreCoinc") {

      coincfreq[p][id].setvalue(value);

   } else if(pfx == "ContOverFlowFreCoinc") {

      coincfreqof[p][id].setvalue(value);

   } else if(pfx == "DivisoreFreCoinc") {

      coincfreqdiv[p].setvalue(value);

   } else if(pfx == "DurataRitatdoCoinc") {

      if(p == X4) {

         linewidth[id].setvalue(value & 0x0F0F);
         linedelay[id].setvalue(value & 0xF0F0);

      } else {

         coincwidth[p][id].setvalue(value & 0x0F0F);
         coincdelay[p][id].setvalue(value & 0xF0F0);
      }

   } else if(pfx == "EnableRitardoCoinc") {

      if(p == X4)
         lineenabledelay.setvalue(value);
      else
         coincenabledelay[p].setvalue(value);

   } else if(pfx == "Multiplexer19In") {

      tl1sigin[id].setvalue(value);

   } else if(pfx == "Multiplexer30Dl") {

      tl1logic[id].setvalue(value);

   } else if(pfx == "Multiplexer8ptrg") {

      tl2sigin[id].setvalue(value);

   } else if(pfx == "Multiplexer16Dl") {

      tl2logic[id].setvalue(value);

   } else if(pfx == "Multiplexer") {

      if(p == XA)
         muxXA[id].setvalue(value);
      else if(p == XB)
         muxXB[id].setvalue(value);
      else if(p == XC)
         muxXC[id].setvalue(value);
      else if(p == X4)
         muxX4[id].setvalue(value);
      else if(p == XF)
         muxXF.setvalue(value);
   }
}

QString QRegisterSet::output(QString pfx, TSIport p, uint8_t id) {

   QString str;
   QString hex;

   hex.sprintf("%04X",getregvalue(pfx, p, id));

   str = getregname(pfx, p, id) + " = 0x" + hex;
   //qDebug() << str;

   return str;
}
