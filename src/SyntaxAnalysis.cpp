/* Autor: Uroš Jevtić Datum: 16.06.2022. */
#include <iostream>
#include <iomanip>

#include "SyntaxAnalysis.h"
#include "IR.h"
#include <List>
#include "LivenesAnalysis.h"
#include "InterferenceGraph.h"

using namespace std;

SyntaxAnalysis::SyntaxAnalysis(LexicalAnalysis& lex, Instructions& inst)
	: lexicalAnalysis(lex), errorFound(false), tokenIterator(lexicalAnalysis.getTokenList().begin()), instructions(inst), instructionPosition(0), registerCounter(0) {}

/*
	Does lexical and syntax analysis, fills the list of instruction successors and predecesors
*/
bool SyntaxAnalysis::Do()
{
	currentToken = getNextToken();
	Q();

	/*Fills the instructions succesors and predecessor list*/
	fillSuccesors();
	fillPredecessor();

	return !errorFound;
}

void SyntaxAnalysis::printSyntaxError(Token token)
{
	cout << "Syntax error! Token: " << token.getValue() << " unexpected" << endl;
}

void SyntaxAnalysis::printTokenInfo(Token token)
{
	cout << setw(20) << left << token.getType();
	cout << setw(25) << right << token.getValue() << endl;
}


/**
* Eats the current token if its type is "t"
* otherwise reports syntax error
*
* param[in] - t - the expected token type
*/
void SyntaxAnalysis::eat(TokenType t)
{
	if (errorFound == false)
	{
		if (currentToken.getType() == t) 
		{
			cout << currentToken.getValue() << endl;
			if (t == T_END_OF_FILE)
				return;
			currentToken = getNextToken();
		}
		else
		{
			printSyntaxError(currentToken);
			errorFound = true;
		}
	}
}


Token SyntaxAnalysis::getNextToken()
{
	if (tokenIterator == lexicalAnalysis.getTokenList().end())
		throw runtime_error("End of input file reached");
	return *tokenIterator++;
}
/*Fills the list with all memory variables*/
void SyntaxAnalysis::fillMemoryVarList(Token& t1, Token& value)
{
	string memName = t1.getValue();
	Variable* var = new Variable(memName, -1, Variable::MEM_VAR, stoi(value.getValue()));
	memoryVariables.push_back(var);
}

/*Fills the list with all register variables*/
void SyntaxAnalysis::fillRegisterVarList(Token& t)
{
	string regName = t.getValue();
	Variable* var = new Variable(regName, registerCounter, Variable::REG_VAR);
	registerVariables.push_back(var);
	registerCounter++;
}

/*Fills the list with all function*/
void SyntaxAnalysis::fillFunctionList(string& name, int position)
{
	functionList.push_back(name);
	Labels label(name, position);
	labelList.push_back(label);
}

/*Fills the list with all labels*/
void SyntaxAnalysis::fillLabelList(string& name, int position)
{
	Labels label(name, position);
	labelList.push_back(label);
}

/*Gets the position of a register variable*/
int SyntaxAnalysis::getRegPosition(const std::string& varName)
{
	for (Variables::iterator it = registerVariables.begin(); it != registerVariables.end(); it++)
	{
		if ((*it)->GetName() == varName)
		{
			return (*it)->getPosition();
		}
	}
}


