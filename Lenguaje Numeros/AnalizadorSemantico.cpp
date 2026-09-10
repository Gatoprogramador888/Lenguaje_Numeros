#include "AnalizadorSemantico.h"

#include <cassert>
#include <cctype>
#include <iostream>
#include <stdexcept>
#include <string>

// ═══════════════════════════════════════════════════════════════════════════════
// Utilidades internas
// ═══════════════════════════════════════════════════════════════════════════════

// Lanza el mensaje de error ya construido en `this->error`.
// Marcado [[noreturn]] para que el compilador sepa que no hay flujo posterior.
[[noreturn]] void Analizador_Tokens_Compilacion::lanzar_error_pos(
    [[maybe_unused]] size_t posicion_token) const
{
    throw std::runtime_error(error.c_str());
}

// Devuelve el índice en obj[] o lanza un runtime_error descriptivo.
// Centraliza la guarda en UN único lugar; ningún otro sitio llama PosOBj
// directamente para luego indexar obj[].
size_t Analizador_Tokens_Compilacion::pos_segura(const std::string& nombre,
    size_t posicion_token)
{
    const size_t pos = administrador.PosOBj(nombre);
    if (pos == SIZE_MAX)
    {
        error = nombre + " no existe.\nLinea: " + std::to_string(linea)
            + ", posicion: " + std::to_string(posiciones[posicion_token]) + ".\n";
        throw std::runtime_error(error.c_str());
    }
    return pos;
}

// ═══════════════════════════════════════════════════════════════════════════════
// Fin_Linea
// ═══════════════════════════════════════════════════════════════════════════════

