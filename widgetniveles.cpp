#include "widgetniveles.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include<QScrollArea>

WidgetNiveles::WidgetNiveles(ArbolAvl* arbolMapas, QWidget* parent)
    : QWidget(parent), arbol(arbolMapas)
{
    QVBoxLayout* contenedorLayout = new QVBoxLayout(this);

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);

    QWidget* contenidoScroll = new QWidget();
    layout = new QVBoxLayout(contenidoScroll);

    scrollArea->setWidget(contenidoScroll);
    contenedorLayout->addWidget(scrollArea);

    actualizar(true);
}

void WidgetNiveles::actualizar(bool ascendente)
{
    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    QList<NodoAvl*> niveles = arbol->obtenerEnOrden(ascendente);

    for (auto* nodo : niveles) {
        QLabel* imagen = new QLabel();
        QString ruta =
            (nodo->nombre == "Ciudad Ruinas") ? ":/imagenes/assets/mapas/War.png" :
                (nodo->nombre == "Gimnasio") ? ":/imagenes/assets/gym.jpeg" :
                (nodo->nombre == "Mall") ? ":/imagenes/assets/mapas/Mall.jpeg" :
                (nodo->nombre == "Gasolinera") ? ":/imagenes/assets/mapas/War5.png" :
                (nodo->nombre == "Supermercado") ? ":/imagenes/assets/mapas/War6.png" :
                ":/imagenes/assets/lab.jpeg";

        imagen->setPixmap(QPixmap(ruta).scaled(120, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        QLabel* info = new QLabel(QString("<b>%1</b><br/>Recompensa: %2<br/>Dificultad: %3")
                                      .arg(nodo->nombre)
                                      .arg(nodo->recompensa)
                                      .arg(nodo->dificultad));
        info->setStyleSheet("color:white;");

        QHBoxLayout* fila = new QHBoxLayout();
        fila->addWidget(imagen);
        fila->addWidget(info);

        QWidget* contenedor = new QWidget();
        contenedor->setLayout(fila);
        layout->addWidget(contenedor);
    }
}
