#include "Interpretar.h"
//Vincular el lib de matematicas prueba

void Interpretar::SetText(string texto)
{
	this->Texto = texto;
	Divisor();
}

char* Interpretar::Sumar(vector<int> n1, vector<int> n2)
{
	Suma suma(n1, n2);
	return suma.Resultado();
}

char* Interpretar::Restar(vector<int> n1, vector<int> n2)
{
	Resta resta(n1,n2);
	return resta.Resultado();
}

char* Interpretar::Multiplicar(vector<int> n1, vector<int> n2)
{
	Multiplicacion multiplicacion(n1, n2);
	return multiplicacion.Resultado();
}

char* Interpretar::Dividir(vector<int> n1, vector<int> num2)
{
	Division division(n1,num2);
	return division.Resultado();
}

void Interpretar::Calcular()
{
	vector<string> valors;
	vector<char> Op;
	vector<int> PosOp;
	listnum n1, n2;
	string var = "";//variables de la suma
	string Var = "";//variable que pidio la operacion
	string resultado = "";
	size_t pos = Texto.find("=");
	int PosConvertir = 0;
	PosOp.push_back(static_cast<int>(pos));

	for (size_t i = 1; i < pos; i++)
	{
		Var += Texto[i];
	}

	for (size_t i = pos; i < Texto.size(); i++)
	{
		switch (Texto[i])
		{
		case '+':
			PosOp.push_back(static_cast<int>(i));
			Op.push_back('+');
			break;
		case '-':
			PosOp.push_back(static_cast<int>(i));
			Op.push_back('-');
			break;
		case '*':
			PosOp.push_back(static_cast<int>(i));
			Op.push_back('*');
			break;
		case '/':
			PosOp.push_back(static_cast<int>(i));
			Op.push_back('/');
			break;
		}
	}



	PosOp.push_back(static_cast<int>(Texto.size()));
	for (int i = 0; i < PosOp.size() - 1; i++)
	{
		for (int k = PosOp[i] + 1; k < PosOp[static_cast<unsigned long long>(i) + 1]; k++)
		{
			var += Texto[k];
		}

		if (!isdigit(var[0]))
		{
			int posobj = A.PosOBj(var);
			var = obj[posobj]->GetValor();
		}

		valors.push_back(var);
		var = "";
	}

	auto ObtenerN1N2 = [](listnum& n1, listnum& n2, vector<string> valor, int Pos, int PosOp, bool suma, bool primeravez, const char* texto) -> int {
		try
		{
			//se podra optimizar?
			ConversionI Conversor;
			if (!primeravez && !suma)
			{
				n1 = Conversor.STOIM(texto);
				if (Pos <= valor.size())n2 = Conversor.STOIM(valor[Pos++].c_str());
			}
			else if(!primeravez)
			{
				n1 = Conversor.STOII(texto);
				if (Pos <= valor.size())n2 = Conversor.STOII(valor[Pos++].c_str());
			}
			else if (!suma)
			{
				n1 = Conversor.STOIM(valor[Pos++].c_str());
				if (Pos <= valor.size())n2 = Conversor.STOIM(valor[Pos++].c_str());
			}
			else
			{
				n1 = Conversor.STOII(valor[Pos++].c_str());
				if (Pos <= valor.size())n2 = Conversor.STOII(valor[Pos++].c_str());
			}
		}
		catch (const char* error)
		{
			printf(error);
		}

		return Pos;
	};

	auto FNewtext = [](string texto)->string
	{
		string nuevotexto = "0" + texto;
		return nuevotexto;
	};

	/************************************************************/
	/*                                                          */
	/* Hacer que el resultado se vaya al numero que se pidio la */
	/*                operacion correspondiente                 */
	/*                                                          */
	/************************************************************/

	/*HACER QUE SE HAGAN CON MULTIPLES OPERACIONES*/

	//Falla al parecer no eh creado reglas para resta, multiplicar, dividir
	//revisar Analizador semantico y divisor

	int ip = 0,iterador = 1;

	for(auto i : Op) {
		switch (i)
		{
		case '+':
			PosConvertir = ObtenerN1N2(n2, n1, valors, PosConvertir, ip, 1, iterador == 1,resultado.c_str());
			resultado = Sumar(n1, n2);
			resultado = FNewtext(resultado);
			A.NuevaIgualdad(Var, resultado);
			break;
		case '-':
			PosConvertir = ObtenerN1N2(n1, n2, valors, PosConvertir, ip, 1,  iterador == 1, resultado.c_str());
			resultado = Restar(n1, n2);
			resultado = FNewtext(resultado);
			A.NuevaIgualdad(Var, resultado);
			break;
		case '*':
			PosConvertir = ObtenerN1N2(n1, n2, valors, PosConvertir, ip, 0, iterador == 1, resultado.c_str());
			resultado = Multiplicar(n1, n2);
			resultado = FNewtext(resultado);
			A.NuevaIgualdad(Var, resultado);
			break;
		case '/':
			PosConvertir = ObtenerN1N2(n1, n2, valors, PosConvertir, ip, 0, iterador == 1, resultado.c_str());
			resultado = Dividir(n1, n2);
			resultado = FNewtext(resultado);
			A.NuevaIgualdad(Var, resultado);
			break;
		}
		//i++;
		iterador++;
	}
	

}

void Interpretar::Imprimir()
{
	cout << Texto << endl;
}

void Interpretar::Divisor()
{
	switch (Texto[0]) {
	case 'O':
		Calcular();
		break;
	case 'P':
		Peticion();
		break;
	case '"':
		Imprimir();
		break;
	case 'N':
		EncontraIgualdad();
		Imprimir();
		break;
	default:
		Imprimir();
	}
	//if (Texto[0] != '0')Imprimir();
	//else Calcular();
}

void Interpretar::Peticion()
{
	string variable,nuevaigualdad;

	for (int i = 1; i < Texto.length(); i++)
	{
		variable += Texto[i];
	}

	cin >> nuevaigualdad;
	A.NuevaIgualdad(variable, nuevaigualdad);
}


void Interpretar::EncontraIgualdad() {
	string variable = "";
	int pos = 0;

	for (int i = 1; i < Texto.length(); i++)
	{
		variable += Texto[i];
	}

	pos = A.PosOBj(variable);
	Texto = obj[pos]->GetValor();
}

