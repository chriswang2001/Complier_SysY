/*
 ============================================================================
 Name        : rdcheck.c
 Author      : Chris Wang
 Version     : 1.0
 Copyright   : Copyright (c) 2021
 Description : use recursive-descent parsing to check grammar
 ============================================================================
 */
#include "tok.h"
#include <stdbool.h>
#include <stdio.h>

extern char *yytext;
extern int yylex();

int tok;

bool CompUnit(void);
bool ConstDecl(void);
bool FuncDef(void);
bool Decl(void);
bool DeclorFuncDef(void);
bool BType(void);
bool ConstDef(void);
bool ConstInitVal(void);
bool VarDecl(void);
bool VarDef(void);
bool InitVal(void);
bool FuncType(void);
bool FuncFParams(void);
bool FuncFParam(void);
bool Block(void);
bool BlockItem(void);
bool Stmt(void);
bool Exp(void);
bool Cond(void);
bool LVal(void);
bool PrimaryExp(void);
bool Number(void);
bool UnaryExp(void);
bool UnaryOp(void);
bool FuncRParams(void);
bool MulExp(void);
bool AddExp(void);
bool RelExp(void);
bool EqExp(void);
bool LAndExp(void);
bool LOrExp(void);
bool ConstExp(void);
bool FuncDef1(void);
bool VarDecl1(void);
bool VarDef1(void);
bool LVal1(void);
bool Exp1(void);

void advance(void)
{
    tok = yylex();
    printf("tok: %d - %s\n", tok, yytext);
}

int main()
{
    advance();

    if (CompUnit())
        printf("Syntax correct\n");
    else
        printf("Syntax error\n");

    return 0;
}

// CompUnit
//	：Decl
//	| FuncDef
//	| CompUnit Decl
//	| CompUnit FuncDef
bool CompUnit(void)
{
    if (tok == tok_CONST)
    {
        if (ConstDecl())
            ;
        else
            return false;
    }
    else if (tok == tok_VOID)
    {
        if (FuncDef())
            ;
        else
            return false;
    }
    else if (tok == tok_INT)
    {
        advance();

        if (tok == tok_ID)
            advance();
        else
            return false;

        if (tok == tok_LPAR)
        {
            if (FuncDef1())
                ;
            else
                return false;
        }
        else
        {
            if (VarDecl1())
                ;
            else
                return false;
        }
    }
    else
        return false;

    if (tok)
        CompUnit();

    return true;
}

//ConstDecl
//	: 'const' BType ConstDef { ',' ConstDef } ';'
bool ConstDecl(void)
{
    if (tok == tok_CONST)
    {
        advance();

        if (BType())
            ;
        else
            return false;

        if (ConstDef())
            ;
        else
            return false;

        while (tok == tok_COMMA)
        {
            advance();
            if (ConstDef())
                ;
            else
            {
                return false;
            }
        }

        if (tok == tok_SEMICOLON)
            advance();
        else
            return false;
    }
    else
        return false;

    return true;
}

// FuncDef
//	: FuncType Ident '(' [FuncFParams] ')' Block
bool FuncDef(void)
{
    if (FuncType())
    {
        if (tok == tok_ID)
        {
            advance();
            if (tok == tok_LPAR)
            {
                advance();
                if (FuncFParams())
                    ;
                else
                    ;
                if (tok == tok_RPAR)
                {
                    advance();
                    if (Block())
                        ;
                    else
                        return false;
                }
                else
                    return false;
            }
            else
            {
                return false;
            }
        }
        else
            return false;
    }
    else
        return false;

    return true;
}

// FuncDef
//	: '(' [FuncFParams] ')' Block
bool FuncDef1(void)
{
    if (tok == tok_LPAR)
    {
        advance();
        if (FuncFParams())
            ;
        else
            ;
        if (tok == tok_RPAR)
        {
            advance();
            if (Block())
                ;
            else
                return false;
        }
        else
            return false;
    }
    else
    {
        return false;
    }

    return true;
}

// Decl
//	: ConstDecl
//	| VarDecl
bool Decl(void)
{
    if (tok == tok_CONST)
    {
        if (ConstDecl())
            ;
        else
            return false;
    }
    else if (VarDecl())
        ;
    else
        return false;

    return true;
}

// VarDecl
//	: VarDef1 { ',' VarDef } ';'
bool VarDecl1(void)
{
    if (VarDef1())
    {
        while (tok == tok_COMMA)
        {
            advance();
            if (VarDef())
                ;
            else
                return false;
        }
        if (tok == tok_SEMICOLON)
        {
            advance();
        }
        else
            return false;
    }
    else
        return false;

    return true;
}

