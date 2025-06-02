#ifndef NPC_H
#define NPC_H

#include <QLabel>
#include <QPixmap>
#include <QTimer>
#include <QVector>

class NPC : public QLabel
{
    Q_OBJECT

public:
    enum class Tipo {
        NPC1,
        NPC2,
        NPC3
    };

    explicit NPC(Tipo tipo, QWidget* parent = nullptr);

    //aqui para la animacion
    void SetAnimacion(const QString& ruta, int cantidadFrames);
    void AvanzarFrame();

    //aqui para Dialogo
    void mostrarDialogo(QLabel* dialogoLabel);
    bool estaHablando() const { return hablando; }

    // UI contextual
    void mostrarHintInteractuar();       // Mostrar "Presiona H para hablar"
    void ocultarHintInteractuar();       // Ocultar label de interacción
    QLabel* getHintLabel() const { return labelPresionaH; }

private:
    Tipo tipo;
    QVector<QPixmap> frames;
    int frameActual;
    QTimer* animacionTimer;
    QTimer* dialogoTimer;

    QVector<QString> dialogos;
    int indiceDialogo;
    QString dialogoActual;
    bool hablando;

    QLabel* labelPresionaH = nullptr;
};

#endif // NPC_H
