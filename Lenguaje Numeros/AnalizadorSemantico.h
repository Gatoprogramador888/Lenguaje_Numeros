#pragma once
#include<iostream>
#include<vector>
#include<string>
#include"CRObjetos.h"
#include<Windows.h>
#include<fstream>
using namespace std;


class AnalizadorSemanticoReglas {
private:
	string texto = "";
	int Tipo = 0;
	bool error = false;
private:
	void Operacion(), Impresion(), Peticion(), Variable();
	bool OPERADORES(int i);
public:
	bool Division();
	void SetTexto(string _texto);
	int Get_Tipo();
	void Limpiar();
};

class AnalizadorSemanticoComprobacion {
private:
	void Impresion(), Operacion(), Creacion(), Peticion();
	ofstream archivo;
public:
	string variable, impresion_peticion, igualdad, dirarchivo;
	vector<string> variables;
	vector<char> operadores;
	bool error = false;
	int tipo = 0;
	void Inicio();
	void Limpiar();
};