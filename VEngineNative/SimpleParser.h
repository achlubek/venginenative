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
#define TOKEN_ASSIGNMENT 2
#define TOKEN_OPERATOR_ADD 3
#define TOKEN_OPERATOR_SUB 4
#define TOKEN_OPERATOR_MUL 5
#define TOKEN_OPERATOR_DIV 6
#define TOKEN_OPERATOR_POW 7
#define TOKEN_STRING 8
#define TOKEN_SEMICOLON 9
#define TOKEN_BRACE_OPEN 10
#define TOKEN_BRACE_CLOSE 11
#define TOKEN_CURLY_OPEN 12
#define TOKEN_CURLY_CLOSE 13
#define TOKEN_COMMA 14

public:
    SimpleParser();
    ~SimpleParser();
    std::vector<SimpleToken> tokenize(std::string str);
};

