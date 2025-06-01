#include "escenabase.h"
#include "npc.h"

class lobby : public EscenaBase {
    Q_OBJECT

private:
    QLabel* lblDialogo;
    QList<NPC*> npcs;
    NPC* npcCercano;

    void configurarEscena() override;
    void configurarObstaculos() override;
    void configurarNPCs() override;
protected:
    void onMovimientoUpdate() override;

public:
    lobby(QWidget* parent = nullptr);
    void keyPressEvent(QKeyEvent* event) override;
};
