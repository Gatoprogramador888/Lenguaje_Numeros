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

bool Tokenizador::Caracter(string palabra)
{
	return (palabra == "." || palabra == "\"" || palabra == "(" ||
		palabra == ")" || palabra == "_");
}

vector<Tokens> Tokenizador::Get_Tokens(vector<string> instruccion)
{
	vector<Tokens> tokens_retornar;
	for (string palabra : instruccion)
	{
		if (Variable(palabra)) { tokens_retornar.push_back(token); }

		else if (Impresion_Peticion(palabra)) { tokens_retornar.push_back(token); }

		else if (palabra == "Operador") { tokens_retornar.push_back(Tokens::OPERACION); }

		else if (palabra == "#") { tokens_retornar.push_back(Tokens::COMENTARIO); return tokens_retornar; }

		else if (palabra[0] >= '0' && palabra[0] <= 9)tokens_retornar.push_back(Tokens::NUMERO_IGUALDAD);

		else if (palabra == ";")tokens_retornar.push_back(Tokens::FIN_COMANDO);

		else if (palabra == ",")tokens_retornar.push_back(Tokens::COMAS);

		else if (palabra == "=")tokens_retornar.push_back(Tokens::IGUAL);

		else if (palabra == "\n")tokens_retornar.push_back(Tokens::FIN_LINEA);

		else if (palabra == "+" || palabra == "-" || palabra == "*" || palabra == "/")tokens_retornar.push_back(Tokens::OPERACION);

		else if (palabra == "=")tokens_retornar.push_back(Tokens::IGUAL);

		else if (palabra == ":")tokens_retornar.push_back(Tokens::DIVISOR);

		else if (Caracter(palabra))tokens_retornar.push_back(Tokens::CARACTER);

		else tokens_retornar.push_back(Tokens::VARIABLE);
	}
	return tokens_retornar;
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