/*Creates instruction using source and destination variables*/
void SyntaxAnalysis::createInstruction(InstructionType type, vector<Token>& dst, vector<Token>& src)
{
	Instruction* instruction;
	Variables* dstVar = new Variables;
	Variables* srcVar = new Variables;
	Variable* var;
	switch (type)
	{
	case I_ADD:
		var = new Variable(dst[0].getValue(), getRegPosition(dst[0].getValue()), Variable::REG_VAR);
		dstVar->push_back(var);
		var = new Variable(src[0].getValue(), getRegPosition(src[0].getValue()), Variable::REG_VAR);
		srcVar->push_back(var);
		var = new Variable(src[1].getValue(), getRegPosition(src[1].getValue()), Variable::REG_VAR);
		srcVar->push_back(var);
		instruction = new Instruction(instructionPosition, type, *dstVar, *srcVar);
		break;
	case I_ADDI:
		var = new Variable(dst[0].getValue(), getRegPosition(dst[0].getValue()), Variable::REG_VAR);
		dstVar->push_back(var);
		var = new Variable(src[0].getValue(), getRegPosition(src[0].getValue()), Variable::REG_VAR);
		srcVar->push_back(var);
		var = new Variable(src[1].getValue(), 0, Variable::NO_TYPE);
		srcVar->push_back(var);
		instruction = new Instruction(instructionPosition, type, *dstVar, *srcVar);
		break;
	case I_SUB:
		var = new Variable(dst[0].getValue(), getRegPosition(dst[0].getValue()), Variable::REG_VAR);
		dstVar->push_back(var);
		var = new Variable(src[0].getValue(), getRegPosition(src[0].getValue()), Variable::REG_VAR);
		srcVar->push_back(var);
		var = new Variable(src[1].getValue(), getRegPosition(src[1].getValue()), Variable::REG_VAR);
		srcVar->push_back(var);
		instruction = new Instruction(instructionPosition, type, *dstVar, *srcVar);
		break;
	case I_LA:
		var = new Variable(dst[0].getValue(), getRegPosition(dst[0].getValue()), Variable::REG_VAR);
		dstVar->push_back(var);
		var = new Variable(src[0].getValue(), 0, Variable::NO_TYPE);
		srcVar->push_back(var);
		instruction = new Instruction(instructionPosition, type, *dstVar, *srcVar);
		break;
	case I_LW:
		var = new Variable(dst[0].getValue(), getRegPosition(dst[0].getValue()), Variable::REG_VAR);
		dstVar->push_back(var);
		var = new Variable(src[0].getValue(), 0, Variable::NO_TYPE);
		srcVar->push_back(var);
		var = new Variable(src[1].getValue(), getRegPosition(src[1].getValue()), Variable::REG_VAR);
		srcVar->push_back(var);
		instruction = new Instruction(instructionPosition, type, *dstVar, *srcVar);
		break;
	case I_LI:
		var = new Variable(dst[0].getValue(), getRegPosition(dst[0].getValue()), Variable::REG_VAR);
		dstVar->push_back(var);
		var = new Variable(src[0].getValue(), getRegPosition(src[0].getValue()), Variable::REG_VAR);
		srcVar->push_back(var);
		instruction = new Instruction(instructionPosition, type, *dstVar, *srcVar);
		break;
	case I_SW:
		var = new Variable(src[0].getValue(), getRegPosition(src[0].getValue()), Variable::REG_VAR);
		srcVar->push_back(var);
		var = new Variable(dst[0].getValue(), 0, Variable::NO_TYPE);
		dstVar->push_back(var);
		var = new Variable(dst[1].getValue(), getRegPosition(dst[1].getValue()), Variable::REG_VAR);
		dstVar->push_back(var);
		instruction = new Instruction(instructionPosition, type, *dstVar, *srcVar);
		break;
	case I_B:
		var = new Variable(dst[0].getValue(), 0, Variable::NO_TYPE);
		dstVar->push_back(var);
		instruction = new Instruction(instructionPosition, type, *dstVar, *srcVar);
		break;
	case I_BLTZ:
		var = new Variable(dst[0].getValue(), 0, Variable::NO_TYPE);
		dstVar->push_back(var);
		var = new Variable(src[0].getValue(), getRegPosition(src[0].getValue()), Variable::REG_VAR);
		srcVar->push_back(var);
		instruction = new Instruction(instructionPosition, type, *dstVar, *srcVar);
		break;
	case I_NOP:
		instruction = new Instruction(instructionPosition, type, *dstVar, *srcVar);
		break;
	case I_OR:
		var = new Variable(dst[0].getValue(), getRegPosition(dst[0].getValue()), Variable::REG_VAR);
		dstVar->push_back(var);
		var = new Variable(src[0].getValue(), getRegPosition(src[0].getValue()), Variable::REG_VAR);
		srcVar->push_back(var);
		var = new Variable(src[1].getValue(), getRegPosition(src[1].getValue()), Variable::REG_VAR);
		srcVar->push_back(var);
		instruction = new Instruction(instructionPosition, type, *dstVar, *srcVar);
		break;
	case I_NOR:
		var = new Variable(dst[0].getValue(), getRegPosition(dst[0].getValue()), Variable::REG_VAR);
		dstVar->push_back(var);
		var = new Variable(src[0].getValue(), getRegPosition(src[0].getValue()), Variable::REG_VAR);
		srcVar->push_back(var);
		var = new Variable(src[1].getValue(), getRegPosition(src[1].getValue()), Variable::REG_VAR);
		srcVar->push_back(var);
		instruction = new Instruction(instructionPosition, type, *dstVar, *srcVar);
		break;
	case I_BEQZ:
		var = new Variable(dst[0].getValue(), 0, Variable::NO_TYPE);
		dstVar->push_back(var);
		var = new Variable(src[0].getValue(), getRegPosition(src[0].getValue()), Variable::REG_VAR);
		srcVar->push_back(var);
		instruction = new Instruction(instructionPosition, type, *dstVar, *srcVar);
		break;
	}
	/*Adds instruction to the list*/
	instructions.push_back(instruction);
	instructionPosition++;
}

