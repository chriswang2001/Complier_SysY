%{
enum Token_T{
	tok_ID=256,		 // 标识符
	tok_INTEGER,	 // 整数
	tok_INT,		 // int
	tok_VOID,		 // void
	tok_CONST,		 // const
	tok_IF,			 // if
	tok_ELSE,		 // else
	tok_WHILE,		 // while
	tok_BREAK,		 // break
	tok_CONTINUE,	 // continue
	tok_RETURN,		 // return
	tok_ADD,		 // +
	tok_SUB,		 // -
	tok_MUL,		 // *
	tok_DIV,		 // /
	tok_MODULO,		 // %
	tok_LESS,		 // <
	tok_LESSEQ,		 // <=
	tok_GREAT,		 // >
	tok_GREATEQ,	 // >=
	tok_NOTEQ,		 // !=
	tok_EQ,		 	 // ==
	tok_NOT,		 // !
	tok_AND, 		 // &&
	tok_OR,			 // ||
	tok_ASSIGN,		 // =
	tok_LPAR,		 // (
	tok_RPAR,		 // )
	tok_LBRACKET,	 // {
	tok_RBRACKET,	 // }
	tok_LSQUARE,	 // [
	tok_RSQUARE,	 // ]
	tok_COMMA,		 // ,
	tok_SEMICOLON	 // ;
};
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
{OCTAL}|{DECIMAL}|{HEXADECIMAL}	{ return tok_INTEGER; }
{IDENTIFIER}  		{return tok_ID;}
.   				{printf("error\n");}
%%

int main(int argc, char **argv)
{
    int tok;
	
	while(tok = yylex()) 
	{
		if(tok == tok_ID || tok == tok_INTEGER)
			printf("%d %s\n",tok, yytext);
		else
			printf("%d\n",tok);
	}

	return 0;
}