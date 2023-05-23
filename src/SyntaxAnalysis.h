/* Autor: Uroš Jevtić Datum: 12.06.2022. */
#include "LexicalAnalysis.h"
#include "Token.h"
#include "IR.h"
#include <vector>
#include <fstream>

/*Structur that represents labels*/
struct Labels
{
	string name;
	int position;
	
	Labels(string& n, int pos) : name(n), position(pos){}
};

class SyntaxAnalysis
{
public:
	/*Constructor*/
	SyntaxAnalysis(LexicalAnalysis& lex, Instructions& instruction);

	/*Method that does lexical analysis*/
	bool Do();

	/*creates MIPS file*/
	void createMIPS();

	/*Returns a list of register variables*/
	Variables& getRegVariables();

	void printAlocated();

private:
	/*Prints the error message and token that caused the error*/
	void printSyntaxError(Token token);

	/*Prints the information about the token*/
	void printTokenInfo(Token token);


	/**
	* Eats the current token if its type is "t"
	* otherwise reports syntax error
	*
	* param[in] - t - the expected token type
	*/
	void eat(TokenType t);


	/*Returns next token from the token list*/
	Token getNextToken();

	/*Nonterminal symbol Q*/
	void Q();

	/*Nonterminal symbol S*/
	void S();

	/*Nonterminal symbol L*/
	void L();

	/*Nonterminal symbol E*/
	void E();

	/*Referenca na modul leksicke analize*/
	LexicalAnalysis& lexicalAnalysis;

	/*Syntax error indicator*/
	bool errorFound;

	/*Iterator to the token list which represents the output of the lexical analysis*/
	TokenList::iterator tokenIterator;

	/*Current token that is being analyzed*/
	Token currentToken;

	/*List of all instructions*/
	Instructions& instructions;
	/*List of all memory variables*/
	Variables memoryVariables;
	/*List of all register variables*/
	Variables registerVariables;
	/*List of all functions*/
	list<string> functionList;
	/*List of all labels*/
	list<Labels> labelList;


	/*Finds instruction positon*/
	int findInstructionPosition(Variables vars);

	/*Fills memory variable list*/
	void fillMemoryVarList(Token& t1, Token& t2);

	/*Fills register variable list*/
	void fillRegisterVarList(Token& t);

	/*Fills instructions list*/
	void fillFunctionList(string& name, int pos);

	/*Fills label list*/
	void fillLabelList(string& label, int pos);

	/*Register counter*/
	int registerCounter;

	/*Instruction position number*/
	int instructionPosition;

	/*Creates instruction*/
	void createInstruction(InstructionType type, vector<Token>& dst, vector<Token>& src);

	/*Fills list of succesors*/
	void fillSuccesors();

	/*Fills list of predecessors*/
	void fillPredecessor();

	/*Gets register position*/
	int getRegPosition(const std::string& regName);

	/*Returns name of assigned register variable*/
	string returnAssignedReg(string r);
	
	/*Checks if memory variable is defined*/
	bool isMemDefined(Token t);

	/*Checks if register is defined*/
	bool isRegDefined(Token t);

};