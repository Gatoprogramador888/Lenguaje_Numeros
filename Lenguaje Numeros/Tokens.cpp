#include "Tokens.h"

#include <cctype>
#include <stdexcept>
#include <string>

// ═══════════════════════════════════════════════════════════════════════════════
// Clasificadores privados
// ═══════════════════════════════════════════════════════════════════════════════

bool Tokenizador::Variable(const std::string& palabra)
{
    static const std::string TIPOS[] = { "int", "decimal", "dynamic", "const"};

    for (const auto& tipo : TIPOS)
    {
        if (palabra == tipo)
        {
            if (palabra == TIPOS[0]) token = Tokens::ENTERO;
            else if (palabra == TIPOS[1]) token = Tokens::DECIMAL;
            else if (palabra == TIPOS[2]) token = Tokens::DINAMICO;
            else if (palabra == TIPOS[3]) token = Tokens::CONSTANTE;
            return true;
        }
    }
    return false;
}

bool Tokenizador::Impresion_Peticion(const std::string& palabra)
{
    // BUG FIX #1 — condición invertida en el original:
    //   if (palabra != TIPO_PETICION[0]) token = IMPRIMIR   ← asigna IMPRIMIR
    //                                                         cuando la palabra
    //                                                         NO es "Pedir"
    // Correcto:
    //   "Pedir"    → PEDIR
    //   "Imprimir" → IMPRIMIR
    static const std::string TIPOS[] = { "input", "print" };

    for (const auto& tipo : TIPOS)
    {
        if (palabra == tipo)
        {
            token = (palabra == TIPOS[0]) ? Tokens::PEDIR : Tokens::IMPRIMIR;
            return true;
        }
    }
    return false;
}

bool Tokenizador::Caracter(const std::string& palabra)
{
    return (palabra == "." || palabra == "_");
}

