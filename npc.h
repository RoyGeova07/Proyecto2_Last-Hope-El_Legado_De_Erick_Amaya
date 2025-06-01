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

    // Animacion
    void SetAnimacion(const QString& ruta, int cantidadFrames);
    void AvanzarFrame();

    // Dialogo
    void mostrarDialogo(QLabel* dialogoLabel);
    bool estaHablando() const { return hablando; }

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
};

#endif // NPC_H
