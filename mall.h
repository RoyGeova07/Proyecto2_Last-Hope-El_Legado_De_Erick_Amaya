#ifndef MALL_H
#define MALL_H
#include "AtributosPersonaje.h"
#include "zombie.h"

class Mall : public AtributosPersonaje {
    Q_OBJECT

private:
    void configurarEscena() override;
    void configurarObstaculos() override;
    QList<Zombie*> zombies;
    Zombie* zombieCercano;

public:
    Mall(QWidget* parent = nullptr);
};


#endif // MALL_H


