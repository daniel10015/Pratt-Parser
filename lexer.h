#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include <string>

class InputBuffer
{
public:
	void GetChar(char&);
	char UngetChar(char);
	std::string UngetString(std::string);
	bool EndOfInput();
private:
	std::vector<char> input_buffer;
};

#define KEYWORDS_COUNT 7
typedef enum 
{
	END_OF_FILE = 0, 
	EXP, SIN, COS, TAN, ARCSIN, ARCCOS, ARCTAN, // append more keywords HERE and don't forget to update KEYWORD_COUNT
	NUM, ID, VAR, EQUAL, NOT_EQUAL, PLUS, MINUS, 
	MULT, DIV, COMMA, LPAREN, RPAREN, LBRAC, RBRAC, LESS, GREATER, 
	ERROR
} TokenType;

class Token 
{
public:
	void Print(); // print tok info to stdout
	std::string lexeme;
	TokenType token_type;
	int line_no; // number in the input		
};

class LexicalAnalyzer
{
public:
	Token GetToken();
	Token peek(int);
	LexicalAnalyzer();
private:
	std::vector<Token> tokenList;
	Token GetTokenMain();
	int line_no;
	int index;
	Token tmp;
	InputBuffer input;

	bool SkipSpace(); // remove whitespace
	int FindKeywordIndex(std::string);
	Token ScanId();
	Token ScanNumber(); // is it ID or number
};

#endif // LEXER_H
