#pragma once
#ifndef TOKENS_H
#define TOKENS_H

#include "Informacion.h"

#include <map>
#include <string>
#include <vector>
#include <iomanip> 
#include <sstream>

// Sin `using namespace std;` en el header — contamina toda TU que lo incluya.

class Tokenizador : public Interfaz_Compilador {
private:
    Tokens                   token = Tokens::NULO;
    std::vector<Informacion> informacion;

    // ── clasificadores ────────────────────────────────────────────────────────
    bool Variable(const std::string& palabra);
    bool Impresion_Peticion(const std::string& palabra);
    bool Caracter(const std::string& palabra);

    void Recopilar_informacion(const Informacion& info, Tokens tok);

public:
    static std::string Get_Tipo(Tokens tok);

    /// Tokeniza una instrucción y devuelve un mapa ordenado "línea.pos" → Informacion.
    /// linea se incrementa en el LLAMADOR, una vez por instrucción completa.
    std::map<std::string, Informacion> Mapa_Informacion(
        const std::vector<std::string>& instruccion,
        const std::vector<Informacion>& info);

    void Limpiar() override
    {
        linea = 0;
        token = Tokens::NULO;
        informacion.clear();
    }

    ~Tokenizador()
    {
        Limpiar();
        informacion.shrink_to_fit();
    }
};

#endif // TOKENS_H