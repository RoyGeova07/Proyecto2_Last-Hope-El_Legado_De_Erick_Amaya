#ifndef ATRIBUTOSPERSONAJE_H
#define ATRIBUTOSPERSONAJE_H

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QKeyEvent>
#include <QRect>
#include "personaje.h"
#include"inventario.h"
#include"inventariowidget.h"
#include "TablaHash.h"
#include "TablaWidget.h"
#include"bala.h"
#include "mapa.h"
#include <QTabWidget>
#include"granada.h"
#include"arbolavl.h"

class AtributosPersonaje : public QWidget {
    Q_OBJECT

protected:
    personaje* jugador;
    QTimer* movimientoTimer;
    QList<QRect> obstaculos;

    QLabel* labelDistancias;

    TablaHash& tablaNiveles;

    // Estado de teclas
    bool shiftPresionado;
    bool izquierdaPresionada;
    bool derechaPresionada;
    bool arribaPresionado;
    bool abajoPresionado;
    bool ZPresionado;
    bool XPresionado;

    bool disparandoAhora=false;//me servira para sincronizar los municiones con la animacion de disparo del personje
    bool puedeDisparar=false;
    QTimer*disparoTimer=nullptr;//para cancelar disparo si se suelta Z

    // Widgets de interfazAdd commentMore actions
    QTabWidget* tabWidget;
    Mapa* mapaWidget;
    QWidget* mapaTab;
    QWidget* tablaTab;
    void inicializarTabWidget();

    InventarioWidget* inventarioWidget;
    TablaWidget* tablaWidget;

    //aqui Metodos virtuales
    virtual void configurarEscena() = 0;
    virtual void configurarObstaculos() = 0;
    virtual void configurarNPCs() {}
    virtual void onMovimientoUpdate() {}

    //aqui para la barra de vida
    QLabel*barraVidaLabel;
    QWidget* barraVidaInterna;
    QLabel* barraVidaTexto;
    QLabel*labelMuniciones;

    QLabel* barraEscudoLabel;
    QWidget* barraEscudoInterna;
    QLabel* barraEscudoTexto;
    void ActualizarBarraEscudo();

    QList<Bala*>balasActivas;
    void ActualizarBarraVida();
    void iniciarCuracion(int cantidad,const QString& tipo);

signals:
    void actualizarDistancias(float principal, float alterna, const QString& origen, const QString& destino);

private:

    QTimer*curacionTimer=nullptr;
    bool estadoCurandose=false;
    void intentarCurarse();
    void terminarCuracion(int cantidad);
    void mostrarSeleccionBotiquin();
    personaje::TipoPersonaje personajeActual=personaje::P1;
    QLabel*labelNotificacion=nullptr;
    void intentarMelee();
    QTimer* meleeTimer = nullptr;
    bool puedeMelee = true;

    void detenerDisparo();

    QString habilidad1 = "";
    QString habilidad2 = "";
    QList<Zombie*> zombiesEnEscena;

public:
    explicit AtributosPersonaje(QWidget* parent = nullptr);
    explicit AtributosPersonaje(personaje*jugador,QWidget*parent=nullptr);
    virtual ~AtributosPersonaje();
    void ResetearMovimiento();
    void inicializarJugador();
    void Movimientos();
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void ActualizarMuniciones();
    void intentarDisparar();
    void CancelarCuracion();
    void mostrarNotificacion(const QString& texto);

    void setZombiesEnEscena(const QList<Zombie*>& lista);

    bool lanzarGranadaHabilitado = true;
    void setPermitirLanzarGranada(bool permitido) { lanzarGranadaHabilitado = permitido; }

    QList<Granada*> granadasActivas;
    void intentarLanzarGranada();

    ArbolAvl*arbolZombies;
    ArbolAvl*arbolMapas;


    bool ordenAscendente = true;


};

#endif // ATRIBUTOSPERSONAJE_H
