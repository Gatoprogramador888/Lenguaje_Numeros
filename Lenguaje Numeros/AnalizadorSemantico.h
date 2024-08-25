#pragma once
#include"CRObjetos.h"
#include"Tokens.h"



class Analizador_Tokens_Compilacion : Interfaz_Compilador
{
private:
	void Fin_Linea(Tokens fin_tokens);
	void Imprimir(), Pedir(), Entero_Decimal_Dinamico(), Operacion();
	vector<Tokens> tokens; 
	vector<string> comandos;
	vector<size_t> posiciones;
public:
	void Inicio_analizacion(map<string, Informacion> mapa);
	void Limpiar() override;
};

class Analizador_Codigo_Compilacion
{
private:
public:
};

class Analizador_Semantico_Interpretacion
{
private:
public:
};