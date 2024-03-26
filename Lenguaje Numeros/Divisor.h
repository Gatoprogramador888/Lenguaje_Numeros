#pragma once
#include<iostream>
#include<string>
#include<stdlib.h>
#include<stdio.h>
#include<vector>
#include<Windows.h>
#include"CRObjetos.h"
using namespace std;


class Divisor {
private:
	//variables
	string texto;
	string variables, igualdades;
	vector<string> igualdadoperadores,operadoresvariable;
	vector<int>posoperadores;
	int Tipo;
private:
	//funciones
	void DividirVariables(), DividirOperaciones(), DividirImpresiones();
	vector<string>OperadoresObtener(string oracion);
	vector<string>OperadoresVariableObtener(vector<int>vectorial);
public:
	void SetText(string txt);
	void Obtener(string& Variable,string& Impresione,string& Igualdad,int& t);
	vector<string> VARAOPE(), OPERADORES();
	bool Dividir();
	void Limpiar();
};