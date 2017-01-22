#include "stdafx.h"
#include "SimpleParser.h"


SimpleParser::SimpleParser()
{
}


SimpleParser::~SimpleParser()
{
}

std::vector<SimpleToken> SimpleParser::tokenize(std::string str)
{
    auto tokens = std::vector<SimpleToken>();

    SimpleToken curtok;
    bool in_string = false;
    int last_start = 0;
    int strl = str.length();

    #define checklast if (last_start != i - 1) { \
        curtok.strdata = str.substr(last_start + 1, i - last_start - 1); \
        curtok.type = TOKEN_NAME; \
        tokens.push_back(curtok); \
        curtok = SimpleToken(); \
    }

    for (int i = 0; i < strl; i++) {
        auto c = str[i];
        if (!in_string) {
            bool isword = true;
            if (c == '"') {
                last_start = i;
                in_string = true;
            }
            else if (c == '=') {
                checklast
                curtok.type = TOKEN_ASSIGNMENT;
                tokens.push_back(curtok);
                curtok = SimpleToken();
                isword = false;
            }
            else if (c == '+') {
                checklast
                curtok.type = TOKEN_OPERATOR_ADD;
                tokens.push_back(curtok);
                curtok = SimpleToken();
                isword = false;
            }
            else if (c == '-') {
                checklast
                curtok.type = TOKEN_OPERATOR_SUB;
                tokens.push_back(curtok);
                curtok = SimpleToken();
                isword = false;
            }
            else if (c == '*') {
                checklast
                curtok.type = TOKEN_OPERATOR_MUL;
                tokens.push_back(curtok);
                curtok = SimpleToken();
                isword = false;
            }
            else if (c == '^') {
                checklast
                curtok.type = TOKEN_OPERATOR_POW;
                tokens.push_back(curtok);
                curtok = SimpleToken();
                isword = false;
            }
            else if (c == '/') {
                checklast
                curtok.type = TOKEN_OPERATOR_DIV;
                tokens.push_back(curtok);
                curtok = SimpleToken();
                isword = false;
            }
            else if (c == ' ') {
                checklast
                isword = false;
            }
            else if (c == '\r') {
                checklast
                isword = false;
            }
            else if (c == '\n') {
                checklast
                isword = false;
            }
            else if (c == '\t') {
                checklast
                isword = false;
            }
            else if (c == ';') {
                checklast
                curtok.type = TOKEN_SEMICOLON;
                tokens.push_back(curtok);
                curtok = SimpleToken();
                isword = false;
            }
            else if (c == ';') {
                checklast
                curtok.type = TOKEN_SEMICOLON;
                tokens.push_back(curtok);
                curtok = SimpleToken();
                isword = false;
            }
            else if (c == '(') {
                checklast
                curtok.type = TOKEN_BRACE_OPEN;
                tokens.push_back(curtok);
                curtok = SimpleToken();
                isword = false;
            }
            else if (c == ')') {
                checklast
                curtok.type = TOKEN_BRACE_CLOSE;
                tokens.push_back(curtok);
                curtok = SimpleToken();
                isword = false;
            }
            else if (c == '{') {
                checklast
                curtok.type = TOKEN_BRACE_OPEN;
                tokens.push_back(curtok);
                curtok = SimpleToken();
                isword = false;
            }
            else if (c == '}') {
                checklast
                curtok.type = TOKEN_CURLY_CLOSE;
                tokens.push_back(curtok);
                curtok = SimpleToken();
                isword = false;
            }
            else if (c == ',') {
                checklast
                curtok.type = TOKEN_COMMA;
                tokens.push_back(curtok);
                curtok = SimpleToken();
                isword = false;
            }

            if (!isword) last_start = i;
        }
        else {
            if (c == '"') {
                curtok.strdata = str.substr(last_start + 1, i - last_start - 1);
                curtok.type = TOKEN_STRING;
                tokens.push_back(curtok);
                curtok = SimpleToken();
                in_string = false;
            }
        }
    }
    if (last_start != strl - 1) {
        curtok.strdata = str.substr(last_start + 1);
        curtok.type = TOKEN_NAME;
        tokens.push_back(curtok);
        curtok = SimpleToken();
    }
    return tokens;
}
