#ifndef LOBBY_H
#define LOBBY_H

#include <QWidget>
#include <QVector>
#include <QRect>
#include <QTimer>
#include <QLabel>
#include <QKeyEvent>

#include "personaje.h"
#include "npc.h"

class lobby : public QWidget
{
    Q_OBJECT

public:
    explicit lobby(QWidget* parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    personaje* jugador;
    QVector<NPC*> npcs;           // Todos los NPCs activos en escena
    NPC* npcCercano = nullptr;    // NPC actualmente más cercano para interacción

    QLabel* lblDialogo;           // Mostrar texto de diálogo
    bool shiftPresionado;
    bool izquierdaPresionada;
    bool derechaPresionada;
    bool arribaPresionado;
    bool abajoPresionado;

    QTimer* movimientoTimer;
    QVector<QRect> obstaculos;    // Lista de áreas que impiden movimiento
};

#endif // LOBBY_H