void Tokenizador::Recopilar_informacion(const Informacion& info, Tokens tok)
{
    Informacion aux = info;
    aux.token = tok;
    informacion.push_back(aux);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Mapa_Informacion
// ═══════════════════════════════════════════════════════════════════════════════

std::map<std::string, Informacion> Tokenizador::Mapa_Informacion(
    const std::vector<std::string>& instruccion,
    const std::vector<Informacion>& info)
{
    // BUG FIX #3 — comillas como size_t, iniciando en 0.
    // Semántica: par = fuera de cadena de texto, impar = dentro.
    //   comillas % 2 == 0  →  fuera de comillas  (rama superior del if)
    //   comillas % 2 == 1  →  dentro de comillas (rama else)
    size_t comillas = 0;
    bool   parentesis = false;
    size_t i = 0;

    std::map<std::string, Informacion> retorno;

    for (const std::string& palabra : instruccion)
    {
        if (comillas % 2 == 0)   // ← fuera de cadena literal
        {
            if (Variable(palabra))           Recopilar_informacion(info[i], token);
            else if (Impresion_Peticion(palabra))  Recopilar_informacion(info[i], token);
            else if (palabra == "operation")       Recopilar_informacion(info[i], Tokens::OPERACION);
            else if (palabra == "#")               break;
            else if (!palabra.empty() && palabra[0] >= '0' && palabra[0] <= '9')
                Recopilar_informacion(info[i], Tokens::NUMERO);
            else if (palabra == ";")               Recopilar_informacion(info[i], Tokens::FIN_COMANDO);
            else if (palabra == ",")               Recopilar_informacion(info[i], Tokens::COMAS);
            else if (palabra == "+" || palabra == "-" ||
                palabra == "*" || palabra == "/")
                Recopilar_informacion(info[i], Tokens::OPERADOR);
            else if (palabra == "=")               Recopilar_informacion(info[i], Tokens::IGUAL);
            else if (palabra == ":")               Recopilar_informacion(info[i], Tokens::DIVISOR);
            else if (palabra == "$")               Recopilar_informacion(info[i], Tokens::TEXTO);
            else if (palabra == "\"")
            {
                Recopilar_informacion(info[i], Tokens::COMILLAS);
                ++comillas;   // comillas pasa a 1 → entramos en la rama else
            }
            else if (palabra == "{")               Recopilar_informacion(info[i], Tokens::LLAVE_IZQUIERDO);
            else if (palabra == "}")               Recopilar_informacion(info[i], Tokens::LLAVE_DERECHO);
            else if (palabra == "(")               Recopilar_informacion(info[i], Tokens::PARENTESIS_IZQUIERDO);
            else if (palabra == ")")               Recopilar_informacion(info[i], Tokens::PARENTESIS_DERECHO);
            // BUG FIX #2 — palabra[0] == NULL compara char con puntero: UB.
            // Correcto: verificar que la cadena no esté vacía antes de acceder [0].
            else if (palabra.empty() || palabra[0] == '\0') { /* ignorar */ }
            else if (palabra == "null")             Recopilar_informacion(info[i], Tokens::NULO);
            else if (std::isalpha(static_cast<unsigned char>(palabra[0])))
                Recopilar_informacion(info[i], Tokens::VARIABLE);
            else                                   Recopilar_informacion(info[i], Tokens::CARACTER);
        }
        else   // comillas % 2 == 1 → dentro de cadena literal
        {
            if (palabra == "{")
            {
                Recopilar_informacion(info[i], Tokens::LLAVE_IZQUIERDO);
                parentesis = true;
            }
            else if (palabra == "}")
            {
                Recopilar_informacion(info[i], Tokens::LLAVE_DERECHO);
                parentesis = false;
            }
            else if (palabra == "\"")
            {
                Recopilar_informacion(info[i], Tokens::COMILLAS);
                ++comillas;   // comillas pasa a 2 (par) → volvemos a la rama if
            }
            // BUG FIX #3 contd. — la condición original era (comillas % 2 != 0),
            // que dentro del else ya era siempre cierto (comillas es impar aquí).
            // La coma se emite cuando estamos dentro de un interpolado con
            // paréntesis abierto; de lo contrario es parte del texto literal.
            else if (palabra == "," && parentesis)
            {
                Recopilar_informacion(info[i], Tokens::COMAS);
            }
            else if (parentesis)
                Recopilar_informacion(info[i], Tokens::VARIABLE);
            else
                Recopilar_informacion(info[i], Tokens::CARACTER);
        }
        ++i;
    }

    // BUG FIX #4 — linea++ no debe ocurrir aquí, dentro de Mapa_Informacion.
    // El original incrementaba linea cada vez que posicion_Token alcanzaba 10,
    // lo que hacía que el número de línea dependiera de la cantidad de tokens
    // y no de los comandos reales del programa.  linea se incrementa en el
    // LLAMADOR (el analizador léxico principal) UNA VEZ por instrucción completa,
    // después de procesar cada línea del archivo fuente.
    //
    // Aquí sólo construimos el mapa con los tokens de la instrucción actual,
    // usando this->linea que el llamador ya habrá establecido correctamente.
    size_t posicion_Token = 1;
    for (const auto& it : informacion)
    {
        std::ostringstream oss;
        oss << std::setw(6) << std::setfill('0') << linea
            << "."
            << std::setw(3) << std::setfill('0') << posicion_Token;
        const std::string ID = oss.str();
        retorno.emplace(ID, it);
        ++posicion_Token;
    }

    return retorno;
}

// ═══════════════════════════════════════════════════════════════════════════════
// Get_Tipo
// ═══════════════════════════════════════════════════════════════════════════════

std::string Tokenizador::Get_Tipo(Tokens tok)
{
    // El array está indexado por el valor entero del enum.
    // Cualquier valor fuera de rango devuelve "NULO" (índice 0).
    static const std::string TIPOS[] = {
        "NULL",
        "INTERGER", "DECIMAL", "DYNAMIC",
        "NUMBER",
        "PRINT", "INPUT",
        "OPERATOR", "OPERATION",
        "COMMAS", "END COMMAND", "SPACE",
        "QUOTATION MARKS",
        "RIGHT PARENTHESIS", "LEFT PARENTHESIS",
        "VARIABLE", "EQUALS", "CHARACTER", "DIVIDER", "TEXT", "CONSTANT", "RIGHT_PARENTHESIS", "LEFT_PARENTHESIS"
    };
    static constexpr size_t N = sizeof(TIPOS) / sizeof(TIPOS[0]);

    const auto idx = static_cast<size_t>(tok);
    return (idx < N) ? TIPOS[idx] : TIPOS[0];
}

std::string Tokenizador::Get_Tipo_Variable(Tipos tipo)
{
	static const std::string TIPOS[] = {
		"INTERGER", "DECIMAL", "DYNAMIC"
	};
	static constexpr size_t N = sizeof(TIPOS) / sizeof(TIPOS[0]);
	const auto idx = static_cast<size_t>(tipo);
    return (idx < N) ? TIPOS[idx] : TIPOS[0];
}