// VarDef1
//	: { '[' ConstExp ']' }
//	| { '[' ConstExp ']' } '=' InitVal
bool VarDef1(void)
{
    // { '[' ConstExp ']' }
    while (tok == tok_LSQUARE)
    {
        advance();
        if (ConstExp())
        {
            if (tok == tok_RSQUARE)
            {
                advance();
            }
            else
                return false;
        }
        else
            return false;
    }
    // '=' InitVal
    if (tok == tok_ASSIGN)
    {
        advance();
        if (InitVal())
            ;
        else
        {
            return false;
        }
    }
    else
        ;

    return true;
}

// BType
//	: 'int'
bool BType(void)
{
    if (tok == tok_INT)
        advance();
    else
        return false;

    return true;
}

// ConstDef
//	: Ident { '[' ConstExp ']' } '=' ConstInitVal
bool ConstDef(void)
{
    if (tok == tok_ID)
    {
        advance();

        while (tok == tok_LSQUARE)
        {
            advance();

            if (ConstExp())
                ;
            else
            {
                return false;
            }

            if (tok == tok_RSQUARE)
                advance();
            else
            {
                return false;
            }
        }

        if (tok == tok_ASSIGN)
            advance();
        else
            return false;

        if (ConstInitVal())
            ;
        else
            return false;
    }
    else
        return false;

    return true;
}

// ConstInitVal
//	: ConstExp
//	| '{' [ ConstInitVal { ',' ConstInitVal } ] '}'
bool ConstInitVal(void)
{
    // ConstExp
    if (tok == tok_LBRACKET)
    {
        advance();
        // [ ConstInitVal { ',' ConstInitVal } ]
        if (ConstInitVal())
        {
            while (tok == tok_COMMA)
            {
                advance();
                if (ConstInitVal())
                {
                    // '}'
                    if (tok == tok_RBRACKET)
                    {
                        advance();
                        return true;
                    }
                    else
                        ;
                }
                else
                    return false;
            }
        }
        else
            return false;
    }
    else if (ConstExp())
        ;
    else
        return false;

    return true;
}

// VarDecl
//	: BType VarDef { ',' VarDef } ';'
bool VarDecl(void)
{
    if (BType())
    {
        if (VarDef())
        {
            while (tok == tok_COMMA)
            {
                advance();
                if (VarDef())
                    ;
                else
                    return false;
            }
            if (tok == tok_SEMICOLON)
            {
                advance();
            }
            else
                return false;
        }
        else
            return false;
    }
    else
        return false;

    return true;
}

// VarDef
//	: Ident { '[' ConstExp ']' }
//	| Ident { '[' ConstExp ']' } '=' InitVal
bool VarDef(void)
{
    if (tok == tok_ID)
    {
        advance();
        // { '[' ConstExp ']' }
        while (tok == tok_LSQUARE)
        {
            advance();
            if (ConstExp())
            {
                if (tok == tok_RSQUARE)
                {
                    advance();
                }
                else
                    return false;
            }
            else
                return false;
        }
        // '=' InitVal
        if (tok == tok_ASSIGN)
        {
            advance();
            if (InitVal())
                ;
            else
            {
                return false;
            }
        }
        else
            ;
    }
    else
        return false;

    return true;
}

// InitVal
//	: Exp
//	| '{' [ InitVal { ',' InitVal } ] '}'
bool InitVal(void)
{
    if (tok == tok_LBRACKET)
    {
        advance();
        // [ InitVal { ',' InitVal } ]
        if (InitVal())
        {
            // { ',' InitVal }
            while (tok == tok_COMMA)
            {
                advance();
                if (InitVal())
                    ;
                else
                    return false;
            }
        }
        else
            ;

        if (tok == tok_RBRACKET)
        {
            advance();
        }
        else
            return false;
    }
    else if (Exp())
        ;
    else
        return false;

    return true;
}

// FuncType
//	: 'void'
//	| 'int'
bool FuncType(void)
{
    if (tok == tok_VOID)
    {
        advance();
    }
    else if (tok == tok_INT)
    {
        advance();
    }
    else
        return false;

    return true;
}

// FuncFParams
//	: FuncFParam { ',' FuncFParam }
bool FuncFParams(void)
{
    if (FuncFParam())
    {
        while (tok == tok_COMMA)
        {
            advance();
            if (FuncFParam())
                ;
            else
                return false;
        }
    }
    else
        return false;

    return true;
}

