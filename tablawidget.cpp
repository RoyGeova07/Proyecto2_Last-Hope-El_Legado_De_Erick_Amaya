#include "tablawidget.h"
#include <QPainter>
#include <QFile>
#include <QDebug>

void TablaWidget::configurarRutasNiveles() {
    rutasNiveles["Nivel1"] = ":/imagenes/assets/mapas/lobby.jpg";
    rutasNiveles["Nivel2"] = ":/imagenes/assets/mapas/War.png";
    rutasNiveles["Nivel3"] = ":/imagenes/assets/gym.jpeg";
    rutasNiveles["Nivel4"] = ":/imagenes/assets/mapas/War5.png";
    rutasNiveles["Nivel5"] = ":/imagenes/assets/mapas/Mall.jpeg";
    rutasNiveles["Nivel6"] = ":/imagenes/assets/mapas/War6.png";
    rutasNiveles["Nivel7"] = ":/imagenes/assets/lab.jpeg";
}

void TablaWidget::configurarRutasNPCs() {
    rutasNPCs["NPC1"] = ":/imagenes/assets/NPC/Idle_NPC1.png";
    rutasNPCs["NPC2"] = ":/imagenes/assets/NPC/Idle_NPC2.png";
    rutasNPCs["NPC3"] = ":/imagenes/assets/NPC/Idle_NPC3.png";
    rutasNPCs["NPC4"] = ":/imagenes/assets/NPC/Idle_NPC4.png";
    rutasNPCs["NPC5"] = ":/imagenes/assets/NPC/Idle_NPC5.png";
    rutasNPCs["NPC6"] = ":/imagenes/assets/NPC/Idle_NPC6.png";
}

TablaWidget::TablaWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layoutPrincipal = new QVBoxLayout(this);

    QLabel *titulo = new QLabel("Last Hope: Unlocked");
    titulo->setAlignment(Qt::AlignCenter);
    titulo->setStyleSheet("font-size: 16px; font-weight: bold; color: white; margin-bottom: 10px;");

    layout = new QGridLayout();
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setHorizontalSpacing(10);
    layout->setVerticalSpacing(15);

    layoutPrincipal->addWidget(titulo);
    layoutPrincipal->addLayout(layout);

    layout->setContentsMargins(5, 5, 5, 5);
    layout->setHorizontalSpacing(10);  // Espacio entre columnas
    layout->setVerticalSpacing(15);    // Espacio entre filas

    configurarRutasNiveles();
    configurarRutasNPCs();

    // Inicializar tablas
    tablaNiveles.insertar("Nivel1", true);
    tablaNiveles.insertar("Nivel2", false);
    tablaNiveles.insertar("Nivel3", false);
    tablaNiveles.insertar("Nivel4", false);
    tablaNiveles.insertar("Nivel5", false);
    tablaNiveles.insertar("Nivel6", false);
    tablaNiveles.insertar("Nivel7", false);

    tablaNPCS.insertar("NPC1", false);
    tablaNPCS.insertar("NPC2", false);
    tablaNPCS.insertar("NPC3", false);
    tablaNPCS.insertar("NPC4", false);
    tablaNPCS.insertar("NPC5", false);
    tablaNPCS.insertar("NPC6", false);

    cargarImagenes();
}

