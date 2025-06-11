#ifndef GASOLINERA_H
#define GASOLINERA_H

#include "AtributosPersonaje.h"
#include "zombie.h"


class Gasolinera : public AtributosPersonaje {
    Q_OBJECT

private:
    void configurarEscena() override;
    void configurarObstaculos() override;
    QList<Zombie*> zombies;
    Zombie* zombieCercano;

public:
    Gasolinera(QWidget* parent = nullptr);
};


#endif // GASOLINERA_H
