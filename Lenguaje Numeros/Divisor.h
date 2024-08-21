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
	string linea;
	vector<string> comandos;
	void Divisor_Caracteres();
	bool  Caracteres(int i), Operadores(int i);
public:
	void Inicio(string _linea)
	{
		linea = _linea;
		Divisor_Caracteres();
	}
	void Limpiar()
	{
		linea = "";
		comandos.clear();
	}
	vector<string> Get_Comandos();
	vector<string> Divisiones_Varias_lineas_Comandos();
};