void TablaWidget::cargarImagenes() {
    // Limpiar layout
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // Nombres personalizados para niveles
    QHash<QString, QString> nombresNiveles = {
        {"Nivel1", "Lobby"},
        {"Nivel2", "Ciudad en Ruinas"},
        {"Nivel3", "Gimnasio"},
        {"Nivel4", "Gasolinera"},
        {"Nivel5", "Mall"},
        {"Nivel6", "Supermercado"},
        {"Nivel7", "Laboratorio"}
    };

    // Cargar niveles
    int columna = 0;
    for (const QString &clave : ordenNiveles) {
        bool descubierto = tablaNiveles.estaDescubierto(clave);
        QString ruta = rutasNiveles.value(clave);

        if(QFile::exists(ruta)) {
            QString nombre = nombresNiveles.value(clave, clave);
            QWidget *imagen = crearImagenConCandado(ruta, descubierto, nombre);
            layout->addWidget(imagen, 0, columna++, Qt::AlignHCenter);
        } else {
            qDebug() << "Error: No se encuentra la imagen" << ruta;
        }
    }

    // Cargar NPCs
    columna = 0;
    for (const QString &clave : ordenNPCs) {
        bool descubierto = tablaNPCS.estaDescubierto(clave);
        QString ruta = rutasNPCs.value(clave);

        if(QFile::exists(ruta)) {
            QPixmap spritesheet(ruta);
            if(!spritesheet.isNull()) {
                QPixmap primerFrame = spritesheet.copy(0, 0, 128, 128);
                QWidget *imagenNPC = crearImagenConCandado("", descubierto, clave);
                QLabel *imagenLabel = imagenNPC->findChild<QLabel *>();
                if (imagenLabel) {
                    imagenLabel->setPixmap(descubierto ? primerFrame : aplicarCandado(primerFrame));
                }
                layout->addWidget(imagenNPC, 1, columna++, Qt::AlignHCenter);

            } else {
                qDebug() << "Error: No se pudo cargar el spritesheet" << ruta;
            }
        } else {
            qDebug() << "Error: No se encuentra el spritesheet" << ruta;
        }
    }
    layout->setRowStretch(0, 1);
    layout->setRowStretch(1, 1);
}

QWidget* TablaWidget::crearImagenConCandado(const QString &ruta, bool descubierto, const QString &nombre) {
    QLabel *labelImagen = new QLabel();
    QPixmap pixmap;

    if(!ruta.isEmpty()) {
        pixmap.load(ruta);
        if(pixmap.isNull()) {
            qDebug() << "Error al cargar imagen:" << ruta;
            pixmap = QPixmap(128, 128);
            pixmap.fill(Qt::red);
        }
    } else {
        pixmap = QPixmap(128, 128);
        pixmap.fill(Qt::blue);
    }

    pixmap = pixmap.scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    if (!descubierto) {
        QPixmap pixmapOscuro = pixmap.copy();
        QPainter painter(&pixmapOscuro);
        painter.fillRect(pixmapOscuro.rect(), QColor(0, 0, 0, 150));
        if(QFile::exists(":/imagenes/assets/items/candado.png")) {
            painter.drawPixmap(pixmapOscuro.width()/2 - 16, pixmapOscuro.height()/2 - 16,
                               QPixmap(":/imagenes/assets/items/candado.png").scaled(32, 32));
        }
        labelImagen->setPixmap(pixmapOscuro);
    } else {
        labelImagen->setPixmap(pixmap);
    }

    QLabel *labelNombre = new QLabel(nombre);
    labelNombre->setAlignment(Qt::AlignCenter);
    labelNombre->setStyleSheet("margin-top: 2px; font-size: 10px;");

    QVBoxLayout *layoutContenedor = new QVBoxLayout();
    layoutContenedor->addWidget(labelImagen);
    layoutContenedor->addWidget(labelNombre);
    layoutContenedor->setSpacing(2);
    layoutContenedor->setContentsMargins(2, 2, 2, 2);

    QWidget *widgetContenedor = new QWidget();
    widgetContenedor->setLayout(layoutContenedor);
    widgetContenedor->setFixedSize(100, 160);


    return widgetContenedor;
}

QPixmap TablaWidget::aplicarCandado(const QPixmap &pixmap) {
    QPixmap resultado = pixmap.copy();
    QPainter painter(&resultado);
    painter.fillRect(resultado.rect(), QColor(0, 0, 0, 150));
    if(QFile::exists(":/imagenes/assets/items/candado.png")) {
        painter.drawPixmap(resultado.width()/2 - 16, resultado.height()/2 - 16,
                           QPixmap(":/imagenes/assets/items/candado.png").scaled(32, 32));
    }
    return resultado;
}
