#include "dialogonpc.h"
#include <QDebug>

DialogoNPC::DialogoNPC(QWidget *parent) : QWidget(parent)
{
    // Configurar la ventana de diálogo
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setStyleSheet("background: rgba(0, 0, 0, 200); border-radius: 10px; border: 2px solid white;");
    setFixedSize(800, 250);

    // Crear layouts
    layoutPrincipal = new QVBoxLayout(this);
    layoutContenido = new QHBoxLayout();
    layoutOpciones = new QVBoxLayout();

    // Crear widgets
    lblImagenNPC = new QLabel(this);
    lblImagenNPC->setFixedSize(150, 150);
    lblImagenNPC->setStyleSheet("border: 1px solid white;");
    lblImagenNPC->setScaledContents(true);

    lblTexto = new QLabel(this);
    lblTexto->setWordWrap(true);
    lblTexto->setStyleSheet("color: white; font-size: 16px; padding: 10px;");
    lblTexto->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    // Organizar layout
    layoutContenido->addWidget(lblImagenNPC);
    layoutContenido->addWidget(lblTexto, 1);

    layoutPrincipal->addLayout(layoutContenido);
    layoutPrincipal->addLayout(layoutOpciones);

    hide();
}

void DialogoNPC::mostrarDialogo(const QString &texto, const QPixmap &imagenNPC, const QStringList &opciones)
{
    // Configurar imagen y texto
    lblImagenNPC->setPixmap(imagenNPC);
    lblTexto->setText(texto);

    // Limpiar opciones anteriores
    qDeleteAll(botonesOpciones);
    botonesOpciones.clear();

    // Crear nuevas opciones
    for (int i = 0; i < opciones.size(); ++i) {
        QPushButton *btn = new QPushButton(QString("%1. %2").arg(QChar('A' + i)).arg(opciones[i]), this);
        btn->setStyleSheet(
            "QPushButton {"
            "   color: white;"
            "   background: rgba(50, 50, 50, 200);"
            "   padding: 8px;"
            "   border: 1px solid white;"
            "   border-radius: 5px;"
            "   min-width: 200px;"
            "   text-align: left;"
            "}"
            "QPushButton:hover {"
            "   background: rgba(70, 70, 70, 200);"
            "}"
            );
        btn->setCursor(Qt::PointingHandCursor);

        connect(btn, &QPushButton::clicked, this, [this, i]() {
            emit opcionSeleccionada(i); // Usando el nombre correcto de la señal
            hide();
        });

        layoutOpciones->addWidget(btn);
        botonesOpciones.append(btn);
    }

    // Mostrar en la parte inferior de la pantalla
    if (parentWidget()) {
        move(
            (parentWidget()->width() - width()) / 2,
            parentWidget()->height() - height() - 20
            );
    }

    show();
    raise();
}

void DialogoNPC::ocultarDialogo()
{
    hide();
}
