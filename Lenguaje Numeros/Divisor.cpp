#include"Divisor.h"


//Divisor2

void Divisor::Divisor_Caracteres()
{
	string comando, operador;


	for (size_t i = 0; i < linea.size(); i++)
	{
		if (!isspace(linea[i]))
		{
			if (!Caracteres(i) && !Operadores(i))
			{
				comando += linea[i];
			}
			else if (Caracteres(i) || Operadores(i))
			{
				comandos.push_back(comando);
				comando = "";
				operador += linea[i];
				comandos.push_back(operador);
				operador = "";
			}
		}
	}
}

bool Divisor::Caracteres(int i)
{
	return (linea[i] == '.' || linea[i] == '"' || linea[i] == '&' ||
		linea[i] == '(' || linea[i] == ')' || linea[i] == ';' ||
		linea[i] == ',' || linea[i] == ':' || linea[i] == '_');
}

bool Divisor::Operadores(int i)
{
	return (linea[i] == '-' || linea[i] == '+' || linea[i] == '/' ||
		linea[i] == '*' || linea[i] == '=');
}


vector<string> Divisor::Get_Comandos()
{
	return comandos;
}

vector<string> Divisor::Divisiones_Varias_lineas_Comandos()
{
	vector<size_t> fin_comando;
	vector<string> lineas;
	size_t posicion_anterior = 0;
	string oracion;

	for (size_t i = 0; i < linea.size(); i++)if (linea[i] == ';')fin_comando.push_back(i);

	for (size_t posicion : fin_comando)
	{
		for (size_t posicion_iterador = posicion_anterior; posicion_iterador < posicion; posicion_iterador++)
		{
			oracion += linea[posicion_iterador];
		}
		lineas.push_back(oracion);
		oracion = "";
		posicion_anterior = posicion + 1;
	}

	return lineas;
}
