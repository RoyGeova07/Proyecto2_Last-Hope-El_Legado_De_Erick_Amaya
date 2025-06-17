#ifndef INVENTARIO_H
#define INVENTARIO_H


#include <QString>
#include <QDebug>

// Nodo del ABB
struct NodoInventario
{
    QString nombre;    // clave de orden
    int cantidad;
    QString tipo;
    QString uso;

    NodoInventario* izquierda;
    NodoInventario* derecha;

    NodoInventario(const QString& n, int cant, const QString& t, const QString& u)
        : nombre(n), cantidad(cant), tipo(t), uso(u), izquierda(nullptr), derecha(nullptr) {}
};

class Inventario
{

private:

    int balas = 110;
    NodoInventario* raiz;

    //aqui funciones auxiliares recursivos
    NodoInventario* insertar(NodoInventario* nodo, NodoInventario* nuevo);
    NodoInventario* eliminar(NodoInventario* nodo, const QString& nombre);
    NodoInventario* encontrarMin(NodoInventario* nodo);
    void listarInOrden(NodoInventario* nodo);

    void destruirArbol(NodoInventario* nodo);
    Inventario();
    ~Inventario();

    // Prohibir copiar
    Inventario(const Inventario&)=delete;
    Inventario& operator=(const Inventario&)=delete;

public:

    static Inventario* getInstance();//singleton

    void insertarObjeto(const QString& nombre, int cantidad, const QString& tipo, const QString& uso);
    void eliminarObjeto(const QString& nombre);
    void buscarObjeto(const QString& nombre);
    void listarInventario();
    void setBalas(int cantidad);
    int getBalas() const;
    NodoInventario* buscar(NodoInventario* nodo, const QString& nombre);

    NodoInventario* obtenerRaiz() { return raiz; }
    void listarInOrden(NodoInventario* nodo, bool ascendente = true);
    void listarInOrdenDescendente(NodoInventario* nodo);
    bool personajeP2Desbloqueado = false;
    bool personajeP3Desbloqueado = false;

    bool getPersonajeP2Desbloqueado() const { return personajeP2Desbloqueado; }
    bool getPersonajeP3Desbloqueado() const { return personajeP3Desbloqueado; }

    void desbloquearPersonajeP2() { personajeP2Desbloqueado = true; }
    void desbloquearPersonajeP3() { personajeP3Desbloqueado = true; }

};

#endif // INVENTARIO_H
