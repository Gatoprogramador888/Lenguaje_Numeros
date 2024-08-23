#include "Tokens.h"


bool Tokenizador::Variable(string palabra)
{
	string TIPO_VARIABLE[] = { "Entero", "Decimal", "Dinamico" };
	for (string Tipo_Variable : TIPO_VARIABLE)
	{
		if (palabra == Tipo_Variable)
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
	for (string Tipo_Peticion : TIPO_PETICION)
	{
		if (palabra == Tipo_Peticion) { 
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

void Tokenizador::Recopilar_informacion(Informacion info,Tokens token)
{
	Informacion aux = info;
	aux.token = token;
	informacion.push_back(aux);
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
			if (Variable(palabra))Recopilar_informacion(info[i], token);

			else if (Impresion_Peticion(palabra))Recopilar_informacion(info[i], token); 

			else if (palabra == "Operador")Recopilar_informacion(info[i], Tokens::OPERACION);

			else if (palabra == "#") { Recopilar_informacion(info[i], Tokens::COMENTARIO); break; }

			else if (palabra[0] >= '0' && palabra[0] <= 9)Recopilar_informacion(info[i], Tokens::NUMERO_IGUALDAD);

			else if (palabra == ";")Recopilar_informacion(info[i], Tokens::FIN_COMANDO);

			else if (palabra == ",")Recopilar_informacion(info[i], Tokens::COMAS);

			else if (palabra == "\n")Recopilar_informacion(info[i], Tokens::FIN_LINEA);

			else if (palabra == "+" || palabra == "-" || palabra == "*" || palabra == "/")Recopilar_informacion(info[i], Tokens::OPERADOR);

			else if (palabra == "=")Recopilar_informacion(info[i], Tokens::IGUAL);

			else if (palabra == ":")Recopilar_informacion(info[i], Tokens::DIVISOR);

			else if (Caracter(palabra,comillas,variables))Recopilar_informacion(info[i], Tokens::CARACTER);

			else if(isalpha(palabra[0]))Recopilar_informacion(info[i], Tokens::VARIABLE);

		}
		else
		{
			if (palabra == "(" || palabra == ")")
			{
				Recopilar_informacion(info[i],Tokens::CARACTER);
				variables++;
			}
			else if (palabra == "\"")
			{
				Recopilar_informacion(info[i], Tokens::CARACTER);
				comillas++;
			}
			else if(variables % 2 != 0)Recopilar_informacion(info[i], Tokens::VARIABLE);
		}
		i++;
	}
	
	size_t posicion_Token = 1;

	for (auto it : informacion)
	{
		string ID = "linea:" + to_string(linea) + "." + to_string(posicion_Token);
		retorno.insert(make_pair(ID, it));
		posicion_Token++;
	}

	return retorno;
}

Tokens Tokenizador::Division_Palabra_Clave(string frase)
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


