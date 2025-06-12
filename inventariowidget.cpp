#include "inventariowidget.h"
#include <QPixmap>
#include <QDebug>


void InventarioWidget::limpiarGrid()
{
    while (QLayoutItem* item = gridInventario->takeAt(0))
    {
        if (QWidget* widget = item->widget())
        {
            widget->deleteLater();
        }
        delete item;
    }
}

InventarioWidget::InventarioWidget(Inventario* inventario, QWidget *parent)
    : QWidget(parent), inventarioRef(inventario)
{
    setWindowTitle("Inventario");
    // Eliminar setFixedSize y dejar que el layout maneje el tamaño

    layoutPrincipal = new QVBoxLayout(this);
    gridInventario = new QGridLayout();

    // Ajustar espaciado para versión compacta
    gridInventario->setSpacing(5);  // Reducido de 10 a 5
    gridInventario->setContentsMargins(5, 5, 5, 5);  // Reducido de 10 a 5

    btnRefrescar = new QPushButton("Refrescar", this);
    btnRefrescar->setFixedHeight(25);  // Botón más compacto
    connect(btnRefrescar, &QPushButton::clicked, this, &InventarioWidget::actualizarVista);

    layoutPrincipal->addLayout(gridInventario);
    layoutPrincipal->addWidget(btnRefrescar);
    layoutPrincipal->setContentsMargins(0, 0, 0, 0);  // Eliminar márgenes externos

    actualizarVista();
}

void InventarioWidget::actualizarVista()
{
    limpiarGrid();

    int fila = 0;
    int columna = 0;

    std::function<void(NodoInventario*)> recorrerInOrden = [&](NodoInventario* nodo)
    {
        if (!nodo) return;

        recorrerInOrden(nodo->izquierda);

        QWidget* itemWidget = new QWidget(this);
        itemWidget->setStyleSheet("background-color: rgba(50,50,50,180); border: 1px solid white; border-radius: 5px;");
        itemWidget->setFixedSize(120, 150);  // Tamaño fijo para cada ítem

        QVBoxLayout* itemLayout = new QVBoxLayout(itemWidget);
        itemLayout->setContentsMargins(5, 5, 5, 5);  // Márgenes internos reducidos

        QLabel* imagenLabel = new QLabel(itemWidget);
        QString rutaImagen = QString(":/imagenes/assets/items/%1.png").arg(nodo->nombre);

        QPixmap pixmap(rutaImagen);
        if (!pixmap.isNull())
            imagenLabel->setPixmap(pixmap.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));  // Imagen más pequeña
        else
            imagenLabel->setText("[Sin imagen]");

        imagenLabel->setAlignment(Qt::AlignCenter);

        // Etiquetas más compactas
        QLabel* nombreLabel = new QLabel(QString("%1").arg(nodo->nombre), itemWidget);
        QLabel* cantidadLabel = new QLabel(QString("x%1").arg(nodo->cantidad), itemWidget);

        // Fuentes más pequeñas
        QFont smallFont;
        smallFont.setPointSize(8);

        nombreLabel->setFont(smallFont);
        cantidadLabel->setFont(smallFont);

        nombreLabel->setStyleSheet("color: white;");
        cantidadLabel->setStyleSheet("color: white;");

        // Layout más compacto
        itemLayout->addWidget(imagenLabel);
        itemLayout->addWidget(nombreLabel);
        itemLayout->addWidget(cantidadLabel);
        itemLayout->addStretch();

        gridInventario->addWidget(itemWidget, fila, columna);

        columna++;
        if (columna >= 3)  // Reducir columnas de 4 a 3
        {
            columna = 0;
            fila++;
        }

        recorrerInOrden(nodo->derecha);
    };

    recorrerInOrden(inventarioRef->obtenerRaiz());
}
