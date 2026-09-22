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
	NULO, ENTERO, DECIMAL, DINAMICO, NUMERO,
	IMPRIMIR, PEDIR, OPERADOR, OPERACION,
	COMAS, FIN_COMANDO, ESPACIO, COMILLAS,
	LLAVE_DERECHO, LLAVE_IZQUIERDO,
	VARIABLE, IGUAL, CARACTER, DIVISOR, TEXTO, CONSTANTE, PARENTESIS_DERECHO, PARENTESIS_IZQUIERDO,
	BOOL, MAYORQUE, MENORQUE, AND, OR, NOT, NEGAR, TRUE, FALSE
};

enum class Tipos : int{
	ENTERO = 1, DECIMAL = 2, DINAMICO = 4, BOOL = 6
};

inline int operator&(Tipos a, Tipos b) {
	return static_cast<int>(a) & static_cast<int>(b);
}

constexpr uint8_t TIPO_ENTERO = 1;
constexpr uint8_t TIPO_DECIMAL = 2;
constexpr uint8_t TIPO_DINAMICO = 4;
constexpr uint8_t TIPO_BOOL = 6;

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
	NOP = 0x01,
	DECLARAR = 0x02,  // Declarar variable
	INPUT = 0x03, // Pedir
	PRINT = 0x04, // Imprimir
	FREE = 0x05, // Liberar variable

	// Agregaremos los de operaciones matemáticas más adelante
	ADD = 0x0A, // Suma
	SUB = 0x0B, // Resta
	MUL = 0x0C, // Multiplicación
	DIV = 0x0D, // División

	//Agregar condicionales
	EQUALS = 0xA1, // ==
	NOT = 0xA2, // NOT
	GREATER = 0xA3, // >
	LESS = 0xA4, // <
	GREATER_THAN = 0xA5, // >=
	LESS_THAN = 0xA6, // <=
	NOT_EQUALS = 0xA7, // !=
	OR = 0xA8, // or
	AND = 0xA9, // and

};

constexpr const char magic[4] = { 'C', 'R', 'B', '\0' };

// Mascaras de Bits para operando (uint64_t)
constexpr uint64_t BIT_CONSTANTE = 1ULL << 63; // Bit 63: 1 = Constante/Decimal
constexpr uint64_t BIT_STRING = 1ULL << 62; // Bit 62: 1 = String (requiere Bit 63 en 1)
constexpr uint64_t BIT_NULL = 1ULL << 61; // Bit 61: 1 = Nulo (requiere Bit 61 en 1) solo compilacion

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