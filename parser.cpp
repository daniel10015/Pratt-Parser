#include <iostream>
#include "parser.h"
#include "lexer.h"

using std::cout;
using std::endl;

void parser::syntaxError()
{
	cout << "syntax error\n";
	exit(1);
}

Token parser::expect(TokenType tt)
{
	Token t1 = lex.GetToken();
	if(t1.token_type != tt)
	{
		#ifdef DEBUG_PARSER
		cout << "expected " << tt << ", but got " << t1.token_type << endl;
		#endif
		syntaxError(); // add calls here
		return {0};
	}
	return t1;
}

int parser::GetLineNo() { return this->line_no; }

tree_node* parser::GetRoot() { return this->root_node; }

parser::parser()
{
	infix_precedence[END_OF_FILE] = -1;
	infix_precedence[VAR] = 0;
	infix_precedence[NUM] = 0;
	infix_precedence[PLUS] = 2;
	infix_precedence[MINUS] = 2;
	infix_precedence[MULT] = 4;
	infix_precedence[DIV] = 4;
	infix_precedence[EXP] = 5;
	infix_precedence[LPAREN] = 6;
	infix_precedence[RPAREN] = 0;
	infix_precedence[LBRAC] = 6;
	infix_precedence[RBRAC] = 0;
	infix_precedence[SIN] = 0;
	infix_precedence[COS] = 0;
	infix_precedence[TAN] = 0;
	infix_precedence[ARCSIN] = 0;
	infix_precedence[ARCCOS] = 0;
	infix_precedence[ARCTAN] = 0;
	// END_OF_FILE = 0, 
	// EXP, SIN, COS, TAN, ARCSIN, ARCCOS, ARCTAN, 
	// NUM, ID, VAR, EQUAL, NOT_EQUAL, PLUS, MINUS, 
	// MULT, DIV, COMMA, LPAREN, RPAREN, LBRAC, RBRAC, LESS, GREATER, 
	// ERROR
	prefix_precedence[END_OF_FILE] = -1;
	prefix_precedence[VAR] = 0;
	prefix_precedence[NUM] = 0;
	prefix_precedence[PLUS] = 2;
	prefix_precedence[MINUS] = 2;
	prefix_precedence[MULT] = 4;
	prefix_precedence[DIV] = 4;
	prefix_precedence[EXP] = 5;
	prefix_precedence[LPAREN] = prefix_precedence.at(MULT);
	prefix_precedence[RPAREN] = 0;
	prefix_precedence[LBRAC] = prefix_precedence.at(MULT);
	prefix_precedence[RBRAC] = 0;
	prefix_precedence[SIN] = 0;
	prefix_precedence[COS] = 0;
	prefix_precedence[TAN] = 0;
	prefix_precedence[ARCSIN] = 0;
	prefix_precedence[ARCCOS] = 0;
	prefix_precedence[ARCTAN] = 0;
}

void parser::PrintTokens()
{
	Token t;
	t=lex.GetToken();
	while(t.token_type != END_OF_FILE)
	{
		t.Print();
		t=lex.GetToken();
	}
}

int8_t parser::getInfixPrecedence(TokenType t) 
{
	if( infix_precedence.find(t) != infix_precedence.end() )
		return  infix_precedence.at(t);
	return -1;
}

int8_t parser::getPrefixPrecedence(TokenType t)
{
	if( prefix_precedence.find(t) != prefix_precedence.end() )
		return  prefix_precedence.at(t);
	return -1;
}

unary_op parser::GetUnaryOp(TokenType t)
{
	switch(t)
	{
		case EXP:
			return EXP_OP;
		case SIN:
			return SIN_OP;
		case COS:
			return COS_OP;
		case TAN:
			return TAN_OP;
		case ARCSIN:
			return ARCSIN_OP;
		case ARCCOS:
			return ARCCOS_OP;
		case ARCTAN:
			return ARCTAN_OP;
		default:
			return ERROR_UN_OP;
	}
}

bool parser::is_identifier(TokenType t)
{
	switch(t)
	{
		//case EXP:
		//case SIN:
		//case COS:
		//case TAN:
		//case ARCSIN:
		//case ARCCOS:
		//case ARCTAN:
		case NUM:
		case ID:
			return true;
		default:
			return false;
	}
}

unary_op parser::GetPrefixOp(TokenType t)
{
	switch(t)
	{
		case EXP:
			return EXP_OP;
		case SIN:
			return SIN_OP;
		case COS:
			return COS_OP;
		case TAN:
			return TAN_OP;
		case ARCSIN:
			return ARCSIN_OP;
		case ARCCOS:
			return ARCCOS_OP;
		case ARCTAN:
			return ARCTAN_OP;
		case MINUS:
			return MINUS_OP;
		case NUM:
			return NUM_OP;
		case ID:
			return ID_OP;
		default:
			return ERROR_UN_OP;
	}
}

bin_op parser::GetBinOp(TokenType t)
{
	switch(t)
	{
	  case PLUS:
			return ADD_OP;
	  case MINUS:
			return SUB_OP;
	  case MULT:
			return MULT_OP;
	  case DIV:
			return DIV_OP;
		default:
			return ERROR_BIN_OP;
	}
}

tree_node* parser::parse()
{
	root_node = parse_block();
	return root_node;
}

