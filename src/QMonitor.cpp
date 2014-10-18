#include "QMonitor.h"

QMonitorParamSet::QMonitorParamSet(void) {

   _init = false;
}

void QMonitorParamSet::init(void) {

   if(!_init) {    // to avoid multiple initialization

      _init = true;

      // define UNDEF QMonitorInfo
      undef_mpinfo.port = UNDEF_PORT;
      undef_mpinfo.mline = UNDEF_MON;
      undef_mpinfo.portname = "undefined port";
      undef_mpinfo.address = 0;
      undef_mpinfo.seqid = 0;
      undef_mpinfo.tag = 0;
      undef_mpinfo.bindline = UNDEF_LINE;
      undef_mpinfo.group = -1;
      undef_mpinfo.desc = "undefined QMonitorInfo";

      unsigned int row;
      bool ok;
      uint32_t val32;
      uint16_t val16;

      QMonitorParamInfo mpinfo;
      QFile file(REGISTER_CONF_FILE);
      QTextStream in(&file);

      QString line;
      QStringList fields;

      if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
         qDebug() << "ERROR opening file !";

      row = 0;
      while (!in.atEnd()) {

         row++;
         line = in.readLine();
         fields = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

         if(fields.size() > 0) {

            mpinfo.init();

            if(fields[0].left(2) == "//")
               continue;

            if(fields.size() < 10) {
               qDebug() << "ERROR: line not complete at line: " << row << endl;
               continue;
            }

            // [0] name
            if(fields[0] == "Multiplexer") {

               // [1] port num
               if(fields[1].toInt() > 4) {   // Xa, Xb, Xc, X4
                  qDebug() << "ERROR: port not valid at line: " << row << endl;
                  continue;
               }
               else mpinfo.port = (TSIport) fields[1].toInt();

               // [2] id
               if(fields[2].toInt() > (TSIPORTID_NUM - 1)) {
                  qDebug() << "ERROR: port id not valid at line: " << row << endl;
                  continue;
               }
               else mpinfo.mline = (TSImon) fields[2].toInt();

               // [3] portname
               mpinfo.portname = fields[3];

               // [4] idname
               mpinfo.idname = fields[4];

               // [5] address
               val32 = fields[5].toInt(&ok, 16);

               if(!ok) {

                  qDebug() << "ERROR: address not valid at line: " << row << endl;
                  continue;
               }
               else mpinfo.address = val32;

               // [11] seqid
               mpinfo.seqid = fields[11].toInt();

               // [7] tag
               val16 = fields[7].toInt(&ok, 16);

               if(!ok) {

                  qDebug() << "ERROR: tag not valid at line: " << row << endl;
                  continue;
               }
               else mpinfo.tag = val16;

               // [6] bindline
               if(fields[6] == "-") {
                  mpinfo.bindline = (TSIline) UNDEF_LINE;

               } else {

                  if(fields[6].toInt() > (PORTINLINE_NUM - 1)) {

                     qDebug() << "ERROR bind line not valid at line: " << row << endl;
                     continue;
                  }
                  else mpinfo.bindline = (TSIline) (fields[6].toInt());
               }

               // [12] group
               mpinfo.group = fields[12].toInt();

               // [13] desc
               mpinfo.desc = "";
               for(int i=13; i<fields.size(); i++)
                  mpinfo.desc += (fields[i] + " ");

               mset_byport.insert(mpinfo.port, mpinfo);
            }

         }	// end if
      }  // end while
   }
}

QList<QMonitorParamInfo> QMonitorParamSet::monparamList(TSIport p) {

   return(mset_byport.values(p));
}

QList<QMonitorParamInfo> QMonitorParamSet::monparamList(TSIport p, TSImon ml) {

   QList<QMonitorParamInfo> outlist;
   QList<QMonitorParamInfo> tmplist = mset_byport.values(p);

   for(int i=0; i<tmplist.size(); i++) {

      if(tmplist[i].mline == ml)
         outlist << tmplist[i];
   }

   return outlist;
}

QMonitorParamInfo QMonitorParamSet::monparam(TSIport p, TSImon ml, TSIline l, int16_t group) {

   QList<QMonitorParamInfo> tmplist = mset_byport.values(p);

   for(int i=0; i<tmplist.size(); i++) {

      if( (tmplist[i].mline == ml) && (tmplist[i].bindline == (TSIline)l) && (tmplist[i].group == group) )
         return tmplist[i];
   }

   return undef_mpinfo;
}

QMonitorParamInfo QMonitorParamSet::monparam(TSIport p, TSImon ml, uint16_t seqid) {

   QList<QMonitorParamInfo> tmplist = mset_byport.values(p);

   for(int i=0; i<tmplist.size(); i++) {

      if( (tmplist[i].mline == ml) && (tmplist[i].seqid == seqid) )
         return tmplist[i];
   }

   return undef_mpinfo;
}

QList<int16_t> QMonitorParamSet::monlineList(TSIport p, TSIline l, int16_t group) {

   QList<QMonitorParamInfo> tmplist = mset_byport.values(p);
   QList<int16_t> outlist;

   for(int i=0; i<tmplist.size(); i++) {

      if( (tmplist[i].bindline == (TSIline)l) && (tmplist[i].group == group) )
         outlist << tmplist[i].mline;
   }

   return outlist;
}
