/* Autor: Uroš Jevtić Datum: 31.05.2022. */
#ifndef __IR__
#define __IR__

#include "Types.h"
#include <iostream>
using namespace std;


/**
 * This class represents one variable from program code.
 */
class Variable
{
public:
	enum VariableType
	{
		MEM_VAR,
		REG_VAR,
		NO_TYPE
	};

	Variable() : m_type(NO_TYPE), m_name(""), m_position(-1), m_assignment(no_assign) {}
	Variable(std::string name, int pos) : m_type(NO_TYPE), m_name(name), m_position(pos), m_assignment(no_assign) {}
	Variable(std::string name, int pos, VariableType v_type, int val = 0) : m_name(name), m_type(v_type), m_position(pos), m_assignment(no_assign), mem_value(val){}
	/*Getters*/
	std::string GetName();
	int getValue();
	int getPosition();
	Regs getAssignment();
	/*Setters*/
	void setAssigment(Regs reg);
//private:
	int mem_value;			//Value of memory variable		
	VariableType m_type;
	std::string m_name;
	int m_position;
	Regs m_assignment;
};


/**
 * This type represents list of variables from program code.
 */
typedef std::list<Variable*> Variables;


/**
 * This class represents one instruction in program code.
 */
class Instruction
{
public:
	Instruction () : m_position(0), m_type(I_NO_TYPE) {}
	Instruction (int pos, InstructionType type, Variables& dst, Variables& src) : m_position(pos), m_type(type), m_dst(dst), m_src(src) 
	{
		fillUseDefVariables();
	}
	/*Getters*/
	InstructionType getType();
	int getPosition();
	Variables getDst();
	Variables getSrc();
	Variables getUse();
	Variables getDef();
	Variables getIn();
	Variables getOut();
	list<Instruction*> getSucc();
	list<Instruction*> getPred();
	/*Seters*/
	void setSucc(Instruction* succ);
	void setPred(Instruction* pred);

//private:
	void fillUseDefVariables();

	int m_position;
	InstructionType m_type;
	
	Variables m_dst;
	Variables m_src;

	Variables m_use;
	Variables m_def;
	Variables m_in;
	Variables m_out;
	std::list<Instruction*> m_succ;
	std::list<Instruction*> m_pred;
};


/**
 * This type represents list of instructions from program code.
 */
typedef std::list<Instruction*> Instructions;

	
#endif