tree_node* parser::parse_block()
{
	TokenType t1 = lex.peek(1).token_type; // ID
	TokenType t2 = lex.peek(2).token_type; // LPAREN
	TokenType t3 = lex.peek(3).token_type; // VAR
	TokenType t4 = lex.peek(4).token_type; // COMMA (or) EQUAL 
	TokenType t5 = lex.peek(5).token_type;
	if(t1==ID && t2==LPAREN && t3==ID)
	{
		if(t4==COMMA || (t4==RPAREN&&t5==EQUAL))
		{
			parse_decl();
			expect(EQUAL);
		}
	}
	return parse_expr(0);
}

// function declaration section
tree_node* parser::parse_decl()
{
	function_name.push_back( expect(ID).lexeme ); // function name
	expect(LPAREN);
	parse_varList();
	expect(RPAREN);
	return nullptr;
}

tree_node* parser::parse_varList()
{
	function_name.push_back( expect(ID).lexeme );
	if(lex.peek(1).token_type==COMMA)
		parse_varList();
	return nullptr;
}

// pratt parsing
// first call should terminate on EOF
tree_node* parser::parse_expr(int8_t precedence)
{
	tree_node* left = parse_prefix(); // becomes possible LHS
	tree_node* temp;
	Token op;
	#ifdef DEBUG_PARSER
	cout << "parse expr: " << lex.peek(1).lexeme << endl; 
	#endif
	while( getInfixPrecedence( lex.peek(1).token_type ) > precedence ) 
	{
			#ifdef DEBUG_PARSER
		cout << "higher precedence, recursively call expr" << endl; 
		#endif
		op = lex.GetToken();
		temp = left;
		left = new tree_node{}; //{ BINARY_OP, left, parse_expr( getInfixPrecedence(op.token_type) ), GetBinOp(op.token_type) }; // lhs, rhs, op
		left->type = BINARY_OP;
		left->binary_op.lhs = temp; // previous left
		left->binary_op.rhs = parse_expr( getInfixPrecedence(op.token_type) );
		left->binary_op.op_type = GetBinOp(op.token_type);
	}
	return left;
}

tree_node* parser::parse_prefix()
{
	Token t1 = lex.GetToken();
	#ifdef DEBUG_PARSER
	cout << "parse prefix: " << t1.lexeme << " " << t1.token_type << endl;
	#endif
	if(t1.token_type==END_OF_FILE) return nullptr;
	if( is_identifier(t1.token_type) )
	{
		#ifdef DEBUG_PARSER
		cout << "is identifier\n";
		#endif
		tree_node* node = new tree_node{};// { PREFIX_OP, nullptr, nullptr, static_cast<bin_op>(0)}; // union cheese jazz club
		node->type = PREFIX_OP;
		node->prefix_op.lexeme = t1.lexeme;
		node->prefix_op.op = GetPrefixOp( t1.token_type );
		node->prefix_op.next = nullptr;
		#ifdef DEBUG_PARSER
		cout << "next token type: " << lex.peek(1).token_type << "\n";
		#endif
		return node; 
	}
	else
	{
		#ifdef DEBUG_PARSER
		cout << "is NOT identifier\n";
		#endif
		int8_t precedence = getPrefixPrecedence(t1.token_type);
		if(precedence!=-1)
		{
			if(t1.token_type == LPAREN || t1.token_type == RPAREN)
				return parse_expr( 0 ); // reset precedence
			else
			{
				tree_node* temp_tn = new tree_node{}; // {PREFIX_OP, nullptr, nullptr, static_cast<bin_op>(0)};
				temp_tn->type = PREFIX_OP;
				temp_tn->prefix_op.op = GetUnaryOp(t1.token_type);
				temp_tn->prefix_op.next = parse_expr( 0 );
				return temp_tn;
			}
		}
		else 
		{
			#ifdef DEBUG_PARSER
			cout << "parse_prefix syntax error\n" << t1.token_type << endl << (int)precedence << endl;
			#endif
			syntaxError();
		}
	}
	return nullptr; // program never reaches here, it just stops the compiler warning
}

void parser::PrintBFS(tree_node* node)
{
	std::queue<tree_node*> q;
	q.push(node);
	tree_node* n = nullptr;
	#ifdef DEBUG_PARSER
	cout << "printbfs\n";
	#endif 

	while(q.front() != nullptr)
	{
		// print top of queue
		n = q.front();
		if(n->type == BINARY_OP)
			cout << n->binary_op.op_type << endl;
		else
			cout << n->prefix_op.lexeme << " " << n->prefix_op.op << endl;
		q.pop();

		// recursive call
		if(n->type==BINARY_OP)
		{
			q.push( n->binary_op.lhs );
			q.push( n->binary_op.rhs );
		}
		else if(n->prefix_op.next!=nullptr)
		{
			q.push( n->prefix_op.next );
		}
	}

/*
	while(!q.empty())
	{
		if(q.front()->type == BINARY_OP)
			cout << q.front()->binary_op.op_type << endl;
		else
			cout << q.front()->prefix_op.lexeme << " " << q.front()->prefix_op.op << endl;
		q.pop();
	}*/
}

// make this std::optional
const std::vector<string>& parser::GetFunctionName()
{
	if( function_name.empty() )
		return std::vector<string>();
	return function_name;
}

// for testing purposes
int main(void)
{
	cout << "usage: ./a.out < input.txt\n\n";
	
	parser p;
	//p.parse();
	p.PrintBFS( p.parse() );
	//p.PrintTokens();
	return 0;
}

// type checking class
type_check::type_check(tree_node* node)
	: root(node)
	{}

std::pair<bool, string> type_check::isFunction()
{
	//if(root->type == BINARY_OP && root->)
	return std::make_pair(false, "");
}
