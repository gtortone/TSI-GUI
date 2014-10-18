#ifndef QMETRICS_H
#define QMETRICS_H

#include <QStringList>
#include <QVector>

#define DTYPE_NUM                5
#define TSIPORT_NUM	 	 5
#define TSIPORTID_NUM	 	 4
#define PORTLINE_NUM            20
#define PORTINLINE_NUM          16
#define PORTOUTLINE_NUM          4
#define PORTCOINC_NUM	 	 8
#define COINCOP_NUM	 	 2
#define COINCFREQDIV_NUM	 4
#define COINCDELAY_NUM		16
#define COINCPERIOD_NUM		16
#define MONPARAM_NUM		43

#define TRG_MONGROUP             0
#define COINC_MONGROUP           1
#define FREQDIV_MONGROUP         2
#define OTHER_MONGROUP           3
#define COINCSTRETCH_MONGROUP    4
#define TRGSTRETCH_MONGROUP      5

#define TL1_LINE_IDNUM           10
#define TL1_LOGIC_IDNUM          4

#define TL2_LINE_IDNUM           4
#define TL2_LOGIC_IDNUM          2

#define MUX_XA_IDNUM             4
#define MUX_XB_IDNUM             4
#define MUX_XC_IDNUM             4
#define MUX_X4_IDNUM             3
#define MUX_XF_IDNUM             1

enum TSIstatus { STOP = 0x00, RUN = 0x01, RESET = 0x10 };
static QStringList const TSIstatus_str = (QStringList() << "STOP" << "RUN" << "STEADY RESET");

// ###################################
// Detector
// ###################################

enum DetectorType { UNDEF_DTYPE = -1, DSSD = 0, PAD, PPAC, MONITOR, IONCHAM };
static QStringList const DetectorType_str = (QStringList() << "DSSD" << "PAD" << "PPAC" << "MONITOR" << "Ion Cham" );
static QVector<unsigned int> const DetectorType_min = (QVector<unsigned int>() << 1 << 0 << 0 << 0 << 0 );
static QVector<unsigned int> const DetectorType_max = (QVector<unsigned int>() << 3 << 1 << 1 << 1 << 1 );

enum TSIport { UNDEF_PORT = -1, XA = 0, XB, XC, X4, XF };
static QStringList const TSIport_str = (QStringList() << "Xa" << "Xb" << "Xc" << "X4" << "XF");

enum TSIline { UNDEF_LINE = -1, TRG0 = 0, TRG1, TRG2, TRG3, TRG4, TRG5, TRG6, TRG7, TRG8, TRG9, TRG10, TRG11, TRG12, TRG13, TRG14, TRG15 };
static QStringList const TSIline_str = (QStringList() << "TRG0" << "TRG1" << "TRG2" << "TRG3" << "TRG4" << "TRG5" << "TRG6" << "TRG7" << "TRG8" << "TRG9" << "TRG10" << "TRG11" << "TRG12" << "TRG13" << "TRG14" << "TRG15");

enum TSImon { UNDEF_MON = -1, MON0 = 0, MON1, MON2, MON3 };
static QStringList const TSImon_str (QStringList() << "MON0" << "MON1" << "MON2" << "MON3");
static QVector<int> const TSImon_max = (QVector<int>() << 4 << 4 << 4 << 3 << 1);

enum TSIcoinc { UNDEF_COINC = -1, COINC0 = 0, COINC1, COINC2, COINC3, COINC4, COINC5, COINC6, COINC7 };
static QStringList const TSIcoinc_str = (QStringList() << "COINC0" << "COINC1" << "COINC2" << "COINC3" << "COINC4" << "COINC5" << "COINC6" << "COINC7");

enum DetectorFace { UNDEF_FACE = -1, FACE_BOTH = 0, FACE_X, FACE_Y };
static QStringList const DetectorFace_str = (QStringList()) << "BOTH" << "X" << "Y";

enum LineEnable { UNDEF_ENABLE = -1, DISABLED = 0, ENABLED };
static QStringList const DetEnabled_str = (QStringList() << "NO" << "YES");

enum CoincOp { UNDEF_COINCOP = -1, AND = 0, OR };
static QStringList const CoincOp_str = (QStringList() << "X AND Y" << "X OR Y");

enum CoincFreqDiv { UNDEF_DIV = -1, DIV1 = 0, DIV2, DIV10, DIV100 };
static QStringList const CoincFreqDiv_str = (QStringList() << "1" << "2" << "10" << "100");

enum CoincPeriod { UNDEF_PER = -1, PER0 = 0, PER1, PER2, PER3, PER4, PER5, PER6, PER7, PER8, PER9, PER10, PER11, PER12, PER13, PER14, PER15 };
static QStringList const CoincPeriod_str = (QStringList() << "PER0" << "PER1" << "PER2" << "PER3" << "PER4" << "PER5" << "PER6" << "PER7" << "PER8" << "PER9" << "PER10" << "PER11" << "PER12"  << "PER13" << "PER14" << "PER15");
static QStringList const CoincPeriod_strval = (QStringList() << "0 ns" << "37.5 ns" << "50 ns" << "62.5 ns" << "75 ns" << "87.5 ns" << "100 ns" << "112.5 ns" << "125 ns" << "137.5 ns" << "150 ns" << "162.5 ns" << "175 ns" << "187.5 ns"  << "200 ns" << "212.5 ns" );

