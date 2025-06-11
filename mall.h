#ifndef MALL_H
#define MALL_H
#include "escenabase.h"
#include "zombie.h"

class Mall : public EscenaBase {
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


