#ifndef INVENTARIOWIDGET_H
#define INVENTARIOWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
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

    void limpiarGrid();
};

#endif // INVENTARIOWIDGET_H