/*Returns the position of instruction*/
int SyntaxAnalysis::findInstructionPosition(Variables vars)
{
	Variables::iterator it = vars.begin();
	for (list<Labels>::iterator l_it = labelList.begin(); l_it != labelList.end(); l_it++)
	{
		if ((*it)->GetName() == (*l_it).name)
		{
			return (*l_it).position;
		}
	}
}

/*Fills the list with succesor variables*/
void SyntaxAnalysis::fillSuccesors()
{
	Instructions::iterator it_end = instructions.end();
	it_end--;
	for (Instructions::iterator it = instructions.begin(); it != it_end; it++)
	{
		//If instruction is branch it finds position of succesor that it is supposed to branch to 
		if ((*it)->getType() == I_B || (*it)->getType() == I_BLTZ)
		{
			int pos = findInstructionPosition((*it)->getDst());
			for (Instructions::iterator it_b = instructions.begin(); it_b != instructions.end(); it_b++)
			{
				if ((*it_b)->getPosition() == pos)
				{
					(*it)->setSucc((*it_b));
				}
			}
		}
		else
		{
			Instructions::iterator pom = ++it;
			it--;
			(*it)->setSucc((*pom));
		}
	}
}
/*Fills the list with predecessor variables*/
void SyntaxAnalysis::fillPredecessor()
{
	Instructions::reverse_iterator it_end = instructions.rend();
	it_end--;
	for (Instructions::reverse_iterator it = instructions.rbegin(); it != it_end; it++)
	{
		Instructions::reverse_iterator pom = ++it;
		it--;
		(*it)->setPred((*pom));
	}
}

bool SyntaxAnalysis::isRegDefined(Token t)
{
	for (Variables::iterator it = registerVariables.begin(); it != registerVariables.end(); it++)
	{
		if (t.getValue() == (*it)->GetName())
		{
			return true;
		}
	}
	throw runtime_error("\nRegister not defined\n");
}
bool SyntaxAnalysis::isMemDefined(Token t)
{
	for (Variables::iterator it = memoryVariables.begin(); it != memoryVariables.end(); it++)
	{
		if (t.getValue() == (*it)->GetName())
		{
			return true;
		}
	}
	throw runtime_error("\nMemory variable not defined\n");
}

