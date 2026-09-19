#include "AnalizadorSemantico.h"

#include <cassert>
#include <cctype>
#include <iostream>
#include <stdexcept>
#include <string>
#include "InfDec.h"

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
    size_t pos = simbolos.BuscarId(nombre);
    if (pos == SIZE_MAX)
    {
        error = nombre + " it doesn't exist.\nLine: " + std::to_string(linea)
            + ", position: " + std::to_string(posiciones[posicion_token]) + ".\n";
        throw std::runtime_error(error.c_str());
    }
    else if (simbolos.Es_Nulo(nombre))
    {
        error = nombre + " It is null; it cannot be used again.\n"
            + "Line:" + std::to_string(linea)
            + ", position: " + std::to_string(posiciones[posicion_token]) + ".\n";
        throw std::runtime_error(error.c_str());
    }
    return pos;
}

void Analizador_Tokens_Compilacion::emit_u8(uint8_t val)
{
    bytecode.push_back(val);
}

void Analizador_Tokens_Compilacion::emit_u8(OpCode op)
{
    emit_u8(static_cast<uint8_t>(op));
}

void Analizador_Tokens_Compilacion::emit_u64(uint64_t val)
{
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&val);
    bytecode.insert(bytecode.end(), bytes, bytes + sizeof(uint64_t));
}

size_t Analizador_Tokens_Compilacion::obtener_o_agregar_constante(const std::string& valor_str)
{
    auto it = mapa_constantes.find(valor_str);
    if (it != mapa_constantes.end()) {
        return it->second; 
    }
    size_t nueva_pos = tabla_constantes.size();
    tabla_constantes.push_back(valor_str);
    mapa_constantes[valor_str] = nueva_pos;
    return nueva_pos;
}

size_t Analizador_Tokens_Compilacion::obtener_o_agregar_string(const std::string& str)
{
    auto it = mapa_strings.find(str);
    if (it != mapa_strings.end()) return it->second;

    size_t nueva_pos = tabla_strings.size();
    tabla_strings.push_back(str);
    mapa_strings[str] = nueva_pos;
    return nueva_pos;
}


void Analizador_Tokens_Compilacion::Guardar_Archivo_CRB()
{
    CerrarScope();
    //if (bytecode.empty() || bytecode.back() != static_cast<uint8_t>(OpCode::HALT)) {
    emit_u8(OpCode::HALT);
    //}

    // 2. CALCULAR POSICIONES (PCT) EN RAM
    uint64_t pos_CED = 28;

    uint64_t tam_CED = sizeof(uint64_t);
    for (const auto& c : tabla_constantes) {
        tam_CED += sizeof(uint64_t) + c.size();
    }

    uint64_t pos_CS = pos_CED + tam_CED;

    uint64_t tam_CS = sizeof(uint64_t);
    for (const auto& str : tabla_strings) {
        tam_CS += sizeof(uint64_t) + str.size();
    }

    uint64_t pos_IC = pos_CS + tam_CS; // Punto donde inicia el Bytecode

    // -------------------------------------------------------------
    // ✔️ CORRECCIÓN AQUÍ: Guardar el Magic Byte en una variable o array
    // -------------------------------------------------------------
    
    archivo_a_compilar.write(magic, 4);

    // Seccion PCT (Escribir los offsets calculados)
    archivo_a_compilar.write(reinterpret_cast<const char*>(&pos_CED), sizeof(pos_CED));
    archivo_a_compilar.write(reinterpret_cast<const char*>(&pos_CS), sizeof(pos_CS));
    archivo_a_compilar.write(reinterpret_cast<const char*>(&pos_IC), sizeof(pos_IC));

    // Seccion CED
    uint64_t cant_CED = tabla_constantes.size();
    archivo_a_compilar.write(reinterpret_cast<const char*>(&cant_CED), sizeof(cant_CED));
    for (const auto& c : tabla_constantes) {
        uint64_t len = c.size();
        archivo_a_compilar.write(reinterpret_cast<const char*>(&len), sizeof(len));
        archivo_a_compilar.write(c.data(), len);
    }

    // Seccion CS
    uint64_t cant_CS = tabla_strings.size();
    archivo_a_compilar.write(reinterpret_cast<const char*>(&cant_CS), sizeof(cant_CS));
    for (const auto& str : tabla_strings) {
        uint64_t len = str.size();
        archivo_a_compilar.write(reinterpret_cast<const char*>(&len), sizeof(len));
        archivo_a_compilar.write(str.data(), len);
    }

    // Seccion IC (Bytecode de comandos acumulado)
    archivo_a_compilar.write(reinterpret_cast<const char*>(bytecode.data()), bytecode.size());

    archivo_a_compilar.flush();
    archivo_a_compilar.close();
}

// ═══════════════════════════════════════════════════════════════════════════════
// Fin_Linea
// ═══════════════════════════════════════════════════════════════════════════════

void Analizador_Tokens_Compilacion::Fin_Linea(Tokens fin_tokens)
{
    if (fin_tokens != Tokens::FIN_COMANDO)
    {
        const std::string err =
            "';' was expected instead of " + comandos[comandos.size() - 1]
            + ".\nLine: " + std::to_string(linea)
            + ", position: " + std::to_string(posiciones[posiciones.size() - 1]) + ".\n";
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
    std::vector<uint64_t> operandos; 
    std::string buffer_texto = "";

    auto flush_buffer_texto = [&]() {
        if (!buffer_texto.empty()) {
            size_t pos_str = obtener_o_agregar_string(buffer_texto);
            operandos.push_back(codificar_string(static_cast<uint64_t>(pos_str)));
            buffer_texto.clear();
        }
        };

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
                    error = comandos[posicion] + " is not of the Print type.\nLine: "
                        + std::to_string(linea) + ", position: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }
                break;

            case Estados::ESPERA_DIVISOR:
                estado = Estados::ESPERA_VARIABLE;
                if (tokens[posicion] != Tokens::DIVISOR)
                {
                    error = "':' was expected instead of " + comandos[posicion] + ".\nLine: "
                        + std::to_string(linea) + ", position: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }
                break;

            case Estados::ESPERA_VARIABLE:
            {
                estado = Estados::ESPERA_COMAS_FIN_COMANDO;
                if (tokens[posicion] != Tokens::VARIABLE)
                {
                    error = comandos[posicion] + " such a variable does not exist.\nLine: "
                        + std::to_string(linea) + ", position: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                    break;
                }
                size_t pos_var = pos_segura(comandos[posicion], posicion);
                operandos.push_back(codificar_variable(static_cast<uint64_t>(pos_var)));
            }
                break;

            case Estados::ESPERA_COMAS_FIN_COMANDO:
                if (tokens[posicion] == Tokens::COMAS)
                    estado = Estados::ESPERA_VARIABLE;
                else if (tokens[posicion] != Tokens::FIN_COMANDO)
                {
                    error = "';' was expected instead of " + comandos[posicion] + ".\nLine: "
                        + std::to_string(linea) + ", position: "
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
                    error = comandos[posicion] + " is not of the Print type.\nLine: "
                        + std::to_string(linea) + ", position: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }
                break;

            case Estados::ESPERA_DIVISOR:
                estado = Estados::ESPERA_TEXTO;
                if (tokens[posicion] != Tokens::DIVISOR)
                {
                    error = "':' was expected instead of " + comandos[posicion] + ".\nLine: "
                        + std::to_string(linea) + ", position: "
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
                    case Tokens::LLAVE_IZQUIERDO:
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
                            error = "'\"' was expected instead of " + comandos[posicion + 1]
                                + ".\nLine: " + std::to_string(linea) + ", position: "
                                + std::to_string(posiciones[posicion]) + ".\n";
                            estado = Estados::ERROR;
                        }
                        break;
                    default:
                        error = "'\"' was expected instead of " + comandos[posicion + 1]
                            + ".\nLine: " + std::to_string(linea) + ", position: "
                            + std::to_string(posiciones[posicion]) + ".\n";
                        estado = Estados::ERROR;
                        break;
                    }
                }
                else
                {
                    error = "';' was expected instead of " + comandos[posicion] + ".\nLine: "
                        + std::to_string(linea) + ", position: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }

                if (tokens[posicion] != Tokens::COMILLAS)
                {
                    error = "'\"' was expected instead of " + comandos[posicion] + ".\nLine: "
                        + std::to_string(linea) + ", position: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }
                break;
            }

            case Estados::ESPERA_CARACTER:
            {
                buffer_texto += comandos[posicion];
                if (posicion + 1 < tokens.size())
                {
                    switch (tokens[posicion + 1])
                    {
                    case Tokens::CARACTER:
                        estado = Estados::ESPERA_CARACTER; break;
                    case Tokens::LLAVE_IZQUIERDO:
                        flush_buffer_texto();
                        estado = Estados::ESPERA_PARENTESIS_IZQUIERDO; break;
                    case Tokens::COMILLAS:
                        flush_buffer_texto();
                        estado = (comillas != 2)
                            ? (++comillas, Estados::ESPERA_COMILLAS)
                            : Estados::ESPERA_COMAS_FIN_COMANDO;
                        break;
                    case Tokens::FIN_COMANDO:
                        if (comillas == 2)
                        {
                            flush_buffer_texto();
                            estado = Estados::ESPERA_COMAS_FIN_COMANDO;
                        }
                        else
                        {
                            error = "'\"' was expected instead of " + comandos[posicion + 1]
                                + ".\nLine: " + std::to_string(linea) + ", position: "
                                + std::to_string(posiciones[posicion]) + ".\n";
                            estado = Estados::ERROR;
                        }
                        break;
                    default:
                        error = "'\"' was expected instead of " + comandos[posicion + 1]
                            + ".\nLine: " + std::to_string(linea) + ", position: "
                            + std::to_string(posiciones[posicion]) + ".\n";
                        estado = Estados::ERROR;
                        break;
                    }
                }
                else
                {
                    error = "';' was expected instead of " + comandos[posicion] + ".\nLine: "
                        + std::to_string(linea) + ", position: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }

                break;
            }

            case Estados::ESPERA_PARENTESIS_IZQUIERDO:
                estado = Estados::ESPERA_VARIABLE;
                if (tokens[posicion] != Tokens::LLAVE_IZQUIERDO)
                {
                    error = "'{' was expected instead of " + comandos[posicion] + ".\nLine: "
                        + std::to_string(linea) + ", position: "
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
                    error = "';' was expected instead of " + comandos[posicion] + ".\nLine: "
                        + std::to_string(linea) + ", position: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }

                if (tokens[posicion] != Tokens::VARIABLE)
                {
                    error = comandos[posicion] + " it is of the ... type "
                        + Tokenizador::Get_Tipo(tokens[posicion])
                        + " instead of VARIABLE.\nLine: " + std::to_string(linea)
                        + ", position: " + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }

                // PosOBj guard — no accedemos a obj[]
                if (estado != Estados::ERROR
                    && simbolos.BuscarId(comandos[posicion]) == SIZE_MAX)
                {
                    error = comandos[posicion] + " it doesn't exist.\nLine: "
                        + std::to_string(linea) + ", position: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }

                if (estado != Estados::ERROR && simbolos.Es_Nulo(comandos[posicion]))
                {
                    error = comandos[posicion] + " It is null; it cannot be used again..\nLine: "
                        + std::to_string(linea) + ", position: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }

                size_t pos_var = pos_segura(comandos[posicion], posicion);
                operandos.push_back(codificar_variable(static_cast<uint64_t>(pos_var)));
            }
            break;

            case Estados::ESPERA_PARENTESIS_DERECHO:
            {
                if (posicion + 1 < tokens.size())
                {
                    switch (tokens[posicion + 1])
                    {
                    case Tokens::CARACTER:
                        estado = Estados::ESPERA_CARACTER; break;
                    case Tokens::LLAVE_IZQUIERDO:
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
                            error = "'\"' was expected instead of " + comandos[posicion + 1]
                                + ".\nLine: " + std::to_string(linea) + ", position: "
                                + std::to_string(posiciones[posicion]) + ".\n";
                            estado = Estados::ERROR;
                        }
                        break;
                    default:
                        error = "'\"' was expected instead of " + comandos[posicion + 1]
                            + ".\nLine: " + std::to_string(linea) + ", position: "
                            + std::to_string(posiciones[posicion]) + ".\n";
                        estado = Estados::ERROR;
                        break;
                    }
                }
                else
                {
                    error = "';' was expected instead of " + comandos[posicion] + ".\nLine: "
                        + std::to_string(linea) + ", position: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }

                if (tokens[posicion] != Tokens::LLAVE_DERECHO)
                {
                    error = "'}' was expected instead of " + comandos[posicion] + ".\nLine: "
                        + std::to_string(linea) + ", position: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }
            }
            break;

            case Estados::ESPERA_COMAS_FIN_COMANDO:
                if (tokens[posicion] == Tokens::COMAS)
                    estado = Estados::ESPERA_VARIABLE;
                else if (tokens[posicion] != Tokens::FIN_COMANDO)
                {
                    error = "';' was expected instead of " + comandos[posicion] + ".\nLine: "
                        + std::to_string(linea) + ", position: "
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
    
    if (!operandos.empty())
    {
        emit_u8(OpCode::PRINT);                             // 1 byte: Opcode (0x03)
        emit_u64(static_cast<uint64_t>(operandos.size()));  // 8 bytes: Número N de operandos
        for (uint64_t operando : operandos)
        {
            emit_u64(operando);                             // 8 bytes por cada elemento
        }
    }
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
    bool ultimo_pusheado = false, ya_existe_global = false, ya_existe_local = false, es_constante = false;
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
                tipo = Tokenizador::Get_Tipo(tokens[posicion]);
            }
            else if (tokens[posicion] == Tokens::CONSTANTE)
            {
                es_constante = true;
            }
            else
            {
                error = "An identifier was expected..\nLine: "
                    + std::to_string(linea) + ", position: "
                    + std::to_string(posiciones[posicion]) + ".\n";
                throw std::runtime_error(error.c_str());
            }
            break;

        case Estados::Espera_DIVISOR:
            if (tokens[posicion] == Tokens::DIVISOR)
                estado = Estados::Espera_VARIABLE;
            else
            {
                error = "':' was expected instead of " + comandos[posicion] + ".\nLine: "
                    + std::to_string(linea) + ", position: "
                    + std::to_string(posiciones[posicion]) + ".\n";
                throw std::runtime_error(error.c_str());
            }
            break;

        case Estados::Espera_VARIABLE:
            {
                // Verificar duplicado en el administrador (líneas anteriores)
                // Y también en las variables ya declaradas en ESTA misma línea
                size_t pos = simbolos.BuscarId(comandos[posicion]);
                ya_existe_global = pos != SIZE_MAX;
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
                    error = comandos[posicion] + " already exists or is of the type "
                        + Tokenizador::Get_Tipo(tokens[posicion])
                        + " not of VARIABLE type.\nLine: " + std::to_string(linea)
                        + ", position: " + std::to_string(posiciones[posicion]) + ".\n";
                    throw std::runtime_error(error.c_str());
                }
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
                    error = "';' was expected instead of.\nLine: " + std::to_string(linea)
                        + ", position: " + std::to_string(posiciones[posicion]) + ".\n";
                }
                else if (tokens[posicion + 1] == Tokens::OPERADOR)
                    estado = Estados::Espera_OPERADOR;
                else if (tokens[posicion + 1] == Tokens::NUMERO)
                    estado = Estados::Espera_IGUALDAD;
                else
                    error = " An VARIABLE was expected." + comandos[posicion]
                    + ".\nLine: " + std::to_string(linea)
                    + ", position: " + std::to_string(posiciones[posicion]) + ".\n";

                if (!error.empty()) throw std::runtime_error(error.c_str());
            }
            else if (tokens[posicion] == Tokens::COMAS
                || tokens[posicion] == Tokens::FIN_COMANDO)
            {
                // BUG FIX #6: usar posicion actual para el mensaje, no posicion-1
                error = "The variable: " + nombre_variable
                    + " has not been initialized.\nLine: " + std::to_string(linea)
                    + ", position: " + std::to_string(posiciones[posicion]) + ".\n";
                std::cout << error;
                variable.valor = (Tipo_Dato != Tokens::DECIMAL) ? "0" : "0.00";
                estado = Estados::Espera_COMA_O_FIN;
                // Re-procesar este token en Espera_COMA_O_FIN
                --posicion;
            }
            else
            {
                error = comandos[posicion] + " it is of the ... type "
                    + Tokenizador::Get_Tipo(tokens[posicion])
                    + " not of the Igua typel.\nLine: " + std::to_string(linea)
                    + ", position: " + std::to_string(posiciones[posicion]) + ".\n";
                throw std::runtime_error(error.c_str());
            }
            break;

        case Estados::Espera_OPERADOR:
            if (comandos[posicion] != "-")
            {
                error = "Variables can only be initialized with positive numbers"
                    " or negative ones.\nLine: " + std::to_string(linea)
                    + ", position: " + std::to_string(posiciones[posicion]) + ".\n";
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
                    error = "Numbers cannot contain characters.: "
                        + comandos[posicion] + ".\nLine: " + std::to_string(linea)
                        + ", position: " + std::to_string(posiciones[posicion]) + ".\n";
                    throw std::runtime_error(error.c_str());
                }
            }

            estado = Estados::Espera_COMA_O_FIN;

            if (tokens[posicion] == Tokens::NUMERO && Tipo_Dato == Tokens::DECIMAL)
            {
                if (comandos[posicion].find('.') == std::string::npos)
                {
                    error = "Equality " + comandos[posicion] + " of the variable "
                        + nombre_variable + " It is incorrect..\nLine: "
                        + std::to_string(linea) + ", position: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    throw std::runtime_error(error.c_str());
                }
            }
            else if (tokens[posicion] == Tokens::NUMERO && Tipo_Dato == Tokens::ENTERO)
            {
                if (comandos[posicion].find('.') != std::string::npos)
                {
                    error = "Equality " + comandos[posicion] + " of the variable "
                        + nombre_variable + " It is incorrect..\nLine: "
                        + std::to_string(linea) + ", position: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    throw std::runtime_error(error.c_str());
                }
            }
            else if (Tipo_Dato != Tokens::DINAMICO)
            {
                error = comandos[posicion] + " It is not of type Number; it is of type "
                    + Tokenizador::Get_Tipo(tokens[posicion]) + ".\nLine: "
                    + std::to_string(linea) + ", position: "
                    + std::to_string(posiciones[posicion]) + ".\n";
                throw std::runtime_error(error.c_str());
            }

            variable.valor += comandos[posicion];

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
                    + " it is not of the end-of-line ';' type.\nLine: "
                    + std::to_string(linea) + ", position: "
                    + std::to_string(posiciones[posicion]) + ".\n";
                throw std::runtime_error(error.c_str());
            }

            variable.tipo = (Tipo_Dato == Tokens::ENTERO) ? Tipos::ENTERO
                : (Tipo_Dato == Tokens::DECIMAL) ? Tipos::DECIMAL
                : Tipos::DINAMICO;

            variable.id = simbolos.Registrar(variable.nombre, variable.tipo, es_constante);
			 
            Variables.push_back(variable);
            ultimo_pusheado = true;
            variable = {};  // limpiar para la próxima variable de la misma línea
            break;

        case Estados::ERROR:
            error = comandos[posicion] + " it is of the type "
                + Tokenizador::Get_Tipo(tokens[posicion])
                + " instead of VARIABLE type.\nLine: " + std::to_string(linea)
                + ", position: " + std::to_string(posiciones[posicion]) + ".\n";
            throw std::runtime_error(error.c_str());
        }
    }

    // ═════════════════════════════════════════════════════════════════════════
    // EMISIÓN BINARIA DE BYTECODE
    // ═════════════════════════════════════════════════════════════════════════
    for (auto& informacion : Variables)
    {
        // 1. Extraer el valor puro (remover sufijos 'i', 'd', 'm')
        std::string val_puro = informacion.valor;

        // 2. Determinar el byte del Tipo de Dato
        uint8_t byte_tipo = TIPO_ENTERO;
        if (informacion.tipo & Tipos::DECIMAL) {
            byte_tipo = TIPO_DECIMAL;
        }
        else if (informacion.tipo & Tipos::DINAMICO) {
            byte_tipo = TIPO_DINAMICO;
        }

        // 3. Evaluar el campo de Igualdad (8 bytes)
        uint64_t igualdad_bytes = 0;
        bool es_decimal = (informacion.tipo & Tipos::DECIMAL) || (val_puro.find('.') != std::string::npos);

        if (es_decimal)
        {
            // Pasa directo a la tabla de constantes/decimales
            size_t pos_const = obtener_o_agregar_constante(val_puro);
            igualdad_bytes = codificar_constante(static_cast<uint64_t>(pos_const));
        }
        else
        {
            // Es un número entero: evaluar si cabe en 64 bits o si requiere tabla de constantes
            try {
                // Intenta convertir a uint64_t / int64_t
                uint64_t num_64 = static_cast<uint64_t>(std::stoll(val_puro));

                if (num_64 < (1ULL << 62))
                    igualdad_bytes = codificar_inline_int(num_64);
                else 
                {
                    size_t pos_const = obtener_o_agregar_constante(val_puro);
                    igualdad_bytes = codificar_constante(static_cast<uint64_t>(pos_const));
                }
            }
            catch (const std::out_of_range&) {
                // Superó los 8 bytes: guardar en la tabla de constantes
                size_t pos_const = obtener_o_agregar_constante(val_puro);
                igualdad_bytes = codificar_constante(static_cast<uint64_t>(pos_const));
            }
        }

        // 4. Emitir los 18 bytes al archivo binario
        emit_u8(OpCode::DECLARAR);                             // 1 byte: Opcode (0x04)
        emit_u64(static_cast<uint64_t>(informacion.id));        // 8 bytes: ID / Posición de la variable
        emit_u64(igualdad_bytes);                              // 8 bytes: Valor o Posición en tabla
        emit_u8(byte_tipo);                                    // 1 byte: Flag de tipo (1, 2 o 4)
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// Operacion
// ═══════════════════════════════════════════════════════════════════════════════

void Analizador_Tokens_Compilacion::Operacion()
{
    // ── Estados de la máquina de estados intacta ──────────────────────────────
    enum class Estados {
        INICIO, DIVISOR, ESPERA_VARIABLE, ESPERA_IGUAL,
        ESPERA_NUMERO, ESPERA_OPERADOR, ESPERA_PARENTESIS_DERECHO, ESPERA_PARENTESIS_IZQUIERDO,
        ESPERA_FIN_COMANDO, ERROR
    };

    Estados     estado = Estados::INICIO;
    Tipos       tipo_destino = Tipos::ENTERO;
    size_t      id_destino = SIZE_MAX;
    uint8_t     parentesis_de_lado_izquierdo{ 0 };

    // ── Estructuras para Shunting Yard ────────────────────────────────────────
    struct ElementoExpr {
        enum class Tipo { OPERANDO, OPERADOR, PARENTESIS_IZQ, PARENTESIS_DER } tipo;
        uint64_t valor_codificado{ 0 };
        char op{ 0 };
    };

    std::vector<ElementoExpr> expresion_infix;
    bool inicio_con_operador{ false };

    // ── Helper local: codifica un operando numérico ──────────────────────────
    auto codificar_numero = [&](const std::string& lexema) -> uint64_t
        {
            const bool es_decimal = (lexema.find('.') != std::string::npos);

            if (es_decimal)
            {
                size_t idx = obtener_o_agregar_constante(lexema);
                return codificar_constante(static_cast<uint64_t>(idx));
            }

            try
            {
                uint64_t val = static_cast<uint64_t>(std::stoll(lexema));
                if (val < (1ULL << 62))
                    return codificar_inline_int(val);
                size_t idx = obtener_o_agregar_constante(lexema);
                return codificar_constante(static_cast<uint64_t>(idx));
            }
            catch (...)
            {
                size_t idx = obtener_o_agregar_constante(lexema);
                return codificar_constante(static_cast<uint64_t>(idx));
            }
        };

    // ── Recorrido FSM (Mantenida exactamente igual) ──────────────────────────
    for (size_t posicion = 0; posicion < tokens.size(); posicion++)
    {
        switch (estado)
        {
        case Estados::INICIO:
            estado = Estados::DIVISOR;
            if (tokens[posicion] != Tokens::OPERACION)
            {
                error = comandos[posicion] + " It is not of the Operation type.\nLine: "
                    + std::to_string(linea) + ", position: "
                    + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
            }
            break;

        case Estados::DIVISOR:
            estado = Estados::ESPERA_VARIABLE;
            if (tokens[posicion] != Tokens::DIVISOR)
            {
                error = "':' expected, not " + comandos[posicion] + ".\nLine: "
                    + std::to_string(linea) + ", position: "
                    + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
            }
            break;

        case Estados::ESPERA_VARIABLE:
        {
            estado = Estados::ESPERA_IGUAL;

            if (tokens[posicion] != Tokens::VARIABLE)
            {
                error = comandos[posicion] + " it is of the type "
                    + Tokenizador::Get_Tipo(tokens[posicion])
                    + " instead of the Variable type.\nLine: " + std::to_string(linea)
                    + ", position: " + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
                break;
            }
            else if (simbolos.Es_Constante(comandos[posicion]))
            {
                error = comandos[posicion] + " It is of the Constant type and cannot be modified.\nLine: "
                    + std::to_string(linea) + ", position: " + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
                break;
            }

            if (posicion + 1 < tokens.size())
            {
                inicio_con_operador = tokens[posicion + 1] == Tokens::OPERADOR;
                estado = inicio_con_operador ? Estados::ESPERA_OPERADOR : Estados::ESPERA_IGUAL;
            }
            else
            {
                error = comandos[posicion] + " The operation is incomplete.\nLine: "
                    + std::to_string(linea) + ", position: " + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
                break;
            }

            size_t pos = pos_segura(comandos[posicion], posicion);
            tipo_destino = simbolos.BuscarTipo(comandos[posicion]);
            id_destino = pos;
            break;
        }

        case Estados::ESPERA_IGUAL:
            inicio_con_operador = false;
            if (tokens[posicion] != Tokens::IGUAL)
            {
                error = "'=' expected, not " + comandos[posicion] + ".\nLine: "
                    + std::to_string(linea) + ", position: "
                    + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
                break;
            }
            if (posicion + 1 >= tokens.size())
            {
                error = "';' expected.\nLine: " + std::to_string(linea)
                    + ", position: " + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
                break;
            }
            else if (tokens[posicion + 1] == Tokens::PARENTESIS_IZQUIERDO)
            {
                estado = Estados::ESPERA_PARENTESIS_IZQUIERDO;
                break;
            }

            estado = Estados::ESPERA_NUMERO;
            break;

        case Estados::ESPERA_PARENTESIS_IZQUIERDO:
        {
            parentesis_de_lado_izquierdo++;
            if (tokens[posicion] != Tokens::PARENTESIS_IZQUIERDO)
            {
                error = "'(' expected, not '" + comandos[posicion] + "'.\nLine: "
                    + std::to_string(linea) + ", position: "
                    + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
                break;
            }

            expresion_infix.push_back({ ElementoExpr::Tipo::PARENTESIS_IZQ, 0, '(' });

            if (posicion + 1 >= tokens.size())
            {
                error = "';' expected.\nLine: " + std::to_string(linea)
                    + ", position: " + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
                break;
            }
            else if (tokens[posicion + 1] == Tokens::PARENTESIS_DERECHO)
            {
                estado = Estados::ESPERA_PARENTESIS_DERECHO;
                break;
            }
            else if (tokens[posicion + 1] == Tokens::PARENTESIS_IZQUIERDO)
            {
                estado = Estados::ESPERA_PARENTESIS_IZQUIERDO;
                break;
            }
            else
            {
                estado = Estados::ESPERA_NUMERO;
                break;
            }
        }
        break;

        case Estados::ESPERA_PARENTESIS_DERECHO:
        {
            parentesis_de_lado_izquierdo--;
            if (tokens[posicion] != Tokens::PARENTESIS_DERECHO)
            {
                error = "')' expected, not '" + comandos[posicion] + "'.\nLine: "
                    + std::to_string(linea) + ", position: "
                    + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
                break;
            }

            expresion_infix.push_back({ ElementoExpr::Tipo::PARENTESIS_DER, 0, ')' });

            if (posicion + 1 >= tokens.size())
            {
                error = "';' expected.\nLine: " + std::to_string(linea)
                    + ", position: " + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
                break;
            }
            else if (tokens[posicion + 1] == Tokens::FIN_COMANDO)
            {
                estado = Estados::ESPERA_FIN_COMANDO;
                break;
            }
            else if (tokens[posicion + 1] == Tokens::PARENTESIS_DERECHO)
            {
                estado = Estados::ESPERA_PARENTESIS_DERECHO;
                break;
            }
            else if (tokens[posicion + 1] == Tokens::OPERADOR)
            {
                estado = Estados::ESPERA_OPERADOR;
                break;
            }
            else
            {
                error = "Operator or ';' expected after ')', not '" + comandos[posicion + 1] + "'.\nLine: "
                    + std::to_string(linea) + ", position: " + std::to_string(posiciones[posicion + 1]) + ".\n";
                estado = Estados::ERROR;
                break;
            }
        }
        break;

        case Estados::ESPERA_NUMERO:
        {
            if (tokens[posicion] != Tokens::VARIABLE && tokens[posicion] != Tokens::NUMERO
                && tokens[posicion] != Tokens::NULO)
            {
                error = comandos[posicion] + " it is of the type "
                    + Tokenizador::Get_Tipo(tokens[posicion])
                    + " instead of Variable or Number.\nLine: " + std::to_string(linea)
                    + ", position: " + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
                break;
            }

            if (posicion + 1 < tokens.size() && tokens[posicion] == Tokens::NULO
                && tokens[posicion + 1] != Tokens::FIN_COMANDO)
            {
                error = "An operation cannot be performed when there is a null assignment."
                    "\nLine: " + std::to_string(linea) + ".\n"
                    + ", position: " + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
                break;
            }
            else if (posicion + 1 < tokens.size())
            {
                if (tokens[posicion + 1] == Tokens::FIN_COMANDO)
                    estado = Estados::ESPERA_FIN_COMANDO;
                else if (tokens[posicion + 1] == Tokens::PARENTESIS_DERECHO)
                    estado = Estados::ESPERA_PARENTESIS_DERECHO;
                else
                    estado = Estados::ESPERA_OPERADOR;
            }
            else
            {
                error = "';' expected.\nLine: " + std::to_string(linea)
                    + ", position: " + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
                break;
            }

            // ── Codificar el operando ─────────────────────────────────────────
            if (tokens[posicion] == Tokens::NULO)
            {
                uint64_t valor = BIT_NULL;
                expresion_infix.push_back({ ElementoExpr::Tipo::OPERANDO, valor, 0 });
            }
            else if (tokens[posicion] == Tokens::NUMERO)
            {
                const bool tiene_punto = (comandos[posicion].find('.') != std::string::npos);

                if (tipo_destino & Tipos::ENTERO && tiene_punto)
                {
                    error = "The number " + comandos[posicion]
                        + " is not of the Integer type.\nLine: " + std::to_string(linea)
                        + ", position: " + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                    break;
                }
                if (tipo_destino & Tipos::DECIMAL && !tiene_punto)
                {
                    error = "The number " + comandos[posicion]
                        + " is not of type Decimal (missing '.').\nLine: " + std::to_string(linea)
                        + ", position: " + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                    break;
                }

                for (char c : comandos[posicion])
                {
                    if (!std::isdigit(static_cast<unsigned char>(c)) && c != '.')
                    {
                        error = "Numbers cannot contain letters.\nLine: "
                            + std::to_string(linea) + ", position: "
                            + std::to_string(posiciones[posicion]) + ".\n";
                        estado = Estados::ERROR;
                        break;
                    }
                }
                if (estado == Estados::ERROR) break;

                expresion_infix.push_back({ ElementoExpr::Tipo::OPERANDO, codificar_numero(comandos[posicion]), 0 });
            }
            else  // VARIABLE
            {
                const size_t pos_op = pos_segura(comandos[posicion], posicion);
                const Tipos  tipo_op = simbolos.BuscarTipo(comandos[posicion]);

                if (tipo_op != tipo_destino
                    && tipo_op != Tipos::DINAMICO
                    && tipo_destino != Tipos::DINAMICO)
                {
                    error = comandos[posicion] + " it is of the type "
                        + Tokenizador::Get_Tipo_Variable(tipo_op)
                        + " and not of the type "
                        + Tokenizador::Get_Tipo_Variable(tipo_destino)
                        + ".\nLine: " + std::to_string(linea)
                        + ", position: " + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                    break;
                }

                expresion_infix.push_back({ ElementoExpr::Tipo::OPERANDO, codificar_variable(static_cast<uint64_t>(pos_op)), 0 });
            }
            break;
        }

        case Estados::ESPERA_OPERADOR:
        {
            if (posicion + 1 >= tokens.size()
                || (tokens[posicion + 1] != Tokens::NUMERO
                    && tokens[posicion + 1] != Tokens::VARIABLE
                    && tokens[posicion + 1] != Tokens::IGUAL
                    && tokens[posicion + 1] != Tokens::OPERADOR
                    && tokens[posicion + 1] != Tokens::PARENTESIS_IZQUIERDO))
            {
                error = "Variable, number or '(' expected after operator '"
                    + comandos[posicion] + "'.\nLine: " + std::to_string(linea)
                    + ", position: " + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
                break;
            }

            if (tokens[posicion] != Tokens::OPERADOR)
            {
                error = comandos[posicion] + " it is of the type "
                    + Tokenizador::Get_Tipo(tokens[posicion])
                    + ", An operator-type role was expected..\nLine: " + std::to_string(linea)
                    + ", position: " + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
                break;
            }

            const char op_char = comandos[posicion][0];

            if (inicio_con_operador && tokens[posicion + 1] == Tokens::IGUAL)
            {
                expresion_infix.push_back({ ElementoExpr::Tipo::OPERANDO, codificar_variable(static_cast<uint64_t>(id_destino)), 0 });
                expresion_infix.push_back({ ElementoExpr::Tipo::OPERADOR, 0, op_char });
                estado = Estados::ESPERA_IGUAL;
                break;
            }
            else if (inicio_con_operador && tokens[posicion + 1] == Tokens::OPERADOR && op_char == comandos[posicion + 1][0])
            {
                posicion++;
                expresion_infix.push_back({ ElementoExpr::Tipo::OPERANDO, codificar_variable(static_cast<uint64_t>(id_destino)), 0 });
                expresion_infix.push_back({ ElementoExpr::Tipo::OPERADOR, 0, op_char });
                expresion_infix.push_back({ ElementoExpr::Tipo::OPERANDO, codificar_numero("1"), 0 });
                estado = Estados::ESPERA_FIN_COMANDO;
                break;
            }
            else if (inicio_con_operador && tokens[posicion + 1] == Tokens::OPERADOR && op_char != comandos[posicion + 1][0])
            {
                error = comandos[posicion] + " and " + op_char
                    + " They are not the same; use ++ or --.\nLine: " + std::to_string(linea)
                    + ", position: " + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
                break;
            }

            expresion_infix.push_back({ ElementoExpr::Tipo::OPERADOR, 0, op_char });

            if (tokens[posicion + 1] == Tokens::PARENTESIS_IZQUIERDO)
                estado = Estados::ESPERA_PARENTESIS_IZQUIERDO;
            else
                estado = Estados::ESPERA_NUMERO;
            break;
        }

        case Estados::ESPERA_FIN_COMANDO:
            if (tokens[posicion] != Tokens::FIN_COMANDO)
            {
                error = "';' expected, not '" + comandos[posicion] + "'.\nLine: "
                    + std::to_string(linea) + ", position: "
                    + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
            }
            break;

        case Estados::ERROR:
            throw std::runtime_error(error.c_str());
        }
    }

    if (parentesis_de_lado_izquierdo != 0)
    {
        error = "')' expected, missing parenthesis.\nLine: "
            + std::to_string(linea) + ", position: "
            + std::to_string(posiciones[posiciones.size() - 1]) + ".\n";
        throw std::runtime_error(error.c_str());
    }

    if (expresion_infix.empty() || id_destino == SIZE_MAX)
        return;

    // Caso asignación simple de valor NULO (ej: a = null;)
    if (expresion_infix.size() == 1 && expresion_infix[0].valor_codificado == BIT_NULL)
    {
        lifetime_guard(codificar_variable(static_cast<uint64_t>(id_destino)));
        return;
    }

    // ═════════════════════════════════════════════════════════════════════════
    // ALGORITMO SHUNTING YARD: Convertir Infix -> Postfix (RPN)
    // ═════════════════════════════════════════════════════════════════════════

    auto precedencia = [](char op) -> int {
        if (op == '*' || op == '/') return 2;
        if (op == '+' || op == '-') return 1;
        return 0;
        };

    std::vector<ElementoExpr> rpn;
    std::vector<ElementoExpr> pila_operadores;

    for (const auto& elem : expresion_infix)
    {
        if (elem.tipo == ElementoExpr::Tipo::OPERANDO)
        {
            rpn.push_back(elem);
        }
        else if (elem.tipo == ElementoExpr::Tipo::PARENTESIS_IZQ)
        {
            pila_operadores.push_back(elem);
        }
        else if (elem.tipo == ElementoExpr::Tipo::PARENTESIS_DER)
        {
            while (!pila_operadores.empty() && pila_operadores.back().tipo != ElementoExpr::Tipo::PARENTESIS_IZQ)
            {
                rpn.push_back(pila_operadores.back());
                pila_operadores.pop_back();
            }
            if (!pila_operadores.empty())
                pila_operadores.pop_back(); // Descartar '('
        }
        else if (elem.tipo == ElementoExpr::Tipo::OPERADOR)
        {
            while (!pila_operadores.empty() && pila_operadores.back().tipo == ElementoExpr::Tipo::OPERADOR &&
                precedencia(pila_operadores.back().op) >= precedencia(elem.op))
            {
                rpn.push_back(pila_operadores.back());
                pila_operadores.pop_back();
            }
            pila_operadores.push_back(elem);
        }
    }

    while (!pila_operadores.empty())
    {
        rpn.push_back(pila_operadores.back());
        pila_operadores.pop_back();
    }

    // ═════════════════════════════════════════════════════════════════════════
    // EMISIÓN DE BYTECODE EVALUANDO RPN
    // ═════════════════════════════════════════════════════════════════════════

    std::vector<size_t> ids_locales_creadas; // Para hacer el free al final en Runtime
    auto limpiar_locales = [&]() {
        for (size_t id_loc : ids_locales_creadas)
        {
            emit_u8(OpCode::FREE);
            emit_u64(static_cast<uint64_t>(id_loc));
        }
        simbolos.LimpiarLocales();
        };

    const uint64_t cod_destino = codificar_variable(static_cast<uint64_t>(id_destino));

    if (rpn.size() == 1 && rpn[0].tipo == ElementoExpr::Tipo::OPERANDO)
    {
        emit_u8(OpCode::ADD);
        emit_u64(cod_destino);
        emit_u64(rpn[0].valor_codificado);
        emit_u64(0);
        limpiar_locales();
        return;
    }

    // Mapeo del tipo para el Bytecode (1: ENTERO, 2: DECIMAL, 4: DINAMICO)
    uint8_t byte_tipo = 1;
    if (tipo_destino == Tipos::DECIMAL) byte_tipo = 2;
    else if (tipo_destino == Tipos::DINAMICO) byte_tipo = 4;

    std::vector<uint64_t> pila_eval;
    size_t local_counter = 0;

    for (size_t idx = 0; idx < rpn.size(); idx++)
    {
        const auto& elem = rpn[idx];

        if (elem.tipo == ElementoExpr::Tipo::OPERANDO)
        {
            pila_eval.push_back(elem.valor_codificado);
        }
        else if (elem.tipo == ElementoExpr::Tipo::OPERADOR)
        {
            if (pila_eval.size() < 2) continue;

            uint64_t src2 = pila_eval.back(); pila_eval.pop_back();
            uint64_t src1 = pila_eval.back(); pila_eval.pop_back();

            OpCode op = OpCode::ADD;
            if (elem.op == '-') op = OpCode::SUB;
            else if (elem.op == '*') op = OpCode::MUL;
            else if (elem.op == '/') op = OpCode::DIV;

            uint64_t target;

            // Si es la última operación de la RPN, escribe en el destino final
            if (idx == rpn.size() - 1)
            {
                target = cod_destino;
            }
            else
            {
                // 1. Registrar local en TablaSimbolos
                std::string nombre_local = "__loc_" + std::to_string(local_counter++);
                size_t id_local = simbolos.RegistrarLocal(nombre_local, tipo_destino, false);

                // Guardar ID para hacerle free al terminar la operación
                ids_locales_creadas.push_back(id_local);

                // 2. EMITIR DECLARAR EN RUNTIME (Para que la VM aloje en el GC)
                emit_u8(OpCode::DECLARAR);                             // Opcode DECLARAR
                emit_u64(static_cast<uint64_t>(id_local));            // ID de la variable local
                emit_u64(0);                                           // Valor inicial (0 / nulo)
                emit_u8(byte_tipo);                                   // Flag de tipo (1, 2 o 4)

                target = codificar_variable(static_cast<uint64_t>(id_local));
            }

            // 3. Emitir la operación matemática sobre la variable creada
            emit_u8(op);
            emit_u64(target);
            emit_u64(src1);
            emit_u64(src2);

            pila_eval.push_back(target);
        }
    }

    limpiar_locales();
}

void Analizador_Tokens_Compilacion::lifetime_guard(uint64_t id_variable)
{
    emit_u8(OpCode::FREE);
    emit_u64(id_variable);  // id inline para identificarlo al parchear

	simbolos.free_list.push_back(id_variable);
    simbolos.EliminarDeTabla(id_variable);
}

//USAR SOLO CUANDO TERMINEN {} BUCLES O FUNCIONES, PARA LIBERAR TODAS LAS VARIABLES DE ESE SCOPE
void Analizador_Tokens_Compilacion::CerrarScope()
{
	//usar tabla local de simbolos para liberar todas las variables de ese scope
    for (auto& [nombre, contenido] : simbolos.tabla_local) {
        emit_u8(OpCode::FREE);
        emit_u64(contenido.id);
        simbolos.free_list.push_back(contenido.id);
    }
    simbolos.tabla_local.clear();
}


// ═══════════════════════════════════════════════════════════════════════════════
// Pedir
// ═══════════════════════════════════════════════════════════════════════════════

void Analizador_Tokens_Compilacion::Pedir()
{
    enum class Estados { INICIO, ESPERA_DIVISOR, ESPERA_VARIABLE, ESPERA_COMA_O_FIN, ERROR };
    Estados estado = Estados::INICIO;

    for (size_t i = 0; i < tokens.size(); i++)
    {
        switch (estado)
        {
        case Estados::INICIO:
            estado = Estados::ESPERA_DIVISOR;
            if (tokens[i] != Tokens::PEDIR)
            {
                error = comandos[i] + " It is not of the \"input\" type; it is of the type "
                    + Tokenizador::Get_Tipo(tokens[i]) + ".\nLine: "
                    + std::to_string(linea) + ", position: "
                    + std::to_string(posiciones[i]) + ".\n";
                estado = Estados::ERROR;
            }
            break;

        case Estados::ESPERA_DIVISOR:
            estado = Estados::ESPERA_VARIABLE;
            if (tokens[i] != Tokens::DIVISOR)
            {
                error = "':' expected, not " + comandos[i] + ".\nLine: "
                    + std::to_string(linea) + ", position: "
                    + std::to_string(posiciones[i]) + ".\n";
                estado = Estados::ERROR;
            }
            break;

        case Estados::ESPERA_VARIABLE:
            estado = Estados::ESPERA_COMA_O_FIN;
            if (tokens[i] != Tokens::VARIABLE)
            {
                error = comandos[i] + " it is of the type "
                    + Tokenizador::Get_Tipo(tokens[i])
                    + " instead of the Variable type.\nLine: " + std::to_string(linea)
                    + ", position: " + std::to_string(posiciones[i]) + ".\n";
                estado = Estados::ERROR;
            }
            else if (simbolos.Es_Constante(comandos[i]))
            {
                error = comandos[i] + " It is of the Constant type and cannot be modified.\nLine: "
                    + std::to_string(linea) + ", position: " + std::to_string(posiciones[i]) + ".\n";
                estado = Estados::ERROR;
            }
            else
            {
                size_t pos_var = pos_segura(comandos[i], i);

                // 2. Codificar como variable (Bits 63 y 62 en 0)
                uint64_t id_codificado = codificar_variable(static_cast<uint64_t>(pos_var));

                // 3. Emitir Opcode (1 byte) y Operando codificado (8 bytes)
                emit_u8(OpCode::INPUT);
                emit_u64(id_codificado);
            }
            break;

        case Estados::ESPERA_COMA_O_FIN:
            if (tokens[i] == Tokens::COMAS)
                estado = Estados::ESPERA_VARIABLE;
            else if (tokens[i] != Tokens::FIN_COMANDO)
            {
                error = "Expected ';' but found " + comandos[i] + ".\nLine: "
                    + std::to_string(linea) + ", position: "
                    + std::to_string(posiciones[i]) + ".\n";
                estado = Estados::ERROR;
            }
            break;

        case Estados::ERROR:
            throw std::runtime_error(error.c_str());
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// Inicio_analizacion / Limpiar
// ═══════════════════════════════════════════════════════════════════════════════

void Analizador_Tokens_Compilacion::Inicio_analizacion(std::map<std::string, Informacion> mapa)
{
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
    case Tokens::CONSTANTE:
    case Tokens::ENTERO:
    case Tokens::DECIMAL:
    case Tokens::DINAMICO:                             Entero_Decimal_Dinamico(); break;
    case Tokens::OPERACION:                            Operacion();              break;
    default:
		archivo_a_compilar.close();
        error = comandos[0] + " It is not a keyword.\nLine: "
            + std::to_string(linea) + ", position: 0.\n";
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

void Analizador_Semantico_Interpretacion::Verificar_Peticion(std::string_view texto, Tipos type)
{
	size_t queNoContenga = texto.find_first_not_of("0123456789.");

	if (queNoContenga != std::string::npos)
	{
		const std::string err =
			"You cannot add special characters; only numbers.\ninput: "
			+ std::string(texto) + ".\n";
		throw std::runtime_error(err.c_str());
	}

	if (type & Tipos::ENTERO && texto.find('.') != std::string::npos)
	{
		const std::string err =
			"You cannot add decimals to an integer.\ninput: "
			+ std::string(texto) + ".\n";
		throw std::runtime_error(err.c_str());
	}
	else if (type & Tipos::DECIMAL && texto.find('.') == std::string::npos)
	{
		const std::string err =
			"You cannot add integers to a decimal.\ninput: "
			+ std::string(texto) + ".\n";
		throw std::runtime_error(err.c_str());
	}

}