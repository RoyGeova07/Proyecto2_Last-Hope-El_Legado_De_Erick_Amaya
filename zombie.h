#ifndef ZOMBIE_H
#define ZOMBIE_H

#include <QLabel>
#include <QPixmap>
#include <QTimer>
#include <QVector>
#include <QPoint>

class Zombie : public QLabel
{
    Q_OBJECT

public:
    enum class Tipo {
        Z1,
        Z2,
        Z3,
        Z4,
        Z5,
        Z6,
        Z7,
        BOSS
    };

    explicit Zombie(Tipo tipo, QWidget* parent = nullptr);

    void perseguirJugador(QWidget* objetivo);
    void setVelocidad(int v) { velocidad = v; }

    void SetAnimacion(const QString& ruta, int cantidadFrames, bool loop);
    void AvanzarFrame();


    // UI contextual
    void mostrarHintInteractuar();
    void ocultarHintInteractuar();
    QLabel* getHintLabel() const { return labelPresionaA; }

    void SetAnimacionMovimientoZombie();
    bool mirandoALaDerecha = false;
    void recibirDanio(int cantidad);
    void setAnimacionHerido();
    bool muerto = false;
    bool animacionLoop = true;

    void pausar();
    void reanudar();
    QRect hitbox() const;
    QPoint centroHitbox() const;  // <-- ¡NUEVA! Devuelve el centro real de la hitbox

signals:
    void ColisionConJugador();

private slots:
    void moverHaciaJugador();

private:
    Tipo tipo;
    QVector<QPixmap> frames;
    int frameActual;
    QTimer* animacionTimer;
    QTimer* movimientoTimer = nullptr;
    QWidget* objetivo = nullptr;
    int velocidad = 2;

    bool atacando = false;
    bool estaAtacando = false;
    bool danioEmitido = false;
    int frameAtaqueFinal = 0;
    void realizarAtaque();

    int vida;
    int vidaMaxima;

    bool enPausa = false;

    QLabel* barraFondo = nullptr;
    QLabel* barraVida = nullptr;

    void inicializarBarraVida();
    void actualizarBarraVida();

    QLabel* labelPresionaA = nullptr;
};

#endif // ZOMBIE_H
