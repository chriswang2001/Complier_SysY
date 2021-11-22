#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_FILE_LEN 30000 //输入文件最大长度
#define MAX_KEY_LEN 9      //关键字最大长度
typedef enum Token_T
{
    tok_ID = 256, // 标识符
    tok_INTEGER,  // 整数
    tok_INT,      // int
    tok_VOID,     // void
    tok_CONST,    // const
    tok_IF,       // if
    tok_ELSE,     // else
    tok_WHILE,    // while
    tok_BREAK,    // break
    tok_CONTINUE, // continue
    tok_RETURN,   // return
    tok_ADD,      // +
    tok_SUB,      // -
    tok_MUL,      // *
    tok_DIV,      // /
    tok_MODULO,   // %
    tok_LESS,     // <
    tok_LESSEQ,   // <=
    tok_GREAT,    // >
    tok_GREATEQ,  // >=
    tok_NOTEQ,    // !=
    tok_EQ,       // ==
    tok_NOT,      // !
    tok_AND,      // &&
    tok_OR,       // ||
    tok_ASSIGN,   // =
    tok_LPAR,     // (
    tok_RPAR,     // )
    tok_LBRACKET, // {
    tok_RBRACKET, // }
    tok_LSQUARE,  // [
    tok_RSQUARE,  // ]
    tok_COMMA,    // ,
    tok_SEMICOLON // ;
} wordtype;

typedef enum chartype
{
    alpha,
    digit,
    sepchar,
    endchar,
    matchchar,
    whitespace,
} chartype;

typedef struct symbol
{
    wordtype wt;
    char *str;
    int length;
    struct symbol *next;
} symbolNode;

