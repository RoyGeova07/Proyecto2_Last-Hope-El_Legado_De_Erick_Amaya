#ifndef GASOLINERA_H
#define GASOLINERA_H

#include "AtributosPersonaje.h"
#include "zombie.h"
#include <QLabel>
#include <QPixmap>
#include <QKeyEvent>

class Gasolinera : public AtributosPersonaje {
    Q_OBJECT

private:
    void configurarEscena() override;
    void configurarObstaculos() override;
    void onMovimientoUpdate() override;
    void verificarZombiesYMostrarMensaje();
    QList<Zombie*> zombies;
    Zombie* zombieCercano;

public:
    explicit Gasolinera(personaje* jugadorExistente,QWidget* parent=nullptr);
    void keyPressEvent(QKeyEvent* event) override;
    void mostrarNotificacion(const QString& texto);

    QLabel* cofreLabel;
    QLabel* mensajeCofre;
    QPixmap cofreCerrado;
    QPixmap cofreAbierto;
    QLabel* labelNotificacion = nullptr;
    bool cofreAbiertoYa = false;

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
};

#endif // GASOLINERA_H
