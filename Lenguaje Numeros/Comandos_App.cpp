#include "Comandos_App.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

//Auxiliares para desensamblar el bytecode
#include <iomanip>
#include <cstdint>


// ── Comandos_App ──────────────────────────────────────────────────────────────

Comandos_App::Comandos_App(int argc, char** argv)
{
    // BUG FIX #1: argv estaba completamente ignorado; las rutas estaban
    // hardcodeadas a un directorio de debug local.
    // Ahora se parsea argv correctamente:
    //   argv[0] = ejecutable  (ignorado)
    //   argv[1] = comando     (compilar / interpretar / compilar_I / ayuda)
    //   argv[2] = archivo 1   (según comando)
    //   argv[3] = archivo 2   (solo para compilar / compilar_I)

	//Desensamblar_CRB("a.crb");
    //compile.Set_Compilar("prueba.crd", "a.crb");
	//inter.Set_Interpretar("a.crb");
    //return;

    if (argc < 2)
    {
        std::cout << "Uso: LenguajeNumeros <comando> [argumentos]\n"
            << "     LenguajeNumeros ayuda comandos\n";
        return;
    }

    const std::string cmd = argv[1];

    // BUG FIX #2: la cadena original era if/if/if/if/else.
    // El else solo se enlazaba al último if (compilar_I), por lo que disparaba
    // para cualquier comando distinto de "compilar_I", incluyendo los válidos.
    // Corrección: if / else if / ... / else.

    if (cmd == "ayuda")
    {
        // ayuda necesita al menos argv[2] para saber qué ayuda mostrar
        if (argc < 3)
        {
            std::cout << "Uso: LenguajeNumeros ayuda <tema>\n";
            return;
        }
        // Set_Ayuda espera string* apuntando desde argv[1]
        // Construimos un array local con los argumentos relevantes
        std::string args[3];
        for (int i = 0; i < 3 && (i + 1) < argc; i++)
            args[i] = argv[i + 1];
        help.Set_Ayuda(args, argc - 1);
    }
    else if (cmd == "compilar")
    {
        if (argc < 4)
        {
            std::cout << "Uso: LenguajeNumeros compilar <archivo_fuente> <archivo_salida>\n";
            return;
        }
        compile.Set_Compilar(argv[2], argv[3]);
    }
    else if (cmd == "interpretar")
    {
        if (argc < 3)
        {
            std::cout << "Uso: LenguajeNumeros interpretar <archivo_compilado>\n";
            return;
        }
        inter.Set_Interpretar(argv[2]);
    }
    else if (cmd == "compilar_I")
    {
        if (argc < 4)
        {
            std::cout << "Uso: LenguajeNumeros compilar_I <archivo_fuente> <archivo_compilado>\n";
            return;
        }
        compile.Set_Compilar(argv[2], argv[3]);
        inter.Set_Interpretar(argv[3]);
    }
    else
    {
        std::cout << "Dicho comando no existe: " << cmd << "\n";
    }
}

Comandos_App::~Comandos_App() {}

// ── Ayuda ─────────────────────────────────────────────────────────────────────

void Tipo_Comandos::Ayuda::Set_Ayuda(std::string* comandos, int argc)
{
    if (argc < 2) return;

    if (comandos[1] == "comandos")   Comandos();
    else if (comandos[1] == "compilar")   Compilar();
    else if (comandos[1] == "sintaxis") { Sintaxis(comandos[2]);}
    else if (comandos[1] == "interpretar") Interpretar();
    else if (comandos[1] == "compilar_I") Compilar_I();
    else TipoAyuda();
}

void Tipo_Comandos::Ayuda::Comandos()
{
    std::cout << "-comandos\n-compilar\n-sintaxis\n-interpretar\n-compilar_I\n";
}

void Tipo_Comandos::Ayuda::Compilar()
{
    std::cout << "\"Lenguaje Numeros\" compilar archivo_codigo.crd archivo_a_compilar.crb\n";
}

void Tipo_Comandos::Ayuda::Sintaxis(std::string comando)
{
    
    if (comando == "todo" || comando == "")
        std::cout << "-todo\n-imprimir\n-pedir\n-declarar\n-operacion\n";
    else if (comando == "imprimir")
        std::cout << "Imprimir : mi_variable;\n"
        "Imprimir : mi_variable,...;\n"
        "Imprimir : $ \"hola mundo\";\n"
        "Imprimir : $ \"El valor es: {mi_variable}.\";\n";
    else if (comando == "pedir")
        std::cout << "Pedir : mi_variable;\nPedir : mi_variable,...;\n";
    else if (comando == "declarar")
        std::cout << "Tipo Variables:\n-Entero\n-Decimal\n-Dinamico\n\n"
        "Sintaxis:\nDinamico : var = 1;\nDinamico : var = 1, var2 = 1.5,...;\n";
    else if (comando == "operacion")
        std::cout << "Operacion : variable = 1;\n"
        "Operacion : variable = otra_variable + 1;\n";
}

void Tipo_Comandos::Ayuda::Interpretar()
{
    std::cout << "\"Lenguaje Numeros\" interpretar archivo_compilado.crb\n";
}

void Tipo_Comandos::Ayuda::Compilar_I()
{
    std::cout << "\"Lenguaje Numeros\" compilar_I archivo_codigo.crd archivo_compilado_interpretar.crb\n";
}

void Tipo_Comandos::Ayuda::TipoAyuda()
{
    std::cout << "comandos:\ncomandos: mostrar comandos.\ncompilar: muestra compilacion.\nsintaxis: muestra sintaxis\n"
        << "interpretar: muestra como interpretar.\ncompilar_I: compilar e interpretar.\n";
}

// ── Compilar ──────────────────────────────────────────────────────────────────

void Tipo_Comandos::Compilar::Set_Compilar(std::string archivo_compilar,
    std::string archivo_compilado)
{
    std::string linea_contenido;
    size_t      linea = 1;
    Analizador_Tokens_Compilacion ATC;
    std::ifstream archivo_a_compilar(archivo_compilar);

    if (!archivo_a_compilar.is_open())
    {
        std::cout << "El archivo " << archivo_compilar << " no existe.\n";
        exit(1);
    }
	
	ATC.Inicio_analizador(archivo_compilado);
    while (std::getline(archivo_a_compilar, linea_contenido))
    {
        for (const std::string& contenido :
            divisor.Divisiones_Varias_lineas_Comandos(linea_contenido))
        {
            try
            {
                ATC.linea = linea;

                divisor.Inicio(contenido);   // ← contenido, no linea_contenido

                auto mapa = tokenizador.Mapa_Informacion(
                    divisor.Get_Comandos(), divisor.Info());

                ATC.Inicio_analizacion(mapa);
                divisor.Limpiar();
                tokenizador.Limpiar();
                ATC.Limpiar();
            }
            catch (std::runtime_error& error)
            {
                std::cerr << contenido << endl;
                std::cerr << error.what();
                archivo_a_compilar.close();

                if (remove(archivo_compilado.c_str()) != 0)
                    std::cout << "El nombre del archivo " << archivo_compilado
                    << " no existe o no se dio correctamente";

                return;
            }
            catch (std::out_of_range& error)
            {
                std::cerr << error.what();
                archivo_a_compilar.close();
                if (remove(archivo_compilado.c_str()) != 0)
                    std::cout << "El nombre del archivo " << archivo_compilado
                    << " no existe o no se dio correctamente";
                return;
            }
        }

        linea++;
    }
    ATC.Guardar_Archivo_CRB();
}

// ── CInterpretar ──────────────────────────────────────────────────────────────

void Tipo_Comandos::CInterpretar::Set_Interpretar(std::string archivo_compilado_interpretar)
{
    Interpretar interprete;
    try
    {
        if (interprete.CargarArchivoCRB(archivo_compilado_interpretar))
        {
            interprete.Ejecutar();
        }
	}
    catch (std::runtime_error& error)
    {
        std::cerr << error.what();
        return;
    }
}