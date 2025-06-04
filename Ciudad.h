#include "escenabase.h"
#include "zombie.h"


class Ciudad : public EscenaBase {
    Q_OBJECT

private:
    void configurarEscena() override;
    void configurarObstaculos() override;
    QList<Zombie*> zombies;
    Zombie* zombieCercano;

public:
    Ciudad(QWidget* parent = nullptr);
};
