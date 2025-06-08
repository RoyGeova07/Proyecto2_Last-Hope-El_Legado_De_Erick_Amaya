#ifndef ESCENABASE_H
#define ESCENABASE_H

#include <QWidget>
#include <QTimer>
#include <QKeyEvent>
#include <QRect>
#include "personaje.h"
#include"inventario.h"
#include"inventariowidget.h"

class EscenaBase : public QWidget {
    Q_OBJECT

protected:
    personaje* jugador;
    QTimer* movimientoTimer;
    QList<QRect> obstaculos;

    // Estado de teclas
    bool shiftPresionado;
    bool izquierdaPresionada;
    bool derechaPresionada;
    bool arribaPresionado;
    bool abajoPresionado;
    bool ZPresionado;

    InventarioWidget* inventarioWidget;

    //aqui Metodos virtuales
    virtual void configurarEscena() = 0;
    virtual void configurarObstaculos() = 0;
    virtual void configurarNPCs() {}
    virtual void onMovimientoUpdate() {}

    //aqui para la barra de vida
    QLabel*barraVidaLabel;
    QWidget* barraVidaInterna;
    QLabel* barraVidaTexto;
    void ActualizarBarraVida();

public:
    explicit EscenaBase(QWidget* parent = nullptr);
    explicit EscenaBase(personaje*jugador,QWidget*parent=nullptr);
    virtual ~EscenaBase();
    void ResetearMovimiento();
    void inicializarJugador();
    void Movimientos();
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
};

#endif // ESCENABASE_H
