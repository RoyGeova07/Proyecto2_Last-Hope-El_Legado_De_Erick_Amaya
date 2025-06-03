#include "escenabase.h"

class Ciudad : public EscenaBase {
    Q_OBJECT

private:
    void configurarEscena() override;
    void configurarObstaculos() override;

public:
    Ciudad(QWidget* parent = nullptr);
};
