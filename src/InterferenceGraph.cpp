/* Autor: Uroš Jevtić Datum: 12.06.2022. */
#include "InterferenceGraph.h"

#include "Constants.h"

using namespace std;

InterferenceGraph::InterferenceGraph(Variables& vars) : regVariables(vars){}


/* Creates interference graph*/
void InterferenceGraph::buildInterferenceGraph(Instructions& instructions)
{
	/*Interference matrix(size*size of reg vars)*/
	im.resize(regVariables.size());
	for (int i = 0; i < im.size(); ++i)
		im[i].resize(regVariables.size());


	//Make interference between defined variable and every out variable
	for (Instructions::iterator it = instructions.begin(); it != instructions.end(); it++)
	{
		for (Variables::iterator def_it = (*it)->m_def.begin(); def_it != (*it)->m_def.end(); def_it++)
		{
			for (Variables::iterator out_it = (*it)->m_out.begin(); out_it != (*it)->m_out.end(); out_it++)
			{
				int defPos = (*def_it)->getPosition();
				int outPos = (*out_it)->getPosition();

				// Variable can't be in interference with itself
				if (defPos != outPos)
				{
					im[defPos][outPos] = __INTERFERENCE__;
					im[outPos][defPos] = __INTERFERENCE__;
				}
				else
				{
					im[defPos][outPos] = __EMPTY__;
					im[outPos][defPos] = __EMPTY__;
				}
			}
		}
	}
}


/* Creates stack from list of all register variables. */
void InterferenceGraph::buildVariableStack()
{
	for (Variables::iterator it = regVariables.begin();
		it != regVariables.end();
		it++)
	{
		varStack.push(*it);
	}
}

/* Applies reg to the variable that has varPos for it's position. */
void InterferenceGraph::applyRegToVariable(int varPos, Regs reg)
{
	for (Variables::iterator it = regVariables.begin();
		it != regVariables.end();
		it++)
	{
		if ((*it)->getPosition() == varPos)
			(*it)->setAssigment(reg);
	}
	
}

/* Allocates real registers to variables according to the interference. */
bool InterferenceGraph::resourceAllocation()
{
	//BuildVariableStack();
	list<Regs> registers;      // List of registers
	list<Variable*> colored;  // Colored registers

	/*Fills the list with all registers*/
	for (int i = 1; i <= __REG_NUMBER__; i++)
		registers.push_back((Regs)i);

	while (!varStack.empty())
	{
		Variable* pom = varStack.top();
		bool interference = false;

		// Check if there is interference between pom and any colored variable
		for (auto it = colored.begin(); it != colored.end(); it++)
		{
			if (im[pom->getPosition()][(*it)->getPosition()] == __INTERFERENCE__)
				interference = true;
		}

		// If there is no interference assign free register to pom
		if (!interference)
		{
			if (registers.empty())
				return false;

			pom->setAssigment(registers.front());
		}
		else
		{
			// If there is interference pick up new register to assign to pom
			if (!registers.empty())
			{
				registers.pop_front();

				/*If there is not enough registers it is not possible to allocate reasources*/
				if (registers.empty())
				{
					return false;
				}

				pom->setAssigment(registers.front());
			}
			else
			{
				return false;
			}
		}

		colored.push_back(pom);
		varStack.pop();
	}
	for (Variables::iterator it = colored.begin(); it != colored.end(); it++)
	{
		cout << (*it)->GetName() << ": t" << (*it)->getAssignment() - 1 << endl;
	}

	return true;
}

/* Prints interference matrix to the console. */
void InterferenceGraph::printInterferenceMatrix()
{
	cout << "  ";
	for (int i = 0; i < im.size(); ++i)
		cout << "r" << i << "  ";
	cout << endl;

	for (int i = 0; i < im.size(); ++i)
	{
		cout << "r" << i << " ";

		for (int j = 0; j < im[i].size(); ++j)
			cout << im[i][j] << "   ";

		cout << endl;
	}
}