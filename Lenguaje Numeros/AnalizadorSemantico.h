#pragma once
#include"CRObjetos.h"
#include"Tokens.h"



class Analizador_Tokens_Compilacion
{
private:
	void Fin_Linea(Tokens fin_tokens, size_t ultimo_caracter);
	void Imprimir(), Pedir(), Entero_Decimal_Dinamico(), Operacion();
	vector<Tokens> tokens; 
	vector<string> _comandos;
public:
	size_t linea = {};
	void Inicio_analizacion(vector<Tokens> _tokens,vector<string> _comandos,size_t caracter);
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