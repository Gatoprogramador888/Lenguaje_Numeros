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
	ENTERO, DECIMAL, DINAMICO, NUMERO_IGUALDAD,
	IMPRIMIR, PEDIR, OPERADOR, OPERACION,
	COMAS, FIN_COMANDO, FIN_LINEA, COMENTARIO,
	ESPACIO, VARIABLE, IGUAL, CARACTER, DIVISOR, NULO
};

struct Informacion
{
	size_t posicion = {};
	string comando = {};
	Tokens token = Tokens::NULO;
};