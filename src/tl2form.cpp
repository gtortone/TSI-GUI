#include "tl2form.h"

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

#define ROW_NUM   TL2GATE_NUM
#define COL_NUM   TL2SIGIN_NUM

#define CELL_WIDTH   90
#define CELL_HEIGHT  25

// DDTL2ListWidget

DDTL2ListWidget::DDTL2ListWidget(QTopology &tpl, QWidget *parent) : QListWidget(parent), topo(tpl) {

}

void DDTL2ListWidget::mouseMoveEvent(QMouseEvent *event) {

   // if not left button - return
   if (!(event->buttons() & Qt::LeftButton)) return;

   if (currentItem() == NULL) return;

   if(itemAt(event->pos()) == NULL) return;

   QDrag *drag = new QDrag(this);
   QMimeData *mimeData = new QMimeData;

   QByteArray itemData;
   itemData.append(currentItem()->data(QVariant(Qt::UserRole).toUInt()).toUInt());

   mimeData->setData("application/tl2siginput", itemData);
   drag->setMimeData(mimeData);

   // start drag
   drag->start(Qt::CopyAction | Qt::MoveAction);
}

void DDTL2ListWidget::refresh(void) {

   clear();

   for(int i=0; i<TL1SIGOUT_NUM; i++) {
      if(topo.TL1logic.allocatedOutput((enum TL1SignalOut) i)) {

         QListWidgetItem *item = new QListWidgetItem();
         item->setData(Qt::UserRole, i);
         item->setText(TL1SignalOut_str[i] + " -> [" + topo.TL1logic.getGateShortDesc(topo.TL1logic.getAssignedGate((enum TL1SignalOut) i)) + "]");

         addItem(item);
      }
   }
}

// DDTL2TableWidget

DDTL2TableWidget::DDTL2TableWidget(QTopology &tpl, QWidget *parent) : QTableWidget(parent), topo(tpl) {

}

bool DDTL2TableWidget::dropMimeData(int row, int column, const QMimeData *mimeData, Qt::DropAction action) {

   if( (action == Qt::CopyAction) || (action == Qt::MoveAction) ) {

      QByteArray data = mimeData->data("application/tl2siginput");
      QTableWidgetItem *item = this->item(row, column);
      enum TL2SignalIn sig = topo.TL2logic.getInput(row, column);

      if(item == NULL) return(false);

      if(sig != UNDEF_TL2IN) {

         if(sig == AVAIL_TL2IN) {

            TL2SignalIn in = (TL2SignalIn) data.at(0);
            topo.TL2logic.assignInput(row, column, in);

         } else {

            // sig == <assigned signal>
            int r = QMessageBox::warning(this, "Message",
                                         "Input line will be overwritter !" "\nAre you sure ?",
                                         QMessageBox::Yes, QMessageBox::No);

            if(r == QMessageBox::Yes) {

               TL2SignalIn in = (TL2SignalIn) data.at(0);
               topo.TL2logic.assignInput(row, column, in);
            }
         }

         refresh();
         return true;
      }
   }

   return false;
}

QStringList DDTL2TableWidget::mimeTypes () const {

   QStringList qstrList;
   // list of accepted mime types for drop
   qstrList.append("application/tl2siginput");
   return qstrList;
}

Qt::DropActions DDTL2TableWidget::supportedDropActions () const {

   // returns what actions are supported when dropping
   return Qt::CopyAction | Qt::MoveAction;
}

void DDTL2TableWidget::refresh(void) {

   bool gateoutput = false;

   for(int r=0; r<TL2GATE_NUM; r++) {

      if(topo.TL2logic.getOutput(r) != AVAIL_TL2OUT)     // gate is assigned as output
         gateoutput = true;
      else
         gateoutput = false;

      for(int c=0; c<TL2SIGIN_NUM; c++) {

         QTableWidgetItem *cell = new QTableWidgetItem();
         Qt::ItemFlags iflags = cell->flags();
         iflags |= Qt::ItemIsDropEnabled;
         iflags &= ~Qt::ItemIsEditable;

         TL2SignalIn input = topo.TL2logic.getInput(r, c);

         if(input != UNDEF_TL2IN) {

            if(input == AVAIL_TL2IN) {  // unassigned input

               cell->setBackground(QBrush(Qt::blue));

            } else {  // assigned input

               if(topo.TL2logic.allocatedGate(r)) {

                  if(gateoutput)
                     cell->setBackground(QBrush(Qt::darkRed));
                  else
                     cell->setBackground(QBrush(Qt::darkGreen));

               } else {

                  cell->setBackground(QBrush(Qt::darkCyan));
               }

                  cell->setText(TL2SignalIn_str[input]);
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

// TL2Form

TL2Form::TL2Form(QTopology &tpl, QWidget *parent) : QWidget(parent), topo(tpl) {

    setupUi(this);

    // table widget
    tw = new DDTL2TableWidget(topo, this);
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

    const QStringList hlabels = QStringList() << "i0" << "i1" << "i2" << "i3" << "i4" << "i5" << "i6" << "i7";
    const QStringList vlabels = topo.TL2logic.getGateShortDescList();

    tw->setHorizontalHeaderLabels(hlabels);
    tw->setVerticalHeaderLabels(vlabels);

    // list widget
    lwin = new DDTL2ListWidget(topo, this);
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

void TL2Form::init(void) {

   tw->refresh();
   lwin->refresh();
   outListRefresh();
}

void TL2Form::displayTableContextMenu(const QPoint &pos) {

   QMenu menu(this);
   QAction *removeAction, *assignOutAction, *unassignOutAction;
   int row = tw->itemAt(pos)->row();
   int col = tw->itemAt(pos)->column();
   enum TL2SignalIn sigin = topo.TL2logic.getInput(row, col);
   enum TL2SignalOut sigout = topo.TL2logic.getOutput(row);
   bool allow_to_remove = true;
   int sigout_conflict = AVAIL_TL2OUT;

   removeAction = menu.addAction("remove entry");
   assignOutAction = menu.addAction("assign output...");
   unassignOutAction = menu.addAction("unassign output");
   unassignOutAction->setEnabled(false);

   if(!topo.TL2logic.allocatedGate(row)) {

      assignOutAction->setEnabled(false);
      unassignOutAction->setEnabled(false);

   } else if(sigout != AVAIL_TL2OUT) {

      removeAction->setEnabled(false);
      assignOutAction->setEnabled(false);
      unassignOutAction->setEnabled(true);
   }

   if((sigin != UNDEF_TL2IN) && (sigin != AVAIL_TL2IN)) {

      QAction *opt = menu.exec(tw->viewport()->mapToGlobal(pos));

      if(opt == assignOutAction) {

         QInputDialog inputdialog;
         QStringList items;

         // display available output lines only
         for(int i=0; i<TL2SIGOUT_NUM;i++) {

            if(!topo.TL2logic.allocatedOutput((enum TL2SignalOut) i))
               items << TL2SignalOut_str[i];
         }

         bool ok;
         QString item = inputdialog.getItem(this, "Select output line", "", items, 0, false, &ok);

         if(ok) {

            topo.TL2logic.assignOutput(row, (enum TL2SignalOut) TL2SignalOut_str.indexOf(item));
         }

      } else if(opt == unassignOutAction) {

         int r = QMessageBox::warning(this, "Message",
                                      "Output line " + TL2SignalOut_str[sigout] + " will be unassigned !\n" "\nAre you sure ?",
                                      QMessageBox::Yes, QMessageBox::No);

         if(r == QMessageBox::Yes)
            topo.TL2logic.unassignOutput(sigout);

      } else if(opt == removeAction) {

         QGate gsel = topo.TL2logic.getGate(row);
         QVector<uint8_t> gselin = gsel.getLineInIndex();

         for(int i=0; i<TL2SIGOUT_NUM; i++) {

            if(topo.TL2logic.allocatedOutput((enum TL2SignalOut) i)) {

               QGate g = topo.TL2logic.getGate(topo.TL2logic.getAssignedGate((enum TL2SignalOut) i));
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
               topo.TL2logic.unassignInput(row, col);

         } else {

           QMessageBox::critical(this, "Message", "Input line removal not allowing!" "\n(input line " + TL2SignalIn_str[sigin] + " used in " + TL2SignalOut_str[sigout_conflict] + " output line)", QMessageBox::Ok);

         } // end of allow_to_remove

      } // end of removeaction

   } // end of (if sigin)

   tw->refresh();
   outListRefresh();
}

void TL2Form::displayListOutDetails(void) {

   if(lwout->currentItem() != NULL) {

      TL2SignalOut sigout = (enum TL2SignalOut) lwout->currentItem()->data(QVariant(Qt::UserRole).toUInt()).toUInt();
      uint8_t row = topo.TL2logic.getAssignedGate(sigout);

      QGate g = topo.TL2logic.getGate(topo.TL2logic.getAssignedGate(sigout));
      QVector<uint8_t> linein = g.getLineInIndex();

      QString lines;

      for(int i=0; i<linein.size(); i++) {

            lines += "  " + TL2SignalIn_str[topo.TL2logic.getInput(row, linein[i])] + "\n";

         if((i + 1) <= (linein.size() - 1))
            lines += "     " + QGateType_str[g.getType()] + "\n";
      }

      QMessageBox::information(this, "Information", TL2SignalOut_str[sigout] + " = \n\n" + lines);
   }
}

void TL2Form::delLineOut(void) {

   if(lwout->currentItem() != NULL) {

      TL2SignalOut sigout = (enum TL2SignalOut) lwout->currentItem()->data(QVariant(Qt::UserRole).toUInt()).toUInt();

      int r = QMessageBox::warning(this, "Message",
                                   "Output line " + TL2SignalOut_str[sigout] + " will be unassigned !\n" "\nAre you sure ?",
                                   QMessageBox::Yes, QMessageBox::No);

      if(r == QMessageBox::Yes) {

         topo.TL2logic.unassignOutput(sigout);

         outListRefresh();
         tw->refresh();
      }
   }
}

void TL2Form::outListRefresh(void) {

   lwout->clear();

   for(int i=0; i<TL2SIGOUT_NUM; i++) {
      if(topo.TL2logic.allocatedOutput((enum TL2SignalOut) i)) {

         QListWidgetItem *item = new QListWidgetItem();
         item->setData(Qt::UserRole, i);
         item->setText(TL2SignalOut_str[i] + " -> [" + topo.TL2logic.getGateShortDesc(topo.TL2logic.getAssignedGate((enum TL2SignalOut) i)) + "]");

         lwout->addItem(item);
      }
   }
}

void TL2Form::clear(void) {

   int r = QMessageBox::warning(this, "Message", "Trigger Level II configuration will be reset - Are you sure ?",
                                QMessageBox::Yes, QMessageBox::No);

   if(r == QMessageBox::Yes) {

      topo.TL2logic.clear();
      init();     // to refresh screen
   }
}

