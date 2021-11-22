%{
#include "tok.h"
int yynum;
%}

IDENTIFIER [_a-zA-Z][_a-zA-Z0-9]*
NOTE_S     \/\/(.)*\n
NOTE_M     \/\*(.|\n)*?\*\/
WHITESPACE	[ \f\n\r\t\v]+
OCTAL 0[0-7]+
DECIMAL	([1-9][0-9]*)|0
HEXADECIMAL (0x|OX)[0-9a-fA-F]+

%%
{NOTE_S}|{NOTE_M}	{ /* ignore note */ }
{WHITESPACE}		{ /* ignore whitespace */ }
"int"				{ return tok_INT; }
"void"				{ return tok_VOID; }
"const"				{ return tok_CONST; }
"if"				{ return tok_IF; }
"else"				{ return tok_ELSE; }
"while"				{ return tok_WHILE; }
"break"				{ return tok_BREAK; }
"continue"			{ return tok_CONTINUE; }
"return"			{ return tok_RETURN; }
"+"					{ return tok_ADD; }
"-"					{ return tok_SUB; }
"*"					{ return tok_MUL; }
"/"					{ return tok_DIV; }
"%"					{ return tok_MODULO; }
"=="				{ return tok_EQ; }
"!="				{ return tok_NOTEQ; }
"<="				{ return tok_LESSEQ; }
">="				{ return tok_GREATEQ; }
"&&"				{ return tok_AND; }
"||"				{ return tok_OR; }
"<"					{ return tok_LESS; }
">"					{ return tok_GREAT; }
"!"					{ return tok_NOT; }
"="					{ return tok_ASSIGN; }
"("					{ return tok_LPAR; }
")"					{ return tok_RPAR; }
"{"					{ return tok_LBRACKET; }
"}"					{ return tok_RBRACKET; }
"["					{ return tok_LSQUARE; }
"]"					{ return tok_RSQUARE; }
"," 				{ return tok_COMMA; }
";"					{ return tok_SEMICOLON; }
{OCTAL}|{DECIMAL}|{HEXADECIMAL}	{ yynum = atoi(yytext); return tok_INTEGER; }
{IDENTIFIER}  		{return tok_ID;}
.   				{printf("error\n");}
%%

void yyputback(char* yytext)
{
	char *tmpStr = strdup(yytext);
	int len = strlen(tmpStr);
	for(int i = len - 1; i >= 0; i--){
		unput(tmpStr[i]);
	}
	unput(' ');
}