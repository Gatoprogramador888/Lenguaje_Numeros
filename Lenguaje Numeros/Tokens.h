#pragma once
#ifndef TOKENS_H
#define TOKENS_H

#include<iostream>
#include<vector>
#include<stdexcept>
#include<string>
using namespace std;

enum class Tokens {
	ENTERO, DECIMAL, DINAMICO, NUMERO_IGUALDAD,
	IMPRIMIR, PEDIR, OPERADOR, OPERACION,
	COMAS, FIN_COMANDO, FIN_LINEA, COMENTARIO,
	ESPACIO, VARIABLE, IGUAL, CARACTER, DIVISOR
};
//Tokens tokens;

class Tokenizador {
private:
	Tokens token;
	bool Variable(string palabra);
	bool Impresion_Peticion(string palabra);
	bool Caracter(string palabra);
public:
	int linea;
	vector<Tokens> Get_Tokens(vector<string> instruccion);
	Tokens Tipo_Division(string frase);
};

#endif // !TOKENS_H