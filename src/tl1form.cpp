#include "tl1form.h"

#include <QtGui>
#include <QTableWidgetItem>
#include <QScrollBar>
#include <QStringList>
#include <QBrush>
#include <QList>
#include <QListWidgetItem>
#include <QByteArray>
#include <QMessageBox>
#include <QInputDialog>

#include "QMetrics.h"

#define ROW_NUM   TL1GATE_NUM
#define COL_NUM   TL1SIGIN_NUM

#define CELL_WIDTH   40
#define CELL_HEIGHT  18

// DDTL1ListWidget

DDTL1ListWidget::DDTL1ListWidget(QTopology &tpl, QWidget *parent) : QListWidget(parent), topo(tpl) {

}

void DDTL1ListWidget::mouseMoveEvent(QMouseEvent *event) {

   // if not left button - return
   if (!(event->buttons() & Qt::LeftButton)) return;

   if (currentItem() == NULL) return;

   if(itemAt(event->pos()) == NULL) return;

   QDrag *drag = new QDrag(this);
   QMimeData *mimeData = new QMimeData;

   QByteArray itemData;
   itemData.append(currentItem()->data(QVariant(Qt::UserRole).toUInt()).toUInt());

   mimeData->setData("application/tl1siginput", itemData);
   drag->setMimeData(mimeData);

   // start drag
   drag->start(Qt::CopyAction | Qt::MoveAction);
}

void DDTL1ListWidget::refresh(void) {

   QList<TSIline> busyl = topo.busyInputLines(X4);

   clear();
   for(int i=0; i<busyl.size(); i++) {

      QPair<unsigned int, unsigned int> detloc = topo.detectorTopoInfo(X4, busyl[i]);
      QDetector det = topo.detector(detloc.first, detloc.second);

      if(det.enabled()) { // if detector included in trigger logic...

         QListWidgetItem *item = new QListWidgetItem();
         //item->setText(topo.telescopeName(detloc.first) + "." + det.name() + " (" + TL1SignalIn_str[busyl[i]] + ")");
         item->setText(topo.telescopeName(detloc.first) + "." + det.name() + " (X4-" + TSIline_str[busyl[i]] + ")");
         item->setData(Qt::UserRole, busyl[i]);
         addItem(item);
      }
   }

   if(topo.busyInputLines(XA).size() != 0) {

      QList<TSIline> bl = topo.busyInputLines(XA);

      for(int i=0; i<bl.size(); i++) {

         QPair<unsigned int, unsigned int> detloc = topo.detectorTopoInfo(XA, bl[i]);
         QDetector det = topo.detector(detloc.first, detloc.second);

         if(det.enabled()) { // if exist a detector included in trigger logic...

            QListWidgetItem *item = new QListWidgetItem();
            item->setText(TL1SignalIn_str[TRGXa]);
            item->setData(Qt::UserRole, TRGXa);
            addItem(item);
            break;
         }
      }
   }

   if(topo.busyInputLines(XB).size() != 0) {

      QList<TSIline> bl = topo.busyInputLines(XB);

      for(int i=0; i<bl.size(); i++) {

         QPair<unsigned int, unsigned int> detloc = topo.detectorTopoInfo(XB, bl[i]);
         QDetector det = topo.detector(detloc.first, detloc.second);

         if(det.enabled()) { // if exist a detector included in trigger logic...

            QListWidgetItem *item = new QListWidgetItem();
            item->setText(TL1SignalIn_str[TRGXb]);
            item->setData(Qt::UserRole, TRGXb);
            addItem(item);
            break;
         }
      }
   }

   if(topo.busyInputLines(XC).size() != 0) {

      QList<TSIline> bl = topo.busyInputLines(XC);

      for(int i=0; i<bl.size(); i++) {

         QPair<unsigned int, unsigned int> detloc = topo.detectorTopoInfo(XC, bl[i]);
         QDetector det = topo.detector(detloc.first, detloc.second);

         if(det.enabled()) { // if exist a detector included in trigger logic...

            QListWidgetItem *item = new QListWidgetItem();
            item->setText(TL1SignalIn_str[TRGXc]);
            item->setData(Qt::UserRole, TRGXc);
            addItem(item);
            break;
         }
      }
   }
}

// DDTL1TableWidget