// FuncFParam
//	: BType Ident ['[' ']' { '[' Exp ']' }]
bool FuncFParam(void)
{
    if (BType())
    {
        if (tok == tok_ID)
        {
            advance();
        }
        else
            return false;

        if (tok == tok_LSQUARE)
        {
            advance();
            if (tok == tok_RSQUARE)
            {
                advance();
            }
            else
                return false;

            while (tok == tok_LSQUARE)
            {
                advance();
                if (Exp())
                    ;
                else
                    return false;

                if (tok == tok_RSQUARE)
                {
                    advance();
                }
                else
                    return false;
            }
        }
        else
            ;
    }
    else
        return false;

    return true;
}

// Block
//	: '{' { BlockItem } '}'
bool Block(void)
{
    if (tok == tok_LBRACKET)
    {
        advance();
        while (BlockItem())
            ;
        if (tok == tok_RBRACKET)
        {
            advance();
        }
        else
        {
            {
                return false;
            }
        }
    }
    else
        return false;

    return true;
}

// BlockItem
//	: Decl
//	| Stmt
bool BlockItem(void)
{
    if (tok == tok_INT || tok == tok_CONST)
    {
        if (Decl())
            ;
        else
            return false;
    }
    else if (Stmt())
        ;
    else
        return false;

    return true;
}

bool LVal1(void)
{
    while (tok == tok_LSQUARE)
    {
        advance();
        if (Exp())
            ;
        else
            return false;
        if (tok == tok_RSQUARE)
        {
            advance();
        }
        else
        {
            {
                return false;
            }
        }
    }

    return true;
}

bool Exp1(void)
{
    if (tok == tok_LPAR)
    {
        advance();
        if (tok == tok_RPAR)
            advance();
        else if (FuncRParams())
        {
            if (tok == tok_RPAR)
                advance();
            else
            {
                return false;
            }
        }
        else
            return false;
    }
    else
        return false;

    return true;
}

// Stmt
//	: LVal '=' Exp ';'
//	| [Exp] ';'
//	| Block
//	| 'if' '(' Cond ')' Stmt [ 'else' Stmt ]
//	| 'while' '(' Cond ')' Stmt
//	| 'break' ';'
//	| 'continue' ';'
//	| 'return' [Exp] ';'
bool Stmt(void)
{
    if (tok == tok_IF)
    {
        advance();
        if (tok == tok_LPAR)
        {
            advance();
            if (Cond())
                ;
            else
            {
                return false;
            }
            if (tok == tok_RPAR)
            {
                advance();
                if (Stmt())
                {
                    if (tok == tok_ELSE)
                    {
                        advance();
                        if (Stmt())
                            ;
                        else
                            return false;
                    }
                    else
                        ;
                }
                else
                    return false;
            }
            else
            {
                return false;
            }
        }
        else
            return false;
    }
    else if (tok == tok_WHILE)
    {
        advance();
        if (tok == tok_LPAR)
        {
            advance();
            if (Cond())
                ;
            else
            {
                return false;
            }
            if (tok == tok_RPAR)
            {
                advance();
                if (Stmt())
                    ;
                else
                    return false;
            }
            else
            {
                return false;
            }
        }
        else
            return false;
    }
    else if (tok == tok_BREAK)
    {
        advance();
        if (tok == tok_SEMICOLON)
        {
            advance();
        }
        else
            return false;
    }
    else if (tok == tok_CONTINUE)
    {
        advance();
        if (tok == tok_SEMICOLON)
        {
            advance();
        }
        else
            return false;
    }
    else if (tok == tok_RETURN)
    {
        advance();
        if (Exp())
            ;
        else
            ;
        if (tok == tok_SEMICOLON)
        {
            advance();
        }
        else
            return false;
    }
    else if (tok == tok_SEMICOLON)
    {
        advance();
        return true;
    }
    else if (tok == tok_LBRACKET)
    {
        if (Block())
            ;
        else
            return false;
    }
    else if (tok == tok_ID)
    {
        advance();

        if (tok == tok_LSQUARE)
        {
            if (LVal1())
            {
                if (tok == tok_ASSIGN)
                {
                    advance();
                    if (Exp())
                    {
                        if (tok == tok_SEMICOLON)
                            advance();
                    }
                    else
                        return false;
                }
                else
                    return false;
            }
        }
        else if (tok == tok_LPAR)
        {
            if (Exp1())
            {
                if (tok == tok_SEMICOLON)
                    advance();
                else
                    return false;
            }
            else
            {
                return false;
            }
        }
        else
        {
            if (tok == tok_ASSIGN)
            {
                advance();
                if (Exp())
                {
                    if (tok == tok_SEMICOLON)
                        advance();
                }
                else
                    return false;
            }
            else
            {
                return false;
            }
        }
    }
    else
        return false;

    return true;
}

// Exp
//	: AddExp
bool Exp(void)
{

    if (AddExp())
        ;
    else
        return false;

    return true;
}

// Cond
//	: LOrExp
bool Cond(void)
{
    if (LOrExp())
        ;
    else
        return false;

    return true;
}

