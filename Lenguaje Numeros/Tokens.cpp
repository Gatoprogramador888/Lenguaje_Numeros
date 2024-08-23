#include "Tokens.h"


bool Tokenizador::Variable(string palabra)
{
	string TIPO_VARIABLE[] = { "Entero", "Decimal", "Dinamico" };
	for (string TV : TIPO_VARIABLE)
	{
		if (palabra == TV)
		{
			if (palabra == TIPO_VARIABLE[0])token = Tokens::ENTERO;
			else if (palabra == TIPO_VARIABLE[1])token = Tokens::DECIMAL;
			else token = Tokens::DINAMICO;
			return true;
		}
	}
	return false;
}

bool Tokenizador::Impresion_Peticion(string palabra)
{
	string TIPO_PETICION[] = { "Pedir", "Imprimir" };
	for (string TP : TIPO_PETICION)
	{
		if (palabra == TP) { 
			if (palabra != TIPO_PETICION[0])token = Tokens::IMPRIMIR;
			else token = Tokens::PEDIR;
			return true;
		}
	}

	return false;
}

bool Tokenizador::Caracter(string palabra, size_t& comillas, size_t& parentesis)
{
	if (palabra == "\"")comillas++;
	return (palabra == "." || palabra == "\"" || palabra == "(" ||
		palabra == ")" || palabra == "_");
}

void Tokenizador::Nueva_Igualdad(Informacion info,Tokens token)
{
	Informacion in = info;
	in.token = token;
	informacion.push_back(in);
}

map<string,Informacion> Tokenizador::Mapa_Informacion(vector<string> instruccion, vector<Informacion> info)
{
	size_t comillas = 0;
	size_t variables = 0;
	size_t i = 0;
	map<string, Informacion> retorno;

	for (string palabra : instruccion)
	{

		if(comillas % 2 != 1)
		{
			if (Variable(palabra))Nueva_Igualdad(info[i], token); 

			else if (Impresion_Peticion(palabra)) { Nueva_Igualdad(info[i], token); }

			else if (palabra == "Operador")Nueva_Igualdad(info[i], Tokens::OPERACION);

			else if (palabra == "#") { Nueva_Igualdad(info[i], Tokens::COMENTARIO); break; }

			else if (palabra[0] >= '0' && palabra[0] <= 9)Nueva_Igualdad(info[i], Tokens::NUMERO_IGUALDAD);

			else if (palabra == ";")Nueva_Igualdad(info[i], Tokens::FIN_COMANDO);

			else if (palabra == ",")Nueva_Igualdad(info[i], Tokens::COMAS);

			else if (palabra == "\n")Nueva_Igualdad(info[i], Tokens::FIN_LINEA);

			else if (palabra == "+" || palabra == "-" || palabra == "*" || palabra == "/")Nueva_Igualdad(info[i], Tokens::OPERADOR);

			else if (palabra == "=")Nueva_Igualdad(info[i], Tokens::IGUAL);

			else if (palabra == ":")Nueva_Igualdad(info[i], Tokens::DIVISOR);

			else if (Caracter(palabra,comillas,variables))Nueva_Igualdad(info[i], Tokens::CARACTER);

			else if(isalpha(palabra[0]))Nueva_Igualdad(info[i], Tokens::VARIABLE);

		}
		else
		{
			if (palabra == "(" || palabra == ")")
			{
				Nueva_Igualdad(info[i],Tokens::CARACTER);
				variables++;
			}
			else if (palabra == "\"")
			{
				Nueva_Igualdad(info[i], Tokens::CARACTER);
				comillas++;
			}
			else if(variables % 2 != 0)Nueva_Igualdad(info[i], Tokens::VARIABLE);
		}
		i++;
	}
	
	size_t demas = 1;

	for (auto it : informacion)
	{
		string ID = "linea:" + to_string(linea) + "." + to_string(demas);
		retorno.insert(make_pair(ID, it));
		demas++;
	}

	return retorno;
}

Tokens Tokenizador::Tipo_Division(string frase)
{
	size_t posicion = frase.find(":");
	string comando, error;

	if (posicion != string::npos)
	{
		for (size_t i = 0; i < posicion; i++)
		{
			if(!isspace(frase[i]))comando += frase[i];
		}

		if (comando == "Entero")return Tokens::ENTERO;
		else if (comando == "Decimal")return Tokens::DECIMAL;
		else if (comando == "Dinamico")return Tokens::DINAMICO;
		else if (comando == "Pedir")return Tokens::PEDIR;
		else if (comando == "Imprimir")return Tokens::IMPRIMIR;
		else if (comando == "Operador")return Tokens::OPERACION;
		else if (comando == "#")return Tokens::COMENTARIO;
	}
	error = posicion != string::npos ? comando : frase;
	error += " no es una palabra clave\nlinea: " + to_string(linea) + ", posicion: 0\n";
	throw runtime_error(error.c_str());
}


