QT += core gui sql
QT += core gui widgets printsupport

INCLUDEPATH += "C:\Program Files\MySQL\MySQL Server 9.5\include"
LIBS += -L"C:\Program Files\MySQL\MySQL Server 9.5\lib" -lmysql


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addmenuwindow.cpp \
    addstaffwindow.cpp \
    databaseconnection.cpp \
    dinninwindow.cpp \
    main.cpp \
    mainwindow.cpp \
    menuwindow.cpp \
    orderdetailwindow.cpp \
    ordertrakingwindow.cpp \
    removemenuwindow.cpp \
    removestaffwindow.cpp \
    showmenuwindow.cpp \
    showmenuwindow2.cpp \
    showstaffwindow.cpp \
    showstaffwindow2.cpp \
    staffwindow.cpp \
    tablewindow.cpp \
    takeawaywindow.cpp \
    updatemenu.cpp \
    updatestaffwindow.cpp \
    user1window.cpp \
    user2window.cpp \
    user3window.cpp \
    user4window.cpp

HEADERS += \
    addmenuwindow.h \
    addstaffwindow.h \
    databaseconnection.h \
    dinninwindow.h \
    mainwindow.h \
    menuwindow.h \
    orderdetailwindow.h \
    ordertrakingwindow.h \
    removemenuwindow.h \
    removestaffwindow.h \
    showmenuwindow.h \
    showmenuwindow2.h \
    showstaffwindow.h \
    showstaffwindow2.h \
    staffwindow.h \
    tablewindow.h \
    takeawaywindow.h \
    updatemenu.h \
    updatestaffwindow.h \
    user1window.h \
    user2window.h \
    user3window.h \
    user4window.h

FORMS += \
    addmenuwindow.ui \
    addstaffwindow.ui \
    dinninwindow.ui \
    mainwindow.ui \
    menuwindow.ui \
    orderdetailwindow.ui \
    ordertrakingwindow.ui \
    removemenuwindow.ui \
    removestaffwindow.ui \
    showmenuwindow.ui \
    showmenuwindow2.ui \
    showstaffwindow.ui \
    showstaffwindow2.ui \
    staffwindow.ui \
    tablewindow.ui \
    takeawaywindow.ui \
    updatemenu.ui \
    updatestaffwindow.ui \
    user1window.ui \
    user2window.ui \
    user3window.ui \
    user4window.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