// LVal
//	: Ident {'[' Exp ']'}
bool LVal(void)
{
    if (tok == tok_ID)
    {
        advance();
        while (tok == tok_LSQUARE)
        {
            advance();
            if (Exp())
                ;
            else
            {
                return false;
            }
            if (tok == tok_RSQUARE)
            {
                advance();
            }
            else
                return false;
        }
    }
    else
        return false;

    return true;
}

// PrimaryExp
//	：'(' Exp ')'
//	| LVal
//	| Number
bool PrimaryExp(void)
{
    if (tok == tok_LPAR)
    {
        advance();
        if (Exp())
            ;
        else
            return false;

        if (tok == tok_RPAR)
        {
            advance();
        }
        else
            return false;
    }
    else if (tok == tok_INTEGER)
    {
        if (Number())
            ;
        else
            return false;
    }
    else if (LVal())
        ;
    else
        return false;

    return true;
}

// Number
//	：IntConst
bool Number(void)
{
    if (tok == tok_INTEGER)
    {
        advance();
    }
    else
        return false;

    return true;
}

// UnaryExp
//	：PrimaryExp
//	| Ident '(' [FuncRParams] ')'
//	| UnaryOp UnaryExp
bool UnaryExp(void)
{

    if (tok == tok_ADD || tok == tok_SUB || tok == tok_NOT)
    {
        advance();
        if (UnaryExp())
            ;
        else
            return false;
    }
    else if (tok == tok_LPAR || tok == tok_INTEGER)
    {
        if (PrimaryExp())
            ;
        else
            return false;
    }
    else if (tok == tok_ID)
    {
        advance();
        if (tok == tok_LPAR)
        {
            if (Exp1())
                ;
            else
            {
                return false;
            }
        }
        else
        {
            if (LVal1())
                ;
            else
            {
                return false;
            }
        }
    }
    else
        return false;

    return true;
}

// UnaryOp
//	：'+'
//	| '−'
//	| '!'
bool UnaryOp(void)
{
    if (tok == tok_ADD || tok == tok_SUB || tok == tok_NOT)
    {
        advance();
    }
    else
        return false;

    return true;
}

// FuncRParams
//	: Exp { ',' Exp }
bool FuncRParams(void)
{
    if (Exp())
    {
        while (tok == tok_COMMA)
        {
            advance();
            if (Exp())
                ;
            else
                return false;
        }
    }
    else
        return false;

    return true;
}

// MulExp
//	: UnaryExp
//	| MulExp ('*' | '/' | '%') UnaryExp
bool MulExp(void)
{

    if (UnaryExp())
        ;
    else
        return false;

    while (tok == tok_MUL || tok == tok_DIV || tok == tok_MODULO)
    {
        advance();
        if (UnaryExp())
            ;
        else
            return false;
    }

    return true;
}

// AddExp
//	: MulExp
//	| AddExp ('+' | '−') MulExp
bool AddExp(void)
{

    if (MulExp())
        ;
    else
        return false;

    while (tok == tok_ADD || tok == tok_SUB)
    {
        advance();
        if (MulExp())
            ;
        else
            return false;
    }

    return true;
}

// RelExp
//	: AddExp
//	| RelExp ('<' | '>' | '<=' | '>=') AddExp
bool RelExp(void)
{

    if (AddExp())
        ;
    else
        return false;

    while (tok == tok_LESS || tok == tok_GREAT || tok == tok_LESSEQ || tok == tok_GREATEQ)
    {
        advance();
        if (AddExp())
            ;
        else
            return false;
    }

    return true;
}

// EqExp
//	: RelExp
//	| EqExp ('==' | '!=') RelExp
bool EqExp(void)
{

    if (RelExp())
        ;
    else
        return false;

    while (tok == tok_EQ || tok == tok_NOTEQ)
    {
        advance();
        if (RelExp())
            ;
        else
            return false;
    }

    return true;
}

// LAndExp
//	: EqExp
//  | LAndExp '&&' EqExp
bool LAndExp(void)
{

    if (EqExp())
        ;
    else
        return false;

    while (tok == tok_AND)
    {
        advance();
        if (EqExp())
            ;
        else
            return false;
    }

    return true;
}

// LOrExp
//	: LAndExp
//	| LOrExp '||' LAndExp
bool LOrExp(void)
{

    if (LAndExp())
        ;
    else
        return false;

    while (tok == tok_OR)
    {
        advance();
        if (LAndExp())
            ;
        else
            return false;
    }

    return true;
}

// ConstExp
//	: AddExp
bool ConstExp(void)
{
    if (AddExp())
        ;
    else
        return false;

    return true;
}
