#include"AnalizadorSemantico.h"

void Analizador_Tokens_Compilacion::Fin_Linea(Tokens fin_tokens, size_t ultimo_caracter)
{
	if (fin_tokens != Tokens::FIN_COMANDO)
	{
		string error = "error de ;\nlinea: " + to_string(linea) + ", posicion: " + to_string(ultimo_caracter) + "\n";
		throw runtime_error(error.c_str());
	}
}

void Analizador_Tokens_Compilacion::Imprimir()
{

	
	//Imprimir:"variable 

}

void Analizador_Tokens_Compilacion::Entero_Decimal_Dinamico()
{

}

void Analizador_Tokens_Compilacion::Operacion()
{
}

void Analizador_Tokens_Compilacion::Pedir()
{

}

void Analizador_Tokens_Compilacion::Inicio_analizacion(vector<Tokens> tokens, vector<string> comandos, size_t caracter)
{
	Fin_Linea(tokens[tokens.size() - 1], caracter);

	switch (tokens[0])
	{
		case Tokens::IMPRIMIR:Imprimir(); break;
		case Tokens::PEDIR:Pedir(); break;
		case Tokens::ENTERO:
		case Tokens::DECIMAL:
		case Tokens::DINAMICO:Entero_Decimal_Dinamico(); break;
		case Tokens::OPERACION:Operacion(); break;

	}
}
