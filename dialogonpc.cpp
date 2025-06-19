#include "dialogonpc.h"
#include <QDebug>

DialogoNPC::DialogoNPC(QWidget *parent) : QWidget(parent),
    lblImagenNPC(new QLabel(this)),
    lblTexto(new QLabel(this)),
    layoutPrincipal(new QVBoxLayout(this)),
    layoutContenido(new QHBoxLayout()),
    layoutOpciones(new QVBoxLayout()),
    botonesOpciones()  // Inicialización explícita
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setStyleSheet("background: rgba(0, 0, 0, 200); border-radius: 10px; border: 2px solid white;");
    setFixedSize(800, 250);

    // Configurar widgets
    lblImagenNPC->setFixedSize(150, 150);
    lblImagenNPC->setStyleSheet("border: 1px solid white;");
    lblImagenNPC->setScaledContents(true);

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
    // Limpiar botones anteriores (seguro)
    limpiarBotones();

    // Configurar contenido
    lblImagenNPC->setPixmap(imagenNPC);
    lblTexto->setText(texto);

    // Crear botones nuevos
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

        // Conexión segura con lambda capture correcta
        connect(btn, &QPushButton::clicked, this, [this, i]() {
            emit opcionSeleccionada(i);
        });

        layoutOpciones->addWidget(btn);
        botonesOpciones.append(btn);
    }

    // Posicionamiento
    if (parentWidget()) {
        move(
            (parentWidget()->width() - width()) / 2,
            parentWidget()->height() - height() - 20
            );
    }

    show();
    raise();
}

void DialogoNPC::limpiarBotones()
{
    // Desconectar todas las señales primero
    for (QPushButton* btn : qAsConst(botonesOpciones)) {
        disconnect(btn, nullptr, nullptr, nullptr);
        btn->deleteLater(); // Eliminación segura
    }
    botonesOpciones.clear();
}

void DialogoNPC::ocultarDialogo()
{
    limpiarBotones();
    hide();
}
