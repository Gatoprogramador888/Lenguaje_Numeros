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
	Resta resta(n1, n2);
	return resta.Resultado();
}

char* Interpretar::Multiplicar(vector<int> n1, vector<int> n2)
{
	Multiplicacion multiplicacion(n1, n2);
	return multiplicacion.Resultado();
}

char* Interpretar::Dividir(vector<int> n1, vector<int> n2)
{
	Division division(n1, n2);
	return division.Resultado();
}

void Interpretar::Calcular()
{
	vector<string> valors;
	vector<char> Op;
	vector<int> PosOp;
	listnum n1, n2;
	string var = "";
	size_t pos = Texto.find("=");
	int PosConvertir = 0;
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

	auto ObtenerN1N2 = [](listnum& n1, listnum& n2, vector<string> valor, int Pos, bool suma) -> int {
		ConversionI Conversor;
		if (!suma)
		{
			n1 = Conversor.STOIM(valor[Pos++].c_str());
			n2 = Conversor.STOIM(valor[Pos++].c_str());
		}
		else
		{
			n1 = Conversor.STOII(valor[Pos++].c_str());
			n2 = Conversor.STOII(valor[Pos++].c_str());
		}
		return Pos;
	};



	for (auto i : Op) {
		switch (i)
		{
		case '+':
			PosConvertir = ObtenerN1N2(n1, n2, valors, PosConvertir, 1);
			Sumar(n1, n2);
			break;
		case '-':
			PosConvertir = ObtenerN1N2(n1, n2, valors, PosConvertir, 1);
			Restar(n1, n2);
			break;
		case '*':
			PosConvertir = ObtenerN1N2(n1, n2, valors, PosConvertir, 0);
			Multiplicar(n1, n2);
			break;
		case '/':
			PosConvertir = ObtenerN1N2(n1, n2, valors, PosConvertir, 0);
			Dividir(n1, n2);
			break;
		}
	}
	

}

void Interpretar::Imprimir()
{
	cout << "Numero a imprimir:" << Texto << endl;
}

void Interpretar::Divisor()
{
	if (Texto[0] != '0')Imprimir();
	else Calcular();
}
