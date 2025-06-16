#include "inventariowidget.h"
#include <QPixmap>
#include <QScrollArea>
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
    : QWidget(parent), inventarioRef(inventario), ordenAscendente(true)
{
    setWindowTitle("Inventario");

    layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->setContentsMargins(5, 5, 5, 5);
    layoutPrincipal->setSpacing(5);

    // Layout para barra de búsqueda y botones
    QHBoxLayout* controlesLayout = new QHBoxLayout();
    controlesLayout->setContentsMargins(0, 0, 0, 0);
    controlesLayout->setSpacing(5);

    // Barra de búsqueda
    barraBusqueda = new QLineEdit(this);
    barraBusqueda->setPlaceholderText("Buscar objeto...");
    barraBusqueda->setClearButtonEnabled(true);
    connect(barraBusqueda, &QLineEdit::textChanged, this, &InventarioWidget::buscarObjetos);

    // Botón de ordenamiento
    btnOrdenar = new QPushButton("Ordenar (A-Z)", this);
    btnOrdenar->setFixedSize(100, 25);
    connect(btnOrdenar, &QPushButton::clicked, this, &InventarioWidget::ordenarInventario);

    // Botón de refrescar
    btnRefrescar = new QPushButton("Refrescar", this);
    btnRefrescar->setFixedSize(100, 25);
    connect(btnRefrescar, &QPushButton::clicked, this, &InventarioWidget::actualizarVista);

    controlesLayout->addWidget(barraBusqueda);
    controlesLayout->addWidget(btnOrdenar);
    controlesLayout->addWidget(btnRefrescar);

    QWidget* controlesWidget = new QWidget(this);
    controlesWidget->setLayout(controlesLayout);
    controlesWidget->setFixedHeight(35); // Altura fija

    gridInventario = new QGridLayout();
    gridInventario->setSpacing(5);
    gridInventario->setContentsMargins(5, 5, 5, 5);

    QWidget* inventarioWidget = new QWidget(this);
    inventarioWidget->setLayout(gridInventario);

    // Scroll area para el inventario
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidget(inventarioWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    layoutPrincipal->addWidget(controlesWidget);
    layoutPrincipal->addWidget(scrollArea);

    layoutPrincipal->setStretch(0, 0);
    layoutPrincipal->setStretch(1, 1);

    actualizarVista();
}

void InventarioWidget::actualizarVista()
{
    buscarObjetos();
}

void InventarioWidget::buscarObjetos()
{
    QString textoBusqueda = barraBusqueda->text().trimmed().toLower();
    limpiarGrid();

    int fila = 0;
    int columna = 0;

    // Función para agregar un ítem al grid
    auto agregarItem = [&](NodoInventario* nodo) {
        QWidget* itemWidget = new QWidget(this);
        itemWidget->setStyleSheet("background-color: rgba(50,50,50,180); border: 1px solid white; border-radius: 5px;");
        itemWidget->setFixedSize(120, 150);

        QVBoxLayout* itemLayout = new QVBoxLayout(itemWidget);
        itemLayout->setContentsMargins(5, 5, 5, 5);

        QLabel* imagenLabel = new QLabel(itemWidget);
        QString rutaImagen = QString(":/imagenes/assets/items/%1.png").arg(nodo->nombre);

        QPixmap pixmap(rutaImagen);
        if (!pixmap.isNull())
            imagenLabel->setPixmap(pixmap.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        else
            imagenLabel->setText("[Sin imagen]");

        imagenLabel->setAlignment(Qt::AlignCenter);

        QLabel* nombreLabel = new QLabel(QString("%1").arg(nodo->nombre), itemWidget);
        QLabel* cantidadLabel = new QLabel(QString("x%1").arg(nodo->cantidad), itemWidget);

        QFont smallFont;
        smallFont.setPointSize(8);

        nombreLabel->setFont(smallFont);
        cantidadLabel->setFont(smallFont);

        nombreLabel->setStyleSheet("color: white;");
        cantidadLabel->setStyleSheet("color: white;");

        itemLayout->addWidget(imagenLabel);
        itemLayout->addWidget(nombreLabel);
        itemLayout->addWidget(cantidadLabel);
        itemLayout->addStretch();

        gridInventario->addWidget(itemWidget, fila, columna);

        columna++;
        if (columna >= 3) {
            columna = 0;
            fila++;
        }
    };

    // Función para recorrer el orden seleccionado
    std::function<void(NodoInventario*)> recorrerOrdenado = [&](NodoInventario* nodo) {
        if (!nodo) return;

        if (ordenAscendente) {
            // Recorrido (A-Z)
            recorrerOrdenado(nodo->izquierda);
            if (textoBusqueda.isEmpty() || nodo->nombre.toLower().contains(textoBusqueda)) {
                agregarItem(nodo);
            }
            recorrerOrdenado(nodo->derecha);
        } else {
            // Recorrido inverso (Z-A)
            recorrerOrdenado(nodo->derecha);
            if (textoBusqueda.isEmpty() || nodo->nombre.toLower().contains(textoBusqueda)) {
                agregarItem(nodo);
            }
            recorrerOrdenado(nodo->izquierda);
        }
    };

    recorrerOrdenado(inventarioRef->obtenerRaiz());
}

void InventarioWidget::ordenarInventario()
{
    ordenAscendente = !ordenAscendente;

    if (ordenAscendente)
    {
        btnOrdenar->setText("Ordenar (A-Z)");
    }
    else
    {
        btnOrdenar->setText("Ordenar (Z-A)");
    }

    buscarObjetos();
}
