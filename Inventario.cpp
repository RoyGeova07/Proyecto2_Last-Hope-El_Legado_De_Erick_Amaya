#include "Inventario.h"

Inventario::Inventario():raiz(nullptr){}

Inventario::~Inventario()
{

    destruirArbol(raiz);

}

Inventario* Inventario::getInstance()
{

    static Inventario instance;
    return &instance;

}

void Inventario::destruirArbol(NodoInventario *nodo)
{

    if(nodo)
    {
        destruirArbol(nodo->izquierda);
        destruirArbol(nodo->derecha);
        delete nodo;
    }

}

void Inventario::insertarObjeto(const QString &nombre, int cantidad, const QString &tipo, const QString &uso)
{

    NodoInventario* nuevo = new NodoInventario(nombre, cantidad, tipo, uso);
    raiz = insertar(raiz, nuevo);

}

NodoInventario* Inventario::insertar(NodoInventario *nodo, NodoInventario *nuevo)
{

    if (!nodo)
        return nuevo;

    if (nuevo->nombre < nodo->nombre)
        nodo->izquierda = insertar(nodo->izquierda, nuevo);
    else if (nuevo->nombre > nodo->nombre)
        nodo->derecha = insertar(nodo->derecha, nuevo);
    else
    {
        // Si ya existe el objeto, simplemente aumentar cantidad
        nodo->cantidad += nuevo->cantidad;
        delete nuevo; // evitar fuga
    }

    return nodo;

}

void Inventario::eliminarObjeto(const QString& nombre)
{
    raiz = eliminar(raiz, nombre);
}

NodoInventario* Inventario::eliminar(NodoInventario *nodo, const QString &nombre)
{

    if (!nodo)
        return nullptr;

    if (nombre < nodo->nombre)
        nodo->izquierda = eliminar(nodo->izquierda, nombre);
    else if (nombre > nodo->nombre)
        nodo->derecha = eliminar(nodo->derecha, nombre);
    else
    {
        // Nodo encontrado
        if (!nodo->izquierda)
        {
            NodoInventario* temp = nodo->derecha;
            delete nodo;
            return temp;
        }
        else if (!nodo->derecha)
        {
            NodoInventario* temp = nodo->izquierda;
            delete nodo;
            return temp;
        }
        else
        {
            NodoInventario* temp = encontrarMin(nodo->derecha);
            nodo->nombre = temp->nombre;
            nodo->cantidad = temp->cantidad;
            nodo->tipo = temp->tipo;
            nodo->uso = temp->uso;

            nodo->derecha = eliminar(nodo->derecha, temp->nombre);
        }
    }

    return nodo;

}

NodoInventario* Inventario::encontrarMin(NodoInventario *nodo)
{

    while (nodo && nodo->izquierda)
        nodo = nodo->izquierda;
    return nodo;

}

void Inventario::buscarObjeto(const QString& nombre)
{
    NodoInventario* res = buscar(raiz, nombre);
    if (res)
    {
        qDebug() << "Objeto encontrado:";
        qDebug() << "Nombre:" << res->nombre;
        qDebug() << "Cantidad:" << res->cantidad;
        qDebug() << "Tipo:" << res->tipo;
        qDebug() << "Uso:" << res->uso;

    }
    else
    {
        qDebug() << "Objeto no encontrado:" << nombre;
    }
}

NodoInventario* Inventario::buscar(NodoInventario* nodo, const QString& nombre)
{
    if (!nodo)
        return nullptr;

    if (nombre == nodo->nombre)
        return nodo;
    else if (nombre < nodo->nombre)
        return buscar(nodo->izquierda, nombre);
    else
        return buscar(nodo->derecha, nombre);
}

void Inventario::listarInventario()
{
    qDebug() << "===== Inventario =====";
    listarInOrden(raiz,true);
    qDebug() << "======================";
}

int Inventario::getBalas() const {
    return balas;
}

void Inventario::setBalas(int cantidad) {
    balas = cantidad;
}


void Inventario::listarInOrden(NodoInventario* nodo, bool ascendente)
{
    if (ascendente)
    {
        if (nodo)
        {
            listarInOrden(nodo->izquierda, ascendente);
            qDebug() << nodo->nombre << "- Cantidad:" << nodo->cantidad << "- Tipo:" << nodo->tipo;
            listarInOrden(nodo->derecha, ascendente);
        }
    }
    else
    {
        listarInOrdenDescendente(nodo);
    }
}

void Inventario::listarInOrdenDescendente(NodoInventario* nodo)
{
    if (nodo)
    {
        listarInOrdenDescendente(nodo->derecha);
        qDebug() << nodo->nombre << "- Cantidad:" << nodo->cantidad << "- Tipo:" << nodo->tipo;
        listarInOrdenDescendente(nodo->izquierda);
    }
}
