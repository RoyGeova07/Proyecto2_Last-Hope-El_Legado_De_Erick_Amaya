#ifndef CAMINOS_H
#define CAMINOS_H

#include"personaje.h"
#include"escenabase.h"

class Caminos:public EscenaBase
{

    Q_OBJECT

private:

    QLabel* fondoLabel;
    QVector<QRect> obstaculosCaminos;
    int rutaActual; // 1 = RUTA_1, 2 = RUTA_2
    QLabel* labelPresionarT;
    void keyPressEvent(QKeyEvent* event);

    void configurarEscena() override;
    void configurarObstaculos() override;
    void cambiarRuta(int nuevaRuta);

    void posicionarJugadorEnCalleRuta1();
    void posicionarJugadorEnCalleRuta2();
    void posicionarJugadorEnCalleRuta3();

protected:

    void onMovimientoUpdate() override;


public:

    explicit Caminos(personaje*jugadorExistente,QWidget* parent = nullptr);
    bool regresandoAlLobby = false;
};

#endif // CAMINOS_H
