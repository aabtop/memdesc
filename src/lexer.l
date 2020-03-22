%option noyywrap
%option never-interactive
%option nounistd
%option warn noyywrap
%option reentrant
%option bison-bridge
%option bison-locations
%option yylineno

ID                [a-zA-Z][a-zA-Z0-9\-\_]*
NATURAL_NUMBER    [1-9][0-9]*
QUOTED_STRING     \"[^\"]*\"

%x IN_COMMENT

%{
#define YY_USER_ACTION \
    yylloc->first_line = yylloc->last_line; \
    yylloc->first_column = yylloc->last_column; \
    for(int i = 0; yytext[i] != '\0'; i++) { \
        if(yytext[i] == '\n') { \
            yylloc->last_line++; \
            yylloc->last_column = 1; \
        } \
        else { \
            yylloc->last_column++; \
        } \
    }
%}

%%

[ \t\r\n]	; // ignore all whitespace

"//".*\n // Eat the comment.

<INITIAL>{ // Handle multiline comments.
"/*"              BEGIN(IN_COMMENT);
}
<IN_COMMENT>{
"*/"      BEGIN(INITIAL);
[^*\n]+   // eat comment in chunks
"*"       // eat the lone star
}

"primitive" { return T_PRIMITIVE; }
"struct" { return T_STRUCT; }
"{" { return T_BLOCK_OPEN; }
"}" { return T_BLOCK_CLOSE; }
";" { return T_SEMICOLON; }
"," { return T_COMMA; }
"[" { return T_ARRAY_OPEN; }
"]" { return T_ARRAY_CLOSE; }
"(" { return T_PARENTHESES_OPEN; }
")" { return T_PARENTHESES_CLOSE; }
"#import" { return T_IMPORT; }
{QUOTED_STRING} {
  // Don't include the actual quotes in the token data.
  yylval->token_text.text = yytext + 1;
  yylval->token_text.length = yyleng - 2;
  return T_QUOTED_STRING;
}

{ID} {
  yylval->token_text.text = yytext;
  yylval->token_text.length = yyleng;
  return T_IDENTIFIER;
}

{NATURAL_NUMBER} {
  yylval->token_text.text = yytext;
  yylval->token_text.length = yyleng;
  return T_NATURAL_NUMBER;
}

%%
