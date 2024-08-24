#pragma once
#ifndef TOKENS_H
#define TOKENS_H
#include "Informacion.h"

using namespace std;

class Tokenizador : public Interfaz_Compilador{
private:
	Tokens token;
	vector<Informacion> informacion;
	bool Variable(std::string palabra);
	bool Impresion_Peticion(std::string palabra);
	bool Caracter(std::string palabra);
	void Recopilar_informacion(Informacion info, Tokens token);
public:

	map<string, Informacion> Mapa_Informacion(vector<std::string> instruccion, vector<Informacion> info);
	void Limpiar() override
	{
		linea = 0;
		informacion.clear();
		token = Tokens::NULO;
	}

	~Tokenizador()
	{
		Limpiar();
		informacion.shrink_to_fit();
	}
};

#endif // !TOKENS_H