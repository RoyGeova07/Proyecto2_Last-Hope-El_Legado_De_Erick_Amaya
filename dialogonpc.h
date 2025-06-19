#ifndef DIALOGONPC_H
#define DIALOGONPC_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class DialogoNPC : public QWidget
{
    Q_OBJECT
public:
    explicit DialogoNPC(QWidget *parent = nullptr);
    void mostrarDialogo(const QString &texto, const QPixmap &imagenNPC, const QStringList &opciones);
    void ocultarDialogo();
    void limpiarBotones();

signals:
    void opcionSeleccionada(int index);

private:
    QLabel *lblImagenNPC;
    QLabel *lblTexto;
    QVBoxLayout *layoutPrincipal;
    QHBoxLayout *layoutContenido;
    QVBoxLayout *layoutOpciones;
    QList<QPushButton*> botonesOpciones;
};

#endif // DIALOGONPC_H
