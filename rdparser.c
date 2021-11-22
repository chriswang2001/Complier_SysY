/*
 ============================================================================
@Author      : JasonHsu
@Date        : 2021-11-16
@Name        : rdparser.c
 ============================================================================
 */
#include "tok.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylex();
extern void yyputback(char *yytext);
extern char *yytext;
extern int yynum;

typedef struct _ast ast;
typedef struct _ast *past;

struct _ast
{
    int ivalue;
    char *svalue;
    char *nodeType;
    past left;
    past right;
    past next;
};

void showAst(past node, int nest);
past CompUnit(void);
past Decl(void);
past ConstDecl(void);
past BType(void);
past ConstDef(void);
past ConstInitVal(void);
past VarDecl(void);
past VarDef(void);
past InitVal(void);
past FuncDef(void);
past FuncType(void);
past FuncFParams(void);
past FuncFParam(void);
past Block(void);
past BlockItem(void);
past Stmt(void);
past Exp(void);
past Cond(void);
past LVal(void);
past PrimaryExp(void);
past Number(void);
past UnaryExp(void);
past FuncRParams(void);
past MulExp(void);
past AddExp(void);
past RelExp(void);
past EqExp(void);
past LAndExp(void);
past LOrExp(void);
past ConstExp(void);
past newExpr(int oper, past left, past right, past next);
past newIdent(char *strVal, past next);
past newType(int type);
past newNum(int value, past next);
past newAstNodePar(char *strVal, char *nodeType, past left, past right, past next);
past newAstNode(void);

int tok;

void advance(void)
{
    tok = yylex();
    printf("tok: %d - %s\n", tok, yytext);
}

int main(void)
{
    advance();
    past r = CompUnit();
    if (r)
    {
        printf("Syntax correct\n");
        showAst(r, 0);
        printf("AST build successfully\n");
    }
    else
        printf("Syntax error\n");

    return 0;
}

past newAstNode(void)
{
    past node = malloc(sizeof(ast));
    if (node == NULL)
    {
        printf("run out of memory.\n");
        exit(0);
    }
    memset(node, 0, sizeof(ast));
    return node;
}

past newAstNodePar(char *strVal, char *nodeType, past left, past right, past next)
{
    past node = (past)malloc(sizeof(ast));
    if (node == NULL)
    {
        printf("run out of memory.\n");
        exit(0);
    }
    memset(node, 0, sizeof(ast));
    node->svalue = strVal;
    node->nodeType = nodeType;
    node->left = left;
    node->right = right;
    node->next = next;
    return node;
}

past newNum(int value, past next)
{
    past var = newAstNode();
    var->nodeType = "intValue";
    var->ivalue = value;
    var->next = next;
    return var;
}

past newType(int type)
{
    past var = newAstNode();
    var->nodeType = "type";
    switch (type)
    {
    case tok_VOID:
        var->svalue = "VOID";
        break;
    case tok_INT:
        var->svalue = "INT";
        break;
    case tok_CONST:
        var->svalue = "CONST";
        break;
    default:
        break;
    }
    var->left = NULL;
    var->right = NULL;
    var->next = NULL;
}

past newIdent(char *strVal, past next)
{
    past var = newAstNode();
    var->nodeType = "varDecl";
    var->svalue = strVal;
    var->next = next;
    return var;
}

past newExpr(int oper, past left, past right, past next)
{
    past var = newAstNode();
    switch (oper)
    {
    case tok_LESSEQ:
        var->svalue = "<=";
        break;
    case tok_GREATEQ:
        var->svalue = ">=";
        break;
    case tok_NOTEQ:
        var->svalue = "!=";
        break;
    case tok_EQ:
        var->svalue = "==";
        break;
    case tok_AND:
        var->svalue = "&&";
        break;
    case tok_OR:
        var->svalue = "||";
        break;
    case tok_ADD:
        var->svalue = "+";
        break;
    case tok_SUB:
        var->svalue = "-";
        break;
    case tok_MUL:
        var->svalue = "*";
        break;
    case tok_DIV:
        var->svalue = "/";
        break;
    case tok_MODULO:
        var->svalue = "%";
        break;
    case tok_LESS:
        var->svalue = "<";
        break;
    case tok_GREAT:
        var->svalue = ">";
        break;
    case tok_NOT:
        var->svalue = "!";
        break;
    default:
        break;
    }
    var->nodeType = "expr";

    var->left = left;
    var->right = right;
    var->next = next;
    return var;
}

