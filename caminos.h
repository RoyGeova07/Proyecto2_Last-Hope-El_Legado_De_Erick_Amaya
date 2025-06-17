#ifndef CAMINOS_H
#define CAMINOS_H

#include"personaje.h"
#include"AtributosPersonaje.h"
#include "dialogonpc.h"
#include "npc.h"

class Caminos:public AtributosPersonaje
{

    Q_OBJECT

private:

    QLabel* fondoLabel;
    QVector<QRect> obstaculosCaminos;
    int rutaActual; // 1 = RUTA_1, 2 = RUTA_2
    QLabel* labelPresionarT;
    void keyPressEvent(QKeyEvent* event);

    DialogoNPC* dialogoNPC;
    bool primeraVez = true;
    QMap<int, QList<NPC*>> npcsPorRuta;
    NPC* npcCercano;



    void configurarEscena() override;
    void configurarObstaculos() override;

    void posicionarJugadorDeVueltaARuta2_Desde_Ruta3();
    void posicionarJugadorDeVueltaA_Ruta1_Desde_Ruta4();
    void posicionarJugadorDeVueltaA_Ruta4_Desde_Ruta5();
    void posicionarJugadorDeVueltaA_Ruta2_Desde_Ruta6();
    void posicionarJugadorDeVueltaA_Ruta6_Desde_Ruta3();
    void posicionarJugadorDeVueltaA_Ruta3_Desde_Ruta6();
    void posicionarJugadorDeVueltaA_Ruta5_Desde_Ruta6();
    void posicionarJugadorDeVueltaA_Ruta6_Desde_Ruta5();
    void posicionarJugadorDeVueltaA_Ruta7_Desde_Ruta3();
    void posicionarJugadorDeVueltaA_Ruta7_Desde_Ruta9();
    void posicionarJugadorDeVueltaA_Ruta8_Desde_Ruta5();
    void posicionarJugadorDeVueltaA_Ruta8_Desde_Ruta9();
    void posicionarJugadorDeVueltaA_Ruta9_Desde_Ruta6();
    void posicionarJugadorDeVueltaA_Ruta9_Desde_Ruta10();
    void posicionarJugadorDeVueltaA_Ruta9_Desde_Ruta7();
    void posicionarJugadorDeVueltaA_Ruta9_Desde_Ruta8();
    void posicionarJugadorDeVueltaA_Ruta10_Desde_Ruta9();

protected:

    void onMovimientoUpdate() override;


public:

    explicit Caminos(personaje*jugadorExistente,QWidget* parent = nullptr);
    bool regresandoAlLobby = false;
    void cambiarRuta(int nuevaRuta);
    void posicionarJugadorEnCalleRuta1();
    void posicionarJugadorEnCalleRuta2();
    void posicionarJugadorEnCalleRuta3();
    void posicionarJugadorEnCalleRuta4();
    void posicionarJugadorEnCalleRuta5();
    void posicionarJugadroEnCalleRuta6();
    void posicionarJugadorEnCalleRuta7();
    void posicionarJugadorEnCalleRuta8();
    void posicionarJugadorEnCalleRuta9();
    void posicionarJugadorEnCalleRuta10();

};

#endif // CAMINOS_H
