#include "inventariowidget.h"
#include <QPixmap>
#include <QDebug>

InventarioWidget::InventarioWidget(Inventario* inventario, QWidget *parent)
    : QWidget(parent), inventarioRef(inventario)
{
    setWindowTitle("Inventario");
    setFixedSize(600, 500);

    layoutPrincipal = new QVBoxLayout(this);
    gridInventario = new QGridLayout();

    // Espaciado bonito xd
    gridInventario->setSpacing(10);
    gridInventario->setContentsMargins(10, 10, 10, 10);

    btnRefrescar = new QPushButton("Refrescar", this);
    connect(btnRefrescar, &QPushButton::clicked, this, &InventarioWidget::actualizarVista);

    layoutPrincipal->addLayout(gridInventario);
    layoutPrincipal->addWidget(btnRefrescar);

    actualizarVista();
}

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

void InventarioWidget::actualizarVista()
{
    limpiarGrid();

    // Recorrer el ABB manualmente
    int fila = 0;
    int columna = 0;

    std::function<void(NodoInventario*)> recorrerInOrden = [&](NodoInventario* nodo)
    {
        if (!nodo) return;

        recorrerInOrden(nodo->izquierda);

        // Crear un widget para mostrar el objeto
        QWidget* itemWidget = new QWidget(this);
        itemWidget->setStyleSheet("background-color: rgba(50,50,50,180); border: 1px solid white; border-radius: 5px;");

        QVBoxLayout* itemLayout = new QVBoxLayout(itemWidget);

        // Cargar imagen si existe
        QLabel* imagenLabel = new QLabel(itemWidget);
        QString rutaImagen = QString(":/imagenes/assets/items/%1.png").arg(nodo->nombre);

        QPixmap pixmap(rutaImagen);
        if (!pixmap.isNull())
            imagenLabel->setPixmap(pixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        else
            imagenLabel->setText("[Sin imagen]");

        imagenLabel->setAlignment(Qt::AlignCenter);

        // Etiquetas de detalles
        QLabel*nombreLabel=new QLabel(QString("Nombre: %1").arg(nodo->nombre), itemWidget);
        QLabel*cantidadLabel=new QLabel(QString("Cantidad: %1").arg(nodo->cantidad), itemWidget);
        QLabel*tipoLabel=new QLabel(QString("Tipo: %1").arg(nodo->tipo), itemWidget);
        QLabel*usoLabel=new QLabel(QString("Uso: %1").arg(nodo->uso), itemWidget);

        nombreLabel->setStyleSheet("color: white;");
        cantidadLabel->setStyleSheet("color: white;");
        tipoLabel->setStyleSheet("color: white;");
        usoLabel->setStyleSheet("color: white;");

        // Agregar al layout
        itemLayout->addWidget(imagenLabel);
        itemLayout->addWidget(nombreLabel);
        itemLayout->addWidget(cantidadLabel);
        itemLayout->addWidget(tipoLabel);
        itemLayout->addWidget(usoLabel);
        itemLayout->addStretch();

        gridInventario->addWidget(itemWidget, fila, columna);

        columna++;
        if (columna >= 4) // 4 columnas por fila
        {
            columna = 0;
            fila++;
        }

        recorrerInOrden(nodo->derecha);
    };

    recorrerInOrden(inventarioRef->obtenerRaiz());
}