// CompUnit
//	：Decl
//	| FuncDef
//	| CompUnit Decl
//	| CompUnit FuncDef
past CompUnit(void)
{
    past root = newAstNodePar("", "CompUnit", NULL, NULL, NULL);
    past tmp = root;
    if (tok == tok_CONST || tok == tok_VOID || tok == tok_INT)
    {
        if (tok == tok_CONST)
        {
            tmp->left = Decl();
            if (tmp->left == NULL)
                return NULL;
        }
        else if (tok == tok_VOID)
        {
            tmp->left = FuncDef();
            if (tmp->left == NULL)
                return NULL;
        }
        else
        {
            char *s0 = strdup(yytext);
            advance();
            if (tok == tok_ID)
            {
                char *s1 = strdup(yytext);
                advance();
                if (tok == tok_LPAR)
                {
                    yyputback(yytext);
                    yyputback(s1);
                    yyputback(s0);
                    advance();
                    tmp->next = FuncDef();
                    if (tmp->next == NULL)
                        return NULL;
                }
                else
                {
                    yyputback(yytext);
                    yyputback(s1);
                    yyputback(s0);
                    advance();
                    tmp->next = Decl();
                    if (tmp->next == NULL)
                        return NULL;
                }
            }
            else
                return NULL;
        }
    }
    else
        return NULL;
    while (1)
    {
        if (tok == '\n' || tok == EOF || tok == 0)
        {
            return root;
        }
        if (tok == tok_CONST || tok == tok_VOID || tok == tok_INT)
        {
            tmp = tmp->next;
            if (tok == tok_CONST)
            {
                tmp->left = Decl();
                if (tmp->left == NULL)
                    return NULL;
            }
            else if (tok == tok_VOID)
            {
                tmp->left = FuncDef();
                if (tmp->left == NULL)
                    return NULL;
            }
            else
            {
                char *s0 = strdup(yytext);
                advance();
                if (tok == tok_ID)
                {
                    char *s1 = strdup(yytext);
                    advance();
                    if (tok == tok_LPAR)
                    {
                        yyputback(yytext);
                        yyputback(s1);
                        yyputback(s0);
                        advance();
                        tmp->next = FuncDef();
                        if (tmp->next == NULL)
                            return NULL;
                    }
                    else
                    {
                        yyputback(yytext);
                        yyputback(s1);
                        yyputback(s0);
                        advance();
                        tmp->next = Decl();
                        if (tmp->next == NULL)
                            return NULL;
                    }
                }
                else
                    return NULL;
            }
        }
        else
            return NULL;
    }

    return root;
}

// Decl
//	: ConstDecl
//	| VarDecl
past Decl(void)
{
    past root = newAstNodePar("", "decl", NULL, NULL, NULL);
    past tmp = root;
    if (tok == tok_CONST)
    {
        root->left = ConstDecl();
        if (root->left == NULL)
            return NULL;
    }
    else if (tok == tok_INT)
    {
        root->left = VarDecl();
        if (root->left == NULL)
            return NULL;
    }
    else
        return NULL;

    return root;
}

