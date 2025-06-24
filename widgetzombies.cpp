#include "widgetzombies.h"
#include <QHBoxLayout>
#include <QDebug>

WidgetZombies::WidgetZombies(ArbolAvl* arbol, QWidget* parent)
    : QWidget(parent), arbolZombies(arbol) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    scroll = new QScrollArea();
    scroll->setWidgetResizable(true);

    QWidget* contenedor = new QWidget();
    layoutZombies = new QVBoxLayout(contenedor);
    layoutZombies->setAlignment(Qt::AlignTop);
    scroll->setWidget(contenedor);
    mainLayout->addWidget(new QLabel("🧟 Lista de Enemigos:"));
    mainLayout->addWidget(scroll);

    actualizar();  // Mostrar inicialmente ordenados por nivel
}

void WidgetZombies::actualizar(bool ordenPorNivel) {
    // Limpiar layout previo
    QLayoutItem* item;
    while ((item = layoutZombies->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    QList<NodoAvl*> enemigos = arbolZombies->obtenerEnOrden(ordenPorNivel);

    for (NodoAvl* nodo : enemigos) {
        QString nombreSprite = nodo->nombre;

        // Reutilizar sprites: BOSS usa Z4, Z7 usa Z6
        if (nombreSprite == "BOSS") nombreSprite = "Z4";
        else if (nombreSprite == "Z7") nombreSprite = "Z6";

        QString rutaSprite = QString(":/imagenes/assets/Zombies/Idle_%1.png").arg(nombreSprite);
        QPixmap spriteSheet(rutaSprite);

        if (!spriteSheet.isNull()) {
            int totalFrames = 9;
            int frameWidth = spriteSheet.width() / totalFrames;
            QPixmap primerFrame = spriteSheet.copy(0, 0, frameWidth, spriteSheet.height());

            // Agrandar si es Z7 o BOSS
            QSize size = (nodo->nombre == "BOSS" || nodo->nombre == "Z7")
                             ? QSize(150, 150)
                             : QSize(100, 100);
            primerFrame = primerFrame.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

            QLabel* imagen = new QLabel();
            imagen->setPixmap(primerFrame);

            QLabel* info = new QLabel(
                QString("<b>%1</b><br/>Vida: %2 | Daño: %3 | Velocidad: %4 | Dificultad: %5")
                    .arg(nodo->nombre)
                    .arg(nodo->vida)
                    .arg(nodo->danio)
                    .arg(nodo->velocidad)
                    .arg(nodo->dificultad)
                );
            info->setStyleSheet("color: white;");
            info->setMinimumWidth(200);

            QHBoxLayout* fila = new QHBoxLayout();
            fila->addWidget(imagen);
            fila->addWidget(info);
            fila->addStretch();

            QWidget* filaWidget = new QWidget();
            filaWidget->setLayout(fila);

            layoutZombies->addWidget(filaWidget);
            layoutZombies->addSpacing(10);  // espacio entre enemigos
        } else {
            qDebug() << "No se encontró sprite para:" << nombreSprite;
        }
    }
}
