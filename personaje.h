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
    void reiniciarEstadoDefensivo();

    void setEscudo(int valor){escudo=std::clamp(valor,0,20);}
    int getEscudo()const{return escudo;}
    void aumentarEscudo(int cantidad) {
        escudo = std::clamp(escudo + cantidad, 0, 20);
    }

    void setAnimacionGranada(int msCallback = 0, std::function<void()> cb = {});

    bool esCascoAplicado() const { return cascoAplicado; }
    bool esChalecoAplicado() const { return chalecoAplicado; }

    void aplicarCasco()
    {
        if(!cascoAplicado)
        {
            aumentarEscudo(10);
            cascoAplicado=true;
        }
    }
    \
    void aplicarChaleco()
    {
        if(!chalecoAplicado)
        {
            aumentarEscudo(10);
            chalecoAplicado=true;
        }
    }

    bool cascoAplicado=false;
    bool chalecoAplicado=false;

    bool escudoAumentadoPorEquipo() const {
        return escudoAumentado;
    }

    void marcarEscudoAumentado(bool estado) {
        escudoAumentado = estado;
    }

    // Método para resetear el estado cuando se quita equipo
    void resetearBonosEquipo() {
        escudoAumentado = false;
    }

private:

    QVector<QPixmap> frames;
    int frameActual;
    QTimer* timer;
    // aquí si la dirección cambió la última vez que se dibujó
    bool ultimaDireccionDerecha;

    int escudo=0;

    bool enPausa=false;

    QVector<QRect> obstaculos; // aquí obstáculo
    int vida;
    int energia;
    int municiones;
    int velocidadMovimiento;

    bool escudoAumentado = false;

};

#endif // PERSONAJE_H
