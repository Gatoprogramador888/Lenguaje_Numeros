#include"AnalizadorSemantico.h"

void Analizador_Tokens_Compilacion::Fin_Linea(Tokens fin_tokens)
{
	if(fin_tokens != Tokens::FIN_COMANDO)
	{ 
		string error = "falta se ; en la Linea: " + to_string(linea) + ", posicion: " + to_string(tokens.size()) + ".\n";
		throw runtime_error(error.c_str());
	}
}

void Analizador_Tokens_Compilacion::Imprimir()
{
	int8_t comillas = 0;
	bool texto = comandos[3] != "$" ? false : true;
	size_t parentesis_derecho = 0, parentesis_izquierdo = 0, posicion = 0;
	vector<size_t> pos_parentesis_derecho, pos_parentesis_izquierdo, pos_comillas;

	for (Tokens token : tokens)
	{
		switch (token)
		{
		case Tokens::COMILLAS: 
			comillas++;
			pos_comillas.push_back(posiciones[posicion]);
			break;
		case Tokens::PARENTESIS_DERECHO: 
			parentesis_derecho++;
			pos_parentesis_derecho.push_back(posiciones[posicion]);
			break;
		case Tokens::PARENTESIS_IZQUIERDO:
			parentesis_izquierdo++;
			pos_parentesis_izquierdo.push_back(posiciones[posicion]);
			break;
		case Tokens::VARIABLE:
			if (administrador.PosOBj(comandos[posicion]) == SIZE_MAX)
			{
				error = "La variable: " + comandos[posicion] + " no existe\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
				throw runtime_error(error.c_str());
			}
			break;
		}
		posicion++;
	}

	if (!texto && (comillas != 0 || parentesis_izquierdo != 0 || parentesis_derecho != 0) )
	{
		error = "Imprimir un texto cuando no se especifico un texto en " + comandos[3] + "\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[3]) + ".\n";
		throw runtime_error(error.c_str());
	}
	else
	{
		if (parentesis_derecho != parentesis_izquierdo)
		{
			size_t aux = pos_parentesis_derecho.size(), aux2 = pos_parentesis_izquierdo.size();
			size_t posicionaux = aux > aux2 ? pos_parentesis_derecho[aux] : pos_parentesis_izquierdo[aux2];
			string faltante = aux > aux2 ? "derecho" : "izquierdo";
			error = "falta parentesis " + faltante + ".\nLinea: " + to_string(linea) + ", posicion: " + to_string(posicionaux) + "\n.";
			throw runtime_error(error.c_str());
		}
		else if (comillas < 2)
		{
			int8_t posicionaux = pos_comillas.size() != 0 ? static_cast<int8_t>(pos_comillas[0]) : 0;
			error = "Faltante de comillas.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posicionaux) + ".\n";
			throw runtime_error(error.c_str());
		}
		else if (comillas > 2)
		{
			size_t posicionaux = pos_comillas[pos_comillas.size()];
			error = "Sobran comillas.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posicionaux) + ".\n";
			throw runtime_error(error.c_str());
		}
	}
}

void Analizador_Tokens_Compilacion::Entero_Decimal_Dinamico()
{

}

void Analizador_Tokens_Compilacion::Operacion()
{
}

void Analizador_Tokens_Compilacion::Pedir()
{
	size_t Comas = 0, Variables = 0;
	vector<size_t> pos_coma;

	for (size_t posicion = 2; posicion < tokens.size(); posicion++)
	{
		switch (tokens[posicion])
		{
		case Tokens::VARIABLE:
			if (administrador.PosOBj(comandos[posicion]) == SIZE_MAX)
			{
				error = "La variable " + comandos[posicion] + " no existe.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
				throw runtime_error(error.c_str());
			}
			Variables++;
			break;
		case Tokens::COMAS:
			Comas++; 
			pos_coma.push_back(posiciones[posicion]);
			break;
		case Tokens::FIN_COMANDO:break;
		default:
			error = comandos[posicion] + " es de tipo " + Tokenizador::Get_Tipo(tokens[posicion]) + " en vez de tipo VARIABLE.\nLinea : " + to_string(linea) + ", posicion : " + to_string(posiciones[posicion]) + ".\n";
		}
	}

	if (Comas != Variables - 1)
	{
		error = "Faltantes de comas.\nLinea: " + to_string(linea) + ", posicion: " + to_string(pos_coma[pos_coma.size()]) + ".\n";
	}

}

void Analizador_Tokens_Compilacion::Inicio_analizacion(map<string, Informacion> mapa)
{
	size_t comando = 0, divisor = 0;
	for (auto iterador : mapa)
	{
		tokens.push_back(iterador.second.token);
		comandos.push_back(iterador.second.comando);
		posiciones.push_back(iterador.second.posicion);
	}
	
	for (size_t posicion = 0; posicion < tokens.size(); posicion++)
	{
		switch (tokens[posicion])
		{
		case Tokens::IMPRIMIR:comando++; break;
		case Tokens::PEDIR:comando++; break;
		case Tokens::ENTERO:comando++; break;
		case Tokens::DECIMAL:comando++; break;
		case Tokens::DINAMICO:comando++; break;
		case Tokens::OPERACION:comando++; break;
		case Tokens::DIVISOR:divisor++; break;
		}
	}

	if (comando > 1)
	{
		error = "Exceso de comandos en la linea: " + to_string(linea) + ".\n";
		throw runtime_error(error.c_str());
	}
	if (divisor != 1)
	{
		error = "Faltante de divisor ':'.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[2]) + ".\n";
		throw runtime_error(error.c_str());
	}

	//revisar que no se repitan 
	Fin_Linea(tokens[tokens.size()]);

	switch (tokens[0])
	{
		case Tokens::IMPRIMIR:Imprimir(); break;
		case Tokens::PEDIR:Pedir(); break;
		case Tokens::ENTERO:
		case Tokens::DECIMAL:
		case Tokens::DINAMICO:Entero_Decimal_Dinamico(); break;
		case Tokens::OPERACION:Operacion(); break;
		default:
			error = comandos[0] + " no es una palabra clave.\nLinea: " + to_string(linea) + ", posicion: 0.\n";
			throw runtime_error(error.c_str());
			break;
	}
}

void Analizador_Tokens_Compilacion::Limpiar()
{
}