// ConstDecl
//	: 'const' BType ConstDef { ',' ConstDef } ';'
past ConstDecl(void)
{
    past root = newAstNodePar("", "ConstDecl", NULL, NULL, NULL);
    past tmp = root;
    // 'const'
    if (tok == tok_CONST)
    {
        advance();

        tmp->left = BType();
        if (tmp->left == NULL)
            return NULL;
        tmp = tmp->left;

        tmp->left = ConstDef();
        if (tmp->left == NULL)
            return NULL;
        tmp = tmp->left;

        while (tok == tok_COMMA)
        {
            advance();
            tmp->next = ConstDef();
            if (tmp->next == NULL)
                return NULL;
            tmp = tmp->next;
        }

        if (tok == tok_SEMICOLON)
        {
            advance();
            return root;
        }
        else
            return NULL;
    }
    else
        return NULL;
    return root;
}

// BType
//	: 'int'
past BType(void)
{
    past root = NULL;
    if (tok == tok_INT)
    {
        root = newType(tok);
        advance();
    }

    return root;
}

// ConstDef
//	: Ident { '[' ConstExp ']' } '=' ConstInitVal
past ConstDef(void)
{
    past root = newAstNodePar("", "ConstDef", NULL, NULL, NULL);
    past tmp = root;
    // Ident
    if (tok == tok_ID)
    {
        past id = newIdent(strdup(yytext), NULL);
        tmp->left = id;
        tmp = tmp->left;
        advance();
        // { '[' ConstExp ']' }
        while (tok == tok_LSQUARE)
        {
            advance();
            // ConstExp
            tmp->next = ConstExp();
            if (tmp->next == NULL)
                return NULL;
            tmp = tmp->next;
            // ']'
            if (tok == tok_RSQUARE)
            {
                advance();
            }
            else
                return NULL;
        }
        // '='
        if (tok == tok_ASSIGN)
        {
            advance();
            // ConstInitVal
            tmp->left = ConstInitVal();
            if (tmp->left == NULL)
                return NULL;
            tmp = tmp->left;
        }
        else
            return NULL;
    }
    else
        return NULL;

    return root;
}

// ConstInitVal
//	: ConstExp
//	| '{' [ ConstInitVal { ',' ConstInitVal } ] '}'
past ConstInitVal(void)
{
    past list = newAstNodePar("", "list", NULL, NULL, NULL);
    past root = newAstNodePar("", "ConstInitVal", list, NULL, NULL);
    list->ivalue = 0;
    past tmp = list;
    // ConstExp
    if (tok == tok_LBRACKET)
    {
        advance();
        // [ ConstInitVal { ',' ConstInitVal } ]
        tmp->left = ConstInitVal();
        if (tmp->left == NULL)
            return NULL;
        tmp = tmp->left;
        while (tok == tok_COMMA)
        {
            advance();
            tmp->next = ConstInitVal();
            if (tmp->next == NULL)
                return NULL;
            tmp = tmp->next;
        }
        // '}'
        if (tok == tok_RBRACKET)
        {
            advance();
            return root;
        }
        else
            return NULL;
    }
    else
    {
        tmp->ivalue++;
        tmp->left = ConstExp();
        if (tmp->left == NULL)
            return NULL;
    }

    return root;
}

// VarDecl
//	: BType VarDef { ',' VarDef } ';'
past VarDecl(void)
{
    past tmp = NULL;
    if (tok == tok_INT)
    {
        past type = newType(tok);
        past item = newAstNodePar("", "list", NULL, NULL, NULL);
        past root = newAstNodePar("", "varDecl", type, item, NULL);
        root->right->ivalue = 0;
        advance();
        if (tok == tok_ID)
        {
            past var = VarDef();
            item->ivalue++;
            item->left = var;
            tmp = var;
        }
        while (tok == tok_COMMA)
        {
            advance();
            item->ivalue++;
            tmp->next = VarDef();
            tmp = tmp->next;
        }
        if (tok == tok_SEMICOLON)
        {
            advance();
            return root;
        }
        else
            return NULL;
    }
    else
        return NULL;
}

