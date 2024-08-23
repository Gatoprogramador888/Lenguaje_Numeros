#pragma once
#ifndef TOKENS_H
#define TOKENS_H
#include "Informacion.h"

using namespace std;

class Tokenizador {
private:
	Tokens token;
	vector<Informacion> informacion;
	bool Variable(std::string palabra);
	bool Impresion_Peticion(std::string palabra);
	bool Caracter(std::string palabra, size_t& comillas, size_t& parentesis);
	void Nueva_Igualdad(Informacion info, Tokens token);
public:
	size_t linea;
	map<string, Informacion> Mapa_Informacion(vector<std::string> instruccion, vector<Informacion> info);
	Tokens Tipo_Division(string frase);
};

#endif // !TOKENS_H