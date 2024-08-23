#include"AnalizadorLexico.h"
#include"Divisor.h"
#include"AnalizadorSemantico.h"
#include"CRObjetos.h"
//#include"Interpretar.h"
#include"Tokens.h"
#include<fstream>
#include<stdio.h>


/*ARREGLAR LA CLASE DE OBJETOS*/
/*MODIFICAR LIBRERIAS SEMANTICAS*/


int main()
{
	//string Frase = "Entero: Variable1 = 01;Decimal:Variable2";
	string Frase = "Entero : Variable1 = 01";
	AnalizadorLexico analizador_lexico;
	Tokenizador tokenizador;
	Divisor divisor;

	try 
	{
		analizador_lexico.linea = 1;
		tokenizador.linea = 1;
		analizador_lexico.SetTexto(Frase);
		analizador_lexico.CaracterRaro();
		tokenizador.Tipo_Division(Frase);
		divisor.Inicio(Frase);

		for (auto i : tokenizador.Mapa_Informacion(divisor.Get_Comandos(), divisor.Info()))
		{
			cout << "ID: " << i.first << ".\ncomando: " << i.second.comando << ".\nToken: " << (int)i.second.token << "." << "\n------------------------------------------\n";
		}
	}
	catch (runtime_error& error)
	{
		cerr<<error.what();
	}
	catch(out_of_range& e)
	{
		cerr << e.what();
	}

	return 0;
}

/*int main(int argc, char** argv)
{
	AnalizadorLexico analizadorlexico;
	Divisor divisor;
	Interpretar I;
	AnalizadorSemanticoReglas analizadorsemanticoreglas;
	AnalizadorSemanticoComprobacion analizadorsemanticocomprobacion;

	obj.push_back(new Objeto("0", "var1"));

	string archivodir;
	string texto;
	string archivocon;
	int i2 = 2;
	bool errores = true;

	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-c") == 0)break;

		i2++;
		archivodir += argv[i];
	}

	for (int i = i2; i < argc; i++)
	{
		archivocon += argv[i];
	}

	archivodir += ".txt";
	archivocon += ".txt";

	ifstream archivo(archivodir);
	fstream archivo2(archivocon, ios::out);
	analizadorsemanticocomprobacion.dirarchivo = archivocon;

	if (!archivo.is_open())
	{
		cout << archivodir << " no existe.";
		system("pause");
		return 1;
	}

	while(!archivo.eof())
	{

		getline(archivo, texto);

		analizadorlexico.SetTexto(texto);

		if (!analizadorlexico.Proceso())
		{
			analizadorsemanticoreglas.SetTexto(analizadorlexico.GetText());
		}
		else break;
		if (!analizadorsemanticoreglas.Division())
		{
			divisor.SetText_Tipo(analizadorlexico.GetText(), analizadorsemanticoreglas.Get_Tipo());
		}
		else break;
		if (!divisor.Dividir())
		{
			divisor.Obtener(analizadorsemanticocomprobacion.variable, analizadorsemanticocomprobacion.impresion_peticion, analizadorsemanticocomprobacion.igualdad, analizadorsemanticocomprobacion.tipo);
			analizadorsemanticocomprobacion.variables = divisor.VARAOPE();
			analizadorsemanticocomprobacion.operadores = divisor.OPERADORES();
			analizadorsemanticocomprobacion.Inicio();
		}
		else {
			cout << "error de division\n\n\n";
			break;
		}
		if (!analizadorsemanticocomprobacion.error) {
		}
		else break;

		analizadorlexico.LimpiarVariables();
		analizadorsemanticocomprobacion.Limpiar();
		divisor.Limpiar();
		analizadorsemanticoreglas.Limpiar();
	}
	cout << "Compilacion exitosa\n";
	system("pause");
	system("cls");
	archivo.close();
	archivo2.close();


	ifstream ArchivoInterpretar(archivocon,ios::in);
	string ContenidoInterpretar = "";

	if (ArchivoInterpretar.fail())
	{
		cout << archivocon << " no existe";
		system("pause");
		return 1;
	}

	while (!ArchivoInterpretar.eof())
	{
		getline(ArchivoInterpretar, ContenidoInterpretar);
		I.SetText(ContenidoInterpretar);
	}

	ArchivoInterpretar.close();
	BOBJ.Borrar();

	return 0;
}*/