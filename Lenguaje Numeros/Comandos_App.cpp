#include "Comandos_App.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <chrono>


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

    //compile.Set_Compilar("prueba.crd", "a.crb");
	inter.Set_Interpretar("a.crb");
    return;

    if (argc < 2)
    {
        std::cout << "Uso: crd <comand> [arguments]\n"
            << "     crd help comands\n";
        return;
    }

    const std::string cmd = argv[1];

    // BUG FIX #2: la cadena original era if/if/if/if/else.
    // El else solo se enlazaba al último if (compilar_I), por lo que disparaba
    // para cualquier comando distinto de "compilar_I", incluyendo los válidos.
    // Corrección: if / else if / ... / else.

    if (cmd == "help")
    {
        // ayuda necesita al menos argv[2] para saber qué ayuda mostrar
        if (argc < 3)
        {
            std::cout << "Uso: crd help <issue>\n";
            return;
        }
        // Set_Ayuda espera string* apuntando desde argv[1]
        // Construimos un array local con los argumentos relevantes
        std::string args[3];
        for (int i = 0; i < 3 && (i + 1) < argc; i++)
            args[i] = argv[i + 1];
        help.Set_Ayuda(args, argc - 1);
    }
    else if (cmd == "-c")
    {
        if (argc < 4)
        {
            std::cout << "Uso: crd -c <source_file.crd> <output_file.crb>\n";
            return;
        }
        compile.Set_Compilar(argv[2], argv[3]);
    }
    else if (cmd == "-i")
    {
        if (argc < 3)
        {
            std::cout << "Uso: crd -i <compiled_file.crb>\n";
            return;
        }
        inter.Set_Interpretar(argv[2]);
    }
    else if (cmd == "-ci")
    {
        if (argc < 4)
        {
            std::cout << "Uso: crd -ci <source_file.crd> <output_file.crb>\n";
            return;
        }
        compile.Set_Compilar(argv[2], argv[3]);
        inter.Set_Interpretar(argv[3]);
    }
    else
    {
        std::cout << "That command does not exist.: " << cmd << "\n";
    }
}

Comandos_App::~Comandos_App() {}

// ── Ayuda ─────────────────────────────────────────────────────────────────────

void Tipo_Comandos::Ayuda::Set_Ayuda(std::string* comandos, int argc)
{
    if (argc < 2) return;

    if (comandos[1] == "commands")   Comandos();
    else if (comandos[1] == "compile")   Compilar();
    else if (comandos[1] == "syntax") { Sintaxis(comandos[2]);}
    else if (comandos[1] == "interpret") Interpretar();
    else if (comandos[1] == "compile_I") Compilar_I();
    else TipoAyuda();
}

void Tipo_Comandos::Ayuda::Comandos()
{
    std::cout << "-commands\n-compile\n-syntax\n-interpret\n-compile_I\n";
}

void Tipo_Comandos::Ayuda::Compilar()
{
    std::cout << "\"crd\" -c code_file.crd file_to_compile.crb\n";
}

void Tipo_Comandos::Ayuda::Sintaxis(std::string comando)
{
    
    if (comando == "all" || comando == "")
        std::cout << "-all\n-print\n-input\n-declare\n-operation\n";
    else if (comando == "print")
        std::cout << "print : my_var;\n"
        "print : my_var,...;\n"
        "print : $ \"hello world\";\n"
        "print : $ \"The value is: {my_var}.\";\n";

    else if (comando == "input")
        std::cout << "input : my_var;\ninput : my_var,...;\n";

    else if (comando == "declare")
        std::cout << "Variable Type:\n-int\n-ecimal\n-dynamic\n\n"
        "Sintaxis:\ndynamic : var = 1;\ndynamic: var = 1, var2 = 1.5,...;\nconst dynamic: pi = 3.1416;\n";

    else if (comando == "operation")
        std::cout << "operation : var = 1;\n"
        << "operation : var = null;\n"
        << "operation : var += other;\n"
        << "operation : var += other + 1;\n"
        << "operation : var++;\n"
        << "operation : var = other + 1;\n";
}

void Tipo_Comandos::Ayuda::Interpretar()
{
    std::cout << "\"crd\" -i compiled_file.crb\n";
}

void Tipo_Comandos::Ayuda::Compilar_I()
{
    std::cout << "!!!ATTENTION: THIS IS EXPERIMENTAL AND MAY ENCOUNTER ISSUES.!!!\n";
    std::cout << "\"crd\" -ci code_file.crd interpret_compiled_file.crb\n";
}

void Tipo_Comandos::Ayuda::TipoAyuda()
{
    std::cout << "commands:\ncommands: show commands.\ncompile: shows compilation.\nsyntax: show syntax\n"
        << "interpret: shows how to interpret.\ncompilar_I: compile and interpret.\n";
}

// ── Compilar ──────────────────────────────────────────────────────────────────

void Tipo_Comandos::Compilar::Set_Compilar(std::string archivo_compilar,
    std::string archivo_compilado)
{
    std::string linea_contenido;
    size_t      linea = 1;
    Analizador_Tokens_Compilacion ATC;
    std::ifstream archivo_a_compilar(archivo_compilar);
    constexpr double duracion_minuto = 60;

    if (!archivo_a_compilar.is_open())
    {
        std::cout << "The archive " << archivo_compilar << " it doesn't exist.\n";
        exit(1);
    }
	
    auto inicio = std::chrono::high_resolution_clock::now();
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
                return;
            }
            catch (std::out_of_range& error)
            {
                std::cerr << error.what();
                archivo_a_compilar.close();
                return;
            }
        }

        linea++;
    }

    ATC.Guardar_Archivo_CRB();

    auto fin = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duracion = fin - inicio;
    std::cout << "The compilation took: " << (duracion.count() / duracion_minuto) << " seconds." << std::endl;
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