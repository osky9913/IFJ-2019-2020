typedef enum {
    TTYPE_ID,       		// ID
    TTYPE_KEYWORD,          // def/else/if,None,pass,return,while
    TTYPE_EOF,              // end of file
    TTYPE_EOL,				// \n

    TTYPE_INT,              //whole number
    TTYPE_DOUBLE,           //desatinne čislo
    TTYPE_STR,              //string
    TTYPE_DOCSTR,			//documentation string
    TTYPE_NONE,             //None 	

    TTYPE_INDENT,
    TTYPE_DEDENT,

    TTYPE_COMMA,            // ','
    TTYPE_COLUMN,           // ':'
    TTYPE_LTBRAC,           // '('
    TTYPE_RTBRAC,           // ')'
    TTYPE_ASSIGN,           // '='
    TTYPE_EXCL,             // '!'

    TTYPE_ISEQ,             // '=='
    TTYPE_ISNEQ,            // '!='
    TTYPE_GTOREQ,           // '>='
    TTYPE_LSOREQ,           // '<='
    TTYPE_GT,               // '>'
    TTYPE_LS,               // '<'
    TTYPE_ADD,              // '+' addition or concatenation
    TTYPE_SUB,              // '-'
    TTYPE_MUL,              // '*'
    TTYPE_DIV,              // '/'
    TTYPE_IDIV,             // '//'
} Ttype;

typedef enum {
	KEY_IF,					
	KEY_DEF,
	KEY_ELSE,
	KEY_NONE,
	KEY_PASS,
	KEY_RETURN,
	KEY_WHILE,
} Tkeyword;

typedef union {
	long integer;
	double decimal;
	t_string string;
	Tkeyword keyword;
} Tattribute;


typedef struct{
    Ttype type;
    Tattribute attr;
} Ttoken;

int get_token();
