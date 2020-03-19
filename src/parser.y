%require "3.0"

%define api.pure full
%locations

%parse-param { ParseResults* parse_results }
%param { void* scanner }

%{
#include <memory>

#include "ast.h"
#include "parse_results.h"

%}

%union {
  struct {
    const char* text;
    int length;
  } token_text;
}

%token<token_text> T_IDENTIFIER
%token<token_text> T_NATURAL_NUMBER
%token T_SINGLE_LINE_COMMENT T_MULTI_LINE_COMMENT
%token T_STRUCT
%token T_BLOCK_OPEN T_BLOCK_CLOSE
%token T_ARRAY_OPEN T_ARRAY_CLOSE
%token T_SEMICOLON

%union { Field* field_declaration; }
%destructor { delete $$; } <field_declaration>
%type <field_declaration> field_declaration

%union { Struct* struct_declaration; }
%destructor { delete $$; } <struct_declaration>
%type <struct_declaration> struct_declaration
%type <struct_declaration> memdesc_declaration

%union { std::vector<Field>* struct_body; }
%destructor { delete $$; } <struct_body>
%type <struct_body> struct_body

%union { unsigned int array_count; }
%type <array_count> maybe_array_count

%type <void> memdesc_declaration_list

%type <void> memdesc_contents 

%start memdesc_contents
%%

memdesc_contents:
		memdesc_declaration_list {
			parse_results->success = true;
		}
;

memdesc_declaration_list:
    %empty {}
  | memdesc_declaration_list memdesc_declaration {
			if ($2 != nullptr) {
				parse_results->structs[$2->name] = std::unique_ptr<Struct>($2);
			}
    }
;

memdesc_declaration: struct_declaration { $$ = $1; };

struct_declaration:
    T_STRUCT T_IDENTIFIER T_BLOCK_OPEN struct_body T_BLOCK_CLOSE T_SEMICOLON {
    $$ = new Struct{std::string($2.text, $2.length), std::move(*($4))};
    delete $4;
};

struct_body:
    %empty {
      $$ = new std::vector<Field>();
    }
  | struct_body field_declaration {
      std::vector<Field>* fields($1);
      fields->emplace_back(std::move(*($2)));
      delete $2;
      $$ = fields;
    }
;

field_declaration:
    T_IDENTIFIER T_IDENTIFIER maybe_array_count T_SEMICOLON {
      std::optional<unsigned int> array_count;
      if ($3 > 0) {
        array_count = $3;
      }

			std::string type_name = std::string($1.text, $1.length);
			auto found = LookupBaseType(*parse_results, type_name);

			if (!found) {
				// Undefined type referenced.
				YYERROR;
			} else {
	      $$ = new Field{Type{*found, 0, array_count},
  	             			 std::string($2.text, $2.length)};
			}
    }
;

maybe_array_count:
    %empty {
      $$ = 0;
    }
  | T_ARRAY_OPEN T_NATURAL_NUMBER T_ARRAY_CLOSE {
      std::string number_as_text($2.text, $2.length);
      long int as_long_int = strtol(number_as_text.c_str(), NULL, 10);
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
    }
;

%%