DDTL1TableWidget::DDTL1TableWidget(QTopology &tpl, QWidget *parent) : QTableWidget(parent), topo(tpl) {

}

bool DDTL1TableWidget::dropMimeData(int row, int column, const QMimeData *mimeData, Qt::DropAction action) {

   if( (action == Qt::CopyAction) || (action == Qt::MoveAction) ) {

      QByteArray data = mimeData->data("application/tl1siginput");
      QTableWidgetItem *item = this->item(row, column);
      enum TL1SignalIn sig = topo.TL1logic.getInput(row, column);

      if(item == NULL) return(false);

      if(sig != UNDEF_TL1IN) {

         if(sig == AVAIL_TL1IN) {

            TL1SignalIn in = (TL1SignalIn) data.at(0);
            topo.TL1logic.assignInput(row, column, in);

         } else {

            // sig == <assigned signal>
            int r = QMessageBox::warning(this, "Message",
                                         "Input line will be overwritter !" "\nAre you sure ?",
                                         QMessageBox::Yes, QMessageBox::No);

            if(r == QMessageBox::Yes) {

               TL1SignalIn in = (TL1SignalIn) data.at(0);
               topo.TL1logic.assignInput(row, column, in);
            }
         }

         refresh();
         return true;
      }
   }

   return false;
}

QStringList DDTL1TableWidget::mimeTypes () const {

   QStringList qstrList;
   // list of accepted mime types for drop
   qstrList.append("application/tl1siginput");
   return qstrList;
}

Qt::DropActions DDTL1TableWidget::supportedDropActions () const {

   // returns what actions are supported when dropping
   return Qt::CopyAction | Qt::MoveAction;
}

void DDTL1TableWidget::refresh(void) {

   bool gateoutput = false;

   for(int r=0; r<TL1GATE_NUM; r++) {

      if(topo.TL1logic.getOutput(r) != AVAIL_TL1OUT)     // gate is assigned as output
         gateoutput = true;
      else
         gateoutput = false;

      for(int c=0; c<TL1SIGIN_NUM; c++) {

         QTableWidgetItem *cell = new QTableWidgetItem();
         Qt::ItemFlags iflags = cell->flags();
         iflags |= Qt::ItemIsDropEnabled;
         iflags &= ~Qt::ItemIsEditable;

         TL1SignalIn input = topo.TL1logic.getInput(r, c);

         if(input != UNDEF_TL1IN) {

            if(input == AVAIL_TL1IN) {  // unassigned input

               cell->setBackground(QBrush(Qt::blue));

            } else {  // assigned input

               if(topo.TL1logic.allocatedGate(r)) {

                  if(gateoutput)
                     cell->setBackground(QBrush(Qt::darkRed));
                  else
                     cell->setBackground(QBrush(Qt::darkGreen));

               } else {

                  cell->setBackground(QBrush(Qt::darkCyan));
               }

               if(input < TRGXa) {

                  QPair<unsigned int, unsigned int> detloc = topo.detectorTopoInfo(X4, (TSIline) input);
                  QDetector det = topo.detector(detloc.first, detloc.second);

                  cell->setText(topo.telescopeName(detloc.first) + "." + det.name());

               } else {

                  cell->setText(TL1SignalIn_str[input]);
               }
            }

            cell->setFlags(iflags);
            cell->setForeground(QBrush(Qt::white));;

         } else {    // disable drop

            cell->setFlags(cell->flags() & ~Qt::ItemIsDropEnabled & ~Qt::ItemIsSelectable & ~Qt::ItemIsEnabled);
         }

         setItem(r, c, cell);
      }
   }
}

// TL1Form

