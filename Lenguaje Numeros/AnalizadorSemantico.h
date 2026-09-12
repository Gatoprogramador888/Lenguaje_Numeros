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

    void emit_u8(uint8_t val);
    void emit_u8(OpCode op);     // Sobrecarga para pasar OpCodes directamente
    void emit_u64(uint64_t val);

    size_t obtener_o_agregar_constante(const std::string& valor_str);

    size_t obtener_o_agregar_string(const std::string& str);

    // ── estado por línea ──────────────────────────────────────────────────────
    std::vector<Tokens>      tokens;
    std::vector<std::string> comandos;
    std::vector<size_t>      posiciones;
    std::vector<uint8_t> bytecode;
    std::vector<std::string> tabla_constantes;
    std::map<std::string, size_t> mapa_constantes;
    std::vector<std::string> tabla_strings;
    std::map<std::string, size_t> mapa_strings;
    std::string              error;
    std::ofstream            archivo_a_compilar;

public:
	void Inicio_analizador(const std::string& nombre_archivo)
	{
		archivo_a_compilar.open(nombre_archivo, std::ios::binary | std::ios::trunc);
		if (!archivo_a_compilar.is_open())
			throw std::runtime_error("Error al crear el archivo binario: " + nombre_archivo);
	}
    void Guardar_Archivo_CRB();
	
    void Inicio_analizacion(std::map<std::string, Informacion> mapa);
    void Limpiar() override;

    ~Analizador_Tokens_Compilacion()
    {
		archivo_a_compilar.close();
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
    void Verificar_Peticion(std::string_view texto, Tipos type);
};