// VarDef
//	: Ident { '[' ConstExp ']' }
//	| Ident { '[' ConstExp ']' } '=' InitVal
past VarDef(void)
{
    past root = NULL;
    if (tok == tok_ID)
    {
        past l = newAstNodePar(strdup(yytext), "varDecl", NULL, NULL, NULL);
        root = newAstNodePar(".", "varDef", l, NULL, NULL);
        advance();
        past list = newAstNodePar("", "list", NULL, NULL, NULL);
        root->left->left = list;
        root->left->left->ivalue = 0;
        past tmp = NULL;
        if (tok == tok_LSQUARE)
        {
            root->left->nodeType = "arrayDecl";
            advance();
            past head = ConstExp();
            root->left->left->left = head;
            root->left->left->ivalue++;
            past tmp = head;
            if (tok == tok_RSQUARE)
            {
                advance();
                while (tok == tok_LSQUARE)
                {
                    advance();
                    past first = ConstExp();
                    tmp->next = first;
                    tmp = tmp->next;
                    root->left->left->ivalue++;
                    if (tok == tok_RSQUARE)
                    {
                        advance();
                    }
                }
                if (tok == tok_ASSIGN)
                {
                    advance();
                    past init = InitVal();
                    root->right = init;
                    return root;
                }
                else
                {
                    return root;
                }
            }
        }
        else if (tok == tok_ASSIGN)
        {
            advance();
            past rightnode = InitVal();
            root->right = rightnode;
            return root;
        }
        else
        {
            return root;
        }
    }
    else
        return NULL;

    return root;
}

// InitVal
//	: Exp
//	| '{' [ InitVal { ',' InitVal } ] '}'
past InitVal(void)
{
    past list = newAstNodePar("", "list", NULL, NULL, NULL);
    past root = newAstNodePar("", "initVal", list, NULL, NULL);
    list->ivalue = 0;
    past tmp = list;
    if (tok == tok_LBRACKET)
    {
        advance();
        // [ InitVal { ',' InitVal } ]
        tmp->left = InitVal();
        if (tmp->left != NULL)
        {
            tmp = tmp->left;
            list->ivalue++;

            // { ',' InitVal }
            while (tok == tok_COMMA)
            {
                advance();
                tmp->next = InitVal();
                if (tmp->next == NULL)
                    return NULL;
                tmp = tmp->next;
                list->ivalue++;
            }
        }

        if (tok == tok_RBRACKET)
        {
            advance();
        }
        else
            return NULL;
    }
    else
    {
        list->ivalue++;
        list->left = Exp();
        if (list->left != NULL)
            return root;
        else
            return NULL;
    }

    return root;
}

// FuncDef
//	: FuncType Ident '(' [FuncFParams] ')' Block
past FuncDef(void)
{
    past root = newAstNode();
    root->nodeType = "funcDef";
    if (tok == tok_VOID || tok == tok_INT)
    {
        past type = FuncType();
        root->left = type;
        if (tok == tok_ID)
        {
            past id = newIdent(strdup(yytext), NULL);
            type->left = id;
            advance();
            if (tok == tok_LPAR)
            {
                advance();
                if (tok != tok_RPAR)
                {
                    past funcFP = FuncFParams();
                    id->left = funcFP;
                    advance();
                }
                else
                {
                    advance();
                }
                past block = Block();
                root->right = block;
                return root;
            }
            else
                return NULL;
        }
        else
            return NULL;
    }
    else
        return NULL;

    return root;
}

// FuncType
//	: 'void'
//	| 'int'
past FuncType(void)
{
    past root = NULL;
    if (tok == tok_VOID || tok == tok_INT)
    {
        root = newType(tok);
        advance();
    }
    else
        return NULL;

    return root;
}

