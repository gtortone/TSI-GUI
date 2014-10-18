#ifndef TL2FORM_H
#define TL2FORM_H

#include <QWidget>
#include <QListWidget>
#include <QTableWidget>
#include "ui_wdg-tl2form.h"
#include "QTopology.h"
#include "QMetrics.h"

class DDTL2ListWidget : public QListWidget {   // DRAG enabled QListWidget

   Q_OBJECT

private:

   QTopology &topo;

public:

   DDTL2ListWidget(QTopology &tpl, QWidget *parent = 0);

   void mouseMoveEvent(QMouseEvent *event);

public slots:

   void refresh(void);
};

class DDTL2TableWidget : public QTableWidget {  // DROP enabled QTableWidget

   Q_OBJECT

private:

   QTopology &topo;

public:

   DDTL2TableWidget(QTopology &tpl, QWidget *parent = 0);

   virtual bool dropMimeData(int row, int column, const QMimeData *data, Qt::DropAction action);

   QStringList mimeTypes() const;
   Qt::DropActions supportedDropActions () const;

public slots:

   void refresh(void);
};

class TL2Form : public QWidget, public Ui::tl2Form {

    Q_OBJECT

private:

   QTopology &topo;
   DDTL2TableWidget *tw;
   DDTL2ListWidget *lwin;
   QList<TSIline> busyl;

public:

   TL2Form(QTopology &tpl, QWidget *parent = 0);
   void init(void);

private slots:

   void displayTableContextMenu(const QPoint &pos);
   void displayListOutDetails(void);
   void delLineOut(void);
   void outListRefresh(void);
   void clear(void);
};

#endif // TL2FORM_H
