#include "AtributosPersonaje.h"
#include "zombie.h"
#include"bala.h"


class Ciudad : public AtributosPersonaje {
    Q_OBJECT

private:
    void configurarEscena() override;
    void configurarObstaculos() override;
    QList<Zombie*> zombies;
    Zombie* zombieCercano;

public:
    Ciudad(QWidget* parent = nullptr);
};
