#pragma once

struct SimpleToken {
public:
    int type;
    int idata;
    string strdata;
};

class SimpleParser
{
#define TOKEN_NAME 1
#define TOKEN_INTEGER 2
#define TOKEN_FLOAT 3
#define TOKEN_ASSIGNMENT 4
#define TOKEN_OPERATOR_ADD 5
#define TOKEN_OPERATOR_SUB 6
#define TOKEN_OPERATOR_MUL 7
#define TOKEN_OPERATOR_DIV 8
#define TOKEN_OPERATOR_POW 9
#define TOKEN_STRING 10
#define TOKEN_SEMICOLON 11
#define TOKEN_BRACE_OPEN 12
#define TOKEN_BRACE_CLOSE 13
#define TOKEN_CURLY_OPEN 14
#define TOKEN_CURLY_CLOSE 15
#define TOKEN_COMMA 16

public:
    SimpleParser();
    ~SimpleParser();
    std::vector<SimpleToken> tokenize(std::string str);
};

