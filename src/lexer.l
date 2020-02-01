%option noyywrap
%option never-interactive
%option nounistd
%option warn noyywrap
%option reentrant
/*%option prefix="memdesc_yy"*/
%option bison-bridge
%option bison-locations

%{
#include <stdio.h>
#include "parser.tab.h"
%}

%%

[ \t\r]	; // ignore all whitespace
[0-9]+\.[0-9]+ 	{
  yylval->fval = static_cast<float>(atof(yytext)); return T_FLOAT;
}
[0-9]+ {
  yylval->ival = atoi(yytext); return T_INT;
}
\n		{return T_NEWLINE;}
"+"		{return T_PLUS;}
"-"		{return T_MINUS;}
"*"		{return T_MULTIPLY;}
"/"		{return T_DIVIDE;}
"("		{return T_LEFT;}
")"		{return T_RIGHT;}
"exit"		{return T_QUIT;}
"quit"		{return T_QUIT;}

%%