TL1Form::TL1Form(QTopology &tpl, QWidget *parent) : QWidget(parent), topo(tpl) {

    setupUi(this);

    busyl = topo.busyInputLines(X4);

    // table widget
    tw = new DDTL1TableWidget(topo, this);
    tw->setParent(this);

    // table geometry
    tw->setColumnCount(COL_NUM);
    tw->setRowCount(ROW_NUM);
    tw->resize(850,580);
    tw->move(10,35);
    tw->setSelectionMode(QAbstractItemView::NoSelection);
    tw->setAcceptDrops(true);

    // set context menu
    tw->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tw, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(displayTableContextMenu(QPoint)));
    connect(infoButton, SIGNAL(clicked()), this, SLOT(displayListOutDetails()));
    connect(delButton, SIGNAL(clicked()), this, SLOT(delLineOut()));

    // set clear button slot
    connect(clearButton, SIGNAL(clicked()), this, SLOT(clear()));

    // table layout
    QFont f = tw->font();
    f.setPixelSize(10);
    tw->setFont(f);
    tw->horizontalScrollBar()->hide();
    tw->verticalScrollBar()->hide();

    for(int i=0; i<COL_NUM; i++)
      tw->setColumnWidth(i, CELL_WIDTH);

    for(int i=0; i<ROW_NUM; i++)
      tw->setRowHeight(i,CELL_HEIGHT);

    const QStringList hlabels = QStringList() << "i0" << "i1" << "i2" << "i3" << "i4" << "i5" << "i6" << "i7" << "i8" << "i9"
                                << "i10" << "i11" << "i12" << "i13" << "i14" << "i15" << "i16" << "i17" << "i18";
    const QStringList vlabels = topo.TL1logic.getGateShortDescList();

    tw->setHorizontalHeaderLabels(hlabels);
    tw->setVerticalHeaderLabels(vlabels);

    // list widget
    lwin = new DDTL1ListWidget(topo, this);
    lwin->setParent(this);

    // list geometry
    lwin->resize(150,200);
    lwin->move(870,75);
    lwin->setSelectionMode(QAbstractItemView::SingleSelection);

    lwin->show();
    tw->show();

    tw->refresh();
    lwin->refresh();

    outListRefresh();
}

void TL1Form::init(void) {

   tw->refresh();
   lwin->refresh();
   outListRefresh();
}

void TL1Form::displayTableContextMenu(const QPoint &pos) {

   QMenu menu(this);
   QAction *removeAction, *assignOutAction, *unassignOutAction;
   int row = tw->itemAt(pos)->row();
   int col = tw->itemAt(pos)->column();
   enum TL1SignalIn sigin = topo.TL1logic.getInput(row, col);
   enum TL1SignalOut sigout = topo.TL1logic.getOutput(row);
   bool allow_to_remove = true;
   int sigout_conflict = AVAIL_TL1OUT;

   removeAction = menu.addAction("remove entry");
   assignOutAction = menu.addAction("assign output...");
   unassignOutAction = menu.addAction("unassign output");
   unassignOutAction->setEnabled(false);

   if(!topo.TL1logic.allocatedGate(row)) {

      assignOutAction->setEnabled(false);
      unassignOutAction->setEnabled(false);

   } else if(sigout != AVAIL_TL1OUT) {

      removeAction->setEnabled(false);
      assignOutAction->setEnabled(false);
      unassignOutAction->setEnabled(true);
   }

   if((sigin != UNDEF_TL1IN) && (sigin != AVAIL_TL1IN)) {

      QAction *opt = menu.exec(tw->viewport()->mapToGlobal(pos));

      if(opt == assignOutAction) {

         QInputDialog inputdialog;
         QStringList items;

         // display available output lines only
         for(int i=0; i<TL1SIGOUT_NUM;i++) {

            if(!topo.TL1logic.allocatedOutput((enum TL1SignalOut) i))
               items << TL1SignalOut_str[i];
         }

         bool ok;
         QString item = inputdialog.getItem(this, "Select output line", "", items, 0, false, &ok);

         if(ok) {

            topo.TL1logic.assignOutput(row, (enum TL1SignalOut) TL1SignalOut_str.indexOf(item));
         }

      } else if(opt == unassignOutAction) {

         int r = QMessageBox::warning(this, "Message",
                                      "Output line " + TL1SignalOut_str[sigout] + " will be unassigned !\n" "\nAre you sure ?",
                                      QMessageBox::Yes, QMessageBox::No);

         if(r == QMessageBox::Yes)
            topo.TL1logic.unassignOutput(sigout);

      } else if(opt == removeAction) {

         QGate gsel = topo.TL1logic.getGate(row);
         QVector<uint8_t> gselin = gsel.getLineInIndex();

         for(int i=0; i<TL1SIGOUT_NUM; i++) {

            if(topo.TL1logic.allocatedOutput((enum TL1SignalOut) i)) {

               QGate g = topo.TL1logic.getGate(topo.TL1logic.getAssignedGate((enum TL1SignalOut) i));
               QVector<uint8_t> gin = g.getLineInIndex();

               for(int j=0; j<gselin.size(); j++) {

                  for(int k=0; k<gin.size(); k++) {

                     if(gselin[j] == gin[k]) {

                        allow_to_remove = false;
                        sigout_conflict = i;
                     }
                  }
               }
            }
         } // end of for

         if(allow_to_remove) {

            int r = QMessageBox::warning(this, "Message",
                                         "Input line will be removed !\nRelated gate inputs will be freed !" "\nAre you sure ?",
                                         QMessageBox::Yes, QMessageBox::No);

            if(r == QMessageBox::Yes)
               topo.TL1logic.unassignInput(row, col);

         } else {

           QMessageBox::critical(this, "Message", "Input line removal not allowing!" "\n(input line " + TL1SignalIn_str[sigin] + " used in " + TL1SignalOut_str[sigout_conflict] + " output line)", QMessageBox::Ok);

         } // end of allow_to_remove

      } // end of removeaction

   } // end of (if sigin)

   tw->refresh();
   outListRefresh();
}

void TL1Form::displayListOutDetails(void) {

   if(lwout->currentItem() != NULL) {

      TL1SignalOut sigout = (enum TL1SignalOut) lwout->currentItem()->data(QVariant(Qt::UserRole).toUInt()).toUInt();
      uint8_t row = topo.TL1logic.getAssignedGate(sigout);

      QGate g = topo.TL1logic.getGate(topo.TL1logic.getAssignedGate(sigout));
      QVector<uint8_t> linein = g.getLineInIndex();

      QString lines;

      for(int i=0; i<linein.size(); i++) {

         if(topo.TL1logic.getInput(row, linein[i]) > X4_TRG15) {

            lines += "  " + TL1SignalIn_str[topo.TL1logic.getInput(row, linein[i])] + "\n";

         } else {

            QPair<unsigned int, unsigned int> detloc = topo.detectorTopoInfo(X4, (TSIline)(topo.TL1logic.getInput(row, linein[i])));
            QDetector det = topo.detector(detloc.first, detloc.second);

            lines += "  " + topo.telescopeName(detloc.first) + "." + det.name() + " (" + TL1SignalIn_str[topo.TL1logic.getInput(row, linein[i])] + ")\n";
         }

         if((i + 1) <= (linein.size() - 1))
            lines += "     " + QGateType_str[g.getType()] + "\n";
      }

      QMessageBox::information(this, "Information", TL1SignalOut_str[sigout] + " = \n\n" + lines);
   }
}

void TL1Form::delLineOut(void) {

   if(lwout->currentItem() != NULL) {

      TL1SignalOut sigout = (enum TL1SignalOut) lwout->currentItem()->data(QVariant(Qt::UserRole).toUInt()).toUInt();

      int r = QMessageBox::warning(this, "Message",
                                   "Output line " + TL1SignalOut_str[sigout] + " will be unassigned !\n" "\nAre you sure ?",
                                   QMessageBox::Yes, QMessageBox::No);

      if(r == QMessageBox::Yes) {

         topo.TL1logic.unassignOutput(sigout);

         outListRefresh();
         tw->refresh();
      }
   }
}

void TL1Form::outListRefresh(void) {

   lwout->clear();

   for(int i=0; i<TL1SIGOUT_NUM; i++) {
      if(topo.TL1logic.allocatedOutput((enum TL1SignalOut) i)) {

         QListWidgetItem *item = new QListWidgetItem();
         item->setData(Qt::UserRole, i);
         item->setText(TL1SignalOut_str[i] + " -> [" + topo.TL1logic.getGateShortDesc(topo.TL1logic.getAssignedGate((enum TL1SignalOut) i)) + "]");

         lwout->addItem(item);
      }
   }
}

void TL1Form::clear(void) {

   int r = QMessageBox::warning(this, "Message", "Trigger Level I and Trigger Level II configurations will be reset - Are you sure ?",
                                QMessageBox::Yes, QMessageBox::No);

   if(r == QMessageBox::Yes) {

      topo.TL1logic.clear();
      topo.TL2logic.clear();
      init();     // to refresh screen
   }
}
