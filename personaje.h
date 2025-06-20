#ifndef PERSONAJE_H
#define PERSONAJE_H

#include <QLabel>
#include <QPixmap>
#include <QTimer>
#include <QFile>
#include <QMap>
#include <QStringList>
#include <QRect>
#include "zombie.h"

// Clase personaje
class personaje : public QLabel
{
    Q_OBJECT

public:

    explicit personaje(QWidget* parent = nullptr);

    enum TipoPersonaje {
        P1 = 0, P2 = 1, P3 = 2
    };

    struct DatosAnimacion {
        QString ruta;
        int frames;
    };

    TipoPersonaje personajeActual = P1;

    bool miradoDerecha;
    void SetAnimacion(const QString& ruta, int cantidadFrames);
    void AvanzarFrame();
    void MoverIzquierda();
    void MoverDerecha();
    void MoverArriba();
    void MoverAbajo();
    void DetenerAnimacion();
    void MoverSiNoColisiona(int dx, int dy, const QVector<QRect>& obstaculos);
    void SetAnimacionMovimiento(int velocidad);
    int getVelocidadMovimiento() const { return velocidadMovimiento; }
    void Morir();
    bool estaMuerto = false;

    bool estaMuertoPersonaje() const { return estaMuerto; }

    DatosAnimacion obtenerAnimacion(const QString& tipo, TipoPersonaje personaje);

    QString animacionActual; // bandera para evitar que se reinicie una animacion si ya esta en curso

    void setVidas(int vidas) {
        vida = vidas;
        guardarDatosJugador();
    }
    void setEnergia(int energia) {
        this->energia = energia;
        guardarDatosJugador();
    }
    void setMuniciones(int municiones) {
        this->municiones = municiones;
    }

    int getVidas() { return vida; }
    int getEnergia() { return energia; }
    int getMuniciones() { return municiones; }
    void guardarDatosJugador();
    QMap<QString, int> cargarDatosJugador();

private:

    QVector<QPixmap> frames;
    int frameActual;
    QTimer* timer;
    // aquí si la dirección cambió la última vez que se dibujó
    bool ultimaDireccionDerecha;

    bool enPausa=false;

    QVector<QRect> obstaculos; // aquí obstáculo
    int vida;
    int energia;
    int municiones;
    int velocidadMovimiento;

};

#endif // PERSONAJE_H
