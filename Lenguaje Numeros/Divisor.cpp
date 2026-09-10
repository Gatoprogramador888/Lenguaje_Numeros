#include "Divisor.h"

#include <cctype>
#include <stdexcept>
#include <string>
#include <vector>

// ═══════════════════════════════════════════════════════════════════════════════
// Divisor_Caracteres
// ═══════════════════════════════════════════════════════════════════════════════
//
// Invariante que se mantiene en todo momento:
//   comandos.size() == posicion.size()
//
// Cada vez que se hace push_back a `comandos` se hace exactamente un
// push_back a `posicion` con la posición del primer carácter del token.
// Esto garantiza que Info() nunca accede fuera de rango.
//
// Tokens que produce:
//   · Identificadores / números / literales con '.' o '_' → un token por palabra
//   · Caracteres especiales (operadores, puntuación, comillas…) → un token cada uno
//   · Espacios y tabulaciones → se descartan (no generan token)
//   · Al final del bucle se emite el token acumulado si no está vacío

void Divisor::Divisor_Caracteres()
{
    std::string acumulado;
    size_t      inicio_acumulado = 0;
    bool        dentro_comillas = false;  // true cuando estamos entre " ... "

    for (size_t i = 0; i < linea_comando.size(); i++)
    {
        const char c = linea_comando[i];

        if (c == '#')break;

        if (c == '"')
        {
            // Emitir lo acumulado antes de la comilla (si hay)
            if (!acumulado.empty())
            {
                comandos.push_back(acumulado);
                posicion.push_back(inicio_acumulado);
                acumulado.clear();
            }
            // Emitir la comilla como token propio
            comandos.push_back("\"");
            posicion.push_back(i);
            // Alternar modo
            dentro_comillas = !dentro_comillas;
        }
        else if (dentro_comillas)
        {
            // Dentro de "..." los espacios son literales pero { y } siguen
            // siendo especiales para poder separar las variables interpoladas
            if (c == '{' || c == '}')
            {
                if (!acumulado.empty())
                {
                    comandos.push_back(acumulado);
                    posicion.push_back(inicio_acumulado);
                    acumulado.clear();
                }
                comandos.push_back(std::string(1, c));
                posicion.push_back(i);
            }
            else
            {
                if (acumulado.empty())
                    inicio_acumulado = i;
                acumulado += c;
            }
        }
        else if (std::isspace(static_cast<unsigned char>(c)))
        {
            if (!acumulado.empty())
            {
                comandos.push_back(acumulado);
                posicion.push_back(inicio_acumulado);
                acumulado.clear();
            }
        }
        else if (!std::isalnum(static_cast<unsigned char>(c))
            && c != '.' && c != '_')
        {
            // Carácter especial fuera de comillas
            if (!acumulado.empty())
            {
                comandos.push_back(acumulado);
                posicion.push_back(inicio_acumulado);
                acumulado.clear();
            }
            comandos.push_back(std::string(1, c));
            posicion.push_back(i);
        }
        else
        {
            if (acumulado.empty())
                inicio_acumulado = i;
            acumulado += c;
        }
    }

    if (!acumulado.empty())
    {
        comandos.push_back(acumulado);
        posicion.push_back(inicio_acumulado);
    }

#ifndef NDEBUG
    if (comandos.size() != posicion.size())
        throw std::logic_error(
            "Divisor: comandos y posicion tienen tamaños distintos — bug interno");
#endif
}

// ═══════════════════════════════════════════════════════════════════════════════
// Get_Comandos
// ═══════════════════════════════════════════════════════════════════════════════

std::vector<std::string> Divisor::Get_Comandos()
{
    return comandos;
}

// ═══════════════════════════════════════════════════════════════════════════════
// Divisiones_Varias_lineas_Comandos
// ═══════════════════════════════════════════════════════════════════════════════

std::vector<std::string> Divisor::Divisiones_Varias_lineas_Comandos(
    const std::string& frase)
{
    std::vector<std::string> lineas;
    std::string oracion;

    for (size_t i = 0; i < frase.size(); i++)
    {
        oracion += frase[i];
        if (frase[i] == ';' || i == frase.size() - 1)
        {
            lineas.push_back(oracion);
            oracion.clear();
        }
    }

    return lineas;
}

// ═══════════════════════════════════════════════════════════════════════════════
// Info
// ═══════════════════════════════════════════════════════════════════════════════

std::vector<Informacion> Divisor::Info()
{
    // BUG FIX #6: el original accedía a posicion[i] asumiendo que
    // posicion.size() == comandos.size(). Si diferían → acceso fuera de rango.
    // La invariante de Divisor_Caracteres() ahora lo garantiza, pero añadimos
    // la comprobación defensiva para que cualquier fallo futuro sea explícito.
    if (comandos.size() != posicion.size())
        throw std::logic_error(
            "Divisor::Info — comandos y posicion tienen tamaños distintos");

    std::vector<Informacion> info;
    info.reserve(comandos.size());

    for (size_t i = 0; i < comandos.size(); i++)
    {
        Informacion inf;
        inf.comando = comandos[i];
        inf.posicion = posicion[i];
        inf.token = Tokens::NULO;
        info.push_back(inf);
    }

    return info;
}