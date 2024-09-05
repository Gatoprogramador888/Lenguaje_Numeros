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
	VARIABLE, IGUAL, CARACTER, DIVISOR, PUNTO
};

struct Informacion
{
	size_t posicion = {};
	string comando = {};
	Tokens token = Tokens::NULO;
};

struct Informacion_Variable 
{
	string nombre = "";
	string valor = "";
	string Tipo = "";
};

class Interfaz_Compilador {
public:
	size_t linea = {};
	virtual void Limpiar() = 0;
};