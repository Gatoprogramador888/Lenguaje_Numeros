#pragma once
#include<iostream>
#include<vector>
#include<string>
#include"CRObjetos.h"
#include<Windows.h>
#include<fstream>
using namespace std;

class AnalizadorSemantico {
private:
	string variable, igualdad,Impresion,Archivo;
	vector<string>Variables, Operadores;
	int tipo = 0;
	ofstream archivo;
private:
	bool Variable(), Operador(), Imprimir();
public:
	string _variable, _igualdad, _impresion;
	vector<string>_variables, _operadores;
	int _tipo = 0;
	void Inicializar();
	bool Dividir(string archivodir);
	void Limpiar();
};