#ifndef INVENTARIOWIDGET_H
#define INVENTARIOWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include"inventario.h"


class InventarioWidget : public QWidget
{
    Q_OBJECT
public:
    explicit InventarioWidget(Inventario* inventario, QWidget *parent = nullptr);

    void actualizarVista();

private:
    Inventario* inventarioRef; // referencia al ABB
    QVBoxLayout* layoutPrincipal;
    QGridLayout* gridInventario;
    QPushButton* btnRefrescar;

    QLineEdit* barraBusqueda;
    QPushButton* btnOrdenar;
    bool ordenAscendente;

    void limpiarGrid();

private slots:
    void buscarObjetos();
    void ordenarInventario();
};

#endif // INVENTARIOWIDGET_H
