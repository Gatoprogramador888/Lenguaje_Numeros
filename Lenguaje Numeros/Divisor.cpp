#include"Divisor.h"


//Divisor2

void Divisor::Divisor_Caracteres()
{
	string comando, operador;

	for (size_t i = 0; i < linea_comando.size(); i++)
	{
		if (!isspace(linea_comando[i]))
		{
			if (!isalpha(linea_comando[i]) && !isdigit(linea_comando[i]) && linea_comando[i] != '.' && linea_comando[i] != '_')
			{
				comandos.push_back(comando);
				comando = "";
				operador += linea_comando[i];
				comandos.push_back(operador);
				operador = "";
				posicion.push_back(i - 1);
				posicion.push_back(i);
			}
			else
			{
				comando += linea_comando[i];
			}
		}
		else
		{
			comandos.push_back(comando);
			comando = "";
			posicion.push_back(i);
		}
	}

}

bool Divisor::Caracteres(size_t i)
{
	return (linea_comando[i] == '.' || linea_comando[i] == '"' || linea_comando[i] == '&' ||
		linea_comando[i] == '(' || linea_comando[i] == ')' || linea_comando[i] == ';' ||
		linea_comando[i] == ',' || linea_comando[i] == ':' || linea_comando[i] == '_');
}

bool Divisor::Operadores(size_t i)
{
	return (linea_comando[i] == '-' || linea_comando[i] == '+' || linea_comando[i] == '/' ||
		linea_comando[i] == '*' || linea_comando[i] == '=');
}


vector<string> Divisor::Get_Comandos()
{
	return comandos;
}

vector<string> Divisor::Divisiones_Varias_lineas_Comandos(string frase)
{
	vector<size_t> fin_comando;
	vector<string> lineas;
	size_t posicion_anterior = 0;
	string oracion;

	for (size_t i = 0; i < frase.size(); i++)
		if (frase[i] == ';')fin_comando.push_back(i+1);

	for (size_t posicion : fin_comando)
	{
		for (size_t posicion_iterador = posicion_anterior; posicion_iterador < posicion; posicion_iterador++)
		{
			oracion += frase[posicion_iterador];
		}
		lineas.push_back(oracion);
		oracion = "";
		posicion_anterior = posicion;
	}

	return lineas;
}

vector<Informacion> Divisor::Info()
{
	vector<Informacion> info;
	Informacion inf;

	for(size_t i = 0; i < comandos.size();i++)
	{
		inf.comando = comandos[i];
		inf.posicion = posicion[i];
		inf.token = Tokens::NULO;
		info.push_back(inf);
	}

	return info;
}

