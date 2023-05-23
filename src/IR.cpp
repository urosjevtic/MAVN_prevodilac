/* Autor: Uroš Jevtić Datum: 12.06.2022. */
#include "IR.h"

#include <iostream>

using namespace std;
/*Variable getters*/
string Variable::GetName()
{
	return m_name;
}

int Variable::getValue()
{
	return mem_value;
}

int Variable::getPosition()
{
	return m_position;
}

Regs Variable::getAssignment()
{
	return m_assignment;
}
/*Variable setters*/

void Variable::setAssigment(Regs reg)
{
	m_assignment = reg;
}

/*Instruction getters*/
Variables Instruction::getDst()
{
	return m_dst;
}
Variables Instruction::getSrc()
{
	return m_src;
}
Variables Instruction::getOut()
{
	return m_out;
}
Variables Instruction::getIn()
{
	return m_in;
}
Variables Instruction::getUse()
{
	return m_use;
}
Variables Instruction::getDef()
{
	return m_def;
}
list<Instruction*> Instruction::getSucc()
{
	return m_succ;
}
list<Instruction*> Instruction::getPred()
{
	return m_pred;
}

/*Instruction setters*/
void Instruction::setSucc(Instruction* succ)
{
	m_succ.push_back(succ);
}

void Instruction::setPred(Instruction* pred)
{
	m_pred.push_back(pred);
}

int Instruction::getPosition()
{
	return m_position;
}

InstructionType Instruction::getType()
{
	return m_type;
}

void Instruction::fillUseDefVariables()
{
	switch (m_type)
	{
	case I_ADD:
	case I_SUB:
	case I_ADDI:
	case I_OR:
	case I_NOR:
		for (Variables::iterator it = m_src.begin(); it != m_src.end(); it++)
		{
			m_use.push_back((*it));
		}
		for (Variables::iterator it = m_dst.begin(); it != m_dst.end(); it++)
		{
			m_def.push_back((*it));
		}
		break;
	case I_LA: 
	case I_LI: 
	case I_LW: 
	case I_SW: 
		for (Variables::iterator it = m_dst.begin(); it != m_dst.end(); it++)
		{
			m_def.push_back((*it));
		}
		break;
	case I_BEQZ: 
	case I_BLTZ: 
		for (Variables::iterator it = m_src.begin(); it != m_src.end(); it++)
		{
			m_use.push_back((*it));
		}
		break;
	}
}

