#ifndef GIMNASIO_H
#define GIMNASIO_H

#include "AtributosPersonaje.h"
#include "zombie.h"

class Gimnasio : public AtributosPersonaje {
    Q_OBJECT

private:
    void configurarEscena() override;
    void configurarObstaculos() override;
    QList<Zombie*> zombies;
    Zombie* zombieCercano;

public:
    Gimnasio(QWidget* parent = nullptr);
};

#endif // GIMNASIO_H