void SyntaxAnalysis::Q()
{
	if (errorFound == false)
	{
		S();
		eat(T_SEMI_COL);
		L();
	}
}

void SyntaxAnalysis::S()
{
	if (errorFound == false)
	{
		Token t;
		switch (currentToken.getType())
		{
		/*Memory variables*/
		case T_MEM:
			eat(T_MEM);
			t = currentToken;
			eat(T_M_ID);
			fillMemoryVarList(t, currentToken);
			eat(T_NUM);
			break;
		/*Register variables*/
		case T_REG:
			eat(T_REG);
			fillRegisterVarList(currentToken);
			eat(T_R_ID);
			break;
		/*Functions*/
		case T_FUNC:
			eat(T_FUNC);
			fillFunctionList(currentToken.getValue(), instructionPosition);
			eat(T_ID);
			break;
		/*Labels*/
		case T_ID:
			fillLabelList(currentToken.getValue(), instructionPosition);
			eat(T_ID);
			eat(T_COL);
			E();
			break;
		default:
			E();
		}
	}
}

void SyntaxAnalysis::L()
{
	if (errorFound == false)
	{
		if (currentToken.getType() == T_END_OF_FILE)
		{
			eat(T_END_OF_FILE);
		}
		else
			Q();
	}
}

void SyntaxAnalysis::E()
{
	vector<Token> src;
	vector<Token> dst;
	if (errorFound == false)
	{
		switch (currentToken.getType())
		{
		case T_ADD:
			eat(T_ADD);
			isRegDefined(currentToken);
			dst.push_back(currentToken);
			eat(T_R_ID);
			eat(T_COMMA);
			isRegDefined(currentToken);
			src.push_back(currentToken);
			eat(T_R_ID);
			eat(T_COMMA);
			isRegDefined(currentToken);
			src.push_back(currentToken);
			eat(T_R_ID);
			createInstruction(I_ADD, dst, src);
			break;
		case T_ADDI:
			eat(T_ADDI);
			isRegDefined(currentToken);
			dst.push_back(currentToken);
			eat(T_R_ID);
			eat(T_COMMA);
			isRegDefined(currentToken);
			src.push_back(currentToken);
			eat(T_R_ID);
			eat(T_COMMA);
			src.push_back(currentToken);
			eat(T_NUM);
			createInstruction(I_ADDI, dst, src);
			break;
		case T_SUB:
			eat(T_SUB);
			isRegDefined(currentToken);
			dst.push_back(currentToken);
			eat(T_R_ID);
			eat(T_COMMA);
			isRegDefined(currentToken);
			src.push_back(currentToken);
			eat(T_R_ID);
			eat(T_COMMA);
			isRegDefined(currentToken);
			src.push_back(currentToken);
			eat(T_R_ID);
			createInstruction(I_SUB, dst, src);
			break;
		case T_LA:
			eat(T_LA);
			isRegDefined(currentToken);
			dst.push_back(currentToken);
			eat(T_R_ID);
			eat(T_COMMA);
			isMemDefined(currentToken);
			src.push_back(currentToken);
			eat(T_M_ID);
			createInstruction(I_LA, dst, src);
			break;
		case T_LW:
			eat(T_LW);
			isRegDefined(currentToken);
			dst.push_back(currentToken);
			eat(T_R_ID);
			eat(T_COMMA);
			src.push_back(currentToken);
			eat(T_NUM);
			eat(T_L_PARENT);
			isRegDefined(currentToken);
			src.push_back(currentToken);
			eat(T_R_ID);
			eat(T_R_PARENT);
			createInstruction(I_LW, dst, src);
			break;
		case T_LI:
			eat(T_LI);
			isRegDefined(currentToken);
			dst.push_back(currentToken);
			eat(T_R_ID);
			eat(T_COMMA);
			src.push_back(currentToken);
			eat(T_NUM);
			createInstruction(I_LI, dst, src);
			break;
		case T_SW:
			eat(T_SW);
			isRegDefined(currentToken);
			src.push_back(currentToken);
			eat(T_R_ID);
			eat(T_COMMA);
			dst.push_back(currentToken);
			eat(T_NUM);
			eat(T_L_PARENT);
			isRegDefined(currentToken);
			dst.push_back(currentToken);
			eat(T_R_ID);
			eat(T_R_PARENT);
			createInstruction(I_SW, dst, src);
			break;
		case T_B:
			eat(T_B);
			dst.push_back(currentToken);
			eat(T_ID);
			createInstruction(I_B, dst, src);
			break;
		case T_BLTZ:
			eat(T_BLTZ);
			isRegDefined(currentToken);
			src.push_back(currentToken);
			eat(T_R_ID);
			eat(T_COMMA);
			dst.push_back(currentToken);
			eat(T_ID);
			createInstruction(I_BLTZ, dst, src);
			break;
		case T_NOP:
			eat(T_NOP);
			createInstruction(I_NOP, dst, src);
			break;
		case T_OR:
			eat(T_OR);
			isRegDefined(currentToken);
			dst.push_back(currentToken);
			eat(T_R_ID);
			eat(T_COMMA);
			isRegDefined(currentToken);
			src.push_back(currentToken);
			eat(T_R_ID);
			eat(T_COMMA);
			isRegDefined(currentToken);
			src.push_back(currentToken);
			eat(T_R_ID);
			createInstruction(I_OR, dst, src);
			break;
		case T_NOR:
			eat(T_NOR);
			isRegDefined(currentToken);
			dst.push_back(currentToken);
			eat(T_R_ID);
			eat(T_COMMA);
			isRegDefined(currentToken);
			src.push_back(currentToken);
			eat(T_R_ID);
			eat(T_COMMA);
			isRegDefined(currentToken);
			src.push_back(currentToken);
			eat(T_R_ID);
			createInstruction(I_NOR, dst, src);
			break;
		case T_BEQZ:
			eat(T_BEQZ);
			isRegDefined(currentToken);
			src.push_back(currentToken);
			eat(T_R_ID);
			eat(T_COMMA);
			dst.push_back(currentToken);
			eat(T_ID);
			createInstruction(I_BEQZ, dst, src);
			break;
		default:
			errorFound = true;
		}
	}
}