chartype type(char c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')
        return alpha;
    if (c >= '0' && c <= '9')
        return digit;
    if (c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' || c == ',' || c == ';')
        return sepchar;
    if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '|' || c == '&' || c == '<' || c == '>' || c == '=' || c == '!')
        return matchchar;
    if (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r')
        return whitespace;
    if (c == '\0' || c == EOF)
        return endchar;
    return -1;
}

int isChar(char s)
{
    switch (s)
    {
    case '+':
        return tok_ADD;
    case '-':
        return tok_SUB;
    case '*':
        return tok_MUL;
    case '/':
        return tok_DIV;
    case '%':
        return tok_MODULO;
    case '<':
        return tok_LESS;
    case '>':
        return tok_GREAT;
    case '!':
        return tok_NOT;
    case '=':
        return tok_ASSIGN;
    case '(':
        return tok_LPAR;
    case ')':
        return tok_RPAR;
    case '{':
        return tok_LBRACKET;
    case '}':
        return tok_RBRACKET;
    case '[':
        return tok_LSQUARE;
    case ']':
        return tok_RSQUARE;
    case ',':
        return tok_COMMA;
    case ';':
        return tok_SEMICOLON;
    default:
        return 0;
    }
}

int isOperator2(char *str)
{
    if (!strcmp(str, ">="))
        return tok_GREATEQ;
    else if (!strcmp(str, "<="))
        return tok_LESSEQ;
    else if (!strcmp(str, "=="))
        return tok_EQ;
    else if (!strcmp(str, "!="))
        return tok_NOTEQ;
    else if (!strcmp(str, "||"))
        return tok_OR;
    else if (!strcmp(str, "&&"))
        return tok_AND;

    return 0;
}

int isKeyword(char *str)
{
    if (!strcmp(str, "const"))
        return tok_CONST;
    else if (!strcmp(str, "int"))
        return tok_INT;
    else if (!strcmp(str, "void"))
        return tok_VOID;
    else if (!strcmp(str, "if"))
        return tok_IF;
    else if (!strcmp(str, "else"))
        return tok_ELSE;
    else if (!strcmp(str, "while"))
        return tok_WHILE;
    else if (!strcmp(str, "break"))
        return tok_BREAK;
    else if (!strcmp(str, "continue"))
        return tok_CONTINUE;
    else if (!strcmp(str, "return"))
        return tok_RETURN;

    return 0;
}

//预处理函数：去掉注释，返回处理后字符串长度，发生错误返回0
int preProcessing(char *s)
{
    int read = 0, write = 0;

    while (s[read] != '\0')
    {
        if (s[read] == '/')
        {
            if (s[read + 1] == '/')
            {
                read += 2;
                while (s[read] != '\n')
                {
                    if (s[read] == '\0')
                        return 0;
                    read++;
                }
                read++;
                continue;
            }
            else if (s[read + 1] == '*')
            {
                int pre = read + 2;
                if (s[pre] == '\0')
                    return 0;
                read += 3;
                while (!(s[pre] == '*' && s[read] == '/'))
                {
                    if (s[read] == '\0')
                        return 0;
                    pre++;
                    read++;
                }
                read++;
                continue;
            }
        }
        s[write++] = s[read++];
    }

    s[write] = '\0';
    return write;
}

//词法分析器，每运行一次返回一个二元组
symbolNode *lexAnalyzer(char *s)
{
    symbolNode *node = (symbolNode *)malloc(sizeof(symbolNode));

    int i = 0, begin;
    while (type(s[i]) == whitespace)
        i++;
    node->str = &s[i];

    begin = i;
    switch (type(s[i]))
    {
    case alpha:
        while (type(s[i + 1]) == alpha || type(s[i + 1]) == digit)
            i++;
        node->length = i + 1 - begin;

        if (node->length < MAX_KEY_LEN)
        {
            char t[MAX_KEY_LEN];
            strncpy(t, &s[begin], node->length);
            t[node->length] = '\0';

            int wt = 0;
            if (wt = isKeyword(t))
            {
                node->wt = wt;
                break;
            }
        }

        node->wt = tok_ID;
        break;

    case digit:
        while (type(s[i + 1]) == digit)
            i++;
        node->length = i + 1 - begin;
        node->wt = tok_INTEGER;
        break;

    case matchchar:
    {
        char t[3] = {'\0'};
        strncpy(t, &s[begin], 2);
        t[2] = '\0';

        int wt = 0;
        if (wt = isOperator2(t))
        {
            node->length = 2;
            node->wt = wt;
        }
        else
        {
            node->wt = isChar(s[begin]);
            node->length = 1;
        }
        break;
    }

    case sepchar:
        node->wt = isChar(s[begin]);
        node->length = 1;
        break;

    case endchar:
        free(node);
        return NULL;

    default:
        //catch error
        free(node);
        printf("error!\n");
        return NULL;
    }

    return node;
}

void printSymbol(symbolNode *head)
{
    symbolNode *t = head->next;
    while (t != NULL)
    {
        char *str = (char *)malloc(t->length + 1);
        strncpy(str, t->str, t->length);
        str[t->length] = '\0';

        if (t->wt == tok_ID || t->wt == tok_INTEGER)
            printf("%d %s\n", t->wt, str);
        else
            printf("%d\n", t->wt);
        free(str);
        t = t->next;
    }
}

void deleteSymbol(symbolNode *head)
{
    while (head != NULL)
    {
        symbolNode *t = head->next;
        free(head);
        head = t;
    }
}

int main(int argc, char **argv)
{
    char text[MAX_FILE_LEN];
    int i = 0;

    //从标准输入读入，并存储在字符串中
    char ch = getchar();
    while (ch != EOF)
    {
        if (i >= MAX_FILE_LEN)
        {
            printf("file is too big!\n");
            return 1;
        }
        text[i++] = ch;

        ch = getchar();
    }
    text[i] = '\0';

    int length = preProcessing(text);
    if (!length)
        return 2;

    symbolNode *head = (symbolNode *)malloc(sizeof(symbolNode));
    symbolNode *t = head, *pre = head;
    t->str = text;
    t->length = 0;

    while (t != NULL)
    {
        t = lexAnalyzer(t->str + t->length);
        pre->next = t;
        pre = t;
    }

    printSymbol(head);
    deleteSymbol(head);
    return 0;
}