#ifndef SUPERMERCADO_H
#define SUPERMERCADO_H

#include "AtributosPersonaje.h"
#include "zombie.h"

class supermercado : public AtributosPersonaje {
    Q_OBJECT

private:
    void configurarEscena() override;
    void configurarObstaculos() override;
    QList<Zombie*> zombies;
    Zombie* zombieCercano;

public:
    supermercado(QWidget* parent = nullptr);
};


#endif // SUPERMERCADO_H
