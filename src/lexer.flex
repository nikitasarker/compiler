%option noyywrap

%{
// Avoid error "error: `fileno' was not declared in this scope"
extern "C" int fileno(FILE *stream);
#include "parser.tab.hpp"
%}


%%



[*]             { yylval.string = new std::string(yytext); return T_TIMES; }
[/]             { return T_DIVIDE; }
[+]             { yylval.string = new std::string(yytext); return T_PLUS; }
[-]             { yylval.string = new std::string(yytext); return T_MINUS; }
[&][&]	        { return T_LAND; }
[|][|]	        { return T_LOR; }
[&]	     		    { yylval.string = new std::string(yytext); return T_AND; }
[|]   	        { return T_OR; }
[\^]						{ return T_XOR; }
[=]			        { return T_ASSIGN; }
[=][=]	        { return T_EQUAL; }
[!][=]					{ return T_NOT_EQUAL; }
[<]			        { return T_LT; }
[>]			        { return T_MT; }
[<][=]			    { return T_LET; }
[>][=]	        { return T_MET; }
[+][+]					{ return T_INCR; }
[-][-]					{ return T_DECR; }
[%]							{ return T_MODULO; }
[!]							{ yylval.string = new std::string(yytext); return T_NOT; }
[(]             { return T_LBRACKET; }
[)]             { return T_RBRACKET; }
[<][<]					{ return T_LSHIFT; }
[>][>]					{ return T_RSHIFT; }
[{]             { return T_LCURLYBRACKET; }
[}]							{ return T_RCURLYBRACKET; }

[[]             { return T_LSQUAREBRACKET; }
[]]							{ return T_RSQUAREBRACKET; }

[:]							{ return T_COLON;}
[;]             { return T_SEMICOLON; }
[,]             { return T_COMMA; }

if 			        { return T_IF; }
else 		        { return T_ELSE; }
while		        { return T_WHILE; }
return	        { return T_RETURN; }
break						{ return T_BREAK; }
continue 				{ return T_CONTINUE; }
for							{ return T_FOR; }
switch					{ return T_SWITCH; }
case 						{ return T_CASE; }
default					{ return T_DEFAULT; }
enum						{ return T_ENUM; }

int 		        { return T_INT; }
void 		        { return T_VOID; }
float 					{ return T_FLOAT; }
double 					{ return T_DOUBLE; }



[0-9]*[.]?[0-9]* {
	 yylval.number=strtod(yytext, 0);
	return T_NUMBER;
}


[A-Z_a-z]+[0-9_]* {
	yylval.string=new std::string(yytext);
	return T_ID;
}


[ ] {}


%%

void yyerror (char const *s)
{
  fprintf (stderr, "Parse error : %s\n", s);
  exit(1);
}
