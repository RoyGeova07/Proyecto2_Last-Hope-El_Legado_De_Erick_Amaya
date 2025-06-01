#include "escenabase.h"

class Nivel01 : public EscenaBase {
    Q_OBJECT

private:
    void configurarEscena() override;
    void configurarObstaculos() override;

public:
    Nivel01(QWidget* parent = nullptr);
};
