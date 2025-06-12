#ifndef ZOMBIE_H
#define ZOMBIE_H
#include <QLabel>
#include <QPixmap>
#include <QTimer>
#include <QVector>

class Zombie : public QLabel
{
    Q_OBJECT

public:
    enum class Tipo {
        Z1,
        Z2,
        Z3
    };

    void perseguirJugador(QWidget*objetivo);
    void setVelocidad(int v){velocidad=v;}

    explicit Zombie(Tipo tipo, QWidget* parent = nullptr);

    //aqui para la animacion
    void SetAnimacion(const QString& ruta, int cantidadFrames,bool loop);
    void AvanzarFrame();

    // UI contextual
    void mostrarHintInteractuar();       // Mostrar "Presiona A para atacar"
    void ocultarHintInteractuar();       // Ocultar label de interacción
    QLabel* getHintLabel() const { return labelPresionaA;}

    void SetAnimacionMovimientoZombie();
    bool mirandoALaDerecha=false;
    void recibirDanio(int cantidad);
    void setAnimacionHerido();
    bool muerto=false;
    bool animacionLoop=true;

signals:

    void ColisionConJugador();

private slots:
    void moverHaciaJugador();

private:
    Tipo tipo;
    QVector<QPixmap> frames;
    int frameActual;
    QTimer*animacionTimer;
    QTimer*movimientoTimer=nullptr;
    QWidget*objetivo=nullptr;
    int velocidad=2;

    bool atacando=false;//esta es para la ANIMACION DE ATACAR
    bool estaAtacando=false;//esta es para CUANDO HACE LA ANIMACION DE ATACAR Y ME QUITA VIDA
    bool danioEmitido=false;
    int frameAtaqueFinal=0;
    void realizarAtaque();

    int vida=10;
    const int vidaMaxima=10;

    QLabel*barraFondo=nullptr;
    QLabel*barraVida=nullptr;

    void inicializarBarraVida();
    void actualizarBarraVida();

    QLabel* labelPresionaA = nullptr;
};

#endif // ZOMBIE_H
