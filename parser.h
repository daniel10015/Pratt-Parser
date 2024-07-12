#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <unordered_map>
#include <string>
#include <queue>
#include <pair>

using std::string;


// a VAR can either be a variable (ie, x) or a function previously defined in a block (which'll append the ast in place) 
/*
 Parser -> block
 block  -> expr
 block  -> decl EQUAL expr
 decl -> ID LPAREN var-list RPAREN
 var-list -> VAR
 var->list -> VAR COMMA var-list
 expr -> expr PLUS expr
 expr -> expr MINUS expr
 expr -> expr MULT expr
 expr -> expr DIV expr
 expr -> sin LPAREN expr RPAREN
 expr -> cos LPAREN expr RPAREN
 expr -> tan LPAREN expr RPAREN
 expr -> arcsin LPAREN expr RPAREN
 expr -> arccos LPAREN expr RPAREN
 expr -> arctan LPAREN expr RPAREN
 expr -> exp LPAREN expr RPAREN
 expr -> exp LPAREN expr COMMA expr RPAREN
 expr -> ...all trig and exp
 ...
 expr -> ID
 ID -> VAR | NUM
*/

enum bin_op
{
	ERROR_BIN_OP=-1, NO_OP, ADD_OP, SUB_OP, MULT_OP, DIV_OP,
};

enum unary_op 
{
	ERROR_UN_OP=-1, EXP_OP, SIN_OP, COS_OP, TAN_OP, 
	ARCSIN_OP, ARCCOS_OP, ARCTAN_OP, MINUS_OP,
	NUM_OP, ID_OP, 
};

enum node_type
{
	BINARY_OP=0, PREFIX_OP,
};

/*
typedef struct tag_tree_node tree_node;

struct binary_op // ie: a+b
{
	tree_node* lhs = nullptr;
	tree_node* rhs = nullptr;
	bin_op op_type = NO_OP;
};

struct prefix_op // ie: exp(5)
{
	string lexeme;
	unary_op op;
};

typedef union node_data
	{
		struct binary_op; // ie: a+b
		struct prefix_op; // ie: exp(5)
	} node_data;
*/

typedef struct tag_tree_node 
{
	node_type type;
	union 
	{
		struct // ie: a+b
		{
		tag_tree_node* lhs = nullptr;
		tag_tree_node* rhs = nullptr;
		bin_op op_type = NO_OP;
		} binary_op;

		struct // ie: exp(5)
		{
			tag_tree_node* next = nullptr;
			string lexeme;
			unary_op op;
		} prefix_op;
	};	
	tag_tree_node() {}
} tree_node;


class parser
{
public:
	parser();
	tree_node* parse();
	int GetLineNo();
	tree_node* GetRoot();

	// extras
	void PrintTokens();
	void PrintBFS(tree_node*);
	const std::vector<string>& GetFunctionName();
private:
	std::unordered_map<TokenType, int8_t> infix_precedence; // consider making these two variables static
	std::unordered_map<TokenType, int8_t> prefix_precedence;
	LexicalAnalyzer lex;
	tree_node* root_node = nullptr;
	int line_no = 1;
	std::vector<string> function_name;

	// parsing handlers
	Token expect(TokenType);
	void syntaxError();
	// utilities
	int8_t getInfixPrecedence(TokenType);
	int8_t getPrefixPrecedence(TokenType);
	bool is_identifier(TokenType);
	unary_op GetPrefixOp(TokenType);
	bin_op GetBinOp(TokenType);
	unary_op GetUnaryOp(TokenType);
	// rest of grammar for rec desc goes here...
	tree_node* parse_block();
	tree_node* parse_decl();
	tree_node* parse_varList();
	tree_node* parse_expr(int8_t prePrecedence);
	tree_node* parse_prefix();
};

class type_check
{
public:
	type_check() = delete;
	type_check(tree_node* node);
private:
	tree_node* root = nullptr;
	std::pair<bool, string> function = {false, ""};

private:
	std::pair<bool, string> isFunction();
};

#endif // PARSER_H
