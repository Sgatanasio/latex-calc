// PREDEFINICIONES DE SÍMBOLOS

#include <string>

struct SymbolEntry {
	std::string name;
	int token;
};

// Operadores aritméticos
static SymbolEntry operators[] = {
	{"+", PLUS},
	{"-", MINUS},
	{"*", TIMES},
	{"/", DIV},
	{"^", POWER},
	{"%", PERCENT},
	{"!", FACTORIAL},
	{"mod", MOD},
};

// Funciones matemáticas
static SymbolEntry functions[] = {
	// Raíces y exponenciales
	{"sqrt", SQRT},
	{"cbrt", CBRT},
	{"exp", EXP},

	// Logaritmos
	{"log", LOG},
	{"log10", LOG10},
	{"log2", LOG2},
	{"ln", LN},

	// Trigonometría
	{"sin", SIN},
	{"cos", COS},
	{"tan", TAN},
	{"arcsin", ARCSIN},
	{"asin", ARCSIN},
	{"arccos", ARCCOS},
	{"acos", ARCCOS},
	{"arctan", ARCTAN},
	{"atan", ARCTAN},

	// Trigonometría hiperbólica
	{"sinh", SINH},
	{"cosh", COSH},
	{"tanh", TANH},
	{"arcsinh", ARCSINH},
	{"asinh", ARCSINH},
	{"arccosh", ARCCOSH},
	{"acosh", ARCCOSH},
	{"arctanh", ARCTANH},
	{"atanh", ARCTANH},

	// Otras funciones
	{"abs", ABS},
	{"floor", FLOOR},
	{"ceil", CEIL},
	{"round", ROUND},
	{"gcd", GCD},
	{"lcm", LCM},
	{"ncr", NCR},
	{"npr", NPR},
};

// Constantes
static SymbolEntry constants[] = {
	{"pi", PI},
	{"π", PI},
	{"e", E},
	{"ans", ANS},

	// Unidades de ángulo
	{"deg", DEG},
	{"rad", RAD},
};

// Símbolos de agrupación, separación y asignación
static SymbolEntry symbols[] = {
	{"(", LPAREN},
	{")", RPAREN},
	{"[", LBRACKET},
	{"]", RBRACKET},
	{"{", LBRACE},
	{"}", RBRACE},
	{"|", BAR},
	{",", COMMA},
	{";", STMT_END},
	{"=", EQUAL},
};
