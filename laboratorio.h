#ifndef LABORATORIO_H
#define LABORATORIO_H

#include "AtributosPersonaje.h"
#include "zombie.h"

class laboratorio : public AtributosPersonaje {
    Q_OBJECT

private:
    void configurarEscena() override;
    void configurarObstaculos() override;
    QList<Zombie*> zombies;
    Zombie* zombieCercano;

public:
    laboratorio(personaje* jugadorExistente,QWidget* parent=nullptr);
};

#endif // LABORATORIO_H
