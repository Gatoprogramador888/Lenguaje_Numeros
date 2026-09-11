#pragma once
#include "CRObjetos.h"
#include "Tokens.h"

#include <fstream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

// ── Analizador de compilación ─────────────────────────────────────────────────

class Analizador_Tokens_Compilacion : public Interfaz_Compilador
{
private:
    // ── helpers ──────────────────────────────────────────────────────────────
    void Fin_Linea(Tokens fin_tokens);
    void Imprimir();
    void Pedir();
    void Entero_Decimal_Dinamico();
    void Operacion();

    // ── helpers internos ─────────────────────────────────────────────────────

    /// Lanza runtime_error si pos == SIZE_MAX, usando el mensaje de error
    /// formateado ya almacenado en `error`.
    /// Centraliza la guarda de PosOBj para que ningún obj[pos] sea UB.
    [[noreturn]] void lanzar_error_pos(size_t posicion_token) const;

    /// Devuelve la posición en obj[] o lanza runtime_error si no existe.
    size_t pos_segura(const std::string& nombre, size_t posicion_token);

    // ── estado por línea ──────────────────────────────────────────────────────
    std::vector<Tokens>      tokens;
    std::vector<std::string> comandos;
    std::vector<size_t>      posiciones;
    std::string              error;
    std::ofstream            archivo_a_compilar;

public:
    void Inicio_analizacion(std::map<std::string, Informacion> mapa,
        const std::string& nombre_archivo);
    void Limpiar() override;

    ~Analizador_Tokens_Compilacion()
    {
        tokens.shrink_to_fit();
        comandos.shrink_to_fit();
        posiciones.shrink_to_fit();
        error.shrink_to_fit();
    }
};

// ── Analizador de interpretación ──────────────────────────────────────────────

class Analizador_Semantico_Interpretacion
{
public:
    /// Lanza si el texto contiene caracteres no numéricos.
    void Verificar_Peticion(const std::string& texto, std::string_view type);
};