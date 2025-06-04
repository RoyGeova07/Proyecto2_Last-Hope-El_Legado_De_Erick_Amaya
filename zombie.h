#ifndef ZOMBIE_H
#define ZOMBIE_H
#include <QLabel>
#include <QPixmap>
#include <QTimer>
#include <QVector>

class Zombie : public QLabel
{
    Q_OBJECT

public:
    enum class Tipo {
        Z1,
        Z2,
        Z3
    };

    explicit Zombie(Tipo tipo, QWidget* parent = nullptr);

    //aqui para la animacion
    void SetAnimacion(const QString& ruta, int cantidadFrames);
    void AvanzarFrame();

    // UI contextual
    void mostrarHintInteractuar();       // Mostrar "Presiona A para atacar"
    void ocultarHintInteractuar();       // Ocultar label de interacción
    QLabel* getHintLabel() const { return labelPresionaA; }

private:
    Tipo tipo;
    QVector<QPixmap> frames;
    int frameActual;
    QTimer* animacionTimer;

    QLabel* labelPresionaA = nullptr;
};

#endif // ZOMBIE_H
