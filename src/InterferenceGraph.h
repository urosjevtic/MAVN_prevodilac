/* Autor: Uroš Jevtić Datum: 12.06.2022. */
#include "IR.h"

typedef vector<std::vector<int>> InterferenceMatrix;
typedef stack<Variable*> VariableStack;


/* Class that represents interference graph.
Builds interference graph and does reasource allocation*/
class InterferenceGraph
{
public:
	InterferenceGraph(Variables& vars);

	/* Creates interference graph from instructions. */
	void buildInterferenceGraph(Instructions& instructions);

	/* Creates stack from list of register variables. */
	void buildVariableStack();

	/* Allocates real registers to variables */
	bool resourceAllocation();

	/* Prints interference matrix to the console. */
	void printInterferenceMatrix();

	InterferenceMatrix im;
	VariableStack varStack;  

private:

	/* Applies reg to the variable that has varPos for its position. */
	void applyRegToVariable(int varPos, Regs reg);

	/* register variables */
	Variables& regVariables;  
};