enum DelayEnable { UNDEF_DELAY = -1, DELAYOFF = 0, DELAYON };
enum CoincDelay { UNDEF_DEL = -1, DEL0 = 0, DEL1, DEL2, DEL3, DEL4, DEL5, DEL6, DEL7, DEL8, DEL9, DEL10, DEL11, DEL12, DEL13, DEL14, DEL15 };
static QStringList const CoincDelay_str = (QStringList() << "DEL0" << "DEL1" << "DEL2" << "DEL3" << "DEL4" << "DEL5" << "DEL6" << "DEL7" << "DEL8" << "DEL9" << "DEL10" << "DEL11" << "DEL12"  << "DEL13" << "DEL14" << "DEL15");
static QStringList const CoincDelay_strval = (QStringList() << "0 ns" << "37.5 ns" << "50 ns" << "62.5 ns" << "75 ns" << "87.5 ns" << "100 ns" << "112.5 ns" << "125 ns" << "137.5 ns" << "150 ns" << "162.5 ns" << "175 ns" << "187.5 ns"  << "200 ns" << "212.5 ns" );

// QGate
enum QGateType { AND_G, OR_G };
static QStringList const QGateType_str = (QStringList() << "AND" << "OR");

// ###################################
// TRIGGER LEVEL I
// ###################################

#define TL1GATE_NUM	30
#define TL1SIGIN_NUM	19
#define TL1SIGPT_NUM	 6
#define TL1SIGOUT_NUM	 8

//enum TL1SignalIn { UNDEF_TL1IN = -2, AVAIL_TL1IN = -1, NONE_TL1IN = 0, X4_TRG0, X4_TRG1, X4_TRG2, X4_TRG3, X4_TRG4, X4_TRG5, X4_TRG6, X4_TRG7, X4_TRG8,
//                   X4_TRG9, X4_TRG10, X4_TRG11, X4_TRG12, X4_TRG13, X4_TRG14, X4_TRG15, TRGXa, TRGXb, TRGXc };
//static QStringList const TL1SignalIn_str = (QStringList() << "NONE_IN" << "X4-TRG0" << "X4-TRG1" << "X4-TRG2" << "X4-TRG3" << "X4-TRG4" << "X4-TRG5" << "X4-TRG6" << "X4-TRG7" << "X4-TRG8" << "X4-TRG9" << "X4-TRG10" << "X4-TRG11" << "X4-TRG12" << "X4-TRG13" << "X4-TRG14" << "X4-TRG15" << "TRGXa" << "TRGXb" << "TRGXc");

enum TL1SignalIn { UNDEF_TL1IN = -2, AVAIL_TL1IN = -1, X4_TRG0 = 0, X4_TRG1, X4_TRG2, X4_TRG3, X4_TRG4, X4_TRG5, X4_TRG6, X4_TRG7, X4_TRG8,
                   X4_TRG9, X4_TRG10, X4_TRG11, X4_TRG12, X4_TRG13, X4_TRG14, X4_TRG15, TRGXa, TRGXb, TRGXc };
static QStringList const TL1SignalIn_str = (QStringList() << "X4-TRG0" << "X4-TRG1" << "X4-TRG2" << "X4-TRG3" << "X4-TRG4" << "X4-TRG5" << "X4-TRG6" << "X4-TRG7" << "X4-TRG8" << "X4-TRG9" << "X4-TRG10" << "X4-TRG11" << "X4-TRG12" << "X4-TRG13" << "X4-TRG14" << "X4-TRG15" << "TRGXa" << "TRGXb" << "TRGXc");


//enum TL1SignalPt { UNDEF_PT = -1, S0, S1, S2, S3, S4, S5 };
//static QStringList const TL1SignalPt_str = (QStringList() << "PT0" << "PT1" << "PT2" << "PT3" << "PT4" << "PT5");

enum TL1SignalOut { AVAIL_TL1OUT = -1, PTRG0_TL1OUT, PTRG1_TL1OUT, PTRG2_TL1OUT, PTRG3_TL1OUT, PTRG4_TL1OUT, PTRG5_TL1OUT, PTRG6_TL1OUT, PTRG7_TL1OUT };
static QStringList const TL1SignalOut_str = (QStringList() << "PTRG0" << "PTRG1" << "PTRG2" << "PTRG3" << "PTRG4" << "PTRG5" << "PTRG6" << "PTRG7");

// ###################################
// TRIGGER LEVEL II
// ###################################

#define TL2GATE_NUM     16
#define TL2SIGIN_NUM	 8
#define TL2SIGOUT_NUM	 4

enum TL2SignalIn { UNDEF_TL2IN = -2, AVAIL_TL2IN = -1, NONE_TL2IN, PTRG0_TL2IN, PTRG1_TL2IN, PTRG2_TL2IN, PTRG3_TL2IN, PTRG4_TL2IN, PTRG5_TL2IN, PTRG6_TL2IN, PTRG7_TL2IN };
static QStringList const TL2SignalIn_str = (QStringList() << "PTRG0" << "PTRG1" << "PTRG2" << "PTRG3" << "PTRG4" << "PTRG5" << "PTRG6" << "PTRG7");

enum TL2SignalOut { AVAIL_TL2OUT = -1, PTRIN0, PTRIN1, PTRIN2, PTRIN3 };
static QStringList const TL2SignalOut_str = (QStringList() << "PTRIN0" << "PTRIN1" << "PTRIN2" << "PTRIN3");

#endif
