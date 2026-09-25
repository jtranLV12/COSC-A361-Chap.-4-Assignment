#include <iostream>
#include <fstream>
#include "toyTokenizer.cpp"
using namespace std;

/******************************************************/
/* main driver */
int main(int argc, char** argv) {
	if (argc < 2) {
		cout << "ERROR - missing file name\n";
		return 1;
	}
	FILE *fp = fopen(argv[1], "r");
	if (!fp) {
		cout << "ERROR - cannot open file " << argv[1] << "\n";
		return 1;
	}
	Tokenizer tk;
	tkInit(&tk, fp);

	Token tokens[MAX_TOKENS];
	int tokenCount = tokenize(&tk, tokens, MAX_TOKENS);

	fclose(fp);

	cout << "Tokens:\n";
	for (int i = 0; i < tokenCount; i++) {
		const Token* t = &tokens[i];
		cout << "  Token(type=" << tokenTypeName(t->tokenType) << ", lexeme='" << t->lexeme << "', " << t->line << ":" << t->column << ")\n";
	}
	return 0;
}