// FuncFParams
//	: FuncFParam { ',' FuncFParam }
past FuncFParams(void)
{
    int list = 0;
    past l = newAstNodePar("", "list", NULL, NULL, NULL);
    past root = newAstNodePar("", "FuncFParams", l, NULL, NULL);
    root->left->ivalue = list;
    past head = FuncFParam();
    root->left->left = head;
    root->left->ivalue++;
    while (tok == tok_COMMA)
    {
        advance();
        root->left->ivalue++;
        head->next = FuncFParam();
        if (head->next == NULL)
            return NULL;

        head = head->next;
    }
    return root;
}

// FuncFParam
//	: BType Ident ['[' ']' { '[' Exp ']' }]
past FuncFParam(void)
{
    int list = 0;
    past leftint = BType();
    past root = NULL;
    if (tok == tok_ID)
    {
        past r = newIdent(strdup(yytext), NULL);
        past root = newAstNodePar("", "FuncFParam", leftint, r, NULL);
        advance();
        past listnode = newAstNodePar("", "list", NULL, NULL, NULL);
        root->right->left = listnode;
        root->right->left->ivalue = list;
        past tmp = NULL;
        if (tok == tok_LSQUARE)
        {
            advance();
            if (tok == tok_RSQUARE)
            {
                past head = newAstNodePar("", "noexp", NULL, NULL, NULL);
                root->right->left->left = head;
                root->right->left->ivalue++;
                tmp = head;
                advance();
                while (tok == tok_LSQUARE)
                {
                    advance();
                    tmp->next = Exp();
                    if (tmp->next == NULL)
                        return NULL;

                    tmp = tmp->next;
                    root->right->left->ivalue++;
                    if (tok == tok_RSQUARE)
                    {
                        advance();
                    }
                }
                return root;
            }
        }
        else
        {
            return root;
        }
    }
    else
        return NULL;

    return root;
}

// Block
//	: '{' { BlockItem } '}'
past Block(void)
{
    past list = newAstNodePar("", "list", NULL, NULL, NULL);
    list->ivalue = 0;
    past block = newAstNodePar("", "blockDecl", NULL, NULL, NULL);
    list->left = block;
    past tmp = block;
    if (tok == tok_LBRACKET)
    {
        advance();
        if (tok != tok_RBRACKET)
        {
            list->ivalue++;
            block->left = BlockItem();
            if (block->left == NULL)
                return NULL;
            tmp = block->left;
        }
        while (tok != tok_RBRACKET)
        {
            list->ivalue++;
            tmp->left = BlockItem();
            if (tmp->left == (void *)(0x10) || tmp->left == NULL)
                return NULL;
            tmp = tmp->left;
        }
    }
    advance();
    return list;
}

