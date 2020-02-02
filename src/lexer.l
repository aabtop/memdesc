%option noyywrap
%option never-interactive
%option nounistd
%option warn noyywrap
%option reentrant
/*%option prefix="memdesc_yy"*/
%option bison-bridge
%option bison-locations

ID                [a-zA-Z][a-zA-Z0-9\-\_]*
NATURAL_NUMBER    [1-9][0-9]*

%%

[ \t\r\n]	; // ignore all whitespace

"//".*\n { return T_SINGLE_LINE_COMMENT; }
"/*".*"*/" { return T_MULTI_LINE_COMMENT; }

"struct" { return T_STRUCT; }
"{" { return T_BLOCK_OPEN; }
"}" { return T_BLOCK_CLOSE; }
";" { return T_SEMICOLON; }
"[" { return T_ARRAY_OPEN; }
"]" { return T_ARRAY_CLOSE; }

{ID} {
  yylval->token_text = yytext;
  return T_IDENTIFIER;
}

{NATURAL_NUMBER} {
  yylval->token_text = yytext;
  return T_NATURAL_NUMBER;
}

%%
