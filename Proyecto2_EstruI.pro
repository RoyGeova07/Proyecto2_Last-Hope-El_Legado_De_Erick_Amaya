QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Ciudad.cpp \
    caminos.cpp \
    dialogonpc.cpp \
    escenabase.cpp \
    gasolinera.cpp \
    grafo.cpp \
    inicio.cpp \
    lobby.cpp \
    main.cpp \
    mainwindow.cpp \
    mapa.cpp \
    npc.cpp \
    personaje.cpp \
    zombie.cpp

HEADERS += \
    Ciudad.h \
    Cola.h \
    Elemento.h \
    caminos.h \
    dialogonpc.h \
    escenabase.h \
    gasolinera.h \
    grafo.h \
    inicio.h \
    lobby.h \
    mainwindow.h \
    mapa.h \
    npc.h \
    personaje.h \
    zombie.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    inicio.qrc \
