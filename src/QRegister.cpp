#include "QRegister.h"

#include <QFile>
#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <cmath>

// ################################### VME REGISTER ControlWord ###################################

// ########## QRegTSIStatus ##########

void QRegTSIStatus::set(enum TSIstatus s) {

   if(s == STOP)
      value = 0x00;
   else if(s == RUN)
      value = 0x01;
   else if(s == RESET)
      value = 0x10;
}

enum TSIstatus QRegTSIStatus::get(void) {

   return((TSIstatus) value);
}

// ################################### VME REGISTER MascheraIngressi{Xa,Xb,Xc,X4} ###################################

// ########## QRegTSIInput ##########

void QRegTSIInput::set(TSIline l, enum LineEnable e) {

   if(!e)
      value -= mask[l];
   else
      value += mask[l];
}

// ################################### VME REGISTER AbilitazioneSimTrg{Xa,Xb,Xc,X4} ###################################

// ########## QRegSimInput ##########

void QRegSimInput::set(TSIline l, enum LineEnable e) {

   if(!e)
      value -= mask[l];
   else
      value += mask[l];
}

// ################################### VME REGISTER GenSimTrigger{Xa,Xb,Xc,X4} ###################################

// ########## QRegSimGen ##########

void QRegSimGen::set(void) {

   value = 0x0001;      // fixed value
}

// ################################### VME REGISTER LogicaCoinc{Xa,Xb,Xc} ###################################

// ########## QRegCoincLogic ##########

void QRegCoincLogic::set(TSIcoinc c, enum CoincOp op) {

   if(!op)
      value -= mask[c];
   else
      value += mask[c];
}

// ################################### VME REGISTER ContatoreFreCoinc{Xa0...7, Xb0...7, Xc0...7} ###################################

// ########## QRegCoincFreq ##########

void QRegCoincFreq::set(uint32_t val) {

   value = val;
}

uint32_t QRegCoincFreq::get(void) {

   return value;
}

// ################################### VME REGISTER ContatoreOverFlowFreCoinc{Xa0...7, Xb0...7, Xc0...7} ###################################

// ########## QRegCoincFreqOF ##########

void QRegCoincFreqOF::set(TSIcoinc c, uint8_t val) {

   if(!val)
      value &= ~( (int)(std::pow(2, mask_width[c]) - 1) << mask_pos[c] );
   else
      value |= (val << mask_pos[c]);
}

uint8_t QRegCoincFreqOF::get(TSIcoinc c) {

   return( (value >> mask_pos[c]) & (int)(std::pow(2, mask_width[c]) - 1) );
}

// ################################### VME REGISTER DivisoreFreCoinc{Xa,Xb,Xc} ###################################

// ########## QRegCoincFreqdiv ##########

void QRegCoincFreqdiv::set(TSIcoinc c, enum CoincFreqDiv div) {

   if(!div)
      value &= ~( (int)(std::pow(2, mask_width[c]) - 1) << mask_pos[c] );
   else
      value |= (div << mask_pos[c]);
}

uint8_t QRegCoincFreqdiv::get(TSIcoinc c) {

   return( (value & (int)(std::pow(2, mask_width[c]) - 1)) >> mask_pos[c]);
}

uint8_t QRegCoincFreqdiv::parse(TSIcoinc c, uint16_t data) {
   
   return( (data >> mask_pos[c]) & (int)(std::pow(2, mask_width[c]) - 1) );
}

// ################################### VME REGISTER DurataRitardoCoinc{Xa0...3, Xb0...3, Xc0...3} ###################################
// == W I D T H ==

void QRegCoincWidth::set(TSIcoinc c, enum CoincPeriod w) {

   if(!w)
      value &= ~( (int)(std::pow(2, mask_width[c]) - 1) << mask_pos[c] );
   else
      value |= (w << mask_pos[c]);
}

// ################################### VME REGISTER DurataRitardoCoinc{Xa0...3, Xb0...3, Xc0...3} ###################################
// == D E L A Y ==

void QRegCoincDelay::set(TSIcoinc c, enum CoincDelay d) {

   if(!d)
      value &= ~( (int)(std::pow(2, mask_width[c]) - 1) << mask_pos[c] );
   else
      value |= (d << mask_pos[c]);
}

// ################################### VME REGISTER DurataRitardo{X40...7} ###################################
// == W I D T H ==

void QRegLineWidth::set(TSIline l, enum CoincPeriod w) {

   if(!w)
      value &= ~( (int)(std::pow(2, mask_width[l]) - 1) << mask_pos[l] );
   else
      value |= (w << mask_pos[l]);
}

// ################################### VME REGISTER DurataRitardo{X40...7} ###################################
// == D E L A Y ==

void QRegLineDelay::set(TSIline l, enum CoincDelay d) {

   if(!d)
      value &= ~( (int)(std::pow(2, mask_width[l]) - 1) << mask_pos[l] );
   else
      value |= (d << mask_pos[l]);
}

// ################################### VME REGISTER EnableRitardoCoinc{Xa,Xb,Xc} ###################################

// ########## QRegCoincEnabledelay ##########

void QRegCoincEnabledelay::set(TSIcoinc c, enum DelayEnable e) {

   if(!e)
      value &= ~( (int)(std::pow(2, mask_width[c]) - 1) << mask_pos[c] );
   else
      value |= (e << mask_pos[c]);
}

// ################################### VME REGISTER EnableRitardo{X4} ###################################

// ########## QRegLineEnabledelay ##########

void QRegLineEnabledelay::set(TSIline l, enum DelayEnable e) {

   if(!e)
      value &= ~( (int)(std::pow(2, mask_width[l]) - 1) << mask_pos[l] );
   else
      value |= (e << mask_pos[l]);
}

// #################### VME REGISTER Multiplexer{XaUno....Quattro, XbUno...Quattro, XcUno...Quattro, X4Uno...Tre, Xf} ####################

void QRegMultiplexer::set(uint8_t index) {

   value = mask[index];    // copy mask to value
};

// ################################### VME REGISTER Multiplexer19In{X40...9} ###################################

// ########## QRegTL1SigIn ##########

void QRegTL1SigIn::set(TL1SignalIn line, uint8_t input) {

   value |= (line << mask_pos[input]);
}

// ################################### VME REGISTER Multiplexer30Dl{X40...3} ###################################

// ########## QRegTL1Logic ##########

void QRegTL1Logic::set(TL1SignalOut line, uint8_t logic) {

   value |= (logic << mask_pos[line]);
}

// ################################### VME REGISTER Multiplexer8ptrg{Xf0...3} ###################################

// ########## QRegTL2SigIn ##########

void QRegTL2SigIn::set(TL2SignalIn line, uint8_t input) {

   value |= (line << mask_pos[input]);
}

// ################################### VME REGISTER Multiplexer16Dl{Xf0...1} ###################################

// ########## QRegTL2Logic ##########

void QRegTL2Logic::set(TL2SignalOut line, uint8_t logic) {

   value |= (logic << mask_pos[line]);
}
