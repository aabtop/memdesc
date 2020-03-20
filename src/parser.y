%require "3.0"

%define api.pure full
%locations

%parse-param { ParseResults* parse_results }
%param { void* scanner }

%{
#include <memory>

#include "ast.h"
#include "parse_results.h"
#include "parser_helper_functions.h"

%}

%union {
  TokenText token_text;
}

%token<token_text> T_IDENTIFIER
%token<token_text> T_NATURAL_NUMBER
%token T_SINGLE_LINE_COMMENT T_MULTI_LINE_COMMENT
%token T_PRIMITIVE
%token T_STRUCT
%token T_BLOCK_OPEN T_BLOCK_CLOSE
%token T_ARRAY_OPEN T_ARRAY_CLOSE
%token T_PARENTHESES_OPEN T_PARENTHESES_CLOSE
%token T_SEMICOLON
%token T_COMMA

%union { Field* field_declaration; }
%destructor { delete $$; } <field_declaration>
%type <field_declaration> field_declaration

%union { Struct* struct_declaration; }
%destructor { delete $$; } <struct_declaration>
%type <struct_declaration> struct_declaration

%union { Primitive* primitive_declaration; }
%destructor { delete $$; } <primitive_declaration>
%type <primitive_declaration> primitive_declaration

%type <void> memdesc_declaration

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
  | memdesc_declaration_list memdesc_declaration {}
;

memdesc_declaration:
    struct_declaration {
			if ($1 != nullptr) {
				parse_results->structs[$1->name] = std::unique_ptr<Struct>($1);
			}
    }
  | primitive_declaration {
			if ($1 != nullptr) {
				parse_results->primitives[$1->name] = std::unique_ptr<Primitive>($1);
			}
    }
;

primitive_declaration:
    T_PRIMITIVE T_IDENTIFIER T_PARENTHESES_OPEN
      T_NATURAL_NUMBER T_COMMA T_NATURAL_NUMBER
    T_PARENTHESES_CLOSE T_SEMICOLON {
      std::string name($2.text, $2.length);

      if (LookupBaseType(*parse_results, name)) {
        // Type already declared.
        YYERROR;
      } else {
        const int MAX_SIZE = std::numeric_limits<int>::max(); 
        auto size = ParseIntInRange($4, 1, MAX_SIZE);
        if (!size) {
          // An invalid value was specified as the primitive size.
          YYERROR;
        }

        auto align = ParseIntInRange($6, 1, MAX_SIZE);
        if (!align) {
          // An invalid value was specified as the primitive alignment.
          YYERROR;
        }

        $$ = new Primitive{
            name, static_cast<int>(*size), static_cast<int>(*align)};
      }      
    };

struct_declaration:
    T_STRUCT T_IDENTIFIER T_BLOCK_OPEN struct_body T_BLOCK_CLOSE T_SEMICOLON {
      std::string name($2.text, $2.length);

      if (LookupBaseType(*parse_results, name)) {
        // Type already declared.
        YYERROR;
      } else {
        $$ = new Struct{name, std::move(*($4))};
        delete $4;
      }
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
	      $$ = new Field{Type{*found, array_count},
  	             			 std::string($2.text, $2.length)};
			}
    }
;

maybe_array_count:
    %empty {
      $$ = 0;
    }
  | T_ARRAY_OPEN T_NATURAL_NUMBER T_ARRAY_CLOSE {
      auto array_size = ParseIntInRange(
          $2, 1, std::numeric_limits<unsigned int>::max());
      if (!array_size) {
        // An invalid value was specified as the array size.
        YYERROR;
      } else {
        $$ = static_cast<unsigned int>(*array_size);
      }
    }
;

%%
