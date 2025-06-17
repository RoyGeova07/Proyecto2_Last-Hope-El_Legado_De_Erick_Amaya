#ifndef GIMNASIO_H
#define GIMNASIO_H

#include "AtributosPersonaje.h"
#include "zombie.h"
#include <QLabel>
#include <QPixmap>
#include <QKeyEvent>

class Gimnasio : public AtributosPersonaje {
    Q_OBJECT

private:
    void configurarEscena() override;
    void configurarObstaculos() override;
    void onMovimientoUpdate() override;
    void verificarZombiesYMostrarMensaje();
    QList<Zombie*> zombies;
    Zombie* zombieCercano;

public:
   explicit Gimnasio(personaje* jugadorExistente,QWidget* parent=nullptr);
    bool eventFilter(QObject* obj, QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void mostrarNotificacion(const QString& texto);

    QLabel* cofreLabel;
    QLabel* mensajeCofre;
    QPixmap cofreCerrado;
    QPixmap cofreAbierto;
    QLabel* labelNotificacion = nullptr;
    bool cofreAbiertoYa = false;
};

#endif // GIMNASIO_H
