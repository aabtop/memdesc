%require "3.0"

%define api.pure full
// %define api.prefix {memdesc_yy}
%locations

%parse-param { void* my_parser }
%param { void* scanner }

%{
#include "ast.h"
%}

%union {
	const char* token_text;
}

%token<token_text> T_IDENTIFIER
%token<token_text> T_NATURAL_NUMBER
%token T_SINGLE_LINE_COMMENT T_MULTI_LINE_COMMENT
%token T_STRUCT
%token T_BLOCK_OPEN T_BLOCK_CLOSE
%token T_ARRAY_OPEN T_ARRAY_CLOSE
%token T_SEMICOLON

%union { Field* field; }
%type <field> field_declaration
%destructor { delete $$; } <field>

%union { unsigned int array_count; }
%type <array_count> maybe_array_count

//%type<ival> expression
//%type<fval> mixed_expression


%start memdesc_contents
%%

memdesc_contents:
		%empty
	| memdesc_declaration memdesc_contents
;

memdesc_declaration:
	  comment
	| struct
;

comment:
		T_SINGLE_LINE_COMMENT
	| T_MULTI_LINE_COMMENT
;

struct:
    T_STRUCT T_IDENTIFIER T_BLOCK_OPEN struct_body T_BLOCK_CLOSE T_SEMICOLON {

};

struct_body:
		%empty
	| field_declaration struct_body
;

field_declaration: T_IDENTIFIER T_IDENTIFIER maybe_array_count T_SEMICOLON {
	std::optional<unsigned int> count;
	if ($3 > 0) {
		count = $3;
	}

	$$ = new Field($1, $2, count);
};

maybe_array_count:
		%empty { $$ = 0; }
	| T_ARRAY_OPEN T_NATURAL_NUMBER T_ARRAY_CLOSE {
			long int as_long_int = strtol($2, NULL, 10);
			unsigned int as_uint =
					as_long_int < 0 ||
					    static_cast<unsigned long int>(as_long_int) >
									std::numeric_limits<unsigned int>::max() ?
					0 :
					static_cast<unsigned int>(as_long_int);

			if (as_long_int == 0) {
				YYERROR;
			}
			$$ = as_uint;
};

%%
