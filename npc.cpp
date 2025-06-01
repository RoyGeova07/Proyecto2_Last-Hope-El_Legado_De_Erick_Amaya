#include "npc.h"
#include <QDebug>

NPC::NPC(Tipo tipo, QWidget* parent)
    : QLabel(parent),
    tipo(tipo),
    frameActual(0),
    indiceDialogo(0),
    hablando(false)
{
    this->setFixedSize(128, 128);
    this->move(600, 500);

    // Timers
    animacionTimer = new QTimer(this);
    connect(animacionTimer, &QTimer::timeout, this, &NPC::AvanzarFrame);

    dialogoTimer = new QTimer(this);
    dialogoTimer->setSingleShot(true);

    // Definición por tipo de NPC
    switch(tipo) {
    case Tipo::NPC1:
        SetAnimacion(":/imagenes/assets/NPC/Hablar1_NPC1.png", 7);
        dialogos = {"¡Bienvenido, Erick!", "Ten 3 de Vida"};
        break;
    case Tipo::NPC2:
        SetAnimacion(":/imagenes/assets/NPC/Hablar1_NPC2.png", 4);
        dialogos = {"El camino más corto es la derecha", "Callejón B tiene un botiquín!"};
        break;
    case Tipo::NPC3:
        SetAnimacion(":/imagenes/assets/NPC/Hablar1_NPC3.png", 6);
        dialogos = {"Ten cuidado con los enemigos", "Callejón C tiene municiones!!"};
        break;
    }
}

void NPC::SetAnimacion(const QString& ruta, int cantidadFrames)
{
    QPixmap spriteSheet(ruta);
    if (spriteSheet.isNull()) {
        qDebug() << "Error al cargar animacion:" << ruta;
        return;
    }

    frames.clear();
    frameActual = 0;

    int frameAncho = spriteSheet.width() / cantidadFrames;

    for (int i = 0; i < cantidadFrames; ++i) {
        QPixmap frame = spriteSheet.copy(i * frameAncho, 0, frameAncho, spriteSheet.height());
        frames.append(frame.scaled(this->size(), Qt::KeepAspectRatio));
    }

    if (!frames.isEmpty()) {
        setPixmap(frames.first());
        animacionTimer->start(100);
    }
}

void NPC::AvanzarFrame()
{
    if (frames.isEmpty() || frames.size() <= 1) return;

    frameActual = (frameActual + 1) % frames.size();
    setPixmap(frames[frameActual]);
}

void NPC::mostrarDialogo(QLabel* dialogoLabel)
{
    if (dialogos.isEmpty() || !dialogoLabel) return;

    hablando = true;
    dialogoActual = dialogos.at(indiceDialogo);

    dialogoLabel->setText(dialogoActual);
    dialogoLabel->raise();
    dialogoLabel->show();

    // Avanzar al siguiente diálogo en rotación
    indiceDialogo = (indiceDialogo + 1) % dialogos.size();

    dialogoTimer->disconnect();
    connect(dialogoTimer, &QTimer::timeout, this, [this, dialogoLabel]() {
        hablando = false;
        if (dialogoLabel) {
            dialogoLabel->hide();
        }
    });

    dialogoTimer->start(5000); // Ocultar después de 5 segundos
}

void NPC::mostrarHintInteractuar()
{
    if (!labelPresionaH) {
        labelPresionaH = new QLabel("Presiona H para hablar", this->parentWidget());
        labelPresionaH->setStyleSheet("background: rgba(0, 0, 0, 180); color: white; padding: 5px; border-radius: 5px;");
        labelPresionaH->setAlignment(Qt::AlignCenter);
        labelPresionaH->setFixedSize(150, 30);
    }

    QPoint posicionLabel = this->pos();
    posicionLabel.setY(posicionLabel.y() - 35); // Posición sobre el NPC
    labelPresionaH->move(posicionLabel);
    labelPresionaH->show();
    labelPresionaH->raise();
}

void NPC::ocultarHintInteractuar()
{
    if (labelPresionaH)
        labelPresionaH->hide();
}



