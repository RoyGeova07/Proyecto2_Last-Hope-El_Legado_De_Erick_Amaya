#ifndef MAPA_H
#define MAPA_H
#include<QWidget>
#include "grafo.h"


class QPushButton;
class QLabel;

class Mapa:public QWidget
{

    Q_OBJECT

public:
    explicit Mapa(QWidget *parent=nullptr);

private:
    void visualizarGrafo(const Grafo& grafo);
    void abrirPestana(const QString& nombreNodo);

};

#endif // INICIO_H
