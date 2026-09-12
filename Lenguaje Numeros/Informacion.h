#pragma once
#include<iostream>
#include<map>
#include<string>
#include<stdlib.h>
#include<stdio.h>
#include<vector>
#include<fstream>
#include<stdexcept>

using namespace std;

enum class Tokens {
	NULO ,ENTERO, DECIMAL, DINAMICO, NUMERO,
	IMPRIMIR, PEDIR, OPERADOR, OPERACION,
	COMAS, FIN_COMANDO,	ESPACIO, COMILLAS,
	PARENTESIS_DERECHO, PARENTESIS_IZQUIERDO,
	VARIABLE, IGUAL, CARACTER, DIVISOR, TEXTO
};

enum class Tipos {
	ENTERO, DECIMAL, DINAMICO
};

constexpr uint8_t TIPO_ENTERO = 1;
constexpr uint8_t TIPO_DECIMAL = 2;
constexpr uint8_t TIPO_DINAMICO = 4;

struct Informacion
{
	size_t posicion = {};
	string comando = {};
	Tokens token = Tokens::NULO;
};

struct Informacion_Variable 
{
	string nombre{};
	string valor{};
	Tipos tipo{};
	size_t id{};
};

// Garantiza que cada OpCode ocupe exactamente 1 byte (uint8_t)
enum class OpCode : uint8_t {
	HALT = 0x00,
	CRB = 0xFF, // Cabecera de archivo
	CED = 0xFE, // Cabecera de sección de constantes
	CS = 0xFD,  // Cabecera de sección de strings
	DECLARAR = 0x01,  // Declarar variable
	INPUT = 0x02, // Pedir
	PRINT = 0x03, // Imprimir

	// Agregaremos los de operaciones matemáticas más adelante
	ADD = 0x0A, // Suma
	SUB = 0x0B, // Resta
	MUL = 0x0C, // Multiplicación
	DIV = 0x0D, // División
};

constexpr const char magic[4] = { 'C', 'R', 'B', '\0' };

// Mascaras de Bits para operando (uint64_t)
constexpr uint64_t BIT_CONSTANTE = 1ULL << 63; // Bit 63: 1 = Constante/Decimal
constexpr uint64_t BIT_STRING = 1ULL << 62; // Bit 62: 1 = String (requiere Bit 63 en 1)

// Helpers de codificación
inline uint64_t codificar_variable(uint64_t pos) {
	return pos; // Bit 63 = 0, Bit 62 = 0
}

inline uint64_t codificar_constante(uint64_t pos) {
	return pos | BIT_CONSTANTE; // Bit 63 = 1, Bit 62 = 0
}

inline uint64_t codificar_string(uint64_t pos) {
	return pos | BIT_CONSTANTE | BIT_STRING; // Bit 63 = 1, Bit 62 = 1
}

inline uint64_t codificar_inline_int(uint64_t val) {
	return val | BIT_STRING; // BIT_STRING = 1<<62; bit 63 queda en 0
}

class Interfaz_Compilador {
public:
	size_t linea = {};
	virtual void Limpiar() = 0;
};