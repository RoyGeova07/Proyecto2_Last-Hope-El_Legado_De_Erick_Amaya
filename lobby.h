#ifndef LOBBY_H
#define LOBBY_H

#include "AtributosPersonaje.h"
#include "npc.h"
#include "personaje.h"
#include "dialogonpc.h"
#include <QLabel>
#include <QList>
#include <QPushButton>
#include <QDialog>
#include <QVBoxLayout>

class lobby : public AtributosPersonaje {
    Q_OBJECT

private:
    QLabel* lblDialogo;
    QLabel* lblDatos;
    QList<NPC*> npcs;
    NPC* npcCercano;
    QLabel* labelPresionarR;
    DialogoNPC* dialogoNPC;
    bool hayPuertaCerca = false;

    void mostrarHintPuerta();
    void ocultarHintPuerta();
    void configurarEscena() override;
    void configurarObstaculos() override;

protected:
    void onMovimientoUpdate() override;

public:
    explicit lobby(personaje* jugadorExistente, QWidget* parent = nullptr);
    void posicionarJugadorEnPuerta();
    void keyPressEvent(QKeyEvent* event) override;
};

#endif // LOBBY_H
