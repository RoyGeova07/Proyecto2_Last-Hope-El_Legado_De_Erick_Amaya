#ifndef CAMINOS_H
#define CAMINOS_H

#include"personaje.h"
#include"escenabase.h"
#include "dialogonpc.h"
#include "npc.h"

class Caminos:public EscenaBase
{

    Q_OBJECT

private:

    QLabel* fondoLabel;
    QVector<QRect> obstaculosCaminos;
    int rutaActual; // 1 = RUTA_1, 2 = RUTA_2
    QLabel* labelPresionarT;
    void keyPressEvent(QKeyEvent* event);

    DialogoNPC* dialogoNPC;
    QMap<int, QList<NPC*>> npcsPorRuta;
    NPC* npcCercano;

    void configurarEscena() override;
    void configurarObstaculos() override;
    void cambiarRuta(int nuevaRuta);

    void posicionarJugadorEnCalleRuta1();
    void posicionarJugadorEnCalleRuta2();
    void posicionarJugadorEnCalleRuta3();
    void posicionarJugadorEnCalleRuta4();
    void posicionarJugadorEnCalleRuta5();
    void posicionarJugadroEnCalleRuta6();

    void posicionarJugadorDeVueltaARuta2_Desde_Ruta3();
    void posicionarJugadorDeVueltaA_Ruta1_Desde_Ruta4();
    void posicionarJugadorDeVueltaA_Ruta4_Desde_Ruta5();
    void posicionarJugadorDeVueltaA_Ruta2_Desde_Ruta6();
    void posicionarJugadorDeVueltaA_Ruta6_Desde_Ruta3();
    void posicionarJugadorDeVueltaA_Ruta3_Desde_Ruta6();
    void posicionarJugadorDeVueltaA_Ruta5_Desde_Ruta6();
    void posicionarJugadorDeVueltaA_Ruta6_Desde_Ruta5();

protected:

    void onMovimientoUpdate() override;


public:

    explicit Caminos(personaje*jugadorExistente,QWidget* parent = nullptr);
    bool regresandoAlLobby = false;
};

#endif // CAMINOS_H
