#include "Interpretar.h"
//Vincular el lib de matematicas prueba

void Interpretar::SetText(string texto)
{
	this->Texto = texto;
	Divisor();
}

string Interpretar::Suma(vector<int> n1, vector<int> n2)
{
	return "suma\n";
}

string Interpretar::Resta(vector<int> n1, vector<int> n2)
{
	return "resta\n";
}

string Interpretar::Multiplicacion(vector<int> n1, vector<int> n2)
{
	return "multiplicacion\n";
}

string Interpretar::Division(vector<int> n1, vector<int> n2)
{
	return "division\n";
}

void Interpretar::Calcular()
{
	vector<string> valors;
	vector<char> Op;
	vector<int> PosOp;
	string var = "";
	size_t pos = Texto.find("=");
	PosOp.push_back(static_cast<int>(pos));


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
		valors.push_back(var);
		var = "";
	}





}

void Interpretar::Imprimir()
{
	cout << Texto << endl;
}

void Interpretar::Divisor()
{
	if (Texto[0] != '0')Imprimir();
	else Calcular();
}
