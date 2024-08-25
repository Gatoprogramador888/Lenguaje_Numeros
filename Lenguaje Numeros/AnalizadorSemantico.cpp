#include"AnalizadorSemantico.h"

void Analizador_Tokens_Compilacion::Fin_Linea(Tokens fin_tokens)
{
	if(fin_tokens != Tokens::FIN_COMANDO)
	{ 
		string error = "falta se ; en la linea: " + to_string(linea) + ", posicion: " + to_string(tokens.size()) + ".\n";
		throw runtime_error(error.c_str());
	}
}

void Analizador_Tokens_Compilacion::Imprimir()
{
	int8_t comillas = 0;
	bool texto = comandos[3] != "$" ? false : true;
	size_t parentesis_derecho = 0, parentesis_izquierdo = 0, posicion = 0;
	string error = "";

	for (Tokens token : tokens)
	{
		switch (token)
		{
		case Tokens::COMILLAS: comillas++;
			break;
		case Tokens::PARENTESIS_DERECHO: parentesis_derecho++;
			break;
		case Tokens::PARENTESIS_IZQUIERDO: parentesis_izquierdo++;
			break;
		case Tokens::VARIABLE:
			if (administrador.PosOBj(comandos[posicion]) == SIZE_MAX)
			{
				error = "La variable: " + comandos[posicion] + " no existe\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
			}
			break;
		}
		posicion++;
	}
	
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

void Analizador_Tokens_Compilacion::Inicio_analizacion(map<string, Informacion> mapa)
{

	for (auto iterador : mapa)
	{
		tokens.push_back(iterador.second.token);
		comandos.push_back(iterador.second.comando);
		posiciones.push_back(iterador.second.posicion);
	}

	Fin_Linea(tokens[tokens.size()]);

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

void Analizador_Tokens_Compilacion::Limpiar()
{
}