// BlockItem
//	: Decl
//	| Stmt
past BlockItem(void)
{
    if (tok == tok_CONST || tok == tok_INT)
    {
        return Decl();
    }
    else
    {
        return Stmt();
    }
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
past Stmt(void)
{
    past root = NULL;
    if (tok == tok_ID)
    {
        char *s0 = strdup(yytext);
        advance();
        yyputback(yytext);
        yyputback(s0);
        if (tok == tok_LPAR)
        {
            advance();
            root = Exp();
            if (root != NULL)
            {
                if (tok == tok_SEMICOLON)
                {
                    advance();
                }
                else
                    return NULL;
            }
            else
                return NULL;
        }
        else
        {
            advance();
            root = LVal();
            if (root != NULL)
            {
                if (tok == tok_ASSIGN)
                {
                    advance();
                    past exp = Exp();
                    past head = newAstNodePar("", "AssignStmt", root, exp, NULL);
                    if (tok == tok_SEMICOLON)
                    {
                        advance();
                        return head;
                    }
                }
                else
                    return NULL;
            }
        }
    }
    else if (tok == tok_IF)
    {
        int list = 1;
        root = newAstNode();
        root->nodeType = "IfStmt";
        root->right = newAstNodePar("", "list", NULL, NULL, NULL);
        root->right->ivalue = list;
        advance();
        if (tok == tok_LPAR)
        {
            advance();
            past cond = Cond();
            root->left = cond;
            if (tok == tok_RPAR)
            {
                advance();
                if (root->right->left = Stmt())
                {
                    if (tok == tok_ELSE)
                    {
                        advance();
                        if (root->right->right = Stmt())
                        {
                            root->right->ivalue++;
                            return root;
                        }
                        else
                            return NULL;
                    }
                    return root;
                }
                else
                    return NULL;
            }
            else
                return NULL;
        }
        else
            return NULL;
    }
    else if (tok == tok_WHILE)
    {
        root = newAstNodePar("", "WhileStmt", NULL, NULL, NULL);
        advance();
        if (tok == tok_LPAR)
        {
            advance();
            past cond = Cond();
            root->left = cond;
            if (tok == tok_RPAR)
            {
                advance();
                if (root->right = Stmt())
                {
                    return root;
                }
                else
                    return NULL;
            }
            else
                return NULL;
        }
        else
            return NULL;
    }
    else if (tok == tok_BREAK)
    {
        root = newAstNodePar("", "BreakStmt", NULL, NULL, NULL);
        advance();
        if (tok == tok_SEMICOLON)
        {
            advance();
            return root;
        }
    }
    else if (tok == tok_CONTINUE)
    {
        root = newAstNodePar("", "ContinueStmt", NULL, NULL, NULL);
        advance();
        if (tok == tok_SEMICOLON)
        {
            advance();
            return root;
        }
    }
    else if (tok == tok_RETURN)
    {
        root = newAstNodePar("", "ReturnStmt", NULL, NULL, NULL);
        advance();

        if (tok != tok_SEMICOLON)
        {
            root->left = Exp();

            if (root->left == NULL)
                return NULL;
        }
        else
        {
            advance();
            return root;
        }

        if (tok == tok_SEMICOLON)
            advance();

        return root;
    }
    else if (tok == tok_LBRACKET)
    {
        return Block();
    }
    else if (tok == tok_SEMICOLON)
    {
        root = newAstNodePar("", "EmptyStmt", NULL, NULL, NULL);
        advance();
        return root;
    }
    else
    {
        past root = newAstNodePar("", "expStmt", NULL, NULL, NULL);
        if (root->left = Exp())
        {
            if (tok == tok_SEMICOLON)
            {
                advance();
                return root;
            }
            return NULL;
        }
        return NULL;
    }
    return root;
}

// Exp: AddExp
//	: AddExp
past Exp()
{
    past a = AddExp();
    return a;
}

// Cond
//	: LOrExp
past Cond(void)
{
    past a = LOrExp();
    return a;
}

// LVal
//	: Ident {'[' Exp ']'}
past LVal(void)
{
    int flag = 1;
    if (tok != tok_ID)
        return NULL;
    past root = newIdent(strdup(yytext), NULL);
    past tmp;
    advance();
    while (tok == tok_LSQUARE)
    {
        advance();
        past exp = Exp();
        if (flag == 1)
        {
            root->left = exp;
            root->nodeType = "arrayDecl";
            tmp = exp;
            flag++;
        }
        else
        {
            tmp->next = exp;
            tmp = tmp->next;
        }
        if (tok == tok_RSQUARE)
            advance();
        else
            return NULL;
    }
    return root;
}

// PrimaryExp
//	：'(' Exp ')'
//	| LVal
//	| Number
past PrimaryExp(void)
{
    if (tok == tok_LPAR)
    {
        advance();
        past pri = Exp();
        if (tok == tok_RPAR)
        {
            advance();
            return pri;
        }
        else
            return NULL;
    }
    else if (tok == tok_ID)
        return LVal();
    else if (tok == tok_INTEGER)
        return Number();

    return NULL;
}

// Number
//	：IntConst
past Number(void)
{
    if (tok == tok_INTEGER)
    {
        past num = newNum(yynum, NULL);
        advance();
        return num;
    }
    return NULL;
}

// UnaryExp
//	：PrimaryExp
//	| Ident '(' [FuncRParams] ')'
//	| UnaryOp UnaryExp
past UnaryExp(void)
{
    past ue = NULL;
    past root = NULL;
    if (tok == tok_ADD || tok == tok_SUB || tok == tok_NOT)
    {
        while (tok == tok_ADD || tok == tok_SUB || tok == tok_NOT)
        {
            int oper = tok;
            advance();
            ue = UnaryExp();
            ue = newExpr(oper, NULL, ue, NULL);
        }
        //       advance();
        return ue;
    }
    else if (tok == tok_ID)
    {
        char *s1 = strdup(yytext);
        ue = newIdent(strdup(yytext), NULL);
        advance();
        if (tok == tok_LPAR)
        {
            advance();
            if (tok == tok_RPAR)
            {
                advance();
                return ue;
            }
            else
            {
                past root = newAstNode();
                root->nodeType = "funcDecl";
                root->left = ue;
                past funcRP = FuncRParams();
                root->right = funcRP;
                if (tok == tok_RPAR)
                {
                    advance();
                    return root;
                }
                else
                    return NULL;
            }
            return NULL;
        }
        // PrimaryExp.LVal
        else
        {
            if (tok == tok_RPAR)
                return ue;
            yyputback(strdup(yytext));
            yyputback(s1);
            advance();
            return LVal();
        }
    }
    else
    {
        root = PrimaryExp();
        return root;
    }

    return root;
}

// FuncRParams
//	: Exp { ',' Exp }
past FuncRParams(void)
{
    int list = 1;
    past root = newAstNode();
    root->nodeType = "list";
    past func = Exp();
    past head = func;
    while (tok == tok_COMMA)
    {
        list++;
        advance();
        past brother = Exp();
        func->next = brother;
        func = func->next;
    }
    root->ivalue = list;
    root->left = head;
    head->nodeType = "parameter";
    head = head->next;
    while (head != NULL)
    {
        head->nodeType = "parameter";
        head = head->next;
    }
    return root;
}

// MulExp
//	: UnaryExp
//	| MulExp ('*' | '/' | '%') UnaryExp
past MulExp(void)
{

    past l = UnaryExp();
    while (tok == tok_MUL || tok == tok_DIV || tok == tok_MODULO)
    {
        int oper = tok;
        advance();
        past r = UnaryExp();
        l = newExpr(oper, l, r, NULL);
    }
    return l;
}

// AddExp
//	: MulExp
//	| AddExp ('+' | '−') MulExp
past AddExp(void)
{

    past l = MulExp();
    while (tok == tok_ADD || tok == tok_SUB)
    {
        int oper = tok;
        advance();
        past r = MulExp();
        l = newExpr(oper, l, r, NULL);
    }
    return l;
}

// RelExp
//	: AddExp
//	| RelExp ('<' | '>' | '<=' | '>=') AddExp
past RelExp(void)
{

    past l = AddExp();
    while (tok == tok_LESS || tok == tok_GREAT || tok == tok_LESSEQ || tok == tok_GREATEQ)
    {
        int oper = tok;
        advance();
        past r = RelExp();
        l = newExpr(oper, l, r, NULL);
    }
    return l;
}

// EqExp
//	: RelExp
//	| EqExp ('==' | '!=') RelExp
past EqExp(void)
{

    past l = RelExp();
    while (tok == tok_EQ || tok == tok_NOTEQ)
    {
        int oper = tok;
        advance();
        past r = RelExp();
        l = newExpr(oper, l, r, NULL);
    }
    return l;
}

// LAndExp
//	: EqExp
//	| LAndExp '&&' EqExp
past LAndExp(void)
{

    past l = EqExp();
    while (tok == tok_AND)
    {
        int oper = tok;
        advance();
        past r = EqExp();
        l = newExpr(oper, l, r, NULL);
    }
    return l;
}

// LOrExp
//	: LAndExp
//	| LOrExp '||' LAndExp
past LOrExp(void)
{

    past l = LAndExp();
    while (tok == tok_OR)
    {
        int oper = tok;
        advance();
        past r = LAndExp();
        l = newExpr(oper, l, r, NULL);
    }
    return l;
}

// ConstExp
//	: AddExp
past ConstExp(void)
{
    past a = AddExp();
    return a;
}

void showAst(past node, int nest)
{
    if (node == NULL)
        return;
    int i = 0;
    for (i = 0; i < nest; i++)
        printf("  ");
    if (strcmp(node->nodeType, "intValue") == 0)
        printf("%s    .    %d\n", node->nodeType, node->ivalue);
    else if (strcmp(node->nodeType, "decl") == 0)
        printf("%s    .    \n", node->nodeType);
    else if (strcmp(node->nodeType, "constDecl") == 0)
        printf("%s    .    \n", node->nodeType);
    else if (strcmp(node->nodeType, "constDef") == 0)
        printf("%s    .    \n", node->nodeType);
    else if (strcmp(node->nodeType, "varDef") == 0)
        printf("%s    .    %s\n", node->nodeType, node->svalue);
    else if (strcmp(node->nodeType, "constInitVal") == 0)
        printf("%s    .    \n", node->nodeType);
    else if (strcmp(node->nodeType, "initVal") == 0)
        printf("%s    .    \n", node->nodeType);
    else if (strcmp(node->nodeType, "funcDef") == 0)
        printf("%s    .    \n", node->nodeType);
    else if (strcmp(node->nodeType, "FuncFParams") == 0)
        printf("%s    .    \n", node->nodeType);
    else if (strcmp(node->nodeType, "FuncFParam") == 0)
        printf("%s    .    \n", node->nodeType);
    else if (strcmp(node->nodeType, "noexp") == 0)
        printf("%s    .    \n", node->nodeType);
    else if (strcmp(node->nodeType, "CompUnit") == 0)
        printf("%s    .    \n", node->nodeType);
    else if (strcmp(node->nodeType, "expr") == 0)
        printf("%s    .    %s\n", node->nodeType, node->svalue);
    else if (strcmp(node->nodeType, "list") == 0)
        printf("%s    .    %d\n", node->nodeType, node->ivalue);
    else if (strcmp(node->nodeType, "varDecl") == 0)
        printf("%s    .    %s\n", node->nodeType, node->svalue);
    else if (strcmp(node->nodeType, "arrayDecl") == 0)
        printf("%s    .    %s\n", node->nodeType, node->svalue);
    else if (strcmp(node->nodeType, "funcDecl") == 0 || strcmp(node->nodeType, "blockDecl") == 0)
        printf("%s    .    \n", node->nodeType);
    else if (strcmp(node->nodeType, "parameter") == 0)
        printf("%s    .    %s\n", node->nodeType, node->svalue);
    else if (strcmp(node->nodeType, "type") == 0)
        printf("%s    .    %s\n", node->nodeType, node->svalue);
    else if (strcmp(node->nodeType, "arrayDecl") == 0)
        printf("%s    .    %s\n", node->nodeType, node->svalue);
    else if (strcmp(node->nodeType, "AssignStmt") == 0 || strcmp(node->nodeType, "IfStmt") == 0 || strcmp(node->nodeType, "WhileStmt") == 0 || strcmp(node->nodeType, "BreakStmt") == 0 || strcmp(node->nodeType, "ContinueStmt") == 0 || strcmp(node->nodeType, "ReturnStmt") == 0 || strcmp(node->nodeType, "EmptyStmt") == 0 || strcmp(node->nodeType, "expStmt") == 0)
        printf("%s    .    \n", node->nodeType);

    showAst(node->left, nest + 1);
    showAst(node->right, nest + 1);
    showAst(node->next, nest);
}
