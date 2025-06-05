#include "escenabase.h"
#include "npc.h"
#include"personaje.h"

class lobby : public EscenaBase {
    Q_OBJECT

private:

    QLabel* lblDialogo;
    QLabel* lblDatos;
    QList<NPC*> npcs;
    NPC* npcCercano;
    QLabel*labelPresionarR;
    bool hayPuertaCerca = false;

    void mostrarHintPuerta();
    void ocultarHintPuerta();
    void configurarEscena() override;
    void configurarObstaculos() override;
protected:
    void onMovimientoUpdate() override;

public:
    explicit lobby(personaje*jugadorExistente, QWidget* parent=nullptr);
    void posicionarJugadorEnPuerta();
    void keyPressEvent(QKeyEvent* event) override;
};