/*Register variables list getter*/
Variables& SyntaxAnalysis::getRegVariables()
{
	return registerVariables;
}

/*Cheks if it should print label in a file*/
string checkLabel(int pos, list<Labels> labelList)
{
	string ret = "";
	for (list<Labels>::iterator it_label = labelList.begin(); it_label != labelList.end(); it_label++)
	{
		if (pos == (*it_label).position)
		{
			ret += (*it_label).name + ":\n";
		}
	}
	return ret;
}

/*Returns name of assigned register variable*/
string SyntaxAnalysis::returnAssignedReg(string r)
{
	for (Variables::iterator it = registerVariables.begin(); it != registerVariables.end(); it++)
	{
		if (r == (*it)->GetName())
		{
			switch ((*it)->getAssignment())
			{
			case t0:
				return "t0";
			case t1:
				return "t1";
			case t2:
				return "t2";
			case t3:
				return "t3";
			}
		}
	}
}

void SyntaxAnalysis::printAlocated()
{
	for (Variables::iterator it = registerVariables.begin(); it != registerVariables.end(); it++)
	{
		cout << (*it)->GetName() << ": " << (*it)->getAssignment() << endl;
	}
}

/*Creates .s file*/
void SyntaxAnalysis::createMIPS()
{
	ofstream MIPSfile("../examples/mips_files/MIPSfile.s");

	/*Fills .globl*/
	MIPSfile << ".globl ";
	for (list<string>::iterator it = functionList.begin(); it != functionList.end(); it++)
	{
		MIPSfile << (*it) << endl;
	}

	/*Fills .data*/
	MIPSfile << "\n";
	MIPSfile << ".data\n";
	for (Variables::iterator it = memoryVariables.begin();
		it != memoryVariables.end();
		it++)
	{
		MIPSfile << "" << (*it)->GetName() << ": \t" << ".word " << (*it)->getValue() << "\n";
	}
	
	/*Fills .text*/
	MIPSfile << "\n";
	MIPSfile << ".text\n";

		for (Instructions::iterator it = instructions.begin(); it != instructions.end(); it++)
		{
			MIPSfile << checkLabel((*it)->getPosition(), labelList);
			switch ((*it)->getType())
			{
			case I_ADD:
				MIPSfile << "\tadd\t\t $" << returnAssignedReg((*it)->m_dst.front()->GetName()) << ", $" << returnAssignedReg((*it)->m_src.front()->GetName()) << ", $" << returnAssignedReg((*it)->m_src.back()->GetName()) <<endl;
				break;
			case I_SUB:
				MIPSfile << "\tsub\t\t $" << returnAssignedReg((*it)->m_dst.front()->GetName()) << ", $" << returnAssignedReg((*it)->m_src.front()->GetName()) << ", $" << returnAssignedReg((*it)->m_src.back()->GetName()) << endl;
				break;
			case I_ADDI:
				MIPSfile << "\taddi\t $" << returnAssignedReg((*it)->m_dst.front()->GetName()) << ", $" << returnAssignedReg((*it)->m_src.front()->GetName()) << ", " << (*it)->m_src.back()->GetName() << endl;
				break;
			case I_LW:
				MIPSfile << "\tlw\t\t $" << returnAssignedReg((*it)->m_dst.front()->GetName()) << ", " << (*it)->m_src.front()->GetName() << "($"  << returnAssignedReg((*it)->m_src.back()->GetName()) << ")" << endl;
				break;
			case I_LA:
				MIPSfile << "\tla\t\t $" << returnAssignedReg((*it)->m_dst.front()->GetName()) << ", " << (*it)->m_src.front()->GetName() << endl;
				break;
			case I_LI:
				MIPSfile << "\tli\t\t $" << returnAssignedReg((*it)->m_dst.front()->GetName()) << ", " << (*it)->m_src.front()->GetName() << endl;
				break;
			case I_SW:
				MIPSfile << "\tsw\t\t $" << returnAssignedReg((*it)->m_src.front()->GetName()) << ", " << (*it)->m_dst.front()->GetName() << "($" << returnAssignedReg((*it)->m_dst.back()->GetName()) << ")" << endl;
				break;
			case I_B:
				MIPSfile << "\tb\t\t " << (*it)->m_dst.front()->GetName() << endl;
				break;
			case I_BLTZ:
				MIPSfile << "\tbltz\t $" << returnAssignedReg((*it)->m_src.front()->GetName()) << ", " << (*it)->m_dst.front()->GetName() << endl; 
				break;
			case I_OR:
				MIPSfile << "\tor\t\t $" << returnAssignedReg((*it)->m_dst.front()->GetName()) << ", $" << returnAssignedReg((*it)->m_src.front()->GetName()) << ", $" << returnAssignedReg((*it)->m_src.back()->GetName()) << endl;
				break;
			case I_NOR:
				MIPSfile << "\tnor\t\t $" << returnAssignedReg((*it)->m_dst.front()->GetName()) << ", $" << returnAssignedReg((*it)->m_src.front()->GetName()) << ", $" << returnAssignedReg((*it)->m_src.back()->GetName()) << endl;
				break;
			case I_BEQZ:
				MIPSfile << "\tbeqz\t $" << returnAssignedReg((*it)->m_src.front()->GetName()) << ", " << (*it)->m_dst.front()->GetName() << endl;
				break;
			}

		}


	MIPSfile.close();
}
