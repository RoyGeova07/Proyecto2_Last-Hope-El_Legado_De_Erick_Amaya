QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AtributosPersonaje.cpp \
    Ciudad.cpp \
    Gimnasio.cpp \
    Inventario.cpp \
    TablaHash.cpp \
    bala.cpp \
    caminos.cpp \
    dialogonpc.cpp \
    gasolinera.cpp \
    grafo.cpp \
    inicio.cpp \
    inventariowidget.cpp \
    laboratorio.cpp \
    lobby.cpp \
    main.cpp \
    mainwindow.cpp \
    mall.cpp \
    mapa.cpp \
    npc.cpp \
    personaje.cpp \
    supermercado.cpp \
    tablawidget.cpp \
    zombie.cpp

HEADERS += \
    AtributosPersonaje.h \
    Ciudad.h \
    Cola.h \
    Elemento.h \
    Gimnasio.h \
    Inventario.h \
    NodoDialogo.h \
    TablaHash.h \
    bala.h \
    caminos.h \
    dialogonpc.h \
    gasolinera.h \
    grafo.h \
    inicio.h \
    inventariowidget.h \
    laboratorio.h \
    lobby.h \
    mainwindow.h \
    mall.h \
    mapa.h \
    npc.h \
    personaje.h \
    supermercado.h \
    tablawidget.h \
    zombie.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    inicio.qrc \
