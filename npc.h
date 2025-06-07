#ifndef NPC_H
#define NPC_H

#include <QLabel>
#include <QTimer>
#include <QVector>
#include <QPixmap>
#include <QStringList>

class QLabel;

class NPC : public QLabel
{
    Q_OBJECT
public:
    enum class Tipo { NPC1, NPC2, NPC3 };

    NPC(Tipo tipo, QWidget* parent = nullptr);
    void SetAnimacion(const QString& ruta, int cantidadFrames);
    void mostrarDialogo(class DialogoNPC* dialogoUI);  // Cambiado a DialogoNPC
    void mostrarHintInteractuar();
    void ocultarHintInteractuar();
    bool estaHablando() const { return hablando; }

signals:
    void dialogoTerminado();

private slots:
    void AvanzarFrame();
    void manejarOpcionSeleccionada(int opcion);

private:
    Tipo tipo;
    QVector<QPixmap> frames;
    int frameActual;
    QStringList dialogos;
    int indiceDialogo;
    bool hablando;
    QTimer* animacionTimer;
    QTimer* dialogoTimer;
    QLabel* labelPresionaH;
    QString dialogoActual;

    QPixmap obtenerImagenNPC() const;
    QStringList obtenerOpcionesDialogo() const;
};

#endif // NPC_H
