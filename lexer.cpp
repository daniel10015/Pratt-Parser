#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include "lexer.h"

using std::string; 
using std::vector;
using std::cout;
using std::endl;

// these GetChar(char) func will get modified so probably all of the input 
// gets passed into vector as an std::stack (instead of a vector)
bool InputBuffer::EndOfInput()
{
	if(!input_buffer.empty())
		return false;
	return std::cin.eof();
}

char InputBuffer::UngetChar(char c)
{
	if(c != EOF)
		input_buffer.push_back(c);
	return c;
}

// get char from standard input, else get it from the buffer
void InputBuffer::GetChar(char& c)
{
	if(input_buffer.empty())
	{
		std::cin.get(c);
		#ifdef DEBUG_LEXER
			cout << "char inbuf (from stream): " << c << endl;
		#endif
		return;
	}
	c = input_buffer.back();
	input_buffer.pop_back();

	#ifdef DEBUG_LEXER
	cout << "char inbuf (from vec): " << c << endl;
	#endif
}

string InputBuffer::UngetString(string s)
{
	for(int i=0; i<static_cast<int>(s.size()); i++)
	{
		input_buffer.push_back(s[s.size()-i-1]);
	}
	return s;
}

void Token::Print()
{
	 cout << "{" << this->lexeme << " , "
         << static_cast<int>(this->token_type) << " , "
         << this->line_no << "}\n";
}

LexicalAnalyzer::LexicalAnalyzer()
{
	this->line_no = 1;
	tmp.lexeme = "";
	tmp.line_no = 1;
	tmp.token_type = ERROR;
	Token tok = GetTokenMain();
	index = 0;
	while(tok.token_type!= END_OF_FILE)
	{
		tokenList.push_back(tok);
		tok = GetTokenMain();
	}
	// END_OF_FILE doesn't get pushed into the list
	#ifdef DEBUG_LEXER
		for( Token tok : tokenList)
		{
			cout << tok.lexeme << endl;
		}
	#endif
}

bool LexicalAnalyzer::SkipSpace()
{
	char c;
	input.GetChar(c);
	bool space_encountered = false;
	while(!input.EndOfInput() && isspace(c))
	{
		space_encountered = true;
		input.GetChar(c);
	}
	if(!input.EndOfInput())
	{
		#ifdef DEBUG_LEXER
			cout << "skipspace: unget\n";
		#endif
		input.UngetChar(c);
	}
	return space_encountered;
}

Token LexicalAnalyzer::ScanNumber()
{
	char c;

	input.GetChar(c);
	if(isdigit(c))
	{
		if(c=='0')
			tmp.lexeme = "0";
		else
		{
			tmp.lexeme="";
			while(!input.EndOfInput() && isdigit(c))
			{
				tmp.lexeme+=c;
				input.GetChar(c);
				#ifdef DEBUG_LEXER
				cout << "ScanNumber() digit candidiate: " << c << endl;
				#endif
			}
			if(!input.EndOfInput())
				input.UngetChar(c);
		}
		tmp.token_type = NUM;
		tmp.line_no = line_no;
		return tmp;
	}
	else
	{
		if(!input.EndOfInput())
			input.UngetChar(c);
		tmp.lexeme="";
		tmp.token_type = ERROR;
		tmp.line_no = line_no;
		return tmp;
	}
}

Token LexicalAnalyzer::GetToken()
{
	Token tok;
	if(index>=static_cast<int>(tokenList.size()))
	{
		tok.lexeme = "";
		tok.line_no = line_no;
		tok.token_type = END_OF_FILE;
	}
	else
	{
		tok = tokenList[index];
		index++;
	}
	return tok;
}

// scan ID or keyword (this was rewritten to handle keywords ie. trig/exp functions)
Token LexicalAnalyzer::ScanId()
{
	char c;
	input.GetChar(c);
	if(isalpha(c))
	{
		tmp.lexeme="";
		while(!input.EndOfInput() && isalnum(c))
		{
			tmp.lexeme+=c;
			input.GetChar(c);
		}
		if(!input.EndOfInput())
			input.UngetChar(c);
		tmp.line_no = line_no;
		int keywordIndex = FindKeywordIndex(tmp.lexeme);
		if(keywordIndex != -1)
			tmp.token_type = (TokenType)keywordIndex;
		else
			tmp.token_type = ID;
	}	
	else
	{
		if(!input.EndOfInput())
			input.UngetChar(c);
		tmp.lexeme="";
		tmp.token_type=ERROR;
	}
	return tmp;
}

int LexicalAnalyzer::FindKeywordIndex(string s)
{
    string keyword[] = { "exp", "sin", "cos", "tan", "arcsin", "arccos", "arctan" };
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return i + 1;
        }
    }
    return -1;
}

// hf --> how far
Token LexicalAnalyzer::peek(int hf)
{
	if(hf <= 0) 
	{
		cout << "error: peeking with <= argument)\n";
		exit(1);
	}

	int peekIndex = index + hf-1;
	if(peekIndex > static_cast<int>(tokenList.size()))
	{
		Token tok;
		tok.lexeme = "";
		tok.line_no = line_no;
		tok.token_type = END_OF_FILE;
		return tok;
	}
	else
		return tokenList[peekIndex];
}

Token LexicalAnalyzer::GetTokenMain()
{
	char c;

	SkipSpace();
	// default eof
	tmp.lexeme = "";
	tmp.line_no = line_no;
	tmp.token_type = END_OF_FILE;
	if(!input.EndOfInput())
		input.GetChar(c);
	else
		return tmp;
	#ifdef DEBUG_LEXER
	cout << "GetTokenMain() char: " << c << endl;
	#endif
	switch(c)
	{
		case '+':   tmp.token_type = PLUS;   
		#ifdef DEBUG_LEXER
		cout << "LEXER:= [+]\n";
		#endif
		   return tmp;
        	case '-':   tmp.token_type = MINUS;     return tmp;
        	case '/':   tmp.token_type = DIV;       return tmp;
        	case '*':   tmp.token_type = MULT;      return tmp;
        	case '=':   tmp.token_type = EQUAL;     return tmp;
        	case ',':   tmp.token_type = COMMA;     return tmp;
        	case '[':   tmp.token_type = LBRAC;     return tmp;
        	case ']':   tmp.token_type = RBRAC;     return tmp;
        	case '(':   tmp.token_type = LPAREN;    
					#ifdef DEBUG_LEXER
					cout << "LEXER:= [(]\n";
					#endif
					return tmp;
        	case ')':   tmp.token_type = RPAREN;    return tmp;
        	case '>':   tmp.token_type = GREATER;   return tmp;
        	case '<':
            		input.GetChar(c);
            		if (c == '>') 
								{
                		tmp.token_type = NOT_EQUAL;
            		} 
							else 
							{
                		if (!input.EndOfInput()) 
										{
                    			input.UngetChar(c);
                		}	
                		tmp.token_type = LESS;
            	}
            	return tmp;
		default:
			if(isdigit(c))
			{
				#ifdef DEBUG_LEXER
					cout << c << " is digit\n";
				#endif

				input.UngetChar(c);
				return ScanNumber();
			}	
			else if(isalpha(c))
			{
				input.UngetChar(c);
				return ScanId();
			}
			else if(input.EndOfInput())
				tmp.token_type = END_OF_FILE;
			else
				tmp.token_type = ERROR;
			return tmp;
	}
}
