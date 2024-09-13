#pragma once
#include"AnalizadorLexico.h"
#include"Divisor.h"
#include"AnalizadorSemantico.h"
#include"CRObjetos.h"
#include"Interpretar.h"
#include"Tokens.h"
#include<fstream>
#include<stdio.h>

using namespace Analizador_Lexico;

namespace Tipo_Comandos {
	class Ayuda;
	class Compilar;
	class Interpretar;
}

class Tipo_Comandos::Compilar
{
public:
	void Set_Compilar(string archivo_compilar, string archivo_compilado);
};

class Tipo_Comandos::Interpretar
{
public:
	void Set_Interpretar(string archivo_compilado_interpretar);
};

class Tipo_Comandos::Ayuda
{
protected:
public:
	void Set_Ayuda(string* comandos, int argc);
	void Comandos();
	void Compilar();
	void Sintaxis(string comando);
	void Interpretar();
	void Compilar_I();
};

class Comandos_App
{
protected:

	Tipo_Comandos::Ayuda* help = new Tipo_Comandos::Ayuda;
	Tipo_Comandos::Compilar* compile = new Tipo_Comandos::Compilar;
	Tipo_Comandos::Interpretar* inter = new Tipo_Comandos::Interpretar;
public:
	Comandos_App(int argc, char** argv);
	~Comandos_App();
};

