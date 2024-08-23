#include"AnalizadorLexico.h"

void AnalizadorLexico::Limpiar()
{
	string aux = texto;
	int j = 0;

	for (int i = 0; i < aux.length(); i++)
	{
		if (aux[i] != ' ')
		{
			aux[j++] = aux[i];
		}
	}
	aux.resize(j);
	this->textoSinEspacio = aux;
}

void AnalizadorLexico::CaracterRaro()
{
	for (int i = 0; i < textoSinEspacio.length(); i++)
	{
		if (ABECEDARIO(i) || NUMEROS(i) || CARACTERES(i) || OPERADORES(i) || textoSinEspacio[i] == ' ')
		{
		}
		else 
		{
			string error = textoSinEspacio[i] + " es un caracter no reconocible\nlinea: " + to_string(linea) + ", posicion: " + to_string(i) + "\n";
			throw runtime_error(error.c_str());
		}
	}
}

bool AnalizadorLexico::ABECEDARIO(int i)
{
	return ((textoSinEspacio[i] >= 'A' && textoSinEspacio[i] <= 'Z') || (textoSinEspacio[i] >= 'a' && textoSinEspacio[i] <= 'z'));
}

bool AnalizadorLexico::NUMEROS(int i)
{
	return ((textoSinEspacio[i] >= '0') && (textoSinEspacio[i] <= '9'));
}

bool AnalizadorLexico::CARACTERES(int i)
{
	return (textoSinEspacio[i] == '.' || textoSinEspacio[i] == '"' || textoSinEspacio[i] == '&' || 
		textoSinEspacio[i] == '(' || textoSinEspacio[i] == ')' || textoSinEspacio[i] == ';' || 
		textoSinEspacio[i] == ',' || textoSinEspacio[i] == ':' || textoSinEspacio[i] == '_');
}

bool AnalizadorLexico::OPERADORES(int i)
{
	return (textoSinEspacio[i] == '-' || textoSinEspacio[i] == '+' || textoSinEspacio[i] == '/' || textoSinEspacio[i] == '*' || textoSinEspacio[i] == '=');
}

void AnalizadorLexico::SetTexto(string txt)
{
	this->texto = txt;
}

bool AnalizadorLexico::Proceso()
{
	Limpiar();
	return true;
}

string AnalizadorLexico::GetText()
{
	return textoSinEspacio;
}

void AnalizadorLexico::LimpiarVariables()
{
	texto = "";
	textoSinEspacio = "";
}
