#ifndef TL1FORM_H
#define TL1FORM_H

#include <QWidget>
#include <QListWidget>
#include <QTableWidget>
#include "ui_wdg-tl1form.h"
#include "QTopology.h"
#include "QMetrics.h"

class DDTL1ListWidget : public QListWidget {   // DRAG enabled QListWidget

   Q_OBJECT

private:

   QTopology &topo;

public:

   DDTL1ListWidget(QTopology &tpl, QWidget *parent = 0);

   void mouseMoveEvent(QMouseEvent *event);

public slots:

   void refresh(void);
};

class DDTL1TableWidget : public QTableWidget {  // DROP enabled QTableWidget

   Q_OBJECT

private:

   QTopology &topo;

public:

   DDTL1TableWidget(QTopology &tpl, QWidget *parent = 0);

   virtual bool dropMimeData(int row, int column, const QMimeData *data, Qt::DropAction action);

   QStringList mimeTypes() const;
   Qt::DropActions supportedDropActions () const;

public slots:

   void refresh(void);
};

class TL1Form : public QWidget, public Ui::tl1Form {

    Q_OBJECT

private:

   QTopology &topo;
   DDTL1TableWidget *tw;
   DDTL1ListWidget *lwin;
   QList<TSIline> busyl;

public:

   TL1Form(QTopology &tpl, QWidget *parent = 0);
   void init(void);

private slots:

   void displayTableContextMenu(const QPoint &pos);
   void displayListOutDetails(void);
   void delLineOut(void);
   void outListRefresh(void);
   void clear(void);
};

#endif // TL1FORM_H
