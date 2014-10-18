# #####################################################################
# Automatically generated by qmake (2.01a) Wed Jul 3 20:13:58 2013
# #####################################################################
TEMPLATE = app
TARGET = TSI-GUI
QMAKE_CLEAN = bin/TSI-GUI
DESTDIR = bin 
OBJECTS_DIR = objs
MOC_DIR = moc
UI_DIR = include
RCC_DIR = res
DEPENDPATH += .
INCLUDEPATH += ./include
INCLUDEPATH += /usr/local/include/qwt-6.1.1
DISTFILES += conf/regparams.conf
LIBS += -lzmq \
    -lqwt
HEADERS += include/zmq.hpp \
    include/v2n-client.h \
    include/topology.h \
    include/tl2form.h \
    include/tl1form.h \
    include/sumdialog.h \
    include/QTopology.h \
    include/QTL2logic.h \
    include/QTL1logic.h \
    include/QRegisterSet.h \
    include/QRegister.h \
    include/QMonitor.h \
    include/QMetrics.h \
    include/QGate.h \
    include/portmon.h \
    include/odetmon.h \
    include/maindialog.h \
    include/logo.h \
    include/helpdialog01.h \
    include/editdetdialog.h \
    include/dssdmon.h \
    include/detwireset.h \
    include/cpform.h \
    include/circularbuffer.h \
    include/CAENVMEtypes.h
FORMS += ui/wdg-tsicp.ui \
    ui/wdg-topology.ui \
    ui/wdg-tl2form.ui \
    ui/wdg-tl1form.ui \
    ui/wdg-portmonform.ui \
    ui/wdg-odetmonform.ui \
    ui/wdg-logo.ui \
    ui/wdg-dssdmonform.ui \
    ui/wdg-detwireset.ui \
    ui/dlg-tsidia.ui \
    ui/dlg-summary.ui \
    ui/dlg-main.ui \
    ui/dlg-editdet.ui
SOURCES += src/v2n-client.cpp \
    src/topology.cpp \
    src/tl2form.cpp \
    src/tl1form.cpp \
    src/sumdialog.cpp \
    src/QTopology.cpp \
    src/QTL2logic.cpp \
    src/QTL1logic.cpp \
    src/QRegisterSet.cpp \
    src/QRegister.cpp \
    src/QMonitor.cpp \
    src/portmon.cpp \
    src/odetmon.cpp \
    src/maindialog.cpp \
    src/main.cpp \
    src/logo.cpp \
    src/helpdialog01.cpp \
    src/editdetdialog.cpp \
    src/dssdmon.cpp \
    src/detwireset.cpp \
    src/cpform.cpp \
    src/circularbuffer.cpp
RESOURCES += res/icons.qrc