void Analizador_Tokens_Compilacion::Fin_Linea(Tokens fin_tokens)
{
    if (fin_tokens != Tokens::FIN_COMANDO)
    {
        const std::string err =
            "Se esperaba ';' en vez de " + comandos[comandos.size() - 1]
            + " .\nLinea: " + std::to_string(linea)
            + ", posicion: " + std::to_string(posiciones[posiciones.size() - 1]) + ".\n";
        throw std::runtime_error(err.c_str());
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// Imprimir
// ═══════════════════════════════════════════════════════════════════════════════

void Analizador_Tokens_Compilacion::Imprimir()
{
    enum class Estados
    {
        INICIO, ESPERA_DIVISOR, ESPERA_COMILLAS,
        ESPERA_PARENTESIS_DERECHO, ESPERA_PARENTESIS_IZQUIERDO, ESPERA_VARIABLE,
        ESPERA_CARACTER, ESPERA_COMAS_FIN_COMANDO, ERROR, ESPERA_TEXTO
    };
    Estados estado = Estados::INICIO;

    const bool Es_Texto = (tokens.size() > 2) && (tokens[2] == Tokens::TEXTO);
    int8_t comillas = Es_Texto ? 1 : 0;
    archivo_a_compilar << "\n>\n";

    for (size_t posicion = 0; posicion < tokens.size(); posicion++)
    {
        if (!Es_Texto)
        {
            switch (estado)
            {
            case Estados::INICIO:
                estado = Estados::ESPERA_DIVISOR;
                if (tokens[posicion] != Tokens::IMPRIMIR)
                {
                    error = comandos[posicion] + " no es de tipo Imprimir.\nLinea: "
                        + std::to_string(linea) + ", posicion: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }
                break;

            case Estados::ESPERA_DIVISOR:
                estado = Estados::ESPERA_VARIABLE;
                if (tokens[posicion] != Tokens::DIVISOR)
                {
                    error = "Se esperaba ':' no " + comandos[posicion] + ".\nLinea: "
                        + std::to_string(linea) + ", posicion: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }
                break;

            case Estados::ESPERA_VARIABLE:
                estado = Estados::ESPERA_COMAS_FIN_COMANDO;
                if (tokens[posicion] != Tokens::VARIABLE)
                {
                    error = comandos[posicion] + " no existe dicha variable.\nLinea: "
                        + std::to_string(linea) + ", posicion: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                    break;
                }
                // PosOBj guard — no accedemos a obj[] directamente aquí
                if (administrador.PosOBj(comandos[posicion]) == SIZE_MAX)
                {
                    error = comandos[posicion] + " no existe dicha variable.\nLinea: "
                        + std::to_string(linea) + ", posicion: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                    break;
                }
                archivo_a_compilar << "{ " << comandos[posicion] << "\n";
                break;

            case Estados::ESPERA_COMAS_FIN_COMANDO:
                if (tokens[posicion] == Tokens::COMAS)
                    estado = Estados::ESPERA_VARIABLE;
                else if (tokens[posicion] != Tokens::FIN_COMANDO)
                {
                    error = "Se esperaba ';' no " + comandos[posicion] + ".\nLinea: "
                        + std::to_string(linea) + ", posicion: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }
                break;

            case Estados::ERROR:
                throw std::runtime_error(error.c_str());

            default: break;
            }
        }
        else  // Es_Texto
        {
            switch (estado)
            {
            case Estados::INICIO:
                estado = Estados::ESPERA_DIVISOR;
                if (tokens[posicion] != Tokens::IMPRIMIR)
                {
                    error = comandos[posicion] + " no es de tipo Imprimir.\nLinea: "
                        + std::to_string(linea) + ", posicion: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }
                break;

            case Estados::ESPERA_DIVISOR:
                estado = Estados::ESPERA_TEXTO;
                if (tokens[posicion] != Tokens::DIVISOR)
                {
                    error = "Se esperaba ':' no " + comandos[posicion] + ".\nLinea: "
                        + std::to_string(linea) + ", posicion: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }
                break;

            case Estados::ESPERA_TEXTO:
                estado = Estados::ESPERA_COMILLAS;
                break;

            case Estados::ESPERA_COMILLAS:
            {
                if (posicion + 1 < tokens.size())
                {
                    switch (tokens[posicion + 1])
                    {
                    case Tokens::CARACTER:
                        estado = Estados::ESPERA_CARACTER; break;
                    case Tokens::PARENTESIS_IZQUIERDO:
                        estado = Estados::ESPERA_PARENTESIS_IZQUIERDO; break;
                    case Tokens::COMILLAS:
                        estado = (comillas != 2)
                            ? (++comillas, Estados::ESPERA_COMILLAS)
                            : Estados::ESPERA_COMAS_FIN_COMANDO;
                        break;
                    case Tokens::FIN_COMANDO:
                        if (comillas == 2)
                            estado = Estados::ESPERA_COMAS_FIN_COMANDO;
                        else
                        {
                            error = "Se esperaba '\"' en vez de " + comandos[posicion + 1]
                                + ".\nLinea: " + std::to_string(linea) + ", posicion: "
                                + std::to_string(posiciones[posicion]) + ".\n";
                            estado = Estados::ERROR;
                        }
                        break;
                    default:
                        error = "Se esperaba '\"' en vez de " + comandos[posicion + 1]
                            + ".\nLinea: " + std::to_string(linea) + ", posicion: "
                            + std::to_string(posiciones[posicion]) + ".\n";
                        estado = Estados::ERROR;
                        break;
                    }
                }
                else
                {
                    error = "Se esperaba ';' no " + comandos[posicion] + ".\nLinea: "
                        + std::to_string(linea) + ", posicion: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }

                if (tokens[posicion] != Tokens::COMILLAS)
                {
                    error = "Se esperaba \" en vez de " + comandos[posicion] + ".\nLinea: "
                        + std::to_string(linea) + ", posicion: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }
                break;
            }

            case Estados::ESPERA_CARACTER:
            {
                if (posicion + 1 < tokens.size())
                {
                    switch (tokens[posicion + 1])
                    {
                    case Tokens::CARACTER:
                        estado = Estados::ESPERA_CARACTER; break;
                    case Tokens::PARENTESIS_IZQUIERDO:
                        estado = Estados::ESPERA_PARENTESIS_IZQUIERDO; break;
                    case Tokens::COMILLAS:
                        estado = (comillas != 2)
                            ? (++comillas, Estados::ESPERA_COMILLAS)
                            : Estados::ESPERA_COMAS_FIN_COMANDO;
                        break;
                    case Tokens::FIN_COMANDO:
                        if (comillas == 2)
                            estado = Estados::ESPERA_COMAS_FIN_COMANDO;
                        else
                        {
                            error = "Se esperaba '\"' en vez de " + comandos[posicion + 1]
                                + ".\nLinea: " + std::to_string(linea) + ", posicion: "
                                + std::to_string(posiciones[posicion]) + ".\n";
                            estado = Estados::ERROR;
                        }
                        break;
                    default:
                        error = "Se esperaba '\"' en vez de " + comandos[posicion + 1]
                            + ".\nLinea: " + std::to_string(linea) + ", posicion: "
                            + std::to_string(posiciones[posicion]) + ".\n";
                        estado = Estados::ERROR;
                        break;
                    }
                }
                else
                {
                    error = "Se esperaba ';' no " + comandos[posicion] + ".\nLinea: "
                        + std::to_string(linea) + ", posicion: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }

                // ESPERA_CARACTER — quitar el espacio sobrante
                if (estado != Estados::ERROR)
                    archivo_a_compilar << comandos[posicion] << "\n";  // era << " \n"
                break;
            }

            case Estados::ESPERA_PARENTESIS_IZQUIERDO:
                estado = Estados::ESPERA_VARIABLE;
                if (tokens[posicion] != Tokens::PARENTESIS_IZQUIERDO)
                {
                    error = "Se esperaba '(' no " + comandos[posicion] + ".\nLinea: "
                        + std::to_string(linea) + ", posicion: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }
                break;

            case Estados::ESPERA_VARIABLE:
            {
                if (posicion + 1 < tokens.size())
                    estado = (tokens[posicion + 1] != Tokens::COMAS)
                    ? Estados::ESPERA_PARENTESIS_DERECHO
                    : Estados::ESPERA_COMAS_FIN_COMANDO;
                else
                {
                    error = "Se esperaba ';' no " + comandos[posicion] + ".\nLinea: "
                        + std::to_string(linea) + ", posicion: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }

                if (tokens[posicion] != Tokens::VARIABLE)
                {
                    error = comandos[posicion] + " es de tipo "
                        + Tokenizador::Get_Tipo(tokens[posicion])
                        + " en vez de VARIABLE.\nLinea: " + std::to_string(linea)
                        + ", posicion: " + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }

                // PosOBj guard — no accedemos a obj[]
                if (estado != Estados::ERROR
                    && administrador.PosOBj(comandos[posicion]) == SIZE_MAX)
                {
                    error = comandos[posicion] + " no existe.\nLinea: "
                        + std::to_string(linea) + ", posicion: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }

                if (estado != Estados::ERROR)
                    archivo_a_compilar << "{ " << comandos[posicion] << "\n";
                break;
            }

            case Estados::ESPERA_PARENTESIS_DERECHO:
            {
                if (posicion + 1 < tokens.size())
                {
                    switch (tokens[posicion + 1])
                    {
                    case Tokens::CARACTER:
                        estado = Estados::ESPERA_CARACTER; break;
                    case Tokens::PARENTESIS_IZQUIERDO:
                        estado = Estados::ESPERA_PARENTESIS_IZQUIERDO; break;
                    case Tokens::COMILLAS:
                        estado = (comillas != 2)
                            ? (++comillas, Estados::ESPERA_COMILLAS)
                            : Estados::ESPERA_COMAS_FIN_COMANDO;
                        break;
                    case Tokens::FIN_COMANDO:
                        if (comillas == 2)
                            estado = Estados::ESPERA_COMAS_FIN_COMANDO;
                        else
                        {
                            error = "Se esperaba '\"' en vez de " + comandos[posicion + 1]
                                + ".\nLinea: " + std::to_string(linea) + ", posicion: "
                                + std::to_string(posiciones[posicion]) + ".\n";
                            estado = Estados::ERROR;
                        }
                        break;
                    default:
                        error = "Se esperaba '\"' en vez de " + comandos[posicion + 1]
                            + ".\nLinea: " + std::to_string(linea) + ", posicion: "
                            + std::to_string(posiciones[posicion]) + ".\n";
                        estado = Estados::ERROR;
                        break;
                    }
                }
                else
                {
                    error = "Se esperaba ';' no " + comandos[posicion] + ".\nLinea: "
                        + std::to_string(linea) + ", posicion: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }

                if (tokens[posicion] != Tokens::PARENTESIS_DERECHO)
                {
                    error = "Se esperaba ')' no " + comandos[posicion] + ".\nLinea: "
                        + std::to_string(linea) + ", posicion: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }
                break;
            }

            case Estados::ESPERA_COMAS_FIN_COMANDO:
                if (tokens[posicion] == Tokens::COMAS)
                    estado = Estados::ESPERA_VARIABLE;
                else if (tokens[posicion] != Tokens::FIN_COMANDO)
                {
                    error = "Se esperaba ';' no " + comandos[posicion] + ".\nLinea: "
                        + std::to_string(linea) + ", posicion: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }
                break;

            case Estados::ERROR:
                throw std::runtime_error(error.c_str());

            default: break;
            }
        }
    }
    archivo_a_compilar << ">>\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
// Entero_Decimal_Dinamico
// ═══════════════════════════════════════════════════════════════════════════════

void Analizador_Tokens_Compilacion::Entero_Decimal_Dinamico()
{
    enum class Estados {
        INICIO, Espera_DIVISOR, Espera_VARIABLE, Espera_IGUAL,
        Espera_IGUALDAD, Espera_COMA_O_FIN, ERROR, Espera_OPERADOR
    };

    Tokens Tipo_Dato = Tokens::ENTERO;
    std::string nombre_variable;
    Estados estado = Estados::INICIO;
    std::vector<Informacion_Variable> Variables;
    Informacion_Variable variable;
    bool ultimo_pusheado = false, ya_existe_global = false, ya_existe_local = false;
    std::string tipo{};

    for (size_t posicion = 0; posicion < tokens.size(); posicion++)
    {
        ultimo_pusheado = false;

        switch (estado)
        {
        case Estados::INICIO:
            if (tokens[posicion] == Tokens::ENTERO
                || tokens[posicion] == Tokens::DECIMAL
                || tokens[posicion] == Tokens::DINAMICO)
            {
                estado = Estados::Espera_DIVISOR;
                Tipo_Dato = tokens[posicion];
                //variable.Tipo = Tokenizador::Get_Tipo(tokens[posicion]);
                tipo = Tokenizador::Get_Tipo(tokens[posicion]);
            }
            else
            {
                error = "Se esperaba un identificador.\nLinea: "
                    + std::to_string(linea) + ", posicion: "
                    + std::to_string(posiciones[posicion]) + ".\n";
                throw std::runtime_error(error.c_str());
            }
            break;

        case Estados::Espera_DIVISOR:
            if (tokens[posicion] == Tokens::DIVISOR)
                estado = Estados::Espera_VARIABLE;
            else
            {
                error = "Se esperaba un ':' no un " + comandos[posicion] + ".\nLinea: "
                    + std::to_string(linea) + ", posicion: "
                    + std::to_string(posiciones[posicion]) + ".\n";
                throw std::runtime_error(error.c_str());
            }
            break;

        case Estados::Espera_VARIABLE:

            // Verificar duplicado en el administrador (líneas anteriores)
            // Y también en las variables ya declaradas en ESTA misma línea
            ya_existe_global = administrador.PosOBj(comandos[posicion]) != SIZE_MAX;
            ya_existe_local = false;
            for (const auto& v : Variables)
                if (v.nombre == comandos[posicion]) { ya_existe_local = true; break; }

            if (tokens[posicion] == Tokens::VARIABLE
                && !ya_existe_global && !ya_existe_local)
            {
                estado = Estados::Espera_IGUAL;
                nombre_variable = comandos[posicion];
                variable.nombre = nombre_variable;
                variable.valor = "";
            }
            else
            {
                error = comandos[posicion] + " ya existe o es de tipo "
                    + Tokenizador::Get_Tipo(tokens[posicion])
                    + " no de tipo Variable.\nLinea: " + std::to_string(linea)
                    + ", posicion: " + std::to_string(posiciones[posicion]) + ".\n";
                throw std::runtime_error(error.c_str());
            }
            break;

        case Estados::Espera_IGUAL:

            if (tokens[posicion] == Tokens::IGUAL)
            {
                // BUG FIX: tokens.at(posicion+1) lanzaba std::out_of_range cuando
                // posicion era el último token; el catch(...) original era demasiado
                // amplio y enmascaraba cualquier otra excepción.
                // Corrección: verificación explícita de bounds antes de acceder.
                if (posicion + 1 >= tokens.size())
                {
                    error = "Se esperaba ';'.\nLinea: " + std::to_string(linea)
                        + ", posicion: " + std::to_string(posiciones[posicion]) + ".\n";
                }
                else if (tokens[posicion + 1] == Tokens::OPERADOR)
                    estado = Estados::Espera_OPERADOR;
                else if (tokens[posicion + 1] == Tokens::NUMERO)
                    estado = Estados::Espera_IGUALDAD;
                else
                    error = "Se esperaba una variable no " + comandos[posicion]
                    + ".\nLinea: " + std::to_string(linea)
                    + ", posicion: " + std::to_string(posiciones[posicion]) + ".\n";

                if (!error.empty()) throw std::runtime_error(error.c_str());
            }
            else if (tokens[posicion] == Tokens::COMAS
                || tokens[posicion] == Tokens::FIN_COMANDO)
            {
                // BUG FIX #6: usar posicion actual para el mensaje, no posicion-1
                error = "La variable: " + nombre_variable
                    + " no ha sido inicializada.\nLinea: " + std::to_string(linea)
                    + ", posicion: " + std::to_string(posiciones[posicion]) + ".\n";
                std::cout << error;
                variable.valor = (Tipo_Dato != Tokens::DECIMAL) ? "0" : "0.00";
                estado = Estados::Espera_COMA_O_FIN;
                // Re-procesar este token en Espera_COMA_O_FIN
                --posicion;
            }
            else
            {
                error = comandos[posicion] + " es de tipo "
                    + Tokenizador::Get_Tipo(tokens[posicion])
                    + " no de tipo Igual.\nLinea: " + std::to_string(linea)
                    + ", posicion: " + std::to_string(posiciones[posicion]) + ".\n";
                throw std::runtime_error(error.c_str());
            }
            break;

        case Estados::Espera_OPERADOR:
            if (comandos[posicion] != "-")
            {
                error = "Las variables solo pueden ser inicializadas con numeros positivos"
                    " o negativos.\nLinea: " + std::to_string(linea)
                    + ", posicion: " + std::to_string(posiciones[posicion]) + ".\n";
                throw std::runtime_error(error.c_str());
            }
            variable.valor = "-";
            estado = Estados::Espera_IGUALDAD;
            break;

        case Estados::Espera_IGUALDAD:
        {

            // Signo negativo: llega como OPERADOR antes del número
            if (tokens[posicion] == Tokens::OPERADOR && comandos[posicion] == "-")
            {
                variable.valor = "-";
                // No cambiar estado, esperar el número en la siguiente iteración
                break;
            }

            // A partir de aquí el resto igual que antes...
            for (char c : comandos[posicion])
            {
                if (!std::isdigit(static_cast<unsigned char>(c)) && c != '.')
                {
                    error = "Los Numeros no pueden tener caracteres: "
                        + comandos[posicion] + ".\nLinea: " + std::to_string(linea)
                        + ", posicion: " + std::to_string(posiciones[posicion]) + ".\n";
                    throw std::runtime_error(error.c_str());
                }
            }

            estado = Estados::Espera_COMA_O_FIN;

            if (tokens[posicion] == Tokens::NUMERO && Tipo_Dato == Tokens::DECIMAL)
            {
                if (comandos[posicion].find('.') == std::string::npos)
                {
                    error = "La igualdad " + comandos[posicion] + " de la variable "
                        + nombre_variable + " es incorrecta.\nLinea: "
                        + std::to_string(linea) + ", posicion: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    throw std::runtime_error(error.c_str());
                }
            }
            else if (tokens[posicion] == Tokens::NUMERO && Tipo_Dato == Tokens::ENTERO)
            {
                if (comandos[posicion].find('.') != std::string::npos)
                {
                    error = "La igualdad " + comandos[posicion] + " de la variable "
                        + nombre_variable + " es incorrecta.\nLinea: "
                        + std::to_string(linea) + ", posicion: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    throw std::runtime_error(error.c_str());
                }
            }
            else if (Tipo_Dato != Tokens::DINAMICO)
            {
                error = comandos[posicion] + " no es de tipo Numero es de tipo "
                    + Tokenizador::Get_Tipo(tokens[posicion]) + ".\nLinea: "
                    + std::to_string(linea) + ", posicion: "
                    + std::to_string(posiciones[posicion]) + ".\n";
                throw std::runtime_error(error.c_str());
            }

            variable.valor += comandos[posicion];

			switch (Tipo_Dato)
			{
				case Tokens::ENTERO:
					variable.Tipo = "ENTERO";
                    variable.valor += "i";
					break;
				case Tokens::DECIMAL:
					variable.Tipo = "DECIMAL";
                    variable.valor += "d";
					break;
				case Tokens::DINAMICO:
					variable.Tipo = "DINAMICO";
                    variable.valor += "m";
					break;
			}
            
            break;
        }

        case Estados::Espera_COMA_O_FIN:

            if (tokens[posicion] == Tokens::COMAS)
            {
                estado = Estados::Espera_VARIABLE;
            }
            else if (tokens[posicion] != Tokens::FIN_COMANDO)
            {
                error = comandos[posicion]
                    + " no es de tipo Fin de linea ';'.\nLinea: "
                    + std::to_string(linea) + ", posicion: "
                    + std::to_string(posiciones[posicion]) + ".\n";
                throw std::runtime_error(error.c_str());
            }
			variable.Tipo = tipo;
            Variables.push_back(variable);
            ultimo_pusheado = true;
            variable = {};  // limpiar para la próxima variable de la misma línea
            break;

        case Estados::ERROR:
            error = comandos[posicion] + " es de tipo "
                + Tokenizador::Get_Tipo(tokens[posicion])
                + " en vez de tipo VARIABLE.\nLinea: " + std::to_string(linea)
                + ", posicion: " + std::to_string(posiciones[posicion]) + ".\n";
            throw std::runtime_error(error.c_str());
        }
    }

    // Escribir cada variable al archivo compilado Y crearla en memoria.
    // Formato:  !
    //           nombre valor
    //           >>
    for (const auto& informacion : Variables)
    {
        archivo_a_compilar << "\n!\n";
        archivo_a_compilar << informacion.nombre << " " << informacion.valor << "\n";
        archivo_a_compilar << ">>\n";
        administrador.Crear(informacion);
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// Operacion
// ═══════════════════════════════════════════════════════════════════════════════

void Analizador_Tokens_Compilacion::Operacion()
{
    enum class Estados {
        INICIO, DIVISOR, ESPERA_VARIABLE, ESPERA_IGUAL,
        ESPERA_NUMERO, ESPERA_OPERADOR, ESPERA_FIN_COMANDO, ERROR
    };
    Estados estado = Estados::INICIO;
    std::string tipo, variable;
    archivo_a_compilar << "\n%\n";

    for (size_t posicion = 0; posicion < tokens.size(); posicion++)
    {
        switch (estado)
        {
        case Estados::INICIO:
            estado = Estados::DIVISOR;
            if (tokens[posicion] != Tokens::OPERACION)
            {
                error = comandos[posicion] + " no es de tipo Operacion.\nLinea: "
                    + std::to_string(linea) + ", posicion: "
                    + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
            }
            break;

        case Estados::DIVISOR:
            estado = Estados::ESPERA_VARIABLE;
            if (tokens[posicion] != Tokens::DIVISOR)
            {
                error = "Se esperaba ':' no " + comandos[posicion] + ".\nLinea: "
                    + std::to_string(linea) + ", posicion: "
                    + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
            }
            break;

        case Estados::ESPERA_VARIABLE:
        {
            estado = Estados::ESPERA_IGUAL;

            if (tokens[posicion] != Tokens::VARIABLE)
            {
                error = comandos[posicion] + " es de tipo "
                    + Tokenizador::Get_Tipo(tokens[posicion])
                    + " en vez de tipo Variable.\nLinea: " + std::to_string(linea)
                    + ", posicion: " + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
                break;
            }

            // BUG FIX: pos_segura() lanza si no existe — solo UNA llamada PosOBj,
            // el índice se reutiliza; no se llama dos veces con riesgo de SIZE_MAX.
            const size_t pos = pos_segura(comandos[posicion], posicion);
            tipo = obj[pos]->GetType();
            variable = comandos[posicion];

            archivo_a_compilar << variable << "\n";
            break;
        }

        case Estados::ESPERA_IGUAL:
            estado = Estados::ESPERA_NUMERO;
            if (tokens[posicion] != Tokens::IGUAL)
            {
                error = "Se esperaba '=' no " + comandos[posicion] + ".\nLinea: "
                    + std::to_string(linea) + ", posicion: "
                    + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
            }
            break;

        case Estados::ESPERA_NUMERO:
        {
            if (tokens[posicion] != Tokens::VARIABLE && tokens[posicion] != Tokens::NUMERO)
            {
                error = comandos[posicion] + " es de tipo "
                    + Tokenizador::Get_Tipo(tokens[posicion])
                    + " en vez de tipo Variable.\nLinea: " + std::to_string(linea)
                    + ", posicion: " + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
                break;
            }

            // Determinar estado siguiente
            if (posicion + 1 < tokens.size())
                estado = (tokens[posicion + 1] != Tokens::FIN_COMANDO)
                ? Estados::ESPERA_OPERADOR
                : Estados::ESPERA_FIN_COMANDO;
            else
            {
                error = "Se esperaba un ';'.\nLinea: " + std::to_string(linea)
                    + ", posicion: " + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
                break;
            }

            if (tokens[posicion] == Tokens::NUMERO)
            {
                // Validar tipo de la variable destino — pos_segura ya validó `variable`
                const size_t pos_var = pos_segura(variable, posicion);

                if (obj[pos_var]->GetType() == "ENTERO"
                    && comandos[posicion].find('.') != std::string::npos)
                {
                    error = "El numero " + comandos[posicion]
                        + " no es de tipo entero.\nLinea: " + std::to_string(linea)
                        + ", posicion: " + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }
                else if (obj[pos_var]->GetType() == "DECIMAL"
                    && comandos[posicion].find('.') == std::string::npos)
                {
                    error = "El numero " + comandos[posicion]
                        + " no es de tipo Decimal.\nLinea: " + std::to_string(linea)
                        + ", posicion: " + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }

                if (estado != Estados::ERROR)
                {
                    for (char c : comandos[posicion])
                    {
                        if (!std::isdigit(static_cast<unsigned char>(c)) && c != '.')
                        {
                            error = "Los Numeros no pueden tener letras.\nLinea: "
                                + std::to_string(linea) + ", posicion: "
                                + std::to_string(posiciones[posicion]) + ".\n";
                            estado = Estados::ERROR;
                            break;
                        }
                    }
                }
            }
            else  // VARIABLE
            {
                const size_t pos_op = pos_segura(comandos[posicion], posicion);
                const std::string& tipo_op = obj[pos_op]->GetType();
                if (tipo_op != tipo
                    && tipo_op != "DINAMICO"
                    && tipo != "DINAMICO")
                {
                    error = comandos[posicion] + " es de tipo " + tipo_op
                        + " y no de tipo " + tipo + ".\nLinea: " + std::to_string(linea)
                        + ", posicion: " + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }

                if (estado != Estados::ERROR)  // ← agregar esta guarda
                {
                    if (posicion + 1 < tokens.size())
                        estado = (tokens[posicion + 1] != Tokens::FIN_COMANDO)
                        ? Estados::ESPERA_OPERADOR
                        : Estados::ESPERA_FIN_COMANDO;
                    else
                    {
                        error = "Se esperaba ';'.\nLinea: " + std::to_string(linea)
                            + ", posicion: " + std::to_string(posiciones[posicion]) + ".\n";
                        estado = Estados::ERROR;
                    }
                }
            }

            if (estado != Estados::ERROR)
            {
                if (tokens[posicion] == Tokens::NUMERO)
                    archivo_a_compilar << comandos[posicion] << "\n";
                else
                    archivo_a_compilar << comandos[posicion] << "\n";
            }
            break;
        }

        case Estados::ESPERA_OPERADOR:
        {
            if (posicion + 1 < tokens.size())
            {
                if (tokens[posicion + 1] == Tokens::NUMERO
                    || tokens[posicion + 1] == Tokens::VARIABLE)
                    estado = Estados::ESPERA_NUMERO;
                else
                {
                    error = "Se esperaba una variable en " + comandos[posicion + 1]
                        + ".\nLinea: " + std::to_string(linea) + ", posicion: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                    break;
                }
            }
            else
            {
                error = "Se esperaba ';' no " + comandos[posicion] + ".\nLinea: "
                    + std::to_string(linea) + ", posicion: "
                    + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
                break;
            }

            if (tokens[posicion] != Tokens::OPERADOR)
            {
                error = comandos[posicion] + " Es de tipo "
                    + Tokenizador::Get_Tipo(tokens[posicion])
                    + " no tipo Operador.\nLinea: " + std::to_string(linea)
                    + ", posicion: " + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
            }

            if (estado != Estados::ERROR)
                archivo_a_compilar << comandos[posicion] << "\n";
            break;
        }

        case Estados::ESPERA_FIN_COMANDO:
            if (tokens[posicion] != Tokens::FIN_COMANDO)
            {
                error = "Se esperaba ';' no " + comandos[posicion] + ".\nLinea: "
                    + std::to_string(linea) + ", posicion: "
                    + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
            }
            break;

        case Estados::ERROR:
            throw std::runtime_error(error.c_str());
        }
    }
    archivo_a_compilar << "%%\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
// Pedir
// ═══════════════════════════════════════════════════════════════════════════════

void Analizador_Tokens_Compilacion::Pedir()
{
    enum class Estados { INICIO, ESPERA_DIVISOR, ESPERA_VARIABLE, ESPERA_COMA_O_FIN, ERROR };
    Estados estado = Estados::INICIO;
    archivo_a_compilar << "\n<\n";

    for (size_t i = 0; i < tokens.size(); i++)
    {
        switch (estado)
        {
        case Estados::INICIO:
            estado = Estados::ESPERA_DIVISOR;
            if (tokens[i] != Tokens::PEDIR)
            {
                error = comandos[i] + " no es de tipo Pedir es de tipo "
                    + Tokenizador::Get_Tipo(tokens[i]) + ".\nLinea: "
                    + std::to_string(linea) + ", posicion: "
                    + std::to_string(posiciones[i]) + ".\n";
                estado = Estados::ERROR;
            }
            break;

        case Estados::ESPERA_DIVISOR:
            estado = Estados::ESPERA_VARIABLE;
            if (tokens[i] != Tokens::DIVISOR)
            {
                error = "Se esperaba ':' no " + comandos[i] + ".\nLinea: "
                    + std::to_string(linea) + ", posicion: "
                    + std::to_string(posiciones[i]) + ".\n";
                estado = Estados::ERROR;
            }
            break;

        case Estados::ESPERA_VARIABLE:
            estado = Estados::ESPERA_COMA_O_FIN;
            if (tokens[i] != Tokens::VARIABLE)
            {
                error = comandos[i] + " es de tipo "
                    + Tokenizador::Get_Tipo(tokens[i])
                    + " en vez de tipo Variable.\nLinea: " + std::to_string(linea)
                    + ", posicion: " + std::to_string(posiciones[i]) + ".\n";
                estado = Estados::ERROR;
            }
            // PosOBj guard — solo verificación, sin indexar obj[]
            else if (administrador.PosOBj(comandos[i]) == SIZE_MAX)
            {
                error = comandos[i] + " no existe.\nLinea: " + std::to_string(linea)
                    + ", posicion: " + std::to_string(posiciones[i]) + ".\n";
                estado = Estados::ERROR;
            }
            else
            {
                archivo_a_compilar << comandos[i] << "\n";
            }
            break;

        case Estados::ESPERA_COMA_O_FIN:
            if (tokens[i] == Tokens::COMAS)
                estado = Estados::ESPERA_VARIABLE;
            else if (tokens[i] != Tokens::FIN_COMANDO)
            {
                error = "Se esperaba ';' no un " + comandos[i] + ".\nLinea: "
                    + std::to_string(linea) + ", posicion: "
                    + std::to_string(posiciones[i]) + ".\n";
                estado = Estados::ERROR;
            }
            break;

        case Estados::ERROR:
            throw std::runtime_error(error.c_str());
        }
    }
    archivo_a_compilar << "<<\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
// Inicio_analizacion / Limpiar
// ═══════════════════════════════════════════════════════════════════════════════

void Analizador_Tokens_Compilacion::Inicio_analizacion(
    std::map<std::string, Informacion> mapa,
    const std::string& nombre_archivo)
{
    //if (linea < 2)
        //archivo_a_compilar.open(nombre_archivo);
    if (!archivo_a_compilar.is_open())
        archivo_a_compilar.open(nombre_archivo, std::ios::trunc);

    for (const auto& [clave, info] : mapa)
    {
        tokens.push_back(info.token);
        comandos.push_back(info.comando);
        posiciones.push_back(info.posicion);
    }

    if (tokens.empty()) return;

    switch (tokens[0])
    {
    case Tokens::IMPRIMIR:                             Imprimir();               break;
    case Tokens::PEDIR:                                Pedir();                  break;
    case Tokens::ENTERO:
    case Tokens::DECIMAL:
    case Tokens::DINAMICO:                             Entero_Decimal_Dinamico(); break;
    case Tokens::OPERACION:                            Operacion();              break;
    default:
        error = comandos[0] + " no es una palabra clave.\nLinea: "
            + std::to_string(linea) + ", posicion: 0.\n";
        throw std::runtime_error(error.c_str());
    }

    Fin_Linea(tokens.at(tokens.size() - 1));
}

void Analizador_Tokens_Compilacion::Limpiar()
{
    tokens.clear();
    comandos.clear();
    posiciones.clear();
    error.clear();
}

// ═══════════════════════════════════════════════════════════════════════════════
// Analizador_Semantico_Interpretacion
// ═══════════════════════════════════════════════════════════════════════════════

void Analizador_Semantico_Interpretacion::Verificar_Peticion(const std::string& texto, std::string_view type)
{
	size_t queNoContenga = texto.find_first_not_of("0123456789.");

	if (queNoContenga != std::string::npos)
	{
		const std::string err =
			"No puedes agregar caracteres raros solo numeros.\nPeticion: "
			+ texto + ".\n";
		throw std::runtime_error(err.c_str());
	}

	if (type == "ENTERO" && texto.find('.') != std::string::npos)
	{
		const std::string err =
			"No puedes agregar decimales a un entero.\nPeticion: "
			+ texto + ".\n";
		throw std::runtime_error(err.c_str());
	}

}