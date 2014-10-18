#ifndef CPFORM_H
#define CPFORM_H

#include "ui_wdg-tsicp.h"
#include "QTopology.h"

#include <QTimer>
#include "qwt_plot_curve.h"
#include "qwt_curve_fitter.h"

#define MAX_FAILSNUM    3

class CPForm : public QWidget, public Ui::tsicpForm {

    Q_OBJECT

private:

   QTopology &topo;
   QTimer *timer;
   uint8_t numfails;

   bool forminit;
   QwtPlotCurve *dthisto;
   QwtPlotCurve *trgraph, *acqgraph;
   double dtx[101], dty[101];
   QVector<double> tratex, tratey;
   QVector<double> acqratex, acqratey;

   void clear_plots(void);

   uint32_t offset_acq_tnum, offset_tot_tnum;
   uint32_t of_acq_tnum, of_tot_tnum;
   uint32_t last_acq_tnum, last_tot_tnum;
   uint32_t delta_acq_tnum, delta_tot_tnum;
   uint32_t acq_tnum, tot_tnum;
   uint32_t abs_acq_tnum, abs_tot_tnum;
   uint32_t abs_last_acq_tnum, abs_last_tot_tnum;
   uint32_t dtime_perc;

   uint32_t offset_elap_time;
   uint32_t of_elap_time;
   uint32_t last_elap_time;
   uint32_t delta_elap_time;
   uint32_t abs_elap_time;
   uint32_t elap_time;
   uint32_t abs_last_elap_time;

public:

   CPForm(QTopology &tpl, QWidget *parent = 0);

protected:

   void showEvent(QShowEvent *ev);
   void hideEvent(QHideEvent *ev);

 private slots:

   void refresh(void);
   void send_stop(void);
   void send_run(void);
   void send_reset(void);
   void update_reftime(void);
   void update_tsistatus(void);
   void fire_sim(void);
   void fire_div(void);
   void refreshSim(void);
   void refreshDiv(void);

 public slots:

   void init(void);
};

#endif // CPFORM_H
