#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include "toyTokenizer.cpp"
using namespace std;

// =====================================================================
// PARSER  — recursive descent following the BNF
// =====================================================================
struct Parser {
    Token*    tokens;
    int       count;
    int       pos;
};

const Token* prCurrent(const Parser &p) { return &p.tokens[p.pos]; }
bool prCheck(const Parser &p, TokenType t) { return prCurrent(p)->tokenType == t; }

void prAdvance(Parser &p) {
    p.pos++;
}

bool prMatch(Parser &p, TokenType t) {
    if (prCheck(p, t)) { prAdvance(p); return true; }
    return false;
}

void prError(const Parser &p, string msg) {
    const Token* t = prCurrent(p);
	cerr << "Parser error at line " << t->line << ": column " << t->column << ": " << msg << " (got " << tokenTypeName(t->tokenType) << ")\n";
    exit(1);
}

const Token* prExpect(Parser &p, TokenType t, string what) {
    if (!prCheck(p, t)) {
		prError(p, "expected " + what + " (" + tokenTypeName(t) + ")");
    }
    const Token* tok = prCurrent(p);
    //prAdvance(p);
    return tok;
}

// -------- Forward decl --------
void parseExpression(Parser &p);

// -------- Terminals --------
void parseVariable(Parser &p) {
    prAdvance(p);
}

void parseNumber(Parser &p) {
    prAdvance(p);
}

// -------- <factor> -> <var> | <const> | ( <expr> ) --------
void parseFactor(Parser &p) {
    if (prCheck(p, TOK_LPAREN)) {
		prAdvance(p);
        parseExpression(p);
        prExpect(p, TOK_RPAREN, ")");
        return;
    }
    if (prCheck(p, TOK_NUMBER)) {
		prAdvance(p);
		return; 
	}
    if (prCheck(p, TOK_VAR)) {
        parseVariable(p);
		return;
    }
    prError(p, "expected factor");
    return;  // unreachable
}

// -------- <term> -> <factor> { (* | /) <factor> } --------
int parseTerm(Parser &p) {
	while (1) {
		parseFactor(p);
        if (prCheck(p, TOK_STAR) || prCheck(p, TOK_SLASH)) prAdvance(p);
        else break;
	}
}

// -------- <expr> -> <term> { (+ | -) <term> } --------
void parseExpression(Parser &p) {
	while (1) {
		parseTerm(p);
        if (prCheck(p, TOK_PLUS) || prCheck(p, TOK_MINUS)) prAdvance(p);
        else break;
	} 
}

// -------- <declaration> -> int <var>{, <var>} --------
void parseDeclaration(Parser& p) {
	do {
		prAdvance(p);
	
    	parseVariable(p);
		if (prCheck(p, TOK_ASSIGN)) {
            prAdvance(p);
			parseExpression(p);
		}
	} while (prCheck(p, TOK_COMMA));
}

// -------- <assign> -> <var> = <expr> --------
void parseAssignment(Parser &p) {
    if (prCheck(p, TOK_VAR)) parseVariable(p);
    else if (prCheck(p, TOK_NUMBER)) parseNumber(p);
    else prError(p, "Variable or Number expected");
    prExpect(p, TOK_ASSIGN, "=");
    prAdvance(p);
    parseExpression(p);
}

// -------- <stmt> -> <assign> | <declaration> --------
void parseStatement(Parser &p) {
    if (prCheck(p, TOK_INT))      parseDeclaration(p);
    else if (prCheck(p, TOK_VAR)) parseAssignment(p);
    else                          prError(p, "expected statement start with either int or identifier");
}

// -------- <program> -> <stmt>; {<stmt>;} --------
void parseProgram(Parser &p) {
	while (1) {
		parseStatement(p);
		//cout << "Expecting semicolon found: " << tokenTypeName(prCurrent(p)->tokenType) << " pos=" << p.pos << "\n";
		//prExpect(p, TOK_SEMI, "';'");
		if (prCheck(p, TOK_EOF)) break;
	}
	
}

// =====================================================================
// MAIN
// =====================================================================
int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <source-file>\n", argv[0]);
        return 1;
    }

    FILE* fp = fopen(argv[1], "r");
    if (!fp) {
        fprintf(stderr, "Cannot open file: %s\n", argv[1]);
        return 1;
    }
    // ---- Tokenize ----
    Tokenizer tk;
    tkInit(&tk, fp);

    Token tokens[MAX_TOKENS];
    int tokenCount = tokenize(&tk, tokens, MAX_TOKENS);

    fclose(fp);

	Parser parser;
	parser.tokens = tokens;
	parser.pos    = 0;

	parseProgram(parser);
	cout << "No errors found\n";

    return 0;
}
