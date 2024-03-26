#include"AnalizadorSemantico.h"
//Varibles hacer el objeto y ya imprimir

bool AnalizadorSemantico::Variable()
{
	return (variable[0] >= 'A' && variable[0] <= 'Z' || variable[0] >= 'a' && variable[0] <= 'z') && Igual();
}

bool AnalizadorSemantico::Operador()
{
	int r = 0,pos = 0,n1 = 0;

	r = stoi(Variables[0]);

	for (int i = 0; i < Operadores.size(); i++)
	{

		if (pos <= Variables.size())
		{
			pos++;
		}
		else
		{
			break;
		}

		if (Operadores[i] == "+")
		{
			n1 = stoi(Variables[pos]);
			r += n1;
		}
		else if (Operadores[i] == "-")
		{
			n1 = stoi(Variables[pos]);
			r -= n1;
		}
		else if (Operadores[i] == "*")
		{
			n1 = stoi(Variables[pos]);
			r *= n1;
		}
		else if (Operadores[i] == "/")
		{
			n1 = stoi(Variables[pos]);
			r /= n1 ;
		}

	}

	string aux = "";
	int j = 0;

	for (int i = 0; i < variable.size(); i++)
	{
		if (variable[i] != '0')
		{
			aux += variable[i];
			j++;
		}
		
	}
	variable = aux;

	char respuesta[100];
	itoa(r, respuesta,100);

	OutputDebugString(respuesta);

	igualdad = to_string(r);

	return true;
}

bool AnalizadorSemantico::Imprimir()
{
	string var = "";
	string igual = "";
	bool crear = false,impresion = false;

	if (Impresion[0] == '(' && Impresion[Impresion.length() - 1] == ')')
	{
		for (int i = 1; i < Impresion.length()-1; i++)
		{
			if (!isdigit(Impresion[i]) && !isalpha(Impresion[i]))return false;
			var += Impresion[i];
		}
		impresion = true;
	}
	else if (Impresion[0] == '(' && Impresion[Impresion.length() - 2] == ')' && Impresion[Impresion.length() - 1] == 'I')
	{
		for (int i = 1; i < Impresion.length() - 2; i++)
		{
			if (!isdigit(Impresion[i]) && !isalpha(Impresion[i]))return false;
			var += Impresion[i];
		}

		for (auto i : obj)
		{
			if (i->GetNombre() != var)crear = false;
			else
			{
				crear = true;
			}
		}


		if (crear == true) 
		{
			cin >> igual;
			A.NuevaIgualdad(var, igual);
		}
		else
		{
			cout << "\nObjeto inexistente(" << var << ")cree uno con ese nombre\n";
			return false;
		}

	}
	else
	{
		cout << "Faltante forma de imprimir\n";
		return false;
	}
	
	if (A.PosOBj(var) > obj.size())
	{
		cout << "Objeto inexistente\n";
		return false;
	}
	else if(impresion)
	{
		cout << obj[A.PosOBj(var)]->GetValor() << endl;
	}

	return true;
}

bool AnalizadorSemantico::Igual()
{
	for (int i = 0; i < igualdad.length(); i++)
	{
		if (!isdigit(igualdad[i])) return false;
	}
	return true;
}

void AnalizadorSemantico::Inicializar()
{
	variable = _variable;
	igualdad = _igualdad;
	Impresion = _impresion;
	Variables = _variables;
	Operadores = _operadores;
	tipo = _tipo;
}

bool AnalizadorSemantico::Dividir()
{
	bool retorno = false;
	switch (tipo)
	{
	case 1:
		if (Variable()) {
			retorno = Variable();
			A.Crear(variable, igualdad);
		}
		else
		{
			cout << "Error En la creacion de variables: " << variable << " no creara correctamente\n";
		}
		break;
	case 2:
		if (Operador()) {
			retorno = Operador();
			A.NuevaIgualdad(variable,igualdad);
		}
		else
		{
			cout << "Error al operar " << variable << endl;
		}
		break;
	case 3:
		retorno = Imprimir();
		break;
	case 4:
		retorno = false;
		break;
	}
	
	return retorno;
}

void AnalizadorSemantico::Limpiar()
{
	variable = "";
	igualdad = "";
	Impresion = "";
	Variables.clear();
	Operadores.clear();
	tipo = 0;
}

