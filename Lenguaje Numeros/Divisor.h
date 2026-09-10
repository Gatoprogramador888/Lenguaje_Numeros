#pragma once

#include "Tokens.h"

#include <string>
#include <vector>

// Sin `using namespace std;` — no se propaga a las TU que incluyan este header.

class Divisor : public Interfaz_Compilador {
private:
    std::string              linea_comando;
    std::vector<std::string> comandos;
    std::vector<size_t>      posicion;

    // Métodos Caracteres(i) y Operadores(i) eliminados — código muerto desde
    // la reescritura de Divisor_Caracteres(). Su lógica está integrada en el
    // único bucle de Divisor_Caracteres().
    void Divisor_Caracteres();

public:
    void Inicio(const std::string& _linea)
    {
        linea_comando = _linea;
        Divisor_Caracteres();
    }

    void Limpiar() override
    {
        linea_comando.clear();
        comandos.clear();
        posicion.clear();
        linea = 0;
    }

    std::vector<std::string> Get_Comandos();

    std::vector<std::string> Divisiones_Varias_lineas_Comandos(
        const std::string& frase);

    std::vector<Informacion> Info();

    ~Divisor()
    {
        Limpiar();
        comandos.shrink_to_fit();
        posicion.shrink_to_fit();
    }
};