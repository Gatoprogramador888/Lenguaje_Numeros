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
        error = nombre + " no existe.\nLinea: " + std::to_string(linea)
            + ", posicion: " + std::to_string(posiciones[posicion_token]) + ".\n";
        throw std::runtime_error(error.c_str());
    }
    else if (simbolos.Es_Nulo(nombre))
    {
        error = nombre + " es nulo no se puede volver a usar\n"
            + "Linea:" + std::to_string(linea)
            + ", posicion: " + std::to_string(posiciones[posicion_token]) + ".\n";
        throw std::runtime_error(error.c_str());
    }
    return pos;
}

void Analizador_Tokens_Compilacion::emit_u8(uint8_t val)
{
    bytecode.push_back(val);
    //archivo_a_compilar.write(reinterpret_cast<const char*>(&val), sizeof(val));
}

void Analizador_Tokens_Compilacion::emit_u8(OpCode op)
{
    emit_u8(static_cast<uint8_t>(op));
}

void Analizador_Tokens_Compilacion::emit_u64(uint64_t val)
{
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&val);
    bytecode.insert(bytecode.end(), bytes, bytes + sizeof(uint64_t));
    //archivo_a_compilar.write(reinterpret_cast<const char*>(&val), sizeof(val));
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
            {
                estado = Estados::ESPERA_COMAS_FIN_COMANDO;
                if (tokens[posicion] != Tokens::VARIABLE)
                {
                    error = comandos[posicion] + " no existe dicha variable.\nLinea: "
                        + std::to_string(linea) + ", posicion: "
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
                buffer_texto += comandos[posicion];
                if (posicion + 1 < tokens.size())
                {
                    switch (tokens[posicion + 1])
                    {
                    case Tokens::CARACTER:
                        estado = Estados::ESPERA_CARACTER; break;
                    case Tokens::PARENTESIS_IZQUIERDO:
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

                break;
            }

            case Estados::ESPERA_PARENTESIS_IZQUIERDO:
                estado = Estados::ESPERA_VARIABLE;
                if (tokens[posicion] != Tokens::PARENTESIS_IZQUIERDO)
                {
                    error = "Se esperaba '{' no " + comandos[posicion] + ".\nLinea: "
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
                    && simbolos.BuscarId(comandos[posicion]) == SIZE_MAX)
                {
                    error = comandos[posicion] + " no existe.\nLinea: "
                        + std::to_string(linea) + ", posicion: "
                        + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                }

                if (estado != Estados::ERROR && simbolos.Es_Nulo(comandos[posicion]))
                {
                    error = comandos[posicion] + " se hizo nulo anteriormente.\nLinea: "
                        + std::to_string(linea) + ", posicion: "
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
                    error = "Se esperaba '}' no " + comandos[posicion] + ".\nLinea: "
                        + std::to_string(linea) + ", posicion: "
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
                    error = comandos[posicion] + " ya existe o es de tipo "
                        + Tokenizador::Get_Tipo(tokens[posicion])
                        + " no de tipo Variable.\nLinea: " + std::to_string(linea)
                        + ", posicion: " + std::to_string(posiciones[posicion]) + ".\n";
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

            variable.tipo = (Tipo_Dato == Tokens::ENTERO) ? Tipos::ENTERO
                : (Tipo_Dato == Tokens::DECIMAL) ? Tipos::DECIMAL
                : Tipos::DINAMICO;

            variable.id = simbolos.Registrar(variable.nombre, variable.tipo);
			 
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
    // ── Estados de la máquina ────────────────────────────────────────────────
    enum class Estados {
        INICIO, DIVISOR, ESPERA_VARIABLE, ESPERA_IGUAL,
        ESPERA_NUMERO, ESPERA_OPERADOR, ESPERA_FIN_COMANDO, ERROR
    };

    Estados     estado = Estados::INICIO;
    Tipos       tipo_destino = Tipos::ENTERO;   
    size_t      id_destino = SIZE_MAX;         

    std::vector<uint64_t> operandos;   // codificados (var / inline[constantes pequeñas] / constante)
    std::vector<char>     operadores;  // '+' '-' '*' '/'

    // ── Helper local: codifica un operando numérico ──────────────────────────
    // Devuelve el valor uint64_t codificado listo para emit_u64.
    auto codificar_numero = [&](const std::string& lexema) -> uint64_t
        {
            const bool es_decimal = (lexema.find('.') != std::string::npos);

            if (es_decimal)
            {
                // Siempre va a la tabla de constantes (bits 10)
                size_t idx = obtener_o_agregar_constante(lexema);
                return codificar_constante(static_cast<uint64_t>(idx));
            }

            // Entero: intentar caber en 62 bits (sin tabla de constantes)
            try
            {
                uint64_t val = static_cast<uint64_t>(std::stoll(lexema));
                if (val < (1ULL << 62))
                    return codificar_inline_int(val);   // bits 01
                // No cabe inline → tabla de constantes
                size_t idx = obtener_o_agregar_constante(lexema);
                return codificar_constante(static_cast<uint64_t>(idx));
            }
            catch (...)
            {
                // stoull falló (negativo o demasiado grande) → tabla de constantes
                size_t idx = obtener_o_agregar_constante(lexema);
                return codificar_constante(static_cast<uint64_t>(idx));
            }
        };

    // ── Recorrido FSM ────────────────────────────────────────────────────────
    for (size_t posicion = 0; posicion < tokens.size(); posicion++)
    {
        switch (estado)
        {
            // ── INICIO ───────────────────────────────────────────────────────────
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

            // ── DIVISOR (':') ─────────────────────────────────────────────────────
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

            // ── ESPERA_VARIABLE (var. destino) ────────────────────────────────────
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

            size_t pos = pos_segura(comandos[posicion], posicion);
            tipo_destino = simbolos.BuscarTipo(comandos[posicion]);
            id_destino = pos;

            break;
        }

        // ── ESPERA_IGUAL ('=') ────────────────────────────────────────────────
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

            // ── ESPERA_NUMERO (operando) ──────────────────────────────────────────
        case Estados::ESPERA_NUMERO:
        {
            if (tokens[posicion] != Tokens::VARIABLE && tokens[posicion] != Tokens::NUMERO 
                && tokens[posicion] != Tokens::NULO)
            {
                error = comandos[posicion] + " es de tipo "
                    + Tokenizador::Get_Tipo(tokens[posicion])
                    + " en vez de Variable o Numero.\nLinea: " + std::to_string(linea)
                    + ", posicion: " + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
                break;
            }

            // Determinar estado siguiente antes de consumir el token
            if (posicion + 1 < tokens.size() && tokens[posicion] == Tokens::NULO
                && tokens[posicion + 1] != Tokens::FIN_COMANDO)
            {
                error = "No se puede hacer una operacion cuando existe una asignacion nula."
                    "\nLinea: "+ std::to_string(linea) + ".\n"
                    + ", posicion: " + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
                break;
            }
            else if (posicion + 1 < tokens.size())
                estado = (tokens[posicion + 1] != Tokens::FIN_COMANDO)
                ? Estados::ESPERA_OPERADOR
                : Estados::ESPERA_FIN_COMANDO;
            else
            {
                error = "Se esperaba ';'.\nLinea: " + std::to_string(linea)
                    + ", posicion: " + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
                break;
            }

            // ── Codificar el operando ─────────────────────────────────────────
            if (tokens[posicion] == Tokens::NULO)
            {
                uint64_t valor = BIT_NULL;
                operandos.push_back(valor);
            }
            else if (tokens[posicion] == Tokens::NUMERO)
            {
                // Validaciones de tipo contra la variable destino
                const bool tiene_punto = (comandos[posicion].find('.') != std::string::npos);

                if (tipo_destino & Tipos::ENTERO && tiene_punto)
                {
                    error = "El numero " + comandos[posicion]
                        + " no es de tipo Entero.\nLinea: " + std::to_string(linea)
                        + ", posicion: " + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                    break;
                }
                if (tipo_destino & Tipos::DECIMAL && !tiene_punto)
                {
                    error = "El numero " + comandos[posicion]
                        + " no es de tipo Decimal (falta '.').\nLinea: " + std::to_string(linea)
                        + ", posicion: " + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                    break;
                }

                // Verificar que solo tenga dígitos y punto
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
                if (estado == Estados::ERROR) break;

                operandos.push_back(codificar_numero(comandos[posicion]));
            }
            else  // VARIABLE
            {

                const size_t pos_op = pos_segura(comandos[posicion], posicion);
                const Tipos  tipo_op = simbolos.BuscarTipo(comandos[posicion]);

                // Compatibilidad de tipos (DINAMICO acepta cualquier cosa)
                if (tipo_op != tipo_destino
                    && tipo_op != Tipos::DINAMICO
                    && tipo_destino != Tipos::DINAMICO)
                {
                    error = comandos[posicion] + " es de tipo "
                        + Tokenizador::Get_Tipo_Variable(tipo_op)
                        + " y no de tipo "
                        + Tokenizador::Get_Tipo_Variable(tipo_destino)
                        + ".\nLinea: " + std::to_string(linea)
                        + ", posicion: " + std::to_string(posiciones[posicion]) + ".\n";
                    estado = Estados::ERROR;
                    break;
                }

                operandos.push_back(codificar_variable(static_cast<uint64_t>(pos_op)));
            }
            break;
        }

        // ── ESPERA_OPERADOR (+, -, *, /) ──────────────────────────────────────
        case Estados::ESPERA_OPERADOR:
        {
            // El siguiente token debe ser un operando válido
            if (posicion + 1 >= tokens.size()
                || (tokens[posicion + 1] != Tokens::NUMERO
                    && tokens[posicion + 1] != Tokens::VARIABLE))
            {
                error = "Se esperaba Variable o Numero despues del operador '"
                    + comandos[posicion] + "'.\nLinea: " + std::to_string(linea)
                    + ", posicion: " + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
                break;
            }

            if (tokens[posicion] != Tokens::OPERADOR)
            {
                error = comandos[posicion] + " es de tipo "
                    + Tokenizador::Get_Tipo(tokens[posicion])
                    + ", se esperaba tipo Operador.\nLinea: " + std::to_string(linea)
                    + ", posicion: " + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
                break;
            }

            const char op_char = comandos[posicion][0];
            if (op_char != '+' && op_char != '-' && op_char != '*' && op_char != '/')
            {
                error = "Operador no valido: '" + comandos[posicion]
                    + "'.\nLinea: " + std::to_string(linea)
                    + ", posicion: " + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
                break;
            }

            operadores.push_back(op_char);
            estado = Estados::ESPERA_NUMERO;
            break;
        }

        // ── ESPERA_FIN_COMANDO (';') ──────────────────────────────────────────
        case Estados::ESPERA_FIN_COMANDO:
            if (tokens[posicion] != Tokens::FIN_COMANDO)
            {
                error = "Se esperaba ';' no '" + comandos[posicion] + "'.\nLinea: "
                    + std::to_string(linea) + ", posicion: "
                    + std::to_string(posiciones[posicion]) + ".\n";
                estado = Estados::ERROR;
            }
            break;

        case Estados::ERROR:
            throw std::runtime_error(error.c_str());
        }
    }

    // ── Guardia de seguridad ─────────────────────────────────────────────────
    if (operandos.empty() || id_destino == SIZE_MAX)
        return;

    // ── Emisión de bytecode con precedencia ──────────────────────────────────
    // Cada instrucción ocupa exactamente 25 bytes:
    //   1 byte  OpCode
    //   8 bytes destino  (siempre la variable resultado)
    //   8 bytes src1
    //   8 bytes src2
    //
    // Los operandos ya consumidos se sustituyen por `cod_destino` en la lista
    // para que la siguiente instrucción pueda usar el resultado intermedio.

    const uint64_t cod_destino = codificar_variable(static_cast<uint64_t>(id_destino));

    if (operandos.size() < 2 && operandos[0] == BIT_NULL)
    {
        lifetime_guard(cod_destino);
        return;
    }
    else if (operandos.size() < 2)
    {
        emit_u8(OpCode::ADD);
        emit_u64(cod_destino);
        emit_u64(operandos[0]);
        emit_u64(0);
    }

    // Helper local: emite una instrucción y "comprime" la lista
    auto emitir_op = [&](size_t i, OpCode op)
        {
            emit_u8(op);
            emit_u64(cod_destino);
            emit_u64(operandos[i]);
            emit_u64(operandos[i + 1]);

			uint64_t id_var = cod_destino & ~(BIT_CONSTANTE | BIT_STRING);
            // El par i / i+1 queda reducido a `destino` en la posición i
            operandos[i] = cod_destino;
            operandos.erase(operandos.begin() + static_cast<std::ptrdiff_t>(i) + 1);
            operadores.erase(operadores.begin() + static_cast<std::ptrdiff_t>(i));
        };

    // Paso 1 — MUL y DIV  (mayor precedencia, izquierda a derecha)
    {
        size_t i = 0;
        while (i < operadores.size())
        {
            if (operadores[i] == '*' || operadores[i] == '/')
            {
                emitir_op(i, operadores[i] == '*' ? OpCode::MUL : OpCode::DIV);
                // No incrementar: el nuevo operandos[i] es el resultado anterior
            }
            else
                ++i;
        }
    }

    // Paso 2 — ADD y SUB  (menor precedencia, izquierda a derecha)
    {
        size_t i = 0;
        while (i < operadores.size())
        {
            emitir_op(i, operadores[i] == '+' ? OpCode::ADD : OpCode::SUB);
            // No incrementar: ídem
        }
    }
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
    case Tokens::ENTERO:
    case Tokens::DECIMAL:
    case Tokens::DINAMICO:                             Entero_Decimal_Dinamico(); break;
    case Tokens::OPERACION:                            Operacion();              break;
    default:
		archivo_a_compilar.close();
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

void Analizador_Semantico_Interpretacion::Verificar_Peticion(std::string_view texto, Tipos type)
{
	size_t queNoContenga = texto.find_first_not_of("0123456789.");

	if (queNoContenga != std::string::npos)
	{
		const std::string err =
			"No puedes agregar caracteres raros solo numeros.\nPeticion: "
			+ std::string(texto) + ".\n";
		throw std::runtime_error(err.c_str());
	}

	if (type & Tipos::ENTERO && texto.find('.') != std::string::npos)
	{
		const std::string err =
			"No puedes agregar decimales a un entero.\nPeticion: "
			+ std::string(texto) + ".\n";
		throw std::runtime_error(err.c_str());
	}
	else if (type & Tipos::DECIMAL && texto.find('.') == std::string::npos)
	{
		const std::string err =
			"No puedes agregar enteros a un decimal.\nPeticion: "
			+ std::string(texto) + ".\n";
		throw std::runtime_error(err.c_str());
	}

}