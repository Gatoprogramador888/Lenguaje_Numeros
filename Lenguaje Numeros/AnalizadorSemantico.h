#pragma once
#include"CRObjetos.h"
#include"Tokens.h"



class Analizador_Tokens_Compilacion : public Interfaz_Compilador
{
private:
	void Fin_Linea(Tokens fin_tokens);
	void Imprimir(), Pedir(), Entero_Decimal_Dinamico(), Operacion();
	vector<Tokens> tokens; 
	vector<string> comandos;
	vector<size_t> posiciones;
	string error = "";
	ofstream archivo_a_compilar;
public:
	void Inicio_analizacion(map<string, Informacion> mapa,string nombre_archivo);
	void Limpiar() override;
	~Analizador_Tokens_Compilacion()
	{
		tokens.shrink_to_fit();
		comandos.shrink_to_fit();
		posiciones.shrink_to_fit();
		error.shrink_to_fit();
	}
};

class Analizador_Semantico_Interpretacion
{
private:
public:
};