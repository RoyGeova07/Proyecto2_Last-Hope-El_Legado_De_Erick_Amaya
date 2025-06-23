#ifndef GRANADA_H
#define GRANADA_H

#include <QLabel>
#include <QTimer>
#include <QPixmap>
#include "zombie.h"

class Granada : public QLabel
{

    Q_OBJECT

public:

    explicit Granada(QWidget* parent = nullptr);
    void lanzar(bool haciaDerecha);

private slots:
    void mover();
    void explotar();
    void avanzarFrameExplosion();

private:
    QTimer* timerMovimiento;
    int velocidad;
    int distanciaRecorrida;
    int maxDistancia;
    QList<Zombie*> enemigos;
    bool direccionDerecha;

    // ---------- Animaciin de explosion ----------
    QVector<QPixmap> framesExplosion;
    QTimer* timerExplosion = nullptr;
    int frameActualExplosion=0;

    float tiempoVuelo=0;
    float posYInicial=0;
    float velocidadY=-18; // componente vertical inicial (negativo porque sube)
    float gravedad=1.2;   // gravedad que lo baja poco a poco

    Granada();
};

#endif // GRANADA_